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

#ifndef _TWOFISH_H_
#define _TWOFISH_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
\ingroup api_security
Prepare twofish key structure for de/encryption.

\param key	pointer on '\\0' terminated key, can be RAM, EEPROM, PROGMEM.
\param keylen	requested keylen 16 (=128bits), 24 (=192bits), 32 (=256bits).
\param skey	result.
\return #NO_ERROR or #ERR_TWOFISH_KEYLEN, #ERR_NO_RAM.
*/
	
extern u8 twofish_setup(u8 *key, u8 keylen, Twofish_key *skey);

/**
Decrypt cipher text to plain text.
\ingroup api_security
\param ct	cipher text of len 16 bytes, stored in RAM, EEPROM or PROGMEM.
\param pt	plain text, RAM or EEPROM.
\param key	
*/

extern void twofish_ecb_decrypt(u8 *ct, u8 *pt, Twofish_key *key);

/**
Encrypt plain text to cipher text.
\ingroup api_security
\param ct	cipher text of len 16 bytes, stored in RAM, EEPROM or PROGMEM.
\param pt	plain text, RAM or EEPROM.
\param key	
*/

extern void twofish_ecb_encrypt(u8 *pt, u8 *ct, Twofish_key *key);

/**
Encrypt in CBC mode.
\ingroup api_security
\param in 	pointer on input buffer (len has to be nr_blocks*16), stored in RAM, EEPROM, PROGMEM
\param out	pointer on output buffer (len has to be nr_blocks*16), RAM or EEPROM
\param nr_blocks	number of blocks
\param key	key

\return #NO_ERROR, #ERR_NO_RAM
*/

extern u8 twofish_cbc_encrypt(u8 *in, u8 *out, u8 nr_blocks, Twofish_key *key);

/**
Decrypt in CBC mode.
\ingroup api_security
\param in 	pointer on input buffer (len has to be nr_blocks*16) stored in RAM, EEPROM, PROGMEM
\param out	pointer on output buffer (len has to be nr_blocks*16), RAM or EEPROM
\param nr_blocks	number of blocks
\param key	key

\return #NO_ERROR, #ERR_NO_RAM
*/

extern u8 twofish_cbc_decrypt(u8 *in, u8 *out, u8 nr_blocks, Twofish_key *key);

#ifdef __cplusplus
}
#endif

#endif
