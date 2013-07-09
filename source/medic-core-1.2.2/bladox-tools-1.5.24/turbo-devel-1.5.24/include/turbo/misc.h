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

#ifndef _MISC_H_
#define _MISC_H_

#ifdef __cplusplus
extern "C" {
#endif


/** Read byte from memory.
\ingroup api_mem
\param addr EEPROM, PROGMEM, RAM memory address
*/
extern u8 rb(const void *addr);

/** Write byte to memory.
\ingroup api_mem
\param addr EEPROM, RAM memory address
\param val value
*/
extern void wb(void *addr, u8 val);

/** Read word from memory.
\ingroup api_mem
\param addr EEPROM, PROGMEM, RAM memory address
*/
extern u16 rw(const void *addr);

/** Write word to memory.
\ingroup api_mem
\param addr EEPROM, RAM memory address
\param val value.
*/
extern void ww(void *addr, u16 val);

/** 
Checks whether \c dcs is 7b coding or not, ref. \ref cf_etsi_03_38.
\ingroup api_tsms
\param dcs	
\return 	0 if not 7b, 1 if 7b coding
*/

extern u8 dcs_is_7b(u8 dcs);

/** 
Returns number of septets fitting into \c len bytes (i.e. len*8/7).
\ingroup api_tsms
\param len	len in bytes (u8)
\return len_7b 	number of septets fitting the \c len bytes
*/

extern u8 dcs_7b_len(u8 len);

/** 
Converts packed 7b to/from unpacked 8b coding.
\ingroup api_tsms
\param buf	pointer on buffer where input/output is stored. For 7b to 8b you
must guarantee that there is enough space for \c nr_chars.
\param nr_chars number of chars (either in 7b or 8b coding)
\param from_to  either #DCS_7_TO_8 or #DCS_8_TO_7

\return 	number of valid bytes in buffer.
*/

extern u8 dcs_78(u8 * buf, u8 nr_chars, u8 from_to);

/** sim_sst()
Returns content of EF_\ref sim_sst \ref cf_etsi_11_11.
First byte is the length of the following array.

\ingroup api_sim
*/

extern const u8 * sim_sst(void);

/** sim_atr()
Returns SIM ATR, ref. \ref cf_iso, \ref cf_etsi_11_11.
First byte is the length of the following array.

\ingroup api_sim
*/

extern const u8 * sim_atr(void);

/** 
This function can be used if appliaction has #ACTION_SMS registered.
In the case that SMS is received and the #ACTION_SMS invoked the incoming 
SMS is not stored to SIM by default (it is up to the application to store
it to SIM).
Calling this function in the #ACTION_SMS handling code causes that SIM
is stored to SIM.
\ingroup api_sim
\sa #ACTION_SMS
*/

extern void store_sms_2_sim(void);

/**
Try to find \c msisdn in the SIM EF_ADN (\ref cf_etsi_11_11) phonebook 
and return name (=alpha) if found.
\ingroup api_msisdn
\attention You have to free() the result once not needed.
\param msisdn		MSISDN to search for, pointer to RAM only
\param msisdn_type	type of MSISDN, either #MSISDN_ADN or #MSISDN_SMS
\return         a character pointer to either alpha or msisdn2str, NULL if malloc error
\sa adn_ready()
*/

extern u8 * get_adn(u8 * msisdn, u8 msisdn_type);

/**
Seek tag (element) in TPDU, i.e. SMS with all elements, \ref cf_etsi_03_38.
\ingroup api_tsms
\param s	pointer of TPDU	(RAM only)
\param tag	element to seek, can be #T_SMS_PID, #T_SMS_DCS, #T_SMS_SCTS, #T_SMS_UDL, #T_SMS_UD, #T_SMS_MR, #T_SMS_VP, #T_SMS_DT, #T_SMS_ST, #T_SMS_AA

\return	NULL if not found, pointer on element if found
*/

extern u8 * tpdu_seek(u8 *s, u8 tag);

/** 
Return value of bit in the array.
\ingroup api_mem
\param buf	pointer on byte array (RAM only)
\param bit

\return 0 or 1.
\sa set_bit()
*/

extern u8 get_bit(u8 * buf, u16 bit);

/** 
Set bit in the array.
\ingroup api_mem
\param buf	pointer on byte array (RAM only)
\param bit	
\param val	0 or 1
\sa get_bit()
*/

extern void set_bit(u8 * buf, u16 bit, u8 val);

/** 
Find a TAG in BER-TLV array. First byte is the overall length.
\ingroup api_stk
\param s	BER-TLV coded element array, RAM only
\param tag	desired TAG

\return offset in s TAG if found, 0 if tag not found otherwise.
*/

extern u8 get_tag(u8 *s, u8 tag);

/**
EF_ADN alpha len, \ref cf_etsi_11_11.
\ingroup api_msisdn
*/

extern u8 adn_len(void);

/**
Checks if all EF_ADN records were read by ME (i.e. Turbo knows them) and
get_adn() returns valid value.
\ingroup api_sim
*/

extern u8 adn_ready(void);

/** 
Used for registration of Turbo SMS application specific tag.
\ingroup api_tsms
\param text	name of application shown user in Turbo SMS access control
(can be from RAM, EEPROM, PROGMEM)
\param mime	application specific Turbo SMS type (can be from RAM, EEPROM, PROGMEM)
\param type	type of Turbo SMS, #SMS_TYPE_MSG or #SMS_TYPE_QA

\return #NO_ERROR or #ERR_NO_EEPROM
*/

extern u8 reg_sms_tag(const u8 * text, const u8 * mime, u8 type);

/**
\ingroup api_tsms
Returns the Turbo SMS Access Control List for requested tag (mime). NULL if not found. The ACL is stored in EEPROM.

\param mime	mime string
\param type	#SMS_TYPE_MSG or #SMS_TYPE_QA
\result		ACL struct stored in EEPROM or NULL if not found
\sa #TSMS_ACL_BLOCKED, #TSMS_ACL_EVERYONE
*/

extern TSMS_ACL * lookup_tsms_tag(const u8 * mime, u8 type);

/**
Converts MSISDN string to MSISDN in either #MSISDN_ADN or #MSISDN_SMS formats. 
\ingroup api_msisdn
\attention You have to free() (or efree()) the result string once not needed.
\param str 	source string - MSISDN in string format, e.g. "606123456" or "+420606123456", can be from RAM, EEPROM, PROGMEM
\param type 	the type resulting MSISDN, either #MSISDN_ADN or #MSISDN_SMS
\param to_src 	resulting memory, #MEM_R indicates that result is malloc'ed, #MEM_E is emalloc'ed

\return 	a character pointer, NULL if failed.
\sa msisdn2str()
*/

extern u8 * str2msisdn(const u8 * str, u8 type, u8 to_src);

/** 
\ingroup api_msisdn
Converts MSISDN in either #MSISDN_ADN or #MSISDN_SMS formats to string. 
\attention You have to free() (or efree()) the result once not needed.
\param msisdn 		source MSISDN in either #MSISDN_ADN or #MSISDN_SMS, can be from RAM, EEPROM, PROGMEM
\param msisdn_type 	the type  MSISDN, either #MSISDN_ADN or #MSISDN_SMS
\param to_src 		resulting memory, #MEM_R indicates that result is malloc'ed, #MEM_E is emalloc'ed

\return 	a character pointer, NULL if failed.
\sa str2msisdn()
*/

extern u8 * msisdn2str(const u8 * msisdn, u8 msisdn_type, u8 to_src);

/** 
Compare two MSISDNs, each can be in either #MSISDN_ADN or #MSISDN_SMS formats.
\ingroup api_msisdn

\param a 	pointer on MSISDN_A, can be from RAM, EEPROM, PROGMEM
\param a_type	type of MSISDN_A, can be #MSISDN_ADN or #MSISDN_SMS
\param b	pointer on MSISDN_B, can be from RAM, EEPROM, PROGMEM
\param b_type 	type of MSISDN_B, can be #MSISDN_ADN or #MSISDN_SMS
\return 	1 if equal, 0 if not equal.
*/

extern u8 msisdncmp(const u8 * a, u8 a_type, const u8 *b, u8 b_type);

/** 
\ingroup api_msisdn
Make a copy of MSISDN.
\attention You have to free() (efree()) the result once not needed.
\param msisdn		pointer on source MSISDN, can be from RAM, EEPROM, PROGMEM
\param type		type of MSISDN, can be #MSISDN_ADN or #MSISDN_SMS
\param to_src		resulting memory, #MEM_R indicates that result is malloc'ed, #MEM_E is emalloc'ed
\return         	a character pointer, NULL if failed.
*/

extern u8 * msisdncpy(const u8 *msisdn, u8 type, u8 to_src);

/** 
Swap half-bytes, e.g. 0x10 = swap_nibble(0x01),
\ingroup api_mem
\param x
*/

extern u8 swap_nibble(u8 x);

/** 
\ingroup api_i18n
Returns string from lc_char array following the user selected language.
If not found the first string is returned.

\code
lc_char PROGMEM lc_My_Text[]={
        LC_EN("My Text")
        LC_CZ("Muj Text")
        LC_END
};

foo()
{
u8 *text = locale(lc_My_Text);
...
}
\endcode

\param str	pointer on lc_char array. Can be from PROGMEM only
*/

extern u8 * locale(const lc_char * str);

/** 
\ingroup api_i18n
Localized list - based on user preferred language desired language list
is used.

\code
#ifdef CONFIG_LANG_EN
u8 PROGMEM t_yes_en[]="Yes";
u8 PROGMEM t_no_en[]="No";

const u8 * PROGMEM l_yesno_en[]=
{
                t_yes_en,
                t_no_en
};
#endif

#ifdef CONFIG_LANG_CZ
u8 PROGMEM t_yes_cz[]="Ano";
u8 PROGMEM t_no_cz[]="Ne";

const u8 * PROGMEM l_yesno_cz[]=
{
                t_yes_cz,
                t_no_cz
};
#endif

lc_list PROGMEM lc_YesNo[]={
        LC_EN_LIST(l_yesno_en)
        LC_CZ_LIST(l_yesno_cz)
        LC_END_LIST
};

foo()
{
select_item (2, locale_list(lc_YesNo), ...);
}
\endcode

\param list pointer on lc_list array. Can be from PROGMEM only.	
\sa select_item()
*/

extern u8 * locale_list(const lc_list *list);

/**
\ingroup api_tsms
Returns user configurable Turbo SMS (or Data Download in general) PID.
Usually 0x7f but can be also 0x00 if operators limits the 0x7f usage.
\return 	user preferred TSMS PID.
*/

extern u8 tsms_pid(void);

/**
\ingroup api_tsms
Returns preferred Turbo SMS DCS related to tsms_pid(). It is 0xF6.
\return 	TSMS DCS.
*/

extern u8 tsms_dcs(void);

/** 
Register action.
\ingroup api_actions
\param action
\return #NO_ERROR or #ERR_RESOURCE_BUSY (some actions can be registered by only one appliacation).
\sa set_up_event_list(), unreg_action()
*/

extern u8 reg_action(u8 action);

/** 
\ingroup api_fs
Associate file type, i.e. 3 letter extension/magic with a given 
file callback function.

Any application can obtain the \c cb with file_type() and work with
the file.

The following extensions are registered by kernel:
- trb - turbo application
- txt - text file

<b>Example scenario.</b>

We have a file containing SMS specific format message, which
viewed as text would not be readable. We have one application (e.g. 
SMSM Archive) that knows
how to display such message and the second application (e.g. memory card viewer)
viewing the message.

<b>Application that knows the file (e.g. SMS Archive)</b>
\code
u8 PROGMEM t_ext_sms[] = "sms";

u8 file_sms_cb (u32 f, u8 action)
{
  if (action == FILE_SELECT)	// for file action SELECT
  {
    return view_msg (f);	// decode sms format and display text
  }
return APP_USE_DEFAULT;		// for any other file actions use default
}

void turbo_handler (u8 action, void *data)
{
  switch (action)
    {
    ....
        case ACTION_APP_REGISTER:
		reg_file_type (t_ext_sms, file_sms_cb);
    ....	
    }
}
\endcode

<b>Application that wants to view the file</b>
\code
t_file_cb *cb = file_type (f);
u8 j;

j = APP_USE_DEFAULT;
if (cb != NULL) 
	j = cb (f, FILE_SELECT);
	
if (j == APP_USE_DEFAULT)
	display_more(f);
\endcode

\param type	pointer on type in PROGMEM	
\param cb 	callback function

\return #NO_ERROR, #ERR_NO_EEPROM.
\sa file_type()
*/

extern u8 reg_file_type(const u8 * type, t_file_cb * cb);

/** 
\ingroup api_stk
\stk
Change EF_IMSI procedure according the \ref cf_etsi_11_11.
*/

extern void change_imsi(void);

/** 
\ingroup api_sim
Register SIM file.
\code
u8 PROGMEM ef_sms_path[] = { 0x3f, 0x00, 0x7f, 0x10, 0x6f, 0x3c };

... (usually ACTION_APP_INIT handling code)
reg_file (ef_sms_path, 3);
...
\endcode

\param file	Pointer on path, PROGMEM only.
\param len	Number of elementes in path.

\return #NO_ERROR or #ERR_NO_RAM, #ERR_RESOURCE_BUSY if error.
\sa unreg_file()
*/

extern u8 reg_file(const u16 * file, u8 len);

/**
\ingroup api_sim
Unregister SIM file.
\param file	Pointer on path, PROGMEM only.
\param len	Number of elementes in path.
\sa reg_file()
*/

extern void unreg_file(const u16 * file, u8 len);

/** 
\ingroup api_actions
Unregister action.
\param action
\sa reg_action()
*/

extern void unreg_action(u8 action);

/** 
\ingroup api_actions
Spawn new SIM Toolkit thread. The new thread applies the function 
start_routine passing it arg as first argument.
\param start_routine
\param arg
\return #NO_ERROR, #ERR_NO_RAM
*/

extern u8 stk_thread(void (*start_routine)(void *),void * arg);

/** 
Append string to buffer.
\ingroup api_misc
\param buf	pointer on buffer, RAM only.
\param str	pointer on string, can be from RAM, EEPROM, PROGMEM.
\return pointer at the new end of buffer.
*/

extern u8 * sprints(u8 *buf, const u8 *str);

/** 
Convert integer to string and append it to the buffer.
\ingroup api_misc
\param buf	pointer on buffer, RAM only.
\param val	integer.
\return pointer at the new end of buffer.
*/

extern u8 * sprinti(u8 *buf, u32 val);

/** 
Append char to buffer.
\ingroup api_misc
\param buf	pointer on buffer, RAM only.
\param c	char.
\return pointer at the new end of buffer.
*/

extern u8 * sprintc(u8 *buf, u8 c);

/** 
Append char as hex to buffer.
\ingroup api_misc
\param buf	pointer on buffer, RAM only.
\param c	char.
\return pointer at the new end of buffer.
*/

extern u8 * sprintch(u8 *buf, u8 c);

/** 
Prepare for UCS2 string, make #STRING_SLV head.

For UCS2 strings the text has to have special header, \ref cf_etsi_11_11.
This functions also informs kernel that sprintc() and sprints() should
respect UCS2 coding.

If you plan to use UCS2 strings then use something like this:

\code
u8 *start_ucs2 (u8 * buf)
{
  if (proc_8 (PROC_8_LANGUAGE) == LC_RUSSIAN)
    buf = sprintucs2_start (buf, DCS_UCS2);

  return buf;
}

void foo()
{
u8 *buf = buf_B ();
u8 *r = buf;

r = start_ucs2 (r);
r = sprints (r, t_foo);
r = end_ucs2 (r);

display_text (buf, NULL);
}

\endcode

\ingroup api_misc
\param buf      pointer on buffer, RAM only
\param dcs      DCS
\return pointer at the new end of buffer
\sa sprintucs2_end()
*/

extern u8 * sprintucs2_start(u8 *buf, u8 dcs);

/** 
Close UCS2 string.

\code
u8 *end_ucs2 (u8 * buf)
{
  if (proc_8 (PROC_8_LANGUAGE) == LC_RUSSIAN)
    sprintucs2_end ();
  else
    buf = sprintc (buf, '\0');
  return buf;
}
\sa sprintucs2_start()
\endcode

\ingroup api_misc
\return pointer at the new end of buffer.
*/

extern void sprintucs2_end(void);

/* 
TODO
\ingroup api_stk
\stk
\param val
*/

extern void retval(u16 val);

/**
STK thread buffer. In this buffer STK command is composed, waits for FETCH
and TERMINAL RESPONSE is stored.
\attention Can be used only in stk_thread().
\ingroup api_mem
*/

extern u8 * buf_A(void);

/** 
General GSM buffer. All the ME<->TURBO<->SIM communication happens here.
\attention Can be modified only in stk_thread() and its content is not valid
after STK command.
\ingroup api_mem
*/

extern u8 * buf_B(void);

/**
Initiate idle malloc heap. It is freed automaticaly when idle task is aborted. 
\ingroup api_idle_task
*/

extern void init_imalloc(void);

/**
Informs if you are in idle task.
\ingroup api_idle_task
*/

extern u8 is_idle(void);

/**
\ingroup api_actions
Wait for ACTION and display alpha text meanwhile. If alpha text is NULL 
more_time() is used and no text is displayed. 

The number of repeatings says how many times display_text() or more_time()
is fetched.
\stk
\param action	#ACTION_EVENT_DATA_AVAILABLE
\param alpha	pointer on ALPHA text, can be from RAM, EEPROM, PROGMEM
\param nr_rep 	number of repeats to display alpha

\return 	ENVELOPE or TERMINAL RESPONSE in buf_A().
*/

extern u8 * wait_for_action(u8 action, const u8 * alpha, u8 nr_rep);

/** 
Returns ME's \ref term_prof. First byte contains length.
\ingroup api_stk
*/

extern const u8 * terminal_profile(void);

/** 
Send command to SIM and receive answer. GSM class only - 0xA0.
\ingroup api_sim
\param ins
\param p1
\param p2
\param p3
\param buf
\return stored in buf
*/

extern u16 sim(u8 ins, u8 p1, u8 p2, u8 p3, u8 * buf);

/** 
Send command to SIM and receive answer.
\ingroup api_sim
\param cla
\param ins
\param p1
\param p2
\param p3
\param buf
\return number of bytes stored in buf
*/

extern u16 simx(u8 cla, u8 ins, u8 p1, u8 p2, u8 p3, u8 * buf);

/**
Send command to SIM and receive answer. Direction control for non-standard INS.
\ingroup api_sim
\param cla
\param ins
\param p1
\param p2
\param p3
\param buf
\param dir      DIR_SIM_FROM, DIR_SIM_TO, DIR_SIM_DEFAULT
\return number of bytes stored in buf
*/

extern u16 simx_d(u8 cla, u8 ins, u8 p1, u8 p2, u8 p3, u8 * buf, u8 dir);


/** 
Same as malloc but in EEPROM.
\ingroup api_mem
\param size
*/

extern void * emalloc(u16 size);

/** 
Same as malloc but in temporary idle task memory. 
\ingroup api_idle_task
\param size
*/

extern void * imalloc(u16 size);

/** 
Free emalloc'ed chunk. 
\ingroup api_mem
\param ptr
*/

extern void efree(void * ptr);

/** 
Free imalloc'ed chunk. 
\ingroup api_idle_task
\param ptr
*/

extern void ifree(void * ptr);

/**
Compose SIM Toolkit command stored in buf_A() and wave for FETCH.
\ingroup api_stk
\stk
\param cmd	command \ref cf_etsi_11_14
\param q	command qualifier \ref cf_etsi_11_14
\param to_id	TO device identities \ref cf_etsi_11_14
\param len	length of data in buf_A().
\return pointer of buffer (=buf_A()) containing TERMINAL RESPONSE.
*/

extern u8 * stk_cmd(u8 cmd, u8 q, u8 to_id, u8 len);

/** 
Store (in EEPROM) pointer on application persistent data.  
\ingroup api_mem
\param data
*/

extern void reg_app_data(void * data);

/** 
Return pointer on application persistent data.
\ingroup api_mem
*/

extern void * app_data(void);

/** 
Create question Turbo SMS header.
\ingroup api_tsms
\param buf	pointer on buffer to create buffer in.
\param mime	pointer on application specificic header tag, stored in RAM, EEPROM or PROGMEM.
\param msisdn	pointer on MSISDN, stored in RAM, EEPROM or PROGMEM.
\param msisdn_type	MSISDN type, #MSISDN_ADN, #MSISDN_SMS.
\return 	pointer in buffer pointing after the header.
*/

extern u8 * create_head_q(u8 *buf, const u8 * mime, const u8 * msisdn, u8 msisdn_type);

/** 
Create \b answer Turbo SMS header based on received \b question Turbo SMS.
\ingroup api_tsms
\param buf	pointer on buffer to create answer head in.
\param msg	pointer on question Turbo SMS.
\return 	pointer to buffer pointing after the header.
*/

extern u8 * create_head_a(u8 *buf, u8 * msg);

/** 
Create message Turbo SMS header.
\ingroup api_tsms
\param buf	pointer on buffer to create answer head in.
\param mime	application specific header indicator (type), can be stored in EEPROM, RAM or PROGMEM.
\return 	pointer in buffer pointing after the header.
*/

extern u8 * create_head_msg(u8 *buf, const u8 * mime);

/** 
Skip Turbo SMS header and point on data.
\ingroup api_tsms
\param msg	pointer on (Turbo) SMS, RAM only.
\return		pointer on data, if not Turbo SMS then on UDL element.
*/

extern u8 * skip_head(u8 * msg);

/**
Returns random 8 bytes. The random number generator is influenced by several variables: previous random, 8bit timer running permanently, EF_BCCH, EF_LOCINFO (random TMSI), random input for GSM algorithm. Everything together is randomly twofished.
\ingroup api_security
\param buf
*/

extern void xrand(u8 buf[8]);

/** 
SIM SELECT. 
\attention once ready with SIM use select(0,0) to reset SIM to the state konwn by ME
\ingroup api_sim
\param path	MEM_R
\param path_len
*/

extern u16 select(u8 * path, u8 path_len);

/**
SIM READ BINARY.
\ingroup api_sim
\param path	MEM_R
\param path_len
\param buf
\param len
*/

extern u16 read_binary(u8 * path, u8 path_len, u8 *buf, u8 len);

/** STK REFRESH EF_SMS (=3F00:7F10:6F3C).
\ingroup api_stk
\stk
*/

extern void refresh_sms(void);


/** 
\ingroup api_prot_mem
Delete protected block \c id from loader memory.
\param id	block id
\return #NO_ERROR, #ERR_PROT_MEM_WRONG_BLOCK, #ERR_NO_RAM
*/

extern u8 prot_mem_delete(u8 id);

/** 
\ingroup api_prot_mem
Create new protected memory block in loader memory.
\param attr 	allowed operation on block #PROT_MEM_READ|#PROT_MEM_ENCRYPT|#PROT_MEM_DECRYPT
\param block	content of te block to be written into flash loader memory
\return >=0 block number, -#ERR_NO_PROT_MEM, -#ERR_NO_RAM
*/

extern b8 prot_mem_make_block(u8 attr, u8 block[16]);

/** 
\ingroup api_prot_mem
Read protected memory block.
\param id	block id
\param out 	content of the block (if successful)
\return #NO_ERROR, #ERR_PROT_MEM_WRONG_BLOCK, #ERR_PROT_MEM_FORBIDDEN
*/

extern u8 prot_mem_read(u8 id, u8 out[16]);

/**
\ingroup api_prot_mem
Use protected memory block as the 128bit twofish key and encrypt \c in to \c out.
\param id	block id
\param in	plain text
\param out	cipher text
\return #NO_ERROR, #ERR_PROT_MEM_WRONG_BLOCK, #ERR_PROT_MEM_FORBIDDEN, #ERR_NO_RAM
*/

extern u8 prot_mem_encrypt(u8 id, u8 in[16], u8 out[16]);

/** 
\ingroup api_prot_mem
Use protected memory block as the 128bit twofish key and encrypt \c in to \c out.
\param id	block id
\param in	cipher text
\param out	plain text
\return #NO_ERROR, #ERR_PROT_MEM_WRONG_BLOCK, #ERR_PROT_MEM_FORBIDDEN, #ERR_NO_RAM
*/

extern u8 prot_mem_decrypt(u8 id, u8 in[16], u8 out[16]);

/** 
Returns file callback for given file
\ingroup api_fs
\param file_id
\return callback or NULL if none is associated to the type of given file.
*/

extern t_file_cb * file_type(u32 file_id);

/**
Copy data from PROGMEM to RAM and clear BSS.
\ingroup api_mem
*/

extern void _init(void);

/**
\ingroup api_mem
Register shared memory key - used for finding/denoting shared
memory between application.

\param key number/byte denoting the shared memory
\param pointer store pointer to be associated with key
\result #NO_ERROR or #ERR_UNKNOWN if cannot be created
*/

extern u8 shmem_reg_key(u8 key, void * pointer);

/**
\ingroup api_mem
Find pointer associated with key. 
\param key look for given key
\result pointer or NULL
*/

extern void * shmem_key(u8 key);

/** 
\ingroup api_misc
Delay microseconds.

\note It is "nop" instruction based and not calibrated. Depends on freq provided by ME.

\param usec
*/

void delayUs(u16 usec);

/** 
\ingroup api_misc
Delay miliseconds.

\note It is "nop" instruction based and not calibrated. Depends on freq provided by ME.

\param msec
*/

void delayMs(u16 msec);

/** 
\ingroup api_sim
Register Logical Channel.
\code

... (usually ACTION_APP_INIT handling code)
reg_channel (WANTED_CHANNEL);
...
\endcode

\param 	channel_nr Logical Channel number, 0-3 according to ISO 7816,
in case of 0 you have to returned assigned channel

\return #NO_ERROR or #ERR_RESOURCE_BUSY if error.
\sa unreg_channel()
\sa apdu_class()
*/

extern u8 reg_channel(u8 channel_nr);

/**
\ingroup api_sim
Unregister Logical Channel.
\param channel_nr Logical Channel number
\sa reg_channel()
*/

extern void unreg_channel(u8 channel_nr);

/**
\ingroup api_sim
Returns class byte of current APDU. It fixes shortsighted
design of File_apdu_data, where this is missing.
Neccessary esp. for #ACTION_LOGICAL_CHANNEL.

\return Class byte of current APDU.
*/

extern u8 apdu_class();

/**
\ingroup api_ipc

\return pointer on name of actual application
*/

extern u8 * app_name();

/**
\ingroup api_ipc

\return 
*/

extern void call_app(u8 * application_name, void *data);


/** 
\ingroup api_sim
Register AID file.
\code
u8 PROGMEM t_aid[] = { 0xA0, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43 };

... (usually ACTION_APP_INIT handling code)
reg_aid (t_aid, sizeof(t_aid));
...
\endcode

\param file	Pointer on path, PROGMEM only.
\param len	Number of bytes.

\return #NO_ERROR or #ERR_NO_RAM, #ERR_RESOURCE_BUSY if error.
\sa unreg_aid()
*/

extern u8 reg_aid(const u8 * file, u8 len);

/**
\ingroup api_sim
Unregister AID file.
\param file	Pointer on path, PROGMEM only.
\param len	Number of bytes.
\sa reg_aid()
*/

extern void unreg_aid(const u8 * file, u8 len);

/**
\ingroup api_mem
Write 256 bytes to PROGMEM (flash) memory. 
\param to_progmem	Pointer on PROGMEM, has to be PROGMEM page bounded
\param from		Pointer on RAM buffer, size 256 bytes	
\return #NO_ERROR or #ERR_NO_PROGMEM if error
*/

extern u8 progmem_write(u8 * progmem_ptr, u8 *from);

#ifdef __cplusplus
}
#endif


#endif
