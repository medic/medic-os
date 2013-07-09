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

#ifndef _TURBO_TYPES_
#define _TURBO_TYPES_

#ifdef __cplusplus
extern "C" {
#endif

/** unsigned 1byte=8bits */
typedef unsigned char u8;
/** unsigned 2bytes=16bits */
typedef unsigned short u16;
/** unsigned 4bytes=32bits */
typedef unsigned long u32;
/** unsigned 8bytes=64bits */
typedef unsigned long long u64;

typedef char b8;
typedef short b16;
typedef long b32;
typedef long long b64;

typedef struct _Twofish_key {
	u32 K[40];
	u8 S[32], start;
} Twofish_key;


typedef struct _lc_char
{
	u8 lang;
	u8 len;
	const u8 s[1000];
}
lc_char;

typedef struct _lc_list
{
	u8 lang;
	const u8 * s;
}
lc_list;

#ifdef __cplusplus
}
#endif


#include "debug.h"

#include "fs.h"
#include "err.h"

#define DEV_AT45DB011B  0x0C
#define DEV_AT45DB021B  0x14
#define DEV_AT45DB041B  0x1C
#define DEV_AT45DB081B  0x24
#define DEV_AT45DB161B  0x2C
#define DEV_AT45DB321B  0x34
#define DEV_AT45DB642B  0x3C
#define DEV_UNKNOWN     0xFF

#define BLOCK_NULL      0x00
#define BLOCK_INFO      0x00
#define PAGE_INFO       0x00

void init_mc(unsigned char *device);

u8 dataFlashBufferRead(u8 bufferNumber, u8 *buffer, u16 addr, u16 size);
u8 dataFlashMemoryToBufferRead(u8 bufferNumber, u16 pageAddr);
u8 dataFlashBufferWrite(u8 bufferNumber, u8 *buffer, u16 addr, u16 pageSize);
u8 dataFlashPageErase(u16 pageNumber);
u8 dataFlashBufferToMemoryProgramWithErase(u8 bufferNumber, u16 pageNumber);


#endif
