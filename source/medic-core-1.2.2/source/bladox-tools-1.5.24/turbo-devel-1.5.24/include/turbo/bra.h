/*
 * Copyright (c) 2004, 2005, 2006 BLADOX, s.r.o.
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

#ifndef _TURBO_BRA_H_
#define _TURBO_BRA_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
\ingroup api_bra

BRA MAC address length. 4 bytes address format A.B.C.D is used.
Every device has unique address burned in - can be modified by application. 255.255.255.255 denotes everybody.
*/

#define DEF_BRA_MAC_LEN		0x04

/**
\ingroup api_bra

BRA packet version. 
*/

#define DEF_BRA_VERSION		0x01

/**
\ingroup api_bra
Reset device MAC address previously modified with bra_mac_set() to factory default.
*/
	
extern void bra_mac_reset();

/**
\ingroup api_bra

Modify device MAC address.
*/
	
extern void bra_mac_set(u8 addr[DEF_BRA_MAC_LEN]);

/**
\ingroup api_bra

Get device MAC address (possibly modified by bra_mac_set()).
*/
	
extern void bra_mac_get(u8 addr[DEF_BRA_MAC_LEN]);

/**
\ingroup api_bra

Function to construct BRA packet.

\param buf	pointer on buffer used for packet construction	
\param dest_addr NULL for everybody or specific destination address
\param tag	application tag - used to select destination application
\param release	indication if target device can go sleep. 1 means yes, target can go idle/sleep, 0 - further communication to follow
\return 	pointer on buf after just constructed header.
*/

extern u8 * bra_make_head(u8 * buf, u8 * dest_addr, u8 * tag, u8 release);

/**
\ingroup api_bra

Version tag in BRA packet.
*/

#define T_BRA_VERSION		0x03

/**
\ingroup api_bra

Destination address tag in BRA packet.
*/

#define T_BRA_DEST		0x04

/**
\ingroup api_bra

Source address tag in BRA packet.
*/

#define T_BRA_SRC		0x08

/**
\ingroup api_bra

Application tag in BRA packet.
*/

#define T_BRA_TAG		12

/**
\ingroup api_bra

Application data in BRA packet.
*/

#define T_BRA_DATA		0x00

/**
\ingroup api_bra

Seek tag in BRA packet header.

\param buf	pointer on buffer containing BRA packet
\param tag	what to seek: #T_BRA_VERSION, #T_BRA_DEST, #T_BRA_SRC, #T_BRA_TAG, #T_BRA_DATA
\result 	pointer on sought data in BRA packet
*/

extern u8 * bra_seek_head (u8 * buf, u8 tag);

/**
\ingroup api_bra

BRA Tx and Rx function - this is the  main call for radio communication.
Tx/Rx loop to communicate that can be used to communicate with both
idle (sleeping) and ready (non-sleeping) devices.

The loop looks following:
\code
while(nr_tx--)
{
tx();
tmp_nr_rx=nr_rx;
while(tmp_nr_rx--)
 {
 delay_ms(after_tx_ms);
 rx();
 if something received:
	check destination addr me or everybody
	check application tag
 }
}
\endcode

\param buf	pointer on buffer with BRA packet
\param len	length of packet to send
\param bra_app_tag required BRA application tag to be received, can be NULL
\param nr_tx	number of Tx
\param nr_rx	number of Rx attempts after Tx
\param after_tx_ms delay between Tx/Rx (and following Rx's), in milisec 
\result		0 - nothing received, >0 length of received packet, <0 error
*/

extern b8 bra_txrx (u8 * buf, u8 len, u8 * bra_app_tag, u8 nr_tx, u8 nr_rx, u8 after_tx_ms);

/**
\ingroup api_bra

LQI - Link Quality Indicatior of the last packed received.

Low byte is RSSI, hi byte is correlation as defined in \ref cf_cc2420.

\result value consists of RSSI|(CORR&0x7f)
*/

extern u16 bra_lqi ();

/**
\ingroup api_bra

Min radio frequency - 2400 MHz. See \ref cf_cc2420.
*/

#define DEF_BRA_MIN_FREQ	2400

/**
\ingroup api_bra

Max radio frequency - 2483 MHz. See \ref cf_cc2420.
*/

#define DEF_BRA_MAX_FREQ	2483


/**
\ingroup api_bra

Set radio frequency, can be between #DEF_BRA_MIN_FREQ and #DEF_BRA_MAX_FREQ.
Default is 2480MHz (channel 26).

\param freq	frequency in MHz
\return #NO_ERROR, #ERR_BRA_WRONG_FREQ
*/

extern u8 bra_set_freq (u16 freq);

/**
\ingroup api_bra

Set radio frequency channel, can be between 11 and 26.
Default is 26.

\param channel	11-26 as defined by ieee802.15.4
\return #NO_ERROR, #ERR_BRA_WRONG_CHANNEL
*/

extern u8 bra_set_802_15_4_channel (u8 channel);

/**
\ingroup api_bra

Set radio Tx (transmit) power. See \ref cf_cc2420.

\param pa	output power: 0-31
*/

extern void bra_set_tx_power (u8 pa);

/**
\ingroup api_bra

Returns current radio frequency.
\result freq  between #DEF_BRA_MIN_FREQ and #DEF_BRA_MAX_FREQ
*/

extern u16 bra_freq ();

/**
\ingroup api_bra

Returns current Tx power.
\result pa 0..31, see \ref cf_cc2420
*/

extern u8 bra_tx_power ();

/**
\ingroup api_bra

Low level read access to CC2420 radio chip registers, RAM and strobe commands. 
See \ref cf_cc2420. 

\param addr if MSB is '1' (i.e. |0x8000) then RAM is used, if 0 then register. Registers 0x00..0x0e are strobe commands.
\result value of register/memory
*/

extern u16 bra_cc2420_read (u16 addr);

/**
\ingroup api_bra

Low level write access to CC2420 radio chip RAM and registers. Write to registers 0x00..0x0e is equivalent to read operation, i.e. strobe command.
See \ref cf_cc2420. 

\param addr 	if MSB is '1' (i.e. |0x8000) RAM is used, registers otherwise
\param val 	value, for RAM only low byte is used
\result 	result of the operation
*/

extern u16 bra_cc2420_write (u16 addr, u16 val);

/**
\ingroup api_bra

Send packet stored in buf of len bytes.

\param buf	buffer containing packet
\param len	lenght of packet, < 128bytes
\result 	result of the operation
*/

extern b8 bra_tx_raw(u8 * buf, u8 len);

/**
\ingroup api_bra

Check if RX radio buffer contains packet, store it in buf and return length.

\param buf	pointer of buffer to store the packet
\result 	length of received packet, 0 if nothing, <0 error
*/

extern b8 bra_rx_raw(u8 *buf);

/**
\ingroup api_bra

Turn radio chip on.

*/

extern void bra_radio_on();

/**
\ingroup api_bra

Turn radio chip off.

*/

extern void bra_radio_off();


#ifdef __cplusplus
}
#endif

#endif
