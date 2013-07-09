/*
 * Copyright (c) 2009 BLADOX, s.r.o.
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


#ifndef _ADMIN_H_
#define _ADMIN_H_

#ifdef __cplusplus
extern "C" {
#endif
	
/**
\ingroup intern_admin

*/

#define DEF_ADMIN_AID	{'B','L','A','D','O','X','O','S','1'}
//#define DEF_ADMIN_AID	{0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46}

// same as CABLE_INFO
#define ADMIN_INFO              0x10
#define ADMIN_INFO_VERSION      0x01
#define ADMIN_INFO_SN           0x02
#define ADMIN_INFO_BTLDR_VER    0x03
#define ADMIN_INFO_TP  		0x04
#define ADMIN_INFO_ATR 		0x05
#define ADMIN_INFO_SIM 		0x06
#define ADMIN_INFO_COUNTER	0x07
// PROG+RAM+EE
#define ADMIN_INFO_MEM		0x08
// ID+MEM
#define ADMIN_INFO_APP 		0x09
#define ADMIN_INFO_IMSI		0x0a
#define ADMIN_INFO_HWD          0x0b
#define ADMIN_INFO_NONCE        0x0c
#define ADMIN_INFO_BTLDR_CONF   0x0d
#define ADMIN_INFO_NR_APPS      0x0e

#define ADMIN_INFO_APPS         0x11

#define ADMIN_UPLOAD_APP        0x0c
#define ADMIN_UPLOAD_APP_START  0x00
#define ADMIN_UPLOAD_APP_CONT   0x01
#define ADMIN_UPLOAD_APP_END    0x02

#define ADMIN_RESET             0x0e

#define ADMIN_REMOVE_APP        0x20
#define ADMIN_BTLDR_GET        	0x21
#define ADMIN_BTLDR_SET        	0x22
#define ADMIN_TRIGGER_NET     	0x23

#define ADMIN_SE_APDU_LIST     	0x24
#define ADMIN_SE_RESET    	0x00
#define ADMIN_SE_APDU    	0x01
#define ADMIN_SE_OFF     	0x02
#define ADMIN_SE_RETVAL  	0x03

#define ADMIN_PROC_GET        	0x25
#define ADMIN_PROC_SET        	0x26

#define ADMIN_TSMS        	0x27

#define ADMIN_END              	0xbb

#define ADMIN_HANDLE            0x81

/*
flags
*/

// send normal SMS back
#define ADMIN_FLAG_REPLY_SMS		0x80
// use STATUS (GET_RESPONSE) to send something back
#define ADMIN_FLAG_STATUS_SMS		0x40
// switch off SE
//#define ADMIN_FLAG_SE_OFF		0x20
// first confirm ENVELOPE and then process command
#define ADMIN_FLAG_DELAYED		0x10
// send normal SMS back if error
#define ADMIN_FLAG_REPLY_SMS_ON_ERROR	0x08
// use default OTA MSISDN
#define ADMIN_FLAG_DEFAULT_MSISDN	0x04


#define DEF_ADMIN_CHECKSUM_OFFSET	1
#define DEF_ADMIN_LEN_OFFSET		2
#define DEF_ADMIN_RND1_OFFSET		3
#define DEF_ADMIN_FLAGS_OFFSET		4
//5
#define DEF_ADMIN_RND2_OFFSET		6
#define DEF_ADMIN_COUNTER_OFFSET	7
#define DEF_ADMIN_CMD_OFFSET		9

/*
format:
B0 sig of key used
B1 checksum
B2 len
B3 rnd
B4 flags
B5 
B6 rnd

B7 counter_low
B8 counter_hi

B9 cmd_1
B10 cmd_2
B11 cmd_3

B12- (128b) data

b0 unecrypted
b1-end 128+11 bytes potentially encrypted
*/

/*
answer:
B0 chksum
B1 rnd
B2 counter_low
B3 counter_hi
B4 rnd
B5 answer_start (len of next)
*/

#define DEF_ADMIN_ANSWER_CHECKSUM_OFFSET	0
#define DEF_ADMIN_ANSWER_RND1_OFFSET		1
#define DEF_ADMIN_ANSWER_COUNTER_OFFSET		2
#define DEF_ADMIN_ANSWER_RND2_OFFSET		4
#define DEF_ADMIN_ANSWER_OFFSET			5

#ifdef __cplusplus
}
#endif

#endif
