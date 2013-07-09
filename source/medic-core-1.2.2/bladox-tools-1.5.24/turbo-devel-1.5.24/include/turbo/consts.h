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


#ifndef _TURBO_CONSTS_
#define _TURBO_CONSTS_

#ifdef __cplusplus
extern "C" {
#endif

/* b7-4 0000 */
	
#define LC_ENGLISH		0x01
#define LC_GERMAN		0x00
#define LC_ITALIAN		0x02
#define LC_FRENCH		0x03
#define LC_SPANISH		0x04
#define LC_DUTCH		0x05
#define LC_SWEDISH		0x06
#define LC_DANISH		0x07
#define LC_PORTUGUESE		0x08
#define LC_FINNISH		0x09
#define LC_NORWEGIAN		0x0a
#define LC_GREEK		0x0b
#define LC_TURKISH		0x0c
#define LC_HUNGARIAN		0x0d
#define LC_POLISH		0x0e
#define LC_UNSPECIFIED		0x0f

/* b7-4 0001 b3-0 0000 GSM 7bit default alphabet */
/* b7-4 0001 b3-0 0001 GSM 7bit default alphabet */

/* b7-4 0010 */

#define LC_CZECH		0x20
#define LC_HEBREW		0x21
#define LC_ARABIC		0x22
#define LC_RUSSIAN		0x23
#define LC_ICELANDIC		0x24

#define LC_AUTOMATIC		0xFF

#define LC_XX(a,b)	{a,sizeof(b),b},
#define LC_XX_LIST(a,b)	{a,b},

/** \ingroup api_i18n
Terminator of lc_char[].
*/
#define LC_END		{0,0,""}
/** \ingroup api_i18n
Terminator of lc_list[].
*/
#define LC_END_LIST	{0,0}

#ifdef CONFIG_LANG_EN
#define LC_EN(a)	LC_XX(LC_ENGLISH,a)
#define LC_EN_LIST(a)	LC_XX_LIST(LC_ENGLISH,a)
#else
/** 
\ingroup api_i18n
Localized string, English, for coding \ref cf_etsi_03_38.
*/
#define LC_EN(a)
/** 
\ingroup api_i18n
Localized array of strings, English, for coding \ref cf_etsi_03_38.
*/
#define LC_EN_LIST(a)	
#endif

#ifdef CONFIG_LANG_DE
#define LC_DE(a)	LC_XX(LC_GERMAN,a)
#define LC_DE_LIST(a)	LC_XX_LIST(LC_GERMAN,a)
#else
/** 
\ingroup api_i18n
Localized string, German, for coding \ref cf_etsi_03_38.
*/

#define LC_DE(a)
/** 
\ingroup api_i18n
Localized array of strings, German, for coding \ref cf_etsi_03_38.
*/

#define LC_DE_LIST(a)
#endif

#ifdef CONFIG_LANG_IT
#define LC_IT(a)	LC_XX(LC_ITALIAN,a)
#define LC_IT_LIST(a)	LC_XX_LIST(LC_ITALIAN,a)
#else
/** 
\ingroup api_i18n
Localized string, Italian, for coding \ref cf_etsi_03_38.
*/

#define LC_IT(a)
/** 
\ingroup api_i18n
Localized array of strings, Italian, for coding \ref cf_etsi_03_38.
*/

#define LC_IT_LIST(a)
#endif

#ifdef CONFIG_LANG_FR
#define LC_FR(a)	LC_XX(LC_FRENCH,a)
#define LC_FR_LIST(a)	LC_XX_LIST(LC_FRENCH,a)
#else
/** 
\ingroup api_i18n
Localized string, French, for coding  \ref cf_etsi_03_38.
*/

#define LC_FR(a)
/** 
\ingroup api_i18n
Localized array of strings, French, for coding \ref cf_etsi_03_38.
*/

#define LC_FR_LIST(a)
#endif

#ifdef CONFIG_LANG_ES
#define LC_ES(a)	LC_XX(LC_SPANISH,a)
#define LC_ES_LIST(a)	LC_XX_LIST(LC_SPANISH,a)
#else
/** 
\ingroup api_i18n
Localized string, Spanish,  for coding \ref cf_etsi_03_38.
*/

#define LC_ES(a)
/** 
\ingroup api_i18n
Localized array of strings, Spanish,  for coding \ref cf_etsi_03_38.
*/

#define LC_ES_LIST(a)
#endif

#ifdef CONFIG_LANG_NL
#define LC_NL(a)	LC_XX(LC_DUTCH,a)
#define LC_NL_LIST(a)	LC_XX_LIST(LC_DUTCH,a)
#else
/** 
\ingroup api_i18n
Localized string, \ref cf_etsi_03_38
*/

#define LC_NL(a)
/** 
\ingroup api_i18n
Localized array of strings, Dutch, for coding \ref cf_etsi_03_38.
*/

#define LC_NL_LIST(a)
#endif

#ifdef CONFIG_LANG_SE
#define LC_SE(a)	LC_XX(LC_SWEDISH,a)
#define LC_SE_LIST(a)	LC_XX_LIST(LC_SWEDISH,a)
#else
/** 
\ingroup api_i18n
Localized string, Swedish, for coding \ref cf_etsi_03_38.
*/

#define LC_SE(a)
/** 
\ingroup api_i18n
Localized array of strings, Swedish, for coding \ref cf_etsi_03_38.
*/

#define LC_SE_LIST(a)
#endif

#ifdef CONFIG_LANG_DK
#define LC_DK(a)	LC_XX(LC_DANISH,a)
#define LC_DK_LIST(a)	LC_XX_LIST(LC_DANISH,a)
#else
/** 
\ingroup api_i18n
Localized string, Danish, for coding \ref cf_etsi_03_38.
*/

#define LC_DK(a)
/** 
\ingroup api_i18n
Localized array of strings, Danish, for coding \ref cf_etsi_03_38.
*/

#define LC_DK_LIST(a)
#endif

#ifdef CONFIG_LANG_PT
#define LC_PT(a)	LC_XX(LC_PORTUGUESE,a)
#define LC_PT_LIST(a)	LC_XX_LIST(LC_PORTUGUESE,a)
#else
/** 
\ingroup api_i18n
Localized string, Portuguese, for coding \ref cf_etsi_03_38.
*/

#define LC_PT(a)
/** 
\ingroup api_i18n
Localized array of strings, Portuguese, for coding \ref cf_etsi_03_38.
*/

#define LC_PT_LIST(a)
#endif

#ifdef CONFIG_LANG_FI
#define LC_FI(a)	LC_XX(LC_FINNISH,a)
#define LC_FI_LIST(a)	LC_XX_LIST(LC_FINNISH,a)
#else
/** 
\ingroup api_i18n
Localized string, Finnish, for coding \ref cf_etsi_03_38.
*/

#define LC_FI(a)
/** 
\ingroup api_i18n
Localized array of strings, Finnish, for coding \ref cf_etsi_03_38.
*/

#define LC_FI_LIST(a)
#endif

#ifdef CONFIG_LANG_NO
#define LC_NO(a)	LC_XX(LC_NORWEGIAN,a)
#define LC_NO_LIST(a)	LC_XX_LIST(LC_NORWEGIAN,a)
#else
/** 
\ingroup api_i18n
Localized string, Norwegian, for coding \ref cf_etsi_03_38.
*/

#define LC_NO(a)
/** 
\ingroup api_i18n
Localized array of strings, Norwegian, for coding \ref cf_etsi_03_38.
*/

#define LC_NO_LIST(a)
#endif

#ifdef CONFIG_LANG_EL
#define LC_EL(a)	LC_XX(LC_GREEK,a)
#define LC_EL_LIST(a)	LC_XX_LIST(LC_GREEK,a)
#else
/** 
\ingroup api_i18n
Localized string, Greek, for coding \ref cf_etsi_03_38.
*/

#define LC_EL(a)
/** 
\ingroup api_i18n
Localized array of strings, Greek, for coding \ref cf_etsi_03_38.
*/

#define LC_EL_LIST(a)
#endif

#ifdef CONFIG_LANG_TR
#define LC_TR(a)	LC_XX(LC_TURKISH,a)
#define LC_TR_LIST(a)	LC_XX_LIST(LC_TURKISH,a)
#else
/** 
\ingroup api_i18n
Localized string, Turkish, for coding \ref cf_etsi_03_38.
*/

#define LC_TR(a)
/** 
\ingroup api_i18n
Localized array of strings, Turkish, for coding \ref cf_etsi_03_38.
*/

#define LC_TR_LIST(a)
#endif

#ifdef CONFIG_LANG_HU
#define LC_HU(a)	LC_XX(LC_HUNGARIAN,a)
#define LC_HU_LIST(a)	LC_XX_LIST(LC_HUNGARIAN,a)
#else
/** 
\ingroup api_i18n
Localized string, Hungarian, for coding \ref cf_etsi_03_38.
*/

#define LC_HU(a)
/** 
\ingroup api_i18n
Localized array of strings, Hungarian, for coding \ref cf_etsi_03_38.
*/

#define LC_HU_LIST(a)
#endif

#ifdef CONFIG_LANG_PL
#define LC_PL(a)	LC_XX(LC_POLISH,a)
#define LC_PL_LIST(a)	LC_XX_LIST(LC_POLISH,a)
#else
/** 
\ingroup api_i18n
Localized string, Polish, for coding \ref cf_etsi_03_38.
*/

#define LC_PL(a)
/** 
\ingroup api_i18n
Localized array of strings, Polish, for coding \ref cf_etsi_03_38.
*/

#define LC_PL_LIST(a)
#endif

#ifdef CONFIG_LANG_UN
#define LC_UN(a)	LC_XX(LC_UNSPECIFIED,a)
#define LC_UN_LIST(a)	LC_XX_LIST(LC_UNSPECIFIED,a)
#else
/** 
\ingroup api_i18n
Localized string, Unspecified/Universal, for coding \ref cf_etsi_03_38.
*/

#define LC_UN(a)
/** 
\ingroup api_i18n
Localized array of strings, Unspecified/Universal, for coding \ref cf_etsi_03_38.
*/

#define LC_UN_LIST(a)
#endif

#ifdef CONFIG_LANG_CZ
#define LC_CZ(a)	LC_XX(LC_CZECH,a)
#define LC_CZ_LIST(a)	LC_XX_LIST(LC_CZECH,a)
#else
/** 
\ingroup api_i18n
Localized string, Czech, for coding \ref cf_etsi_03_38.
*/

#define LC_CZ(a)
/** 
\ingroup api_i18n
Localized array of strings, Czech, for coding \ref cf_etsi_03_38.
*/

#define LC_CZ_LIST(a)
#endif

#ifdef CONFIG_LANG_HE
#define LC_HE(a)	LC_XX(LC_HEBREW,a)
#define LC_HE_LIST(a)	LC_XX_LIST(LC_HEBREW,a)
#else
/** 
\ingroup api_i18n
Localized string, Hebrew, for coding \ref cf_etsi_03_38.
*/

#define LC_HE(a)
/** 
\ingroup api_i18n
Localized array of strings, Hebrew, for coding \ref cf_etsi_03_38.
*/

#define LC_HE_LIST(a)
#endif

#ifdef CONFIG_LANG_AR
#define LC_AR(a)	LC_XX(LC_ARABIC,a)
#define LC_AR_LIST(a)	LC_XX_LIST(LC_ARABIC,a)
#else
/** 
\ingroup api_i18n
Localized string, Arabic, for coding \ref cf_etsi_03_38.
*/

#define LC_AR(a)
/** 
\ingroup api_i18n
Localized array of strings, Arabic, for coding \ref cf_etsi_03_38.
*/

#define LC_AR_LIST(a)
#endif

#ifdef CONFIG_LANG_RU
#define LC_RU(a)	LC_XX(LC_RUSSIAN,a)
#define LC_RU_LIST(a)	LC_XX_LIST(LC_RUSSIAN,a)
#else
/** 
\ingroup api_i18n
Localized string, Russian, for coding \ref cf_etsi_03_38.
*/

#define LC_RU(a)
/** 
\ingroup api_i18n
Localized array of strings, Russian, for coding \ref cf_etsi_03_38.
*/

#define LC_RU_LIST(a)
#endif

#ifdef CONFIG_LANG_IS
#define LC_IS(a)	LC_XX(LC_ICELANDIC,a)
#define LC_IS_LIST(a)	LC_XX_LIST(LC_ICELANDIC,a)
#else
/** 
\ingroup api_i18n
Localized string, Icelandic, for coding \ref cf_etsi_03_38.
*/

#define LC_IS(a)
/** 
\ingroup api_i18n
Localized array of strings, Icelandic, for coding \ref cf_etsi_03_38.
*/

#define LC_IS_LIST(a)
#endif


/* NONREGISTERABLE: */
/** Invoked for application registration. 
\ingroup api_actions

Called only once when the application is loaded into the memory.
By registration we mean one time
process, when application can e.g. allocate EEPROM structures, reg_sms_tag(),
reg_file_type() etc. 
*/
#define ACTION_APP_REGISTER			0x00
/** Invoked when application is unloaded from memory. 
\ingroup api_actions

Application should clear the resources it used (EEPROM).
*/
#define ACTION_APP_UNREGISTER			0x01

/** Invoked for application to initialize its internals. 
\ingroup api_actions
\sa _init()
*/
#define ACTION_APP_INIT				0x02
/** Invoked when \b message \ref api_tsms of registered #SMS_TYPE_MSG type is received. 
\ingroup api_actions
*/
#define ACTION_SMS_MSG				0x03
/** Invoked when \b question \ref api_tsms of registered #SMS_TYPE_QA type is received. 
\ingroup api_actions
*/
#define ACTION_SMS_Q				0x04
/** Invoked when \b answer \ref api_tsms of registered #SMS_TYPE_QA type is received.
\ingroup api_actions
*/
#define ACTION_SMS_A				0x05
/** Invoked when menu item was selected by the user. 
\ingroup api_actions

The application can have more than one item in top level menu inserted (max. 4).
Example:
\code

lc_char PROGMEM lc_Item_A[]={
        LC_EN("Item A")
        LC_END
};

lc_char PROGMEM lc_Item_B[]={
        LC_EN("Item B")
        LC_END
};

void action_menu(Menu_selection_data * x)
{
if(x->item==0)
 {
	// Item A
 }
else  //x==1
 {
	// Item B 
 }
}

void turbo_handler(u8 action, void * data)
{
 switch(action)
  {
   ...
	case    ACTION_INSERT_MENU:
		insert_menu(locale(lc_Item_A));
		insert_menu(locale(lc_Item_B));
		break;
	case    ACTION_MENU_SELECTION:
		stk_thread(action_menu, data);
		break;
   ...
  }
}
\endcode

*/
#define ACTION_MENU_SELECTION			0x06

/** Invoked when ME requests file registered by the reg_file() call. 
\ingroup api_actions

\code
u8 PROGMEM fake_file_path[]= { 0x3F, 0x00, 0x7F, 0x20, 0x6F, 0x07 };

void handle_fake_file(File_apdu_data *fa)
{
}

void turbo_handler(u8 action, void * data)
{
switch(action)
{
...
	case    ACTION_APP_INIT:
		reg_file(fake_file_path,3);
		break;
	case    ACTION_FILE_APDU:
		handle_fake_file(data);
		break;
...
}
\endcode
*/
#define ACTION_FILE_APDU			0x07

/** Invoked when ME signals EVENT that timer has expired.
\ingroup api_actions

*/
#define ACTION_TIMER_EXPIRATION			0x08

/** Invoked when ME sends GSM_TERMINAL_PROFILE cmd. Since then the 
return value of terminal_profile() is valid.
\ingroup api_actions
*/
#define ACTION_TERMINAL_PROFILE			0x09

/** Request for application to insert its menu items. 
Application may insert more than one item by insert_menu().
This action is invoked whenever either SIM or turbo wants to set up new
menu list. It can happen ad hoc, e.g. user loads new application or 
changes language, etc.
\ingroup api_actions
*/
#define ACTION_INSERT_MENU    			0x0A

/** Invoked upon start up when ME is ready to handle STK commands, after
menu is set.
\ingroup api_actions
*/
#define ACTION_FIRST_STK    			0x0B

/** Invoked when ME manages/calls channel registered by the reg_channel() call. 
\ingroup api_actions

\code

void handle_channel(File_apdu_data *fa)
{
}

void turbo_handler(u8 action, void * data)
{
switch(action)
{
...
	case    ACTION_APP_INIT:
		reg_channel(WANTED_CHANNEL_NUMBER);
		break;
	case    ACTION_LOGICAL_CHANNEL:
		handle_channel(data);
		break;
...
}
\endcode
\sa apdu_class()
*/

#define ACTION_LOGICAL_CHANNEL				0x0C


/** Invoked by other applications. It's up to the called application
to impement security and protocol.
 
\ingroup api_actions

\code

void turbo_handler(u8 action, void * data)
{
switch(action)
{
...
	case    ACTION_IPC:
		handle_request(data);
		break;
...
}
\endcode
\sa call_app()
*/

#define ACTION_IPC				0x0D


/* REGISTERABLE IRQ: */
#define ACTIONS_IRQ_START			0x10

#define ACTION_SIG_INTERRUPT0          		0x10
#define ACTION_SIG_INTERRUPT1          		0x11
/*
#define ACTION_SIG_INTERRUPT2          		0x12
#define ACTION_SIG_INTERRUPT3          		0x13
#define ACTION_SIG_INTERRUPT4          		0x14
*/
#define ACTION_SIG_INTERRUPT5          		0x12
#define ACTION_SIG_INTERRUPT6          		0x13
/*
#define ACTION_SIG_INTERRUPT7          		0x17
#define ACTION_SIG_OUTPUT_COMPARE2     		0x18
#define ACTION_SIG_OVERFLOW2           		0x19
#define ACTION_SIG_INPUT_CAPTURE1      		0x1A
#define ACTION_SIG_OUTPUT_COMPARE1A   		0x1B
#define ACTION_SIG_OUTPUT_COMPARE1B    		0x1C
#define ACTION_SIG_OVERFLOW1           		0x1D
*/
#define ACTION_SIG_OUTPUT_COMPARE0     		0x14
#define ACTION_SIG_OVERFLOW0           		0x15
/*
#define ACTION_SIG_SPI                 		0x20
#define ACTION_SIG_UART0_RECV          		0x21
#define ACTION_SIG_UART0_DATA          		0x22
#define ACTION_SIG_UART0_TRANS         		0x23
*/
#define ACTION_SIG_ADC                 		0x16
/* #define ACTION_SIG_EEPROM_READY        		0x25 */
#define ACTION_SIG_COMPARATOR          		0x17
/* #define ACTION_SIG_OUTPUT_COMPARE1C    		0x27 */
#define ACTION_SIG_INPUT_CAPTURE3      		0x18
#define ACTION_SIG_OUTPUT_COMPARE3A    		0x19
#define ACTION_SIG_OUTPUT_COMPARE3B    		0x1A
#define ACTION_SIG_OUTPUT_COMPARE3C    		0x1B
#define ACTION_SIG_OVERFLOW3           		0x1C
/*
#define ACTION_SIG_UART1_RECV          		0x2D
#define ACTION_SIG_UART1_DATA          		0x2E
#define ACTION_SIG_UART1_TRANS         		0x2F
*/
#define ACTION_SIG_2WIRE_SERIAL        		0x1D
/* #define ACTION_SIG_SPM_READY           		0x31 */

#define NR_ACTIONS_IRQ		14

/* REGISTERABLE TURBO: */
#define ACTIONS_TURBO_START			0x40



/** Register this action if you want turbo (your
application) to operate also between APDUs.
\ingroup api_actions


Following should be considered when using idle task:
-# registration has to happen during early during SIM initialization. This
is because we have to inform ME not to stop clocks.
-# It may not be possible to reregister ACTION_IDLE_TASK because
some phones ask about clock stopping only once.
-# Idle operation consumes more power - turbo does not sleep and clocks
are on.
-# Clock frequency can be lower than during normal operation (1/3).
*/

#define ACTION_IDLE_TASK			0x40

/** 
\ingroup api_actions
Invoked when SMS is received.
\sa store_sms_2_sim()
*/
#define ACTION_SMS				0x41

/** 
\ingroup api_actions
Invoked when ME asks SIM APDU RUN GSM ALGORITHM.
*/
#define ACTION_RUN_GSM_ALGORITHM		0x42

/** 
\ingroup api_actions
Invoked when ME updates EF_LOCI
*/

#define ACTION_LOCI_UPDATE  			0x43
/** 
\ingroup api_actions
Invoked when ME updates EF_BCCH
*/

#define ACTION_BCCH_UPDATE  			0x44

/** 
\ingroup api_actions
Invoked by ME APDU STATUS command. Usually every 30sec in idle.
*/

#define ACTION_STATUS       			0x45

/** 
\ingroup api_actions
Invoked by ME_CMD_VERIFY_CHV, ME_CMD_CHANGE_CHV, ME_CMD_DISABLE_CHV, ME_CMD_ENABLE_CHV, ME_CMD_UNBLOCK_CHV commands.
*/

#define ACTION_CHV       			0x46


#define NR_ACTIONS_TURBO	 7

/* REGISTERABLE STK: */
#define ACTIONS_STK_START			0x80



/**
\ingroup api_actions
ENVELOPE SMS-PP DOWNLOAD, \ref cf_etsi_11_14
*/

#define ACTION_SMS_PP_DOWNLOAD			0x80


/** 
\ingroup api_actions
ENVELOPE CELL BROADCAST DOWNLOAD, \ref cf_etsi_11_14
*/

#define ACTION_SMS_CB_DOWNLOAD			0x81


/** 
\ingroup api_actions
ENVELOPE CALL CONTROL, \ref cf_etsi_11_14
*/

#define ACTION_CALL_CONTROL   			0x82

/** 
\ingroup api_actions
ENVELOPE MO SMS CONTROL, \ref cf_etsi_11_14
*/

#define ACTION_MO_SMS_CONTROL  			0x83

#define ACTIONS_EVENTS_START			0x84


/** 
\ingroup api_actions
ENVELOPE MT Call Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_MT_CALL			0x84

/** 
\ingroup api_actions
ENVELOPE Call Connected Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_CALL_CONNECTED		0x85

/** 
\ingroup api_actions
ENVELOPE Call Disconnected Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_CALL_DISCONNECTED		0x86

/** 
\ingroup api_actions
ENVELOPE Location Status Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_LOCATION_STATUS		0x87

/**
\ingroup api_actions
ENVELOPE User Activity Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_USER_ACTIVITY		0x88

/** 
\ingroup api_actions
ENVELOPE Idle Screen Available Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_IDLE_SCREEN		0x89

/** 
\ingroup api_actions
ENVELOPE Card Reader Status Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_CARD_READER_STATUS		0x8A

/** 
\ingroup api_actions
ENVELOPE Language Selection Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_LANGUAGE_SELECTION		0x8B

/**
\ingroup api_actions
ENVELOPE Browser Termination Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_BROWSER_TERMINATION	0x8C

/** 
\ingroup api_actions
ENVELOPE Data Available Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_DATA_AVAILABLE		0x8D

/** 
\ingroup api_actions
ENVELOPE Channel Status Event, \ref cf_etsi_11_14
*/

#define ACTION_EVENT_CHANNEL_STATUS		0x8E

#define ACTIONS_EVENTS_END			0x8E

#define NR_ACTIONS_STK		 15

/** 
\ingroup api_fs
Memory Card present and formatted 
*/
#define MC_OK			0x00
/** 
\ingroup api_fs
Memory Card not present 
*/
#define MC_NOT_PRESENT		0x01
/** 
\ingroup api_fs
Memory Card present but not formatted 
*/
#define MC_NOT_READY   		0x02


/** 
\ingroup api_msisdn
Indicates string format ('\\0' terminated) MSISDN.
*/
#define MSISDN_STR      0x00

/** 
\ingroup api_msisdn
Indicates ADN format MSISDN.
*/
#define MSISDN_ADN      0x01

/** 
\ingroup api_msisdn
Indicates SMS format MSISDN.
*/
#define MSISDN_SMS      0x02

/** 
\ingroup api_tsms
Indicates Status Report Request
*/
#define SMS_SRR      	0x80

/** 
\ingroup api_tsms
Indicates User Data Header Indicator
*/
#define SMS_UDHI      	0x40


#define DISPLAY_TEXT_LEN_MIN	1
#define DISPLAY_TEXT_LEN_MAX	240

/** 
\ingroup api_img
Indicates black and white image.
*/
#define IMG_BW		0x11

/** 
\ingroup api_img
Indicates color image.
*/
#define IMG_COLOR	0x21

/** 
\ingroup api_stk
Max. ALPHA (EF_ADN) length.
*/

#define ALPHA_LEN	17

/** 
\ingroup api_tsms
If set then all incoming TSMS of given tag are trashed. This is the first check when TSMS is received.
*/

#define TSMS_ACL_BLOCKED	0x01

/** 
\ingroup api_tsms
If set then access list (list of allowed numbers) is not checked and TSMS is passed to application.
*/

#define TSMS_ACL_EVERYONE	0x02



/** 
User OK.
\ingroup api_spider
*/
#define APP_OK		0
/** 
User end of application 
\ingroup api_spider
*/
#define APP_END		1
/** 
Spider node enter 
\ingroup api_spider
*/
#define APP_ENTER	2
/** 
Spider node leave 
\ingroup api_spider
*/
#define APP_LEAVE    	3
/** 
User selected YES 
\ingroup api_stk
*/
#define APP_YES 	4
/** 
User selected NO 
\ingroup api_stk
*/
#define APP_NO 		5
/** 
User back 
\ingroup api_spider
*/
#define APP_BACK 	6
/* 
TODO pass_to_sim() 
\ingroup api_actions
*/
#define APP_PASS	7
/** 
Filetype callback related, use default behaviour 
\ingroup api_fs
*/
#define APP_USE_DEFAULT	8

/** 
\ingroup api_fs
File callback - SELECT action 
*/
#define FILE_SELECT		0x00
/** 
\ingroup api_fs
File callback - DETAIL action 
*/
#define FILE_DETAIL		0x01
/** 
\ingroup api_fs
File callback - DELETE action 
*/
#define FILE_DELETE		0x02
/** 
\ingroup api_fs
File callback - MOVE action 
*/
#define FILE_MOVE		0x03
/** 
\ingroup api_fs
File callback - RENAME action 
*/
#define FILE_RENAME		0x04


/**
\ingroup api_mem
Indicates RAM.
*/

#define MEM_R		0x01

/**
\ingroup api_mem
Indicates PROGMEM (flash memory).
*/
#define MEM_P		0x02
/**
\ingroup api_mem
Indicates EEPROM.
*/
#define MEM_E		0x04

/**
\ingroup api_mem
Indicates memory card.
*/
#define MEM_D		0x08

/**
\ingroup api_i18n
Indicates that data are stored in [STYLE][LEN][VALUE].
\sa \ref string_slv
*/

#define STRING_SLV	0x84

#define MEM_L		0x00
#define MEM_H		0x80

/** 
\ingroup api_spider
Indicates that FROM node if from RAM.
*/
#define MEM_F_R		1

/** 
\ingroup api_spider
Indicates that FROM node if from PROGMEM.
*/
#define MEM_F_P		2
/** 
\ingroup api_spider
Indicates that FROM node if from EEPROM.
*/
#define MEM_F_E		4

/** 
\ingroup api_spider
Indicates that FROM node if from Memory Card.
*/
#define MEM_F_D		8

/** 
\ingroup api_spider
Indicates that TO node if from RAM.
*/
#define MEM_T_R		16

/** 
\ingroup api_spider
Indicates that TO node if from PROGMEM.
*/
#define MEM_T_P		32

/** 
\ingroup api_spider
Indicates that TO node if from EEPROM.
*/
#define MEM_T_E		64

/** 
\ingroup api_spider
Indicates that TO node if from Memory Card.
*/
#define MEM_T_D		128

/** 
\ingroup api_spider
Indicated EEPROM edges.
\sa spider_set_order()
*/

#define ORDER_E		0x00

/** 
\ingroup api_spider
Indicates RAM edges.
\sa spider_set_order()
*/

#define ORDER_R		0x01

/** 
\ingroup api_spider
Indicates PROGMEM edges.
\sa spider_set_order()
*/

#define ORDER_P		0x02

/** 
\ingroup api_spider
Indicates Memory Card edges.
\sa spider_set_order()
*/

#define ORDER_D		0x03

/** 
\ingroup api_tsms
Pack 8b to 7b coding. \ref cf_etsi_03_38. 
*/
#define DCS_8_TO_7	0x00
/** 
\ingroup api_tsms
Unpack 7b to 8b coding. \ref cf_etsi_03_38. 
*/
#define DCS_7_TO_8	0x01

/** 
\ingroup api_tsms
Protocol Identifier, ref. \ref cf_etsi_03_40. 
*/
#define T_SMS_PID	0x01
/** 
\ingroup api_tsms
Data Coding Scheme, ref. \ref cf_etsi_03_40 and \ref cf_etsi_03_38.
*/
#define T_SMS_DCS	0x02
/** 
\ingroup api_tsms
Service Centre Time Stamp, ref. \ref cf_etsi_03_40. 
*/
#define T_SMS_SCTS	0x03
/** 
\ingroup api_tsms
User Data Length, ref. \ref cf_etsi_03_40. 
*/
#define T_SMS_UDL	0x04
/** 
\ingroup api_tsms
User Data, ref. \ref cf_etsi_03_40. 
*/
#define T_SMS_UD	0x05
/** 
\ingroup api_tsms
Message Reference, \ref cf_etsi_03_40. 
*/
#define T_SMS_MR	0x06
/** 
\ingroup api_tsms
Validity Period, ref. \ref cf_etsi_03_40. 
*/
#define T_SMS_VP	0x07
/** 
\ingroup api_tsms
Discharge Time, ref. \ref cf_etsi_03_40. 
*/
#define T_SMS_DT	0x08
/** 
\ingroup api_tsms
Status, ref. \ref cf_etsi_03_40. 
*/
#define T_SMS_ST	0x09
/** 
\ingroup api_tsms
Any Address, substitute for Recipient Address, Destination Address, Originating Address, ref. \ref cf_etsi_03_40. 
*/
#define T_SMS_AA	0x0a

/** 
\ingroup api_tsms
\ref cf_etsi_03_40. 
*/
#define TP_MTI_DELIVER		0x00
/** 
\ingroup api_tsms
\ref cf_etsi_03_40. 
*/
#define TP_MTI_DELIVER_REPORT	0x00
/** 
\ingroup api_tsms
\ref cf_etsi_03_40. 
*/
#define TP_MTI_SUBMIT		0x01
/** 
\ingroup api_tsms
\ref cf_etsi_03_40. 
 */
#define TP_MTI_SUBMIT_REPORT	0x01
/** 
\ingroup api_tsms
\ref cf_etsi_03_40. 
*/
#define TP_MTI_STATUS_REPORT	0x02
/** 
\ingroup api_tsms
\ref cf_etsi_03_40. 
*/
#define TP_MTI_COMMAND		0x02
/** 
\ingroup api_tsms
\ref cf_etsi_03_40. 
*/
#define TP_MTI_RESERVED		0x03

#define NULL	((u16)0)
/** 
\ingroup api_stk
"End NULL" means User End.
*/
#define ENULL		((void*)0xFFFF)
//#define BNULL		0xFFFE

/**
Indicates Turbo SMS Message Type.
\ingroup api_tsms
*/

#define SMS_TYPE_MSG		0x00

/**
Indicates Turbo SMS Question/Answer Type.
\ingroup api_tsms
*/

#define SMS_TYPE_QA		0x01

/**
\ingroup api_spider
List attribute - name only. Entries starting with \b . are hidden.
*/

#define SPIDER_STYLE_LS			0x00

/**
\ingroup api_spider
List attribute - \b name.type for file or \b name/ for dir.
Entries starting with \b . are hidden.
*/

#define SPIDER_STYLE_LS_L 		0x01

/**
\ingroup api_spider
List attribute - \b name.type for file or \b name/ for dir.
Entries starting with \b . are not hidden.
*/

#define SPIDER_STYLE_LS_AL		0x02


/**
\ingroup api_spider
Or'ed to inform spider that items can be in UCS2.
*/

#define SPIDER_STYLE_UCS2		0x80

/**
\ingroup api_stk
Indicates UDP IP protocol.
*/

#define IPPROTO_UDP	0x01

/**
\ingroup api_stk
Indicates TCP IP protocol.
*/

#define IPPROTO_TCP	0x02

/**
\ingroup api_stk
Indicates CSD bearer.
*/

#define BEARER_CSD	0x01

/**
\ingroup api_stk
Indicates GPRS bearer.
*/

#define BEARER_GPRS	0x02

/**
\ingroup api_stk
Indicates DEFAULT bearer.
*/

#define BEARER_DEFAULT	0x03


/**
\ingroup api_spider
Constant denoting use of built-in localized "New". SNodeP only.
*/

#define  LC_NEW			0x0001

/**
\ingroup api_spider
Constant denoting use of built-in localized "Delete". SNodeP only.
*/

#define  LC_DELETE		0x0002

/**
\ingroup api_spider
Constant denoting use of built-in localized "View". SNodeP only.
*/

#define  LC_VIEW		0x0003

/**
\ingroup api_spider
Constant denoting use of built-in localized "Edit". SNodeP only.
*/

#define  LC_EDIT		0x0004

/**
\ingroup api_spider
Constant denoting use of built-in localized "Protect". SNodeP only.
*/

#define  LC_PROTECT		0x0005

/**
\ingroup api_spider
Constant denoting use of built-in localized "Unprotect". SNodeP only.
*/

#define  LC_UNPROTECT		0x0006

/**
\ingroup api_spider
Constant denoting use of built-in localized "Help". SNodeP only.
*/

#define  LC_HELP     		0x0007


/**
\ingroup api_stk
play_tone() constant, \ref cf_etsi_11_11.
*/

#define TONE_DIAL	0x01

/**
\ingroup api_stk
play_tone() constant, \ref cf_etsi_11_11.
*/

#define TONE_BUSY	0x02

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_CONGESTION	0x03

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_RADIO_ACK	0x04

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_RADIO_NO	0x05

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_ERROR	0x06

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_WAITING	0x07

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_RINGING	0x08

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_BEEP	0x10

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_POSITIVE	0x11

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define TONE_NEGATIVE	0x12

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define DURATION_MIN		0x00

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define DURATION_SEC		0x01

/**
play_tone() constant, \ref cf_etsi_11_11.
\ingroup api_stk
*/

#define DURATION_10_SEC 	0x02

/**
\ingroup intern_app_format
Indicates manifest.
*/
#define FILE_APP_TAG		0x01

/**
\ingroup api_prot_mem
Indicate read operation permision - block can be read.
*/

#define PROT_MEM_READ			0x20

/**
\ingroup api_prot_mem
Indicate encrypt operation permision - block can be used for encryption.
*/

#define PROT_MEM_ENCRYPT		0x40

/**
\ingroup api_prot_mem
Indicate decrypt operation permision - block can be used for decryption.
*/

#define PROT_MEM_DECRYPT		0x80


/**
Unknown APDU INS.
\ingroup api_sim
*/

#define ME_CMD_UNKNOWN			0x00

/**
APDU INS SELECT command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_SELECT			0xa4

/**
APDU INS STATUS command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_STATUS			0xf2

/**
APDU INS READ BINARY command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_READ_BINARY		0xb0

/**
APDU INS UPDATE BINARY command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_UPDATE_BINARY		0xd6

/**
APDU INS READ RECORD command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_READ_RECORD		0xb2		

/**
APDU INS UPDATE RECORD command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_UPDATE_RECORD		0xdc		

/**
APDU INS SEEK command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_SEEK			0xa2		

/**
APDU INS INCREASE command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_INCREASE			0x32		

/**
APDU INS VERIFY CHV command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_VERIFY_CHV		0x20

/**
APDU INS CHANGE CHV command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_CHANGE_CHV		0x24		

/**
APDU INS DISABLE CHV command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_DISABLE_CHV		0x26		

/**
APDU INS ENABLE CHV command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_ENABLE_CHV		0x28		

/**
APDU INS UNBLOCK CHV command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_UNBLOCK_CHV		0x2c		

/**
APDU INS INVALIDATE command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_INVALIDATE		0x04		

/**
APDU INS REHABILITATE command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_REHABILITATE		0x44		

/**
APDU INS RUN GSM ALGORITHM command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_RUN_GSM_ALGORITHM	0x88		

/**
APDU INS GET RESPONSE command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_GET_RESPONSE		0xc0		

/**
APDU INS \ref term_prof command.
\ref cf_etsi_11_11, \ref cf_etsi_11_14
\ingroup api_sim
*/

#define ME_CMD_TERMINAL_PROFILE		0x10		

/**
APDU INS ENVELOPE command.
\ref cf_etsi_11_11, \ref cf_etsi_11_14
\ingroup api_sim
\ref cf_iso_7816
*/

#define ME_CMD_ENVELOPE			0xc2		

/**
APDU INS FETCH command.
\ref cf_etsi_11_11, \ref cf_etsi_11_14
\ingroup api_sim
*/

#define ME_CMD_FETCH			0x12		

/**
APDU INS TERMINAL RESPONSE command.
\ref cf_etsi_11_11, \ref cf_etsi_11_14
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_TERMINAL_RESPONSE	0x14		

/**
APDU INS SLEEP command. Deprecated, do not use.
\ref cf_etsi_11_11, \ref cf_etsi_11_14
\ingroup api_sim
*/

#define ME_CMD_SLEEP			0xfa

/* TIA/EIA-533/41? NAM GSM-AMPS */

/**
APDU INS INTERNAL AUTH command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_INTERNAL_AUTH		0x88

/**
APDU INS AKEY VALIDATION command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_AKEY_VALIDATION		0x86

/**
APDU INS ASK RANDOM command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_ASK_RANDOM		0x8A

/**
APDU INS UPDATE SSD command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_UPDATE_SSD		0x84

/**
APDU INS CONFIRM SSD command.
\ref cf_etsi_11_11
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_CONFIRM_SSD		0x82

/**
APDU INS CMEA ENCRYPT command.
\ref cf_etsi_11_11
\ingroup api_sim
*/

#define ME_CMD_CMEA_ENCRYPT		0x8C

/**
APDU INS MANAGE_CHANNEL command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_MANAGE_CHANNEL		0x70

/**
APDU INS ERASE BINARY command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_ERASE_BINARY		0x0E

/**
APDU INS VERIFY command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_VERIFY			0x20

/**
APDU INS EXTERNAL AUTHENTICATE  command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_EXTERNAL_AUTHENTICARE		0x82

/**
APDU INS GET CHALLENGE command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_GET_CHALLENGE		0x84


/**
APDU INS INTERNAL AUTHENTICATE command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_INTERNAL_AUTHENTICATE		0x88


/**
APDU INS GET_DATA command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_GET_DATA			0xCA

/**
APDU INS WRITE BINARY command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_WRITE_BINARY		0xD0

/**
APDU INS WRITE RECORD command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_WRITE_RECORD		0xD2

/**
APDU INS PUT DATA command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_PUT_DATA			0xDA

/**
APDU INS APPEND RECORD command.
\ref cf_iso_7816
\ingroup api_sim
*/

#define ME_CMD_APPEND_RECORD		0xE2

// CDMA, CSIM

#define ME_CMD_STORE_ESN_MEID_ME	0xDE
//#define ME_CMD_CONFIRM_SSD		0x82
//#define ME_CMD_UPDATE_SSD		0x84
#define ME_CMD_BASE_STATION_CHALLENGE	0x8A
#define ME_CMD_GENERATE_KEY		0x8E
#define ME_CMD_AUTH			0x88

#define ME_CMD_GEN_PUB_KEY		0x50
#define ME_CMD_KEY_GEN_REQ		0x52
#define ME_CMD_COMMIT			0xCC
#define ME_CMD_VALIDATE			0xCE
#define ME_CMD_CONF_REQ			0x54
#define ME_CMD_DOWNLOAD_REQ		0x56
#define ME_CMD_SSPR_CONF_REQ		0xEA
#define ME_CMD_SSPR_DL_REQ		0xEC
#define ME_CMD_OTAPA_REQ		0xEE
#define ME_CMD_PUZL_CONF_REQ		0xF4
#define ME_CMD_PUZL_DL_REQ		0xF6
#define ME_CMD_3GPD_CONF_REQ		0xFC
#define ME_CMD_3GPD_DL_REQ		0x48
#define ME_CMD_SECURE_MODE		0x4A
#define ME_CMD_FRESH			0x4C
#define ME_CMD_SER_KEY_GEN_REQ		0x4E
#define ME_CMD_MMD_CONF_REQ		0xC4
#define ME_CMD_MMD_DL_REQ		0xC6
#define ME_CMD_MMS_CONF_REQ		0x42
#define ME_CMD_MMS_DL_REQ		0x46
#define ME_CMD_SYS_TAG_CONF_REQ		0xC8
#define ME_CMD_SYS_TAG_DL_REQ		0xCA
#define ME_CMD_COMP_IP_AUTH		0x80
#define ME_CMD_BCMCS			0x58
#define ME_CMD_APP_AUTH			0x5A
#define ME_CMD_UMAC_GEN			0x5E
#define ME_CMD_CONFIRM_KEYS		0x5C
#define ME_CMD_S_DAFE_VER_DEC		0x40
#define ME_CMD_TLS_GEN_MASTER_SEC	0x42
#define ME_CMD_TLS_GEN_VERIFY_DATA	0x44
#define ME_CMD_TLS_VER_GEN_KEY_BLOCK	0x46




/**
RFU, do not use.
\ref cf_etsi_11_14
\ingroup api_stk
*/


#define STK_CMD_NULL			0x00

/**
Proactive SIM REFRESH command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_REFRESH			0x01

/**
Proactive SIM MORE TIME command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_MORE_TIME		0x02

/**
Proactive SIM POLL INTERVAL command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_POLL_INTERVAL		0x03

/**
Proactive SIM POLLING OFF command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_POLLING_OFF		0x04

/**
Proactive SIM SET UP EVENT LIST command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SET_UP_EVENT_LIST	0x05

/**
Proactive SIM SET UP CALL command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SET_UP_CALL		0x10

/**
Proactive SIM SEND SS command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SEND_SS			0x11

/**
Proactive SIM SEND USSD command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SEND_USSD		0x12

/**
Proactive SIM SEND SMS command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SEND_SMS		0x13

/**
Proactive SIM SEND DTMF command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SEND_DTMF		0x14

/**
Proactive SIM LAUNCH BROWSER command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_LAUNCH_BROWSER		0x15

/**
Proactive SIM PLAY TONE command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_PLAY_TONE		0x20

/**
Proactive SIM DISPLAY TEXT command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_DISPLAY_TEXT		0x21

/**
Proactive SIM GET INKEY command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_GET_INKEY		0x22

/**
Proactive SIM GET INPUT command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_GET_INPUT		0x23

/**
Proactive SIM SELECT ITEM command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SELECT_ITEM		0x24

/**
Proactive SIM SET UP MENU command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SET_UP_MENU		0x25

/**
Proactive SIM PROVIDE LOCAL INFO command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_PROVIDE_LOCAL_INFO	0x26	

/**
Proactive SIM TIMER MANAGEMENT command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_TIMER_MANAGEMENT	0x27

/**
Proactive SIM SET UP IDLE MODE TEXT command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SET_UP_IDLE_MODE_TEXT	0x28

/**
Proactive SIM PERFORM CARD APDU command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_PERFORM_CARD_APDU	0x30

/**
Proactive SIM POWER ON CARD command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_POWER_ON_CARD		0x31

/**
Proactive SIM POWER OFF CARD command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_POWER_OFF_CARD		0x32

/**
Proactive SIM GET READER STATUS command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_GET_READER_STATUS	0x33

/**
Proactive SIM RUN AT COMMAND command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_RUN_AT_COMMAND		0x34

/**
Proactive SIM LANGUAGE NOTIFICATION command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_LANGUAGE_NOTIFICATION	0x35

/**
Proactive SIM OPEN CHANNEL command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_OPEN_CHANNEL		0x40

/**
Proactive SIM CLOSE CHANNEL command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_CLOSE_CHANNEL		0x41

/**
Proactive SIM RECEIVE DATA command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_RECEIVE_DATA		0x42

/**
Proactive SIM SEND DATA command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_SEND_DATA		0x43

/**
Proactive SIM GET CHANNEL STATUS command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_GET_CHANNEL_STATUS	0x44

/**
RFU, Proactive SIM command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_RESERVER_TIA_EIA_136	0x60

/**
RDU, Proactive SIM command.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define STK_CMD_END_OF_PROACTIVE_SESSION	0x81

/**
ENVELOPE SMS-PP DOWNLOAD Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_ME_TO_SIM_SMS_PP_DOWNLOAD	0xd1

/**
ENVELOPE CB DOWNLOAD Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_ME_TO_SIM_CB_DOWNLOAD		0xd2

/**
ENVELOPE SIM MENU SELECTION Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_ME_TO_SIM_MENU_SELECTION	0xd3

/**
ENVELOPE CALL CONTROLL Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_ME_TO_SIM_CALL_CONTROL		0xd4

/**
ENVELOPE MO SMS CONTROL Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_ME_TO_SIM_MO_SMS_CONTROL	0xd5

/**
ENVELOPE EVENT DOWNLOAD Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_ME_TO_SIM_EVENT_DOWNLOAD	0xd6

/**
ENVELOPE TIMER EXPIRATION Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_ME_TO_SIM_TIMER_EXPIRATION	0xd7

/**
RFU
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_ME_TO_SIM_RESERVED_TIA_EIA_136	0xdf

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define BER_TLV_SIM_TO_ME_PROACTIVE_CMD		0xd0

/**
Command Details Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CMD_DETAILS				0x01

/**
Device Identities Tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_DEVICE_ID				0x02

/**
Result tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_RESULT				0x03

/**
Duration tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_DURATION				0x04

/**
Alpha identifier tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_ALPHA_ID				0x05

/**
Address tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_ADDRESS				0x06

/**
Capability configuration tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CAPABILITY_CONF			0x07

/**
Subaddress tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_SUBADDRESS				0x08

/**
Suplementary services string tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_SS_STRING				0x09

/**
USSD string tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_USSD_STRING				0x0a

/**
SMS TPDU tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_SMS_TPDU				0x0b

/**
Cell broadcast page tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CB_PAGE				0x0c

/**
Text string tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_TEXT_STRING				0x0d	

/**
Tone tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_TONE					0x0e

/**
Item tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_ITEM					0x0f

/**
Item ID tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_ITEM_ID				0x10

/**
Response length tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_RESPONSE_LEN				0x11

/**
File list tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_FILE_LIST				0x12

/**
Location information tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_LOCATION_INFO				0x13

/**
IMEI tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_IMEI					0x14

/**
Help request tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_HELP_REQUEST				0x15

/**
Network Measurement Result tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_NMR					0x16

/**
Default text tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_DEFAULT_TEXT				0x17

/**
Items next axtion indicator tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_ITEMS_NEXT_ACTION_INDICATOR		0x18

/**
Event list tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_EVENT_LIST				0x19

/**
Cause tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CAUSE					0x1a

/**
Location status tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_LOCATION_STATUS			0x1b

/**
Transaction identifier tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_TRANSACTION_ID			0x1c

/**
BCCH channel list tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_BCCH_CHANNEL_LIST			0x1d

/**
Icon ID tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_ICON_ID				0x1e

/**
Item icon ID list tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_ITEM_ICON_ID_LIST			0x1f

/**
Card reader status tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CARD_READER_STATUS			0x20

/**
Card ATR tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CARD_ATR				0x21

/**
C-APDU tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_C_APDU				0x22

/**
R-APDU tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_R_APDU				0x23

/**
Timer ID tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_TIMER_ID				0x24

/**
Timer value tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_TIMER_VALUE				0x25

/**
Date tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_DATE					0x26

/**
Call control requested action tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CALL_CONTROL_REQUESTED_ACTION		0x27

/**
AT command tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_AT_CMD				0x28

/**
AT response tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_AT_RESPONSE				0x29

/**
BC repeat indicator tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_BC_REPEAT_INDICATOR			0x2a

/**
Immediate response tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_IMMEDIATE_RESPONSE			0x2b

/**
DTMF string tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_DTMF_STRING				0x2c

/**
Language tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_LANGUAGE				0x2d

/**
Timing advance tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_TIMING_ADVANCE			0x2e

/**
RFU
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_RESERVED_2F				0x2f

/**
Browser ID tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_BROWSER_ID				0x30

/**
URL tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_URL					0x31

/**
Bearer tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_BEARER				0x32

/**
Provisioning reference file tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_PROVISIONING_REFERENCE_FILE		0x33

/**
Browser termination cause tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_BROWSER_TERMINATION_CAUSE		0x34

/**
Bearer description tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_BEARER_DESCRIPTION			0x35

/**
Channel data tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CHANNEL_DATA				0x36

/**
Channel data length tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CHANNEL_DATA_LEN			0x37

/**
Channel status tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CHANNEL_STATUS			0x38

/**
Buffer size tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_BUFFER_SIZE				0x39

/**
Card reader ID tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_CARD_READER_ID			0x3a

/**
RFU
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_NOT_USED_3B				0x3b

/**
SIM-ME interface transport level tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_SIM_ME_INTERFACE_TRANSPORT_LEVEL	0x3c

/**
RFU
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_NOT_USED_3D				0x3d

/**
Other address tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_OTHER_ADDRESS				0x3e

/**
Network access name tag
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_NETWORK_ACCESS_NAME			0x47

/**
RFU
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_RESERVED_ETIA_136_60			0x60

/**
RFU
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define T_RESERVED_ETIA_136_61			0x61

/**
Comprehension required flag.
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define CR_FLAG					0x80

/** \ingroup api_tsms
DCS 8bit
\ref cf_etsi_03_38
*/

#define DCS_8_BIT_DATA				0x04

/** \ingroup api_tsms
DCS 7bit GSM default alphabet, packed
\ref cf_etsi_03_38
*/

#define DCS_GSM_7_BIT_ALPHA			0x00

/** \ingroup api_tsms
DCS 16bit UCS2
\ref cf_etsi_03_38
*/

#define DCS_UCS2				0x08

/*
#define MC_CLASS_0				0x00
#define MC_CLASS_1				0x01
#define MC_CLASS_2				0x02
#define MC_CLASS_3				0x03
*/

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define DEV_ID_KEYPAD				0x01

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define DEV_ID_DISPLAY				0x02

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define DEV_ID_EARPIECE				0x03

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define DEV_ID_SIM				0x81

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define DEV_ID_ME				0x82

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define DEV_ID_NETWORK				0x83

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_REFRESH_SIM_INIT_FULL_FILE      0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_REFRESH_SIM_FILE_CHANGE         0x01

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_REFRESH_SIM_INIT_FILE_CHANGE    0x02

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_REFRESH_SIM_INIT                0x03

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_REFRESH_SIM_RESET               0x04


/**
\ingroup api_stk
Concat string with localized "Delete" and question mark. I.e. result is "Delete (string)?".
*/

#define Q_YESNO_DELETE_QMARK		0x01

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_YESNO_QMARK			0x02


/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_DISPLAY_TEXT_NORMAL_PRIORITY	0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_DISPLAY_TEXT_HIGH_PRIORITY	0x01

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_DISPLAY_TEXT_DELAY_CLEAR	0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_DISPLAY_TEXT_USER_CLEAR	0x80


/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_SELECT_ITEM_NO_PRESENTATION	0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_SELECT_ITEM_CHOICE		0x01

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_SELECT_ITEM_NAVIGATION	0x03

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_SELECT_ITEM_HELP		0x80



/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INPUT_DIGITS		0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INPUT_ALPHABET		0x01

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INPUT_UCS2		0x02

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INPUT_NO_ECHO		0x04

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INPUT_SMS_PACKED		0x08

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INPUT_HELP		0x80


/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INKEY_DIGIT		0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INKEY_CHAR		0x01

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INKEY_UCS2		0x02

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INKEY_YESNO		0x04

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_GET_INKEY_HELP		0x80


/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_OPEN_CHANNEL_ONDEMAND			0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_OPEN_CHANNEL_IMMEDIATE		0x01

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_OPEN_CHANNEL_AUTOMATIC_RECONNECTION	0x02


/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_SEND_DATA_BUFFERED   			0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_SEND_DATA_IMMEDIATELY			0x01


/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_DISPLAY_ICON_SELF_EXP		0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_DISPLAY_ICON_NOT_SELF_EXP	0x01


/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_LAUNCH_BROWSER_NEW		0x00

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_LAUNCH_BROWSER_REUSE		0x02

/**
\ref cf_etsi_11_14
\ingroup api_stk
*/

#define Q_LAUNCH_BROWSER_REOPEN		0x03

/**
\ingroup api_sim
*/

#define DIR_SIM_TO                      0x00

/**
\ingroup api_sim
*/

#define DIR_SIM_FROM                    0x01

/**
\ingroup api_sim
*/

#define DIR_SIM_DEFAULT                 0x02

#ifdef __cplusplus
}
#endif

#endif
