/*
 * Copyright (c) 2004, 2005 BLADOX, s.r.o.
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE BLADOX OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.  
 */

#ifndef _STKLIB_H_
#define _STKLIB_H_

#include "types.h"
#include "consts.h"

#ifdef __cplusplus
extern "C" {
#endif


/** 
Wrapper around the STK REFRESH command. The purpose
REFRESH is to inform ME that something on SIM was changed by the SIM.
The \c what argument says what was changed, ie. what to refresh.
\ingroup api_stk
\stk
\param what	#Q_REFRESH_SIM_INIT_FULL_FILE, #Q_REFRESH_SIM_FILE_CHANGE, #Q_REFRESH_SIM_INIT_FILE_CHANGE, #Q_REFRESH_SIM_INIT, #Q_REFRESH_SIM_RESET.
\param nr_files	number of paths, only if \c what is #Q_REFRESH_SIM_FILE_CHANGE or #Q_REFRESH_SIM_INIT_FILE_CHANGE
\param len	length of \c filebuf in bytes, only if \c what is #Q_REFRESH_SIM_FILE_CHANGE or #Q_REFRESH_SIM_INIT_FILE_CHANGE
\param filebuf	pointer on paths, only if \c what is #Q_REFRESH_SIM_FILE_CHANGE or #Q_REFRESH_SIM_INIT_FILE_CHANGE, RAM only

\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * refresh(u8 what, u8 nr_files, u8 len, u8 *filebuf);

/** 
\ingroup api_stk
Wrapper around the STK SEND SHORT MESSAGE command.
\stk

\code

#include <config.h>
#include <turbo/turbo.h>

u8 PROGMEM t_sms_ex[]="Example sms";
u8 PROGMEM t_ms[]="+420123456789";

void action_sms(void *data)
{
u8 *sms_data=malloc(100);
u8 *ms=str2msisdn(t_ms, MSISDN_ADN, MEM_R);

memcpy(sms_data, t_sms_ex, sizeof(t_sms_ex)); 
send_sms(sms_data, sizeof(t_sms_ex), ms, MSISDN_ADN, 0xF6, 0, NULL, NULL);

free(sms_data);
free(ms);
}
\endcode

As of \c pid the usual values are:
- 0x00	the simple MS-to-SC short message (i.e. ordinary SMS).
- 0x7f 	the SIM Data Download message.


The PID 0x7f is used by the STK applications to send and receive
SMS without the user intervention, ME must pass the message to the
SIM using ENVELOPE SMS_PP_DOWNLOAD EVENT.
\attention 
The usage of PID 0x7F can be restricted by the mobile operator (although
there is no technical nor security reason).

If you want to use PID 0x7f we suggest to use tsms_pid() call instead. 
The return value of tsms_pid() is configurable by user and can be set to 0x00
in case his operator does not permit 0x7f PID.


\c dcs typical values are:
- 0x00 - default alphabet, 7bit coding, class 0.
- 0xF6 - 8bit data,  class 2.

Class 0 means that is upon receiving ME what to do with the incoming message 
(pass it to SIM or store it in ME memory), Class 2 means SIM specific 
message, i.e. ME should pass it to SIM.

Ref. \ref cf_etsi_03_38 for alphabets and details.

If \c dcs is 7b then the user data has to be already compressed.

\param ud	pointer on the data of SMS (i.e. message itself), always from #MEM_R.
\param len	length of data in bytes.
\param msisdn	pointer on MSISDN, \ref api_msisdn.
\param da_type	type of MSISDN, \ref api_msisdn.
\param dcs	DCS (Data Coding Scheme), ref. \ref cf_etsi_03_38.
\param pid	PID of SMS, ref. \ref cf_etsi_03_40.
\param alpha	alpha (optional), \ref string_alpha or unpacked \ref string_slv, from RAM, EEPROM, PROGMEM.
\param tpdu	returns pointer on resulting TPDU if not NULL.
\return 	TERMINAL RESPONSE in buf_A().
\sa get_tag()
*/

extern u8 * send_sms(u8 *ud, u8 len, u8 * msisdn, u8 da_type, u8 dcs, u8 pid, const u8 * alpha, u8 * tpdu);

/** 
Wrapper around the STK SET UP CALL command.
\ingroup api_stk
\stk
\param msisdn	MSISDN, can be from RAM, EEPROM, PROGMEM.
\param da_type	#MSISDN_ADN or #MSISDN_SMS.
\param alpha	alpha, \ref string_alpha or unpacked \ref string_slv, can be from RAM, EEPROM or PROGMEM.
\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * set_up_call(u8 *msisdn, u8 da_type, const u8 *alpha);

/** 
In case EVENT DOWNLOAD actions are registered (and unregistered) ad-hoc 
you must call set_up_event_list() to pass the actual event list to ME.

The event list is updated always if application registers 
ACTION_EVENT_? during ACTION_APP_INIT.

The event actions are: #ACTION_EVENT_MT_CALL, #ACTION_EVENT_CALL_CONNECTED, #ACTION_EVENT_CALL_DISCONNECTED, #ACTION_EVENT_LOCATION_STATUS, #ACTION_EVENT_USER_ACTIVITY, #ACTION_EVENT_IDLE_SCREEN, #ACTION_EVENT_CARD_READER_STATUS, #ACTION_EVENT_LANGUAGE_SELECTION, #ACTION_EVENT_BROWSER_TERMINATION, #ACTION_EVENT_DATA_AVAILABLE, #ACTION_EVENT_CHANNEL_STATUS.

\ingroup api_stk
\stk
\return 	TERMINAL RESPONSE in buf_A().
\sa reg_action(), unreg_action()
*/

extern u8 * set_up_event_list(void);

/** 
Wrapper around STK PROVIDE LOCAL INFO command.
\ingroup api_stk
\stk
\return 	TERMINAL RESPONSE in buf_A().
\sa get_tag(), #T_LOCATION_INFO
*/

extern u8 * provide_local_info(void);

/** 
\ingroup api_stk
Wrapper around STK PROVIDE LOCAL INFO command. 
\stk
\return 	TERMINAL RESPONSE in buf_A().
\sa get_tag(), #T_IMEI
*/

extern u8 * provide_imei(void);

/** 
\ingroup api_stk
Wrapper around STK PROVIDE LOCAL INFO command. 
\stk
\return 	TERMINAL RESPONSE in buf_A().
\sa get_tag(), #T_NMR, #T_BCCH_CHANNEL_LIST
*/

extern u8 * provide_nmr(void);

/** 
\ingroup api_stk
Wrapper around STK PROVIDE LOCAL INFO command. 
\stk
\return 	TERMINAL RESPONSE in buf_A().
\sa get_tag(), #T_DATE
*/

extern u8 * provide_date(void);

/** 
\ingroup api_stk
Wrapper around STK PROVIDE LOCAL INFO command. 
\stk
\return 	TERMINAL RESPONSE in buf_A().
\sa get_tag(), #T_LANGUAGE
*/

extern u8 * provide_language(void);

/** 
\ingroup api_stk
Wrapper around STK PROVIDE LOCAL INFO command. 
\stk
\return 	TERMINAL RESPONSE in buf_A().
\sa get_tag(), #T_TIMING_ADVANCE
*/

extern u8 * provide_ta(void);

/** 
Wrapper around STK POLL INTERVAL command. 
\ingroup api_stk
\stk
\param units	#DURATION_MIN, #DURATION_SEC, #DURATION_10_SEC
\param duration	
\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * poll_interval(u8 units, u8 duration);

/** 
Wrapper around STK PLAY TONE command. 
\ingroup api_stk
\stk
\param alpha	alpha text, \ref string_alpha or unpacked \ref string_slv, can be from RAM, EEPROM or PROGMEM
\param tone	#TONE_DIAL, #TONE_BUSY, #TONE_CONGESTION, #TONE_RADIO_ACK, #TONE_RADIO_NO, #TONE_ERROR, #TONE_WAITING, #TONE_RINGING, #TONE_BEEP, #TONE_POSITIVE, #TONE_NEGATIVE
\param units	#DURATION_MIN, #DURATION_SEC, #DURATION_10_SEC
\param duration	
\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * play_tone(const u8 *alpha, u8 tone, u8 units, u8 duration);

/** 
\ingroup api_stk
Compose and display text. It can consists of two parts - head and body. 
If the total length exceeds display_text_len then head is not displayed.
\stk
\param body	body, \ref string_alpha or \ref string_slv, from RAM, EEPROM, PROGMEM
\param head	head, \ref string_alpha or \ref string_slv, from RAM, EEPROM, PROGMEM
\return #APP_BACK if user back, #APP_END if user end, #APP_OK if user ok
*/

extern u8 display_text(const u8 *body, const u8 *head);

/** 
\ingroup api_stk
Wrapper around STK DISPLAY TEXT command.
\stk
\param text	text, \ref string_alpha or \ref string_slv, from RAM, EEPROM, PROGMEM.
\param q	#Q_DISPLAY_TEXT_NORMAL_PRIORITY or #Q_DISPLAY_TEXT_HIGH_PRIORITY, #Q_DISPLAY_TEXT_DELAY_CLEAR or #Q_DISPLAY_TEXT_USER_CLEAR.
\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * display_text_raw(const u8 *text, u8 q);

/** 
\ingroup api_stk
Wrapper around STK SELECT ITEM command.
\stk
\param nr_items	number of items
\param items	pointer on array of pointers on strings, \ref string_alpha or unpacked \ref string_slv
\param alpha	alpha text, \ref string_alpha or unpacked \ref string_slv, can be stored in RAM, EEPROM or PROGMEM
\param text_r	extra first item, \ref string_alpha or unpacked \ref string_slv, can be in RAM, EEPROM, PROGMEM
\param def_item	default item, 1...nr_items
\param q	#Q_SELECT_ITEM_NO_PRESENTATION, #Q_SELECT_ITEM_CHOICE, #Q_SELECT_ITEM_NAVIGATION, or'ed #Q_SELECT_ITEM_HELP
\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * select_item(u8 nr_items, const u8 ** items, const u8 *alpha, const u8 * text_r, u8 def_item, u8 q);

/** 
\ingroup api_stk
Wrapper around STK GET INPUT command.
\stk
\param text		Text, \ref string_alpha or \ref string_slv, from RAM, EEPROM, PROGMEM.
\param resp_len_min 	Minimum response length.
\param resp_len_max	Maximum repsonse length.
\param default_text	Default text, \ref string_alpha or \ref string_slv, from RAM, EEPROM, PROGMEM.
\param q	#Q_GET_INPUT_DIGITS or #Q_GET_INPUT_ALPHABET, or'ed #Q_GET_INPUT_UCS2, #Q_GET_INPUT_NO_ECHO, #Q_GET_INPUT_SMS_PACKED, #Q_GET_INPUT_HELP

\return NULL if user back, #ENULL if user END, pointer to buf_A() on tag T_TEXT_STRING.
*/

extern u8 * get_input(const u8 * text, u8 resp_len_min, u8 resp_len_max, const u8 * default_text, u8 q);

/** 
\ingroup api_stk
Wrapper around STK GET INKEY command.
\stk
\param text	Text, \ref string_alpha or \ref string_slv, from RAM, EEPROM, PROGMEM.
\param q	#Q_GET_INKEY_DIGIT or #Q_GET_INKEY_CHAR or #Q_GET_INKEY_YESNO,
#Q_GET_INKEY_UCS2, #Q_GET_INKEY_HELP

\return NULL if user back, #ENULL if user END, pointer to buf_A() on tag T_TEXT_STRING.
*/

extern u8 * get_inkey(const u8 * text, u8 q);

/** 
\ingroup api_stk
Yes/No user interface function.
\stk
\param text	Text, \ref string_alpha or unpacked \ref string_slv, can be from RAM, EEPROM, PROGMEM.
\param concat where 0 means no modification of text, #Q_YESNO_DELETE_QMARK means modification to "Delete 'text'?", and #Q_YESNO_QMARK means concat with '?', i.e. "text?"
\param def	Default answer 0/1 (not applicable if #PROC_8_CONFIG_BINARY_CHOICE set)
\return #APP_YES if yes, #APP_NO if no, #APP_BACK if user back, #APP_END if user end.
*/

extern u8 yesno(const u8 *text, u8 concat, u8 def);

/** 
\ingroup api_msisdn
Inquire user for MSISDN, ref. \ref api_msisdn.
\stk
\param alpha	if not NULL then return name of MSISDN (if available).
\return		MSISDN, NULL if user cancel, ENULL if user terminated whole application
*/

extern u8 * msisdn(u8 alpha[ALPHA_LEN]);

/** 
\ingroup api_security
Ask for password/key. The resulting key is in format: key[0] is nr of chars
(septets), then follow the chars compressed to 7b padded with 
len_8-key[0].
\note Number of chars is (len_8-1)*8/7, not the \c len_8.
\stk
\param key	pointer on buf[len_8] to fill the resulting key in
\param len_8	number of bytes the resulting key should have
\param text 	if NULL then default "Key" text is used, \ref string_alpha or \ref string_slv, can be from RAM, EEPROM, PROGMEM

\return #APP_OK if \c key valid, #APP_END if user end, #APP_BACK if user back.
*/

extern u8 password(u8 * key, u8 len_8, const u8 * text);

/** 
\ingroup api_stk
Wrapper around the STK SET UP MENU command.
\stk
\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * set_up_menu(void);

/** 
\ingroup api_err
Display localized error message text.
\stk
\param error_no	#ERR_NO_PROGMEM, #ERR_NO_RAM, #ERR_UNKNOWN, #ERR_NO_APP_SLOT, #ERR_MC_NOT_PRESENT, #ERR_MC_NOT_READY, #ERR_NO_EEPROM, #ERR_BAD_INPUT, #ERR_WRONG_KEY, #ERR_FILE_TOO_LONG, #ERR_NO_SPACE, #ERR_BAD_FILE
\return 	#APP_END, #APP_BACK
*/

extern u8 perror(u8 error_no);

/** 
\ingroup api_stk
Wrapper around STK MORE TIME command.
\stk
\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * more_time(void);

/** 
\ingroup api_stk
Convenience function, display text "Not implemented".
\stk
\return 	#APP_END, #APP_BACK
*/

extern u8 not_implemented(void);

/** 
\ingroup api_img
\stk
Wrapper around the STK DISPLAY TEXT command with T_ICON_ID object.
\param w	width of image
\param h	height of image
\param fmt	#IMG_BW or #IMG_COLOR
\param img_fnc	image function (generates the image itself)
\param text	text, \ref string_alpha or \ref string_slv, from RAM, EEPROM, PROGMEM.
\param icon_q  #Q_DISPLAY_ICON_SELF_EXP or #Q_DISPLAY_ICON_NOT_SELF_EXP, \ref cf_etsi_11_14 #T_ICON_ID

\return #APP_BACK if user back, #APP_END if user end, #APP_OK if user ok.
*/

extern u8 display_icon(u8 w, u8 h, u8 fmt, u8 (*img_fnc)(u16), const u8 *text, u8 icon_q);

/** 
\ingroup api_img
In case you wanted to write a new STK command wrapper using T_ICON_ID use this
function to set the image generating function.
\param w	width
\param h	height
\param fmt	#IMG_BW or #IMG_COLOR
\param fnc	image function (generates the image itself)
*/

extern void set_img(u8 w, u8 h, u8 fmt, u8 (*fnc)(u16));

/** 
\ingroup api_stk
AKA "more" command - show content of file as text.
\stk
\param file	Inode pointer on file.
\return 	#APP_END, #APP_BACK
*/

extern u8 display_more(u32 file);

/** 
\ingroup api_stk
Wrapper around the STK OPEN CHANNEL command. Application has to register #ACTION_EVENT_DATA_AVAILABLE to be notified when data are received.

Low level function returning TERMINAL RESPONSE. To get the channel number use
the following:
\code
u8 *res;
u8 j;
u8 ch;

res = open_channel_raw (...);
j = get_tag (res, T_CHANNEL_STATUS);
if (j != 0)
{
  ch = res[j+2] & 0x07;
  if (ch != 0)
  {
  // open OK
  }
}
else
{
// open ERROR
}
\endcode

\stk
\param dst	destination (protocol, port, ip address).
\param q	(#Q_OPEN_CHANNEL_ONDEMAND or #Q_OPEN_CHANNEL_IMMEDIATE)|#Q_OPEN_CHANNEL_AUTOMATIC_RECONNECTION
\param buf_size
\return 	TERMINAL RESPONSE in buf_A().
\sa close_channel(), get_channel_status(), #ACTION_EVENT_DATA_AVAILABLE, get_tag(), #T_CHANNEL_STATUS
*/

extern u8 * open_channel_raw(Sockaddr * dst, u8 q, u16 buf_size);

/** 
\ingroup api_stk
Wrapper around the STK CLOSE CHANNEL command.
\stk
\param channel
\sa open_channel_raw(), get_channel_status()
*/

extern void close_channel(u8 channel);

/** 
\ingroup api_stk
Wrapper around the STK GET CHANNEL STATUS command.
\stk
\return 	TERMINAL RESPONSE in buf_A().
\sa open_channel_raw(), close_status()
*/

extern u8 * get_channel_status(void);

/**
Wrapper around the STK SEND DATA command.
\ingroup api_stk
\stk
\param channel
\param data	can be from RAM, EEPROM, PROGMEM
\param data_len
\param q	#Q_SEND_DATA_BUFFERED or #Q_SEND_DATA_IMMEDIATELY
\return 	TERMINAL RESPONSE in buf_A().
\sa open_channel_raw()
*/

extern u8 * send_data_raw(u8 channel, const u8 * data, u8 data_len, u8 q);

/** 
\ingroup api_stk
Wrapper around the STK RECEIVE DATA command.
\stk
\param channel
\param len

\return 	TERMINAL RESPONSE in buf_A().
\sa open_channel_raw(), #ACTION_EVENT_DATA_AVAILABLE, get_tag(), #T_CHANNEL_DATA_LEN, #T_CHANNEL_DATA
*/

extern u8 * receive_data_raw(u8 channel, u8 len);

/** 
\ingroup api_stk
Wrapper around the STK LAUNCH BROWSER command.
\stk
\param url	URL, can be stored in RAM, EEPROM, PROGMEM.
\param q	#Q_LAUNCH_BROWSER_NEW or #Q_LAUNCH_BROWSER_REUSE or #Q_LAUNCH_BROWSER_REOPEN
\return 	TERMINAL RESPONSE in buf_A().
*/

extern u8 * launch_browser(const u8 * url, u8 q);

/** 
\ingroup api_stk
Wrapper around STK TIMER MANAGEMENT command - start timer.
\stk
\param h	hours
\param m	minutes
\param s	seconds

\return assigned timer id, 0 if none available.
*/

extern u8 timer_start(u8 h, u8 m, u8 s);

/** 
\ingroup api_stk
Wrapper around STK TIMER MANAGEMENT command - stop timer.
\stk
\param t	timer id
\return tag #T_TIMER_VALUE converted to u32.
*/

extern u32 timer_stop(u8 t);

/** 
\ingroup api_stk
Wrapper around STK TIMER MANAGEMENT command - current timer value.
\stk
\param t	timer id
\return object #T_TIMER_VALUE converted to u32.
*/

extern u32 timer_value(u8 t);

/**
\ingroup api_stk
Insert top-level menu item. Can be used in #ACTION_INSERT_MENU handling code only. 
\note Application can insert max. 4 menus.
Items are numbered 0, 1, 2, 3.
\param text	Menu item text, \ref string_alpha or unpacked \ref string_slv, can be from RAM, EEPROM, PROGMEM.	
\sa #ACTION_INSERT_MENU, set_menu_alpha().
*/

extern void insert_menu(const u8 * text);

/**
\ingroup api_stk
Set top-level menu label. Can be used in #ACTION_INSERT_MENU handling code only. 
\param text	Menu item text, \ref string_alpha or unpacked \ref string_slv, can be from RAM, EEPROM, PROGMEM.
\sa #ACTION_INSERT_MENU
*/

extern void set_menu_alpha(const u8 * text);


/** The spider_init() creates and initiates the spider context. 
\ingroup api_spider

\code
void foo()
{
SCtx *c;

c=spider_init();
if(c==NULL){perror(ERR_NO_RAM);return;}
... // modifications to c
spider(c);
}
\endcode

\return NULL if memory error
*/

extern SCtx * spider_init();

/** The spider() starts the traversing of the menu structure and destroys
the context at the end of application.
\ingroup api_spider
\stk
\param ctx	context
*/

extern void spider(SCtx * ctx);

/** 
\ingroup api_spider
Append RAM edge to the current node.
\param ctx
\param to	TO node, can be from RAM, EEPROM, PROGMEM

\return #NO_ERROR if OK, #ERR_NO_RAM if error.
*/

extern u8 spider_append_r(SCtx * ctx, const SNode * to);

/** 
\ingroup api_spider
Delete all RAM edges leading from the current node.
\param ctx
*/

extern void spider_clear_r(SCtx * ctx);

/** 
\ingroup api_spider
Set order in which edges are evaluated. 
\param ctx
\param o1	#MEM_R or #MEM_P or #MEM_E or #MEM_D
\param o2	#MEM_R or #MEM_P or #MEM_E or #MEM_D
\param o3	#MEM_R or #MEM_P or #MEM_E or #MEM_D
\param o4	#MEM_R or #MEM_P or #MEM_E or #MEM_D
*/

extern void spider_set_order(SCtx * ctx, u8 o1,u8 o2,u8 o3,u8 o4);

/** 
\ingroup api_spider
Skip parent node - return to node previous to parent node.
\param ctx

\return NULL if no parent, ENULL if parent.
*/

extern u8 * spider_back(SCtx *ctx);

#ifdef __cplusplus
}
#endif

#endif
