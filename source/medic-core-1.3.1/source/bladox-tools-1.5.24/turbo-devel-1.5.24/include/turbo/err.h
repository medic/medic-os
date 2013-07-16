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

#ifndef _ERR_H_
#define _ERR_H_

#ifdef __cplusplus
extern "C" {
#endif



/**
\ingroup api_err 
No error, 0 value.
*/

#define	NO_ERROR		0x00

/**
\ingroup api_err 
Not enough space in PROGMEM.
Can be passed to perror().
*/

#define ERR_NO_PROGMEM		0x01

/**
\ingroup api_err 
Not enough RAM.
Can be passed to perror().
*/

#define ERR_NO_RAM		0x02

/**
\ingroup api_err 
Some unknown error, no better description.
Can be passed to perror().
*/

#define ERR_UNKNOWN		0x03

/**
\ingroup api_err 
No free application slot. There is limited number of applications (=slots)
that can be installed.
Can be passed to perror().
*/

#define ERR_NO_APP_SLOT		0x04

/**
\ingroup api_err 
Memory card not present.
Can be passed to perror().
*/

#define ERR_MC_NOT_PRESENT	0x05

/**
\ingroup api_err 
Memory card not formated.
Can be passed to perror().
*/

#define ERR_MC_NOT_READY	0x06

/**
\ingroup api_err 
Not enough EEPROM space.
Can be passed to perror().
*/

#define ERR_NO_EEPROM		0x07

/**
\ingroup api_err 
Wrong input inserted.
Can be passed to perror().
*/

#define ERR_BAD_INPUT		0x08

/**
\ingroup api_err 
Wrong key.
Can be passed to perror().
*/

#define ERR_WRONG_KEY		0x09

/**
\ingroup api_err 
File encryption can be used only on files of limited length (2KB). This
is because of required time to perform de/encryption.
Can be passed to perror().
*/

#define ERR_FILE_TOO_LONG	0x0a

/**
\ingroup api_err 
Memory card is full.
Can be passed to perror().
*/

#define ERR_NO_SPACE		0x0b

/**
\ingroup api_err 
Operation cannot be performed on given file.
Can be passed to perror().
*/

#define ERR_BAD_FILE		0x0c

/**
\ingroup api_err 
Async admin packet
*/


#define ERR_ADMIN_ASYNC			0x0d

/**
\ingroup api_err 
Async admin packet
*/


#define ERR_ADMIN_CHKSUM		0x0e

/**
\ingroup api_err 
APDU retval different than requested
*/


#define ERR_ADMIN_APDU  		0x0f


/**
\ingroup api_err 
Wrong requested twofish key length. Keylen can be 16, 24 or 32.
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_TWOFISH_KEYLEN		0x40

/**
\ingroup api_err 
Usually for reg_action() or reg_type(). Some actions and files
can be registered by only one appliaction.
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_RESOURCE_BUSY		0x41

/**
\ingroup api_err 
Operation not permitted on protected memory.
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_PROT_MEM_FORBIDDEN		0x42

/**
\ingroup api_err 
Wrong block id passed to operation on protected memory.
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_PROT_MEM_WRONG_BLOCK	0x43

/**
\ingroup api_err 
Bad parameters.
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_BAD_PARAMS			0x44

/**
\ingroup api_err 
No protected memory block available (protected memory full).
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_NO_PROT_MEM			0x45

/**
\ingroup api_err 
File system error, read position requested higher than length of file.
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_WRONG_POS			0x46

/**
\ingroup api_err 
No such file.
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_FILE_NOT_FOUND		0x47

/**
\ingroup api_err 
Turbo is locked, applications cannot be upload, only reset to default is allowed (removes all applications and resets EEPROM).
Cannot be passed to perror() (too specific to inform user).
*/

#define ERR_TURBO_LOCKED		0x48

/**
\ingroup api_err 

Wrong radio frequency set - must be between #DEF_BRA_MIN_FREQ and #DEF_BRA_MAX_FREQ.

*/

#define ERR_BRA_WRONG_FREQ		0x49

/**
\ingroup api_err 

Wrong ieee802.15.4 channel set, must be between 11..26.
*/

#define ERR_BRA_WRONG_CHANNEL		0x4a


/**
\ingroup api_err 
Cannot be passed to perror().
*/

#define ERR_NOT_SUPPORTED		0x4b


#ifdef __cplusplus
}
#endif

#endif
