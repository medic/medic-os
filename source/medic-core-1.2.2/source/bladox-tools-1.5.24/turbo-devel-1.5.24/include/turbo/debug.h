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

#ifndef _TURBO_DEBUG_H
#define _TURBO_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif


void __dbc(unsigned char txValue);	/* char */
void __dbs(unsigned char *txString);	/* string */
void __dbsp(u8 *txString, u8);	/* string from flash */
void __dbch(unsigned char txHex);	/* char as hex */
void __dbih(unsigned int txHex);	/* int as hex */

void _db_malloc(void);
void _db_imalloc(void);
void _db_emalloc(void);
void _db_stack(void);

void __df_print(u16);

void __edbc(u8 txValue);
void edbg();



void _s_stack();

#ifdef DEBUG

#define dbs(a) 	__dbs(a)
#define dbsp(a) {static unsigned char __attribute__ ((progmem)) s[]=a; __dbsp(s, MEM_L);}
#define dbc(a) 	__dbc(a)
#define dbch(a) __dbch(a)
#define dbih(a) __dbih(a) 
#define db_malloc() _db_malloc()
#define db_imalloc() _db_imalloc()
#define db_emalloc() _db_emalloc()
#define db_stack() _db_stack()

#define df_print(a) __df_print(a) 

#else

#define dbs(a)  
#define dbsp(a)
#define dbc(a)
#define dbch(a)
#define dbih(a)
#define db_malloc() 
#define db_imalloc() 
#define db_emalloc() 
#define db_stack() 

#define df_print(a)

#endif


#define DEBUG_STACK 
/* #undef DEBUG_STACK */

#ifdef DEBUG_STACK
#define s_stack()	_s_stack()
#else
#define s_stack()
#endif

#ifdef EDEBUG
#define edbc(a) __edbc(a)
#define edbi(a) __edbi(a)
#else
#define edbc(a)
#define edbi(a)
#endif

#ifdef __cplusplus
}
#endif

#endif
