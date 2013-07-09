/*
 * Copyright (c) 2004 BLADOX, s.r.o.
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

#ifndef __TPROG_PUBLIC_CALLS_
#define __TPROG_PUBLIC_CALLS_

#include "types.h"
//#include "consts.h"
//#include "err.h"

/**
Same as malloc but in EEPROM.
\ingroup api_mem
\param size
*/

extern void * emalloc(u16 size);

/**
Free emalloc'ed chunk.
\ingroup api_mem
\param ptr
*/

extern void efree(void * ptr);

/** Read byte from memory.
\ingroup api_mem
\param addr EEPROM, PROGMEM, RAM memory address.
*/
extern u8 rb(const void *addr);

/** Write byte to memory.
\ingroup api_mem
\param addr EEPROM, RAM memory address.
\param val value.
*/
extern void wb(void *addr, u8 val);

/** Read word from memory.
\ingroup api_mem
\param addr EEPROM, PROGMEM, RAM memory address.
*/
extern u16 rw(const void *addr);

/** Write word to memory.
\ingroup api_mem
\param addr EEPROM, RAM memory address.
\param val value.
*/
extern void ww(void *addr, u16 val);

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
Register action.
\ingroup api_actions
\param action
\return #NO_ERROR or #ERR_RESOURCE_BUSY (some actions can be registered by only
one appliacation).
\sa unreg_action()
*/
extern u8 reg_action(u8 action);

/**
\ingroup api_actions
Unregister action.
\param action
\sa reg_action()
*/
extern void unreg_action(u8 action);

/**
\ingroup api_usb
Send answer on action.
\param result
\param len
\param data
*/
void usb_send(u8 result, u16 len, u8 *data);

/**
\ingroup api_usb
Receive char from USB, blocking.
*/
u8 usb_get_char(void);

/**
\ingroup api_usb
Send char to USB, blocking.
*/
u8 usb_put_char(u8 c);

/**
\ingroup api_usb
Send char to USB, non blocking.
*/
u8 usb_put_char_nb(u8 c);


/**
Delay \par us microseconds
\ingroup api_hwd
*/

void delayUs(u16 us);

/**
Delay \par ms miliseconds
\ingroup api_hwd
*/
void delayMs(u16 ms);

/**
Debug char
*/
void dbc(u8 val);

/**
Debug string
*/
void dbsp(u8 *s);

/**
Debug char HEX
*/
void dbch(u8 val);

/**
Debug int HEX
*/
void dbih(u16 val);

/**
TODO
\ingroup api_hwd
*/
void ext_sbi(u8 port,u8 line);

/**
TODO
\ingroup api_hwd
*/
void ext_cbi(u8 port,u8 line);

/**
Turn a led on. All other leds are off.
\param #LED_NO, #LED_RUN, #LED_DBG, #LED_PRG, #LED_MMC, #LED_SIM, #LED_TFM, #LED_PFM, #LED_ERR
\ingroup api_hwd
*/
void led_on(u8 led);

/**
Set hardware to default state. This is done always when entering and leaving the application mode.
\ingroup api_hwd
*/
void hwd_set_default(void);

/**
Read byte from extern 256KB SRAM.
\param addr
\ingroup api_mem
*/
u8 ext_sram_rb(u32 addr);

/**
Write byte to extern 256KB SRAM.
\param addr
\param value
\ingroup api_mem
*/
void ext_sram_wb(u32 addr,u8 value);

/**
Pointer on buffer 1072 bytes long. Can be used only during one action. Content can be rewritten between actions.
\ingroup api_mem
*/
u8 * buf_A(void);

/**
Pointer on buffer 1072 bytes long. Can be used only during one action. Content can be rewritten between actions.
\ingroup api_mem
*/
u8 * buf_B(void);

#define LED_NO          0x00
#define LED_RUN         0x01
#define LED_DBG         0x02
#define LED_PRG         0x03
#define LED_MMC         0x04
#define LED_SIM         0x05
#define LED_TFM         0x06
#define LED_PFM         0x07
#define LED_ERR         0x08

/** Invoked for application registration.
\ingroup api_actions

Called only once when the application is loaded into the memory.
By registration we mean one time
process, when application can e.g. allocate EEPROM structures.
*/

#define ACTION_APP_REGISTER                     0x00

/** Invoked when application is unloaded from memory.
\ingroup api_actions

Application should clear the resources it used (EEPROM).
*/

#define ACTION_APP_UNREGISTER                   0x01

/** Invoked for application to initialize its internals.
\ingroup api_actions
*/
// TODO _init() as in Turbo Adapter???

#define ACTION_APP_INIT                         0x02

#define ACTION_APP_VERSION                      0x03

/* REGISTERABLE IRQ: */
#define ACTIONS_IRQ_START                       0x10

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INTERRUPT0                   0x10
/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INTERRUPT1                   0x11

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INTERRUPT2                   0x12

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INTERRUPT3                   0x13

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INTERRUPT4                   0x14

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INTERRUPT5                   0x15

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INTERRUPT6                   0x16

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INTERRUPT7                   0x17

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OUTPUT_COMPARE2              0x18

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OVERFLOW2                    0x19

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INPUT_CAPTURE1               0x1A

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OUTPUT_COMPARE1A             0x1B

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OUTPUT_COMPARE1B             0x1C

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OVERFLOW1                    0x1D

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OUTPUT_COMPARE0              0x1E

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OVERFLOW0                    0x1F

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_SPI                          0x20

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_UART0_RECV                   0x21

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_UART0_DATA                   0x22

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_UART0_TRANS                  0x23

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_ADC                          0x24

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_EEPROM_READY                 0x25

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_COMPARATOR                   0x26

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OUTPUT_COMPARE1C             0x27 

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_INPUT_CAPTURE3               0x28

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OUTPUT_COMPARE3A             0x29

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OUTPUT_COMPARE3B             0x2A

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OUTPUT_COMPARE3C             0x2B

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_OVERFLOW3                    0x2C

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_UART1_RECV                   0x2D

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_UART1_DATA                   0x2E

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_UART1_TRANS                  0x2F

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_2WIRE_SERIAL                 0x30

/** 
\ingroup api_actions
Register if application should receive given IRQ.
*/

#define ACTION_SIG_SPM_READY                    0x31 

#define NR_ACTIONS_IRQ         	(0x31-0x10+1) 

#define NO_ERROR                0x00
#define ERR_NO_PROGMEM          0x01
#define ERR_NO_RAM              0x02
#define ERR_NO_APP_SLOT         0x04
#define ERR_NO_EEPROM           0x07
#define ERR_BAD_FILE            0x0c

#define ERR_RESOURCE_BUSY         0x41

#endif
