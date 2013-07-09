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

#ifndef _TURBO_TYPES_
#define _TURBO_TYPES_

#ifndef LIBTFS
#include <avr/io.h>
#include <avr/pgmspace.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** 
\ingroup api_mem 
unsigned 1byte=8bits 
*/
typedef unsigned char u8;
/** 
\ingroup api_mem 
unsigned 2bytes=16bits 
 */
typedef unsigned short u16;
/** 
\ingroup api_mem 
unsigned 4bytes=32bits 
*/
typedef unsigned long u32;

/** 
\ingroup api_mem 
signed 1byte=8bits 
*/
typedef char b8;
/** 
\ingroup api_mem 
signed 2bytes=16bits 
*/
typedef short b16;
/** 
\ingroup api_mem 
signed 4bytes=32bits 
*/
typedef long b32;

/**
\ingroup api_actions
Turbo handler type definition.
*/
  
typedef void (t_turbo_main)(u8 action, void *data);

typedef struct _USB_Data {
u16 len;
u8 * buf;
} USB_Data;


#ifdef __cplusplus
}
#endif

#endif
