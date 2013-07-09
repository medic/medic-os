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

#ifndef _TURBO_PROC_H_
#define _TURBO_PROC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
\ingroup api_proc
\return version of boot loader
*/

extern u8 version_loader(void);

/**
\ingroup api_proc
\return version of hardware: #TURBO_SIM_TOOLKIT_ADAPTER, #TURBO_PROGRAMMER, #TURBO_PROGRAMMER_2, #BRUM_NBX_24, #TURBO_SIM_TOOLKIT_ADAPTER_EVO, #TURBO_SIM_TOOLKIT_ADAPTER_BRA, #BRUM_NBX_24_USB
*/

extern u8 version_hwd(void);

/**
\ingroup api_proc
\return version of kernel
*/

extern u32 version_turbo(void);

/**
\ingroup api_proc
\return serial number - unique per turbo
*/

extern void serial_number(u8 buf[14]);

/** \ingroup api_proc 
\param x 	can be: #PROC_8_LANGUAGE, #PROC_8_DISPLAY_TEXT_LEN, #PROC_8_CONFIG_FAKE_PHONEBOOK, #PROC_8_CONFIG_IMG, #PROC_8_MT_FETCH, #PROC_8_CONFIG_LAUNCH_BROWSER, #PROC_8_CONFIG_NETWORKING, #PROC_8_CONFIG_NR_CHANNELS, #PROC_8_CONFIG_TIMER, #PROC_8_CONFIG_ALPHA, #PROC_8_CONFIG_PREF_ITEM, #PROC_8_CONFIG_BINARY_CHOICE, #PROC_8_CARD_PHASE, #PROC_8_ME_SPEED, #PROC_8_CLOCK_SETTING, #PROC_8_SEQS, #PROC_8_CONFIG_CABLE, #PROC_8_CONFIG_SETUP, #PROC_8_CONFIG_SIM_FIRST, #PROC_8_CONFIG_INIT_BOOSTER.

\return configuration and system info information
*/

extern u8 proc_8(u8 x);

/** \ingroup api_proc 
\param x 	can be: #PROC_16_STATUS_COUNT, #PROC_16_NR_FLASHINGS,#PROC_16_FREE_PROGMEM, #PROC_16_MAX_PROGMEM, #PROC_16_FREE_RAM, #PROC_16_MAX_RAM, #PROC_16_FREE_EEPROM, #PROC_16_MAX_EEPROM, #PROC_16_FREE_MALLOC, #PROC_16_MAX_MALLOC.
\return returns system info information
*/

extern u16 proc_16(u8 x);

/** \ingroup api_proc 
modify system values
\param x 	can be: #PROC_8_LANGUAGE, #PROC_8_DISPLAY_TEXT_LEN, #PROC_8_CONFIG_FAKE_PHONEBOOK, #PROC_8_CONFIG_IMG, #PROC_8_MT_FETCH, #PROC_8_CONFIG_ALPHA, #PROC_8_CONFIG_PREF_ITEM, #PROC_8_CONFIG_BINARY_CHOICE, #PROC_8_SEQS, #PROC_8_CONFIG_SETUP, #PROC_8_CONFIG_SIM_FIRST, #PROC_8_CONFIG_INIT_BOOSTER.
\param val 	new value.
*/

extern void set_proc_8(u8 x, u8 val);

/**
\ingroup api_proc
Used language.
\return language
*/


#define PROC_8_LANGUAGE				0x01

/**
\ingroup api_proc
Display length.
\return u8 length
*/

#define PROC_8_DISPLAY_TEXT_LEN			0x02

/**
\ingroup api_proc
Usage of fake phonebook compiled in kernel.
\return 0/1.
*/

#define PROC_8_CONFIG_FAKE_PHONEBOOK		0x03

/**
\ingroup api_proc
Image support compliled in kernel.
\return 0/1.
*/

#define PROC_8_CONFIG_IMG			0x04

/**
\ingroup api_proc
Multi-thread FETCH setting.
\return 0/1.
*/

#define PROC_8_MT_FETCH				0x05

/**
\ingroup api_proc
launch_browser() compiled in kernel.
\return 0/1.
*/

#define PROC_8_CONFIG_LAUNCH_BROWSER		0x06

/**
\ingroup api_proc
Networking support compiled in kernel.
\return 0/1.
*/

#define PROC_8_CONFIG_NETWORKING		0x07

/**
\ingroup api_proc
Number of networking channels supported by kernel.
\return number of channels
*/

#define PROC_8_CONFIG_NR_CHANNELS		0x08

/**
\ingroup api_proc
Timer support compiled in kernel.
\return 0/1.
*/

#define PROC_8_CONFIG_TIMER			0x09

/**
\ingroup api_proc
Number of timers supported by kernel.
\return 0/1.
*/

#define PROC_8_CONFIG_NR_TIMERS			0x0a

/**
\ingroup api_proc
TAG_ALPHA used by STK functions. 
\return 0/1.
*/

#define PROC_8_CONFIG_ALPHA			0x0b

/**
\ingroup api_proc
TAG_PREFERRED_ITEM used by spider. 
\return 0/1.
*/

#define PROC_8_CONFIG_PREF_ITEM			0x0c

/**
\ingroup api_proc
get_inkey() binary choice used by yesno().
\return 0/1.
*/

#define PROC_8_CONFIG_BINARY_CHOICE		0x0d

/**
\ingroup api_proc
\return SIM EF_PHASE
*/

#define PROC_8_CARD_PHASE			0x0e

/**
\ingroup api_proc
ME-TURBO (and TURBO-SIM) UART speed. 0 means default f/372 speed, 1 means f/64.
*/

#define PROC_8_ME_SPEED				0x0f

/**
\ingroup api_proc
SIM DF clock stopping. Or'ed 0xF0 means that ME does not request this value
via STATUS command (bad for resetting for idle task).
\return 0/1
*/

#define PROC_8_CLOCK_SETTING			0x10

/**
\ingroup api_proc
Max. number of Turbo SMS sequence numbers (questions) kept by kernel until answers
received.
\return number of sequence numbers kept
*/

#define PROC_8_SEQS         			0x11

/**
\ingroup api_proc
Cable interface compiled in kernel.
\return 0/1.
*/

#define PROC_8_CONFIG_CABLE			0x12
	
/**
\ingroup api_proc
Number of \ref sim_sst bytes requested by ME. This can indicate services
support of ME.
\return number of bytes
*/

#define PROC_8_ME_SST   			0x13

/**
\ingroup api_proc
Display "Setup" menu item. You have to do set_up_menu() to recreate menu.
\return 0/1.
*/

#define PROC_8_CONFIG_SETUP   			0x14

/**
\ingroup api_proc
SIM applications menu first/last. You have to do set_up_menu() to recreate menu.

\return 0/1.
*/

#define PROC_8_CONFIG_SIM_FIRST 		0x15

/**
\ingroup api_proc
If application init #ACTION_APP_INIT action is to be done in first APDU or later after PIN check. Warning: if set then cannot be reset with SIM PIN and possible recovery has to be done with firmware reflash - Turbo Programmer is needed.

\return 0/1.
*/

#define PROC_8_CONFIG_INIT_BOOSTER 		0x16



/**
\ingroup api_proc
Number of STATUS commands since power on - u16 so will overflow in about 22
days (with 1 STATUS every 30 sec).
*/

#define PROC_16_STATUS_COUNT			0x01

/**
\ingroup api_proc
Number of PROGMEM application flashings. Set to zero by firmware reflash.
*/
#define PROC_16_NR_FLASHINGS			0x02


/**
\ingroup api_proc
Total free PROGMEM (can be fragmented - for max. fragment use #PROC_16_MAX_PROGMEM).
*/

#define PROC_16_FREE_PROGMEM			0x03

/**
\ingroup api_proc
Max. PROGMEM fragment.
*/

#define PROC_16_MAX_PROGMEM			0x04

/**
\ingroup api_proc
Total free RAM (can be fragmented - for max. fragment use #PROC_16_MAX_RAM).
*/

#define PROC_16_FREE_RAM			0x05

/**
\ingroup api_proc
Max. RAM fragment.
*/

#define PROC_16_MAX_RAM				0x06

/**
\ingroup api_proc
Total free EEPROM  (can be fragmented - for max. fragment use #PROC_16_MAX_EEPROM).
*/

#define PROC_16_FREE_EEPROM			0x07

/**
\ingroup api_proc
Max. EEPROM emalloc() chunk.
*/

#define PROC_16_MAX_EEPROM			0x08

/**
\ingroup api_proc
Total free MALLOC heap (can be fragmented - for max. fragment use #PROC_16_MAX_MALLOC).
*/

#define PROC_16_FREE_MALLOC			0x09

/**
\ingroup api_proc
Max. malloc() chunk.
*/

#define PROC_16_MAX_MALLOC			0x0a

#ifdef __cplusplus
}
#endif

#endif
