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


#ifndef _CABLE_H_
#define _CABLE_H_

#ifdef __cplusplus
extern "C" {
#endif
	

/**
\ingroup intern_cable
create_file() wrapper.

The structure is:
- #CABLE_CREATE_FILE - 1 byte
- sort flag (#SORT_APPEND, #SORT_SMS, #SORT) - 1 byte
- path string, '\\0' terminated
- name string, '\\0' terminated

\return b32 inode pointer of error
*/

#define CABLE_CREATE_FILE	0x01

/**
\ingroup intern_cable
create_dir() wrapper.

The structure is:
- #CABLE_CREATE_DIR - 1 byte
- sorting flag (#SORT_APPEND, #SORT_SMS, #SORT) - 1 byte
- path string, '\\0' terminated
- name string, '\\0' terminated

\return b32 inode pointer of error
*/

#define CABLE_CREATE_DIR 	0x02

/**
\ingroup intern_cable
mc_append() wrapper.

\note Data should be 128 bytes max.

The structure is:
- #CABLE_FILE_APPEND - 1 byte
- file_id - 4 bytes
- length of date - 1 byte
- data 
*/

#define CABLE_FILE_APPEND	0x03

/**
\ingroup intern_cable
mc_read() wrapper.

The structure is:
- #CABLE_FILE_READ - 1 byte
- file_id - 4 bytes
- length of data - 1 byte
- position in file - 4 bytes

\return data
*/

#define CABLE_FILE_READ		0x04

/**
\ingroup intern_cable
mc_find() wrapper.

The structure is:
- #CABLE_FILE_FIND - 1 byte
- path string, '\\0' terminated
- name string, '\\0' terminated

\return <0 not found, >0 u32 inode pointer, u32 get_len(), u32 get_len_raw().
*/

#define CABLE_FILE_FIND		0x05

/**
\ingroup intern_cable
mc_detect() wrapper.

The structure is:
- #CABLE_MC_INFO

\return u8 id, u8 number of blocks, u16 number of pages, u32 root()
*/

#define CABLE_MC_INFO		0x06

/**
\ingroup intern_cable
get_inode() wrapper.

The structure is:
- #CABLE_GET_INODE - 1 byte
- inode_id - 4 bytes

\return inode content
*/

#define CABLE_GET_INODE		0x07

/**
\ingroup intern_cable
set_inode() cable interface wrapper.

The structure is:
- #CABLE_SET_INODE - 1 byte
- inode_id - 4 bytes
- inode content - sizeof(Inode) bytes
*/

#define CABLE_SET_INODE		0x08

/**
\ingroup intern_cable
Read file system block.

The structure is:
- #CABLE_READ_BLOCK - 1 byte
- block id - 2 bytes
- offet - 1 byte
- len - 1 byte

\return data of lenght \c len
*/

#define CABLE_READ_BLOCK	0x09

/**
\ingroup intern_cable
Write file system block.

The structure is:
- #CABLE_WRITE_BLOCK - 1 byte
- block id - 2 bytes
- offet - 1 byte
- len - 1 byte
- data
*/

#define CABLE_WRITE_BLOCK	0x0a

/**
\ingroup intern_cable
delete_inode() wrapper.

The structure is:
- #CABLE_DELETE_INODE - 1 byte
- inode id - 4 bytes
*/

#define CABLE_DELETE_INODE	0x0b


/**
\ingroup intern_cable
General tag indicating application upload.
Used in combination with #CABLE_UPLOAD_APP_START, #CABLE_UPLOAD_APP_CONT, #CABLE_UPLOAD_APP_END.

Usage is:
-# #CABLE_UPLOAD_APP_START
-# repeat of #CABLE_UPLOAD_APP_CONT
-# #CABLE_UPLOAD_APP_END
*/

#define CABLE_UPLOAD_APP	0x0c

/**
\ingroup intern_cable
Indicates beginning of application upload.

Structure is:
- #CABLE_UPLOAD_APP - 1 byte
- #CABLE_UPLOAD_APP_START - 1 byte

\return #NO_ERROR or #ERR_NO_RAM.
*/

#define CABLE_UPLOAD_APP_START	0x00

/**
\ingroup intern_cable
Upload part of application.

Structure is:
- #CABLE_UPLOAD_APP - 1 byte
- #CABLE_UPLOAD_APP_CONT - 1 byte
- length of following data - 1 byte
- data 

\return #NO_ERROR or #ERR_BAD_FILE, #ERR_NO_APP_SLOT, #ERR_NO_PROGMEM, #ERR_NO_RAM.
*/

#define CABLE_UPLOAD_APP_CONT 	0x01

/**
\ingroup intern_cable
Finnish aplication upload.

Structure is:
- #CABLE_UPLOAD_APP - 1 byte
- #CABLE_UPLOAD_APP_END - 1 byte
- name of application, '\\0' terminated

\return #NO_ERROR, #ERR_NO_RAM, #ERR_NO_EEPROM, #ERR_BAD_FILE (crc error).
*/

#define CABLE_UPLOAD_APP_END  	0x02

/**
\ingroup intern_cable
Remove all applications.

The structure is:
-# #CABLE_CLEAR_APPS	- 1 byte
*/

#define CABLE_CLEAR_APPS	0x0d

/**
\ingroup intern_cable
Remove all applications.

The structure is:
-# #CABLE_RESET_DEFAULT	- 1 byte
*/

#define CABLE_RESET_DEFAULT	0x0e

/**
\ingroup intern_cable
Remove all applications.

The structure is:
-# #CABLE_LOCK_TURBO	- 1 byte
*/

#define CABLE_LOCK_TURBO	0x0f

/**
\ingroup intern_cable
Get Turbo OS information - version, serial number.

The structure is:
-# #CABLE_INFO      	- 1 byte
*/

#define CABLE_INFO		0x10

#define CABLE_INFO_VERSION	0x01
#define CABLE_INFO_SN		0x02

#if 0
/**
\ingroup intern_cable

Reset 2nd chip. Returns ATR or zero length if no 2nd chip.

The structure is:
-# #CABLE_TWIN_ON              - 1 byte
*/

#define CABLE_TWIN_ON          0x11

/**
\ingroup intern_cable

Transmit tx bytes, receive rx bytes.

The structure is:
-# #CABLE_TWIN_TXRX            - 1 byte
- length of following (tx) data - 1 byte
- tx APDU

return len+4, incl. T1 header/footer
*/

#define CABLE_TWIN_APDU_T1     0x12

/**
\ingroup intern_cable

Transmit tx bytes, receive rx bytes.

The structure is:
-# #CABLE_TWIN_TXRX            - 1 byte
- length of following (tx) data - 1 byte
- length of expected read (rx) data - 1 byte
- tx data
*/

#define CABLE_TWIN_TXRX                0x13
#endif

#ifdef __cplusplus
}
#endif

#endif
