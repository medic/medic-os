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

#ifndef _FS_H
#define _FS_H

#include "consts.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
\ingroup api_fs
Format memory card.

\attention Only for tfslib, i.e. programmer. Not available for Turbo.
\inode
*/
extern void format(void);

/** Max. name len (including terminal '\\0')
\ingroup api_fs
*/
#define INODE_TEXT_LEN	17

/** Inode unused */
#define INODE_TYPE_UNUSED				0x00
/** Regular file */
#define INODE_TYPE_FILE					0x01
/** Directory */
#define INODE_TYPE_DIR					0x02
/** File, only data encrypted, key validation. */
#define INODE_TYPE_PROTECTED_KEY_DATA			0x03
/** File, all data, len and type are encrypted, no key validation */
#define INODE_TYPE_PROTECTED_DATA_LEN_TYPE		0x04

/** Append */
#define SORT_APPEND		0x00
/** Dir insert alphabetically, files - last in on top  */
#define SORT_SMS		0x01
#define SORT_SMS_SKIP		0x02
#define SORT_SMS_SKIP_1		(SORT_SMS_SKIP | 0x10)
#define SORT_SMS_SKIP_2		(SORT_SMS_SKIP | 0x20)
#define SORT_SMS_SKIP_3		(SORT_SMS_SKIP | 0x30)
#define SORT_SMS_SKIP_4		(SORT_SMS_SKIP | 0x40)
#define SORT_SMS_SKIP_5		(SORT_SMS_SKIP | 0x50)
#define SORT_SMS_SKIP_6		(SORT_SMS_SKIP | 0x60)
#define SORT_SMS_SKIP_7		(SORT_SMS_SKIP | 0x70)

/**
\ingroup api_fs
Sort, case insensitive.
*/

#define SORT			0x03

/**
\ingroup api_fs
\image html fs_inode.png "Inode"
*/

typedef struct _inode 
{
/** Inode type, #INODE_TYPE_UNUSED, #INODE_TYPE_FILE, #INODE_TYPE_DIR, #INODE_TYPE_PROTECTED_KEY_DATA, #INODE_TYPE_PROTECTED_DATA_LEN_TYPE. */
u8 type;
/** Parent block */
u16 parent;
/** Parent index */
u8  parent_index;
/** Content block */
u16 content;
/** Content index */
u8 content_index;
/** Previous inode block */
u16 prev;
/** Previous inode index */
u8 prev_index;
/** Next inode block */
u16 next;
/** Next inode index */
u8 next_index;
/** First content block */
u16 first_block;
/* ^^^ LEN 15 */
/** Name, \ref string_alpha */
u8 text[INODE_TEXT_LEN];
} Inode;

/**
\ingroup api_fs
<b>M</b>emory <b>C</b>ard FILE structure. Like FILE but spcific for turbo file
system (FILE will be based on MCFILE once implemented).
*/

typedef struct _MCFILE 
{
u8 _opened_status;
u32 _opened_len;
u16 _opened_block_off;
u16 _opened_block;
u16 _opened_block_cur;
u8 _opened_prolog_len;
} MCFILE;

/**
\ingroup api_fs
Returns the index part of inode pointer.
\param fp	inode pointer.
\return 	inode index.
*/


extern u8  fs_ino_index(u32 fp);

/**
\ingroup api_fs
Returns the block part of inode pointer.
\param fp	inode pointer.
\return 	inode block.
*/


extern u16 fs_ino_block(u32 fp);

/**
\ingroup api_fs
Converts the block and index to the inode pointer.
\param block	
\param index
\return inode pointer.
*/

extern u32 fs_ino_p(u16 block, u8 index);

/**
\ingroup api_fs
Returns inode pointer of parent.
\param ino
\return inode pointer.
*/

extern u32 ino_parent(Inode *ino);

/**
\ingroup api_fs
Returns the content of inode.
\param ino	Pointer on Inode structure to be filled with inode.
\param fp	Inode pointer.
*/

extern void get_inode(Inode *ino, u32 fp);

/**
\ingroup api_fs
Sets new inode structure.
\param ino	Pointer on Inode structure containing new inode content.
\param fp	inode pointer.
*/

extern void set_inode(Inode *ino, u32 fp);

/**
\ingroup api_fs
Change working directory to directory pointed by pointer.
\inode
\param fp	inode pointer.
*/

extern void cd_i(u32 fp);

/**
\ingroup api_fs
Change working directory to a given path.
\inode
\param path	Path, can be stored in RAM, EEPROM, PROGMEM.
\return inode pointer of directory if successful, -#ERR_FILE_NOT_FOUND if failed.
*/

extern b32 cd(const u8 *path);

/**
\ingroup api_fs
Returns inode pointer of working directory.
\return inode pointer.
*/

extern u32 pwd(void);

/**
\ingroup api_fs
Returns inode pointer of root directory.
\return root inode pointer.
*/

extern u32 root(void);

/**
\ingroup api_fs
Create the new file in a given directory.

\code
u8 PROGMEM t_dir_name[] = "My Directory";
u8 PROGMEM t_file_name[] = "My File";
u8 PROGMEM t_file_ext[] = "mxt";	// My Extension

void foo()
{
b32 dir;
b32 file;
MCFILE *fd;

cd_i(root());
dir=cd(t_dir_name);

if(dir<0) 
{
// dir "/My Direcory" does not exist
}

file=create_dir(dir, t_file_name, SORT); // create "/My Directory/My File"

if(file<0)
{
// could not create
}

mc_set_type (file, t_file_ext);

fd = mc_open (file, FILE_APPEND);
mc_append (fd, some_data, len_of_some_data);
mc_close (fd);
}
\endcode

\inode
\param dir	Directory inode pointer.
\param name	File name, \ref string_alpha. Can be RAM, EEPROM, PROGMEM.
\param sort_flag Required sorting, #SORT_APPEND, #SORT_SMS, #SORT_SMS_SKIP, #SORT.
\return inode pointer>0 if successfull, -#ERR_NO_RAM, -#ERR_NO_SPACE.
*/

extern b32 create_file(u32 dir, const u8 *name, u8 sort_flag);

/**
\ingroup api_fs
Create the new dir in a given directory.

\code
u8 PROGMEM t_dir_name[] = "My Directory";

void foo()
{
b32 dir;

dir=create_dir(root(), t_dir_name, SORT); // create "/My Directory"

}
\endcode
\inode
\param dir	Directory inode pointer.
\param name	New directory name, \ref string_alpha. Can be RAM, EEPROM, PROGMEM.
\param sort_flag Required sorting, #SORT_APPEND, #SORT_SMS, #SORT_SMS_SKIP, #SORT.
\return inode pointer>0 if successfull, -#ERR_NO_RAM, -#ERR_NO_SPACE.
*/

extern b32 create_dir(u32 dir, const u8 *name, u8 sort_flag);


/**
\ingroup api_fs
Encrypt file pointed on by inode pointer with key and twofish 128bit cypher.
\inode
\param fp	Inode pointer.
\param key	Key, all 16 bytes has to be valid, RAM only
\param flag	#INODE_TYPE_PROTECTED_KEY_DATA or #INODE_TYPE_PROTECTED_DATA_LEN_TYPE.	
\return 	#NO_ERROR, #ERR_BAD_PARMS if wrong flag, #ERR_BAD_FILE if not file, #ERR_NO_RAM if memory problem.
*/

extern u8 mc_file_encrypt(u32 fp, u8 key[16], u8 flag);

/**
\ingroup api_fs
Decrypt encrypted file.
\inode
\param desc	Inode pointer on encrypted file (either #INODE_TYPE_PROTECTED_KEY_DATA or INODE_TYPE_PROTECTED_DATA_LEN_TYPE.
\param key	Key, all 16 bytes has to be valid, RAM only.
\return 	#NO_ERROR, #ERR_BAD_FILE if not encrypted file, #ERR_WRONG_KEY if #INODE_TYPE_PROTECTED_KEY_DATA and supplied key is wrong, #ERR_NO_RAM if memory problem.
*/

extern u8 mc_file_decrypt(u32 desc, u8 key[16]);

/**
\ingroup api_fs
Delete \c all files in the directory.
\note Because of memory reasons the number of deleted files is limited to 85. 
Use the function more times to delete all files.
 
\inode
\param fp 	Directory inode pointer.
\return 	#NO_ERROR, #ERR_BAD_FILE.
*/

extern b16 delete_files(u32 fp);

/**
\ingroup api_fs
Delete file or directory - including subdirs.
\inode
\param fp	File pointer.
\return #NO_ERROR, -#ERR_BAD_FILE, -#ERR_NO_RAM.
*/

extern b16 delete_inode(u32 fp);

/**
\ingroup api_fs
Set file type (3 bytes extension/magic).
\inode
\param fp	Inode pointer on file.
\param type	Pointer on u8 type[3], can be RAM, EEPROM, PROGMEM.
\return #ERR_BAD_FILE, #NO_ERROR.
*/

extern u8 mc_set_type(u32 fp, const u8 * type);

/**
\ingroup api_fs
Returns file type (3 bytes extension/magic).
\inode
\param fp 	Inode pointer on file.
\param type	Returned type.
\return #ERR_BAD_FILE, #NO_ERROR.
*/

extern u8 mc_get_type(u32 fp, u8 type[3]);	

/**
\ingroup api_fs
Returns length of file. 
In the case of #INODE_TYPE_PROTECTED_DATA_LEN_TYPE it returns number of 256
byte blocks.
In the case of #INODE_TYPE_PROTECTED_KEY_DATA it returns length ceiled to be
divisible by 16 (size of encryption chunk - 128 bits).
\inode
\param fp	Inode pointer.
\return len>=0 if OK, -#ERR_BAD_FILE if failed.
\sa \ref intern_fs
*/

extern b32 get_len(u32 fp);

/**
\ingroup api_fs
Returns length of file.In the case of #INODE_TYPE_PROTECTED_DATA_LEN_TYPE it returns -#ERR_BAD_FILE.
\attention In case of #INODE_TYPE_PROTECTED_KEY_DATA this value is wrong - it
is not ceiled to be divisible by 16, see get_len().
\inode
\param fp 	Inode pointer.
\return len>=0 if OK, -#ERR_BAD_FILE if failed.
\sa \ref intern_fs
*/

extern b32 get_len_raw(u32 fp);

/**
\ingroup api_fs
Indicates READ operation.
*/

#define FILE_READ	0x01

/**
\ingroup api_fs
Indicates REWRITE operation.
*/

#define FILE_REWRITE	0x02

/**
\ingroup api_fs
Indicates APPEND operation.
*/
#define FILE_APPEND	0x04

/**
\ingroup api_fs
Open file for reading or writing (=appending).
\inode
\param fp	Inode pointer.
\param flag 	#FILE_READ, #FILE_APPEND or #FILE_REWRITE
\return NULL if failed, MCFILE struct if OK.
*/

extern MCFILE * mc_open(u32 fp, u8 flag);

/**
\ingroup api_fs
Read \c len bytes into \c buf at \c pos from \c file.
\param file
\param pos	position 
\param buf	buffer to fill
\param len	length of read
\return nr. of bytes read>=0 if OK, -#ERR_WRONG_POS, -#ERR_BAD_FILE if error.
*/

extern b16 mc_read(MCFILE *file, u32 pos, u8 * buf, u8 len);

/**
\ingroup api_fs
Write \c len bytes in \c buf at \c pos in \c file.
Pos has to be <= file_len - len, otherwise only truncated len.
\param file
\param pos	position
\param buf	buffer to write
\param len	length of data in buffer
\return nr. of bytes rewriten>=0 OK, -#ERR_WRONG_POS, -#ERR_BAD_FILE if error.
*/

extern b16 mc_rewrite(MCFILE *file, u32 pos, u8 * buf, u8 len);

/**
\ingroup api_fs
Appends \c len bytes from \c buf to \c file.
\param file
\param len 	length of data in buffer
\param buf	buffer
\return #NO_ERROR if OK, -#ERR_BAD_FILE if error.
*/

extern b16 mc_append(MCFILE *file, u8 * buf, u8 len);

/**
\ingroup api_fs
Closes file - must be called to flush all content to memory card and to free
all memory.
\param file
\return #NO_ERROR.
*/

extern b16 mc_close(MCFILE *file);

/**
\ingroup api_fs
Truncate file - clear all the content.
\inode
\param fp	inode pointer.
\return #NO_ERROR, -#ERR_BAD_FILE.
*/

extern b16 mc_clear(u32 fp);

/**
\ingroup api_fs
Move file/dir form one dir to another.
\param what	Inode pointer on what to move.
\param to	Inode pointer on target directory.
\param sort_flag Required sorting, #SORT_APPEND, #SORT_SMS, #SORT_SMS_SKIP, #SORT.
\return #NO_ERROR, -#ERR_NO_RAM.
*/

extern b32 mc_move(u32 what, u32 to, u8 sort_flag);

/**
\ingroup api_fs
Interpret inode name as MSISDN. Name is expected to be #MSISDN_ADN, while searched MSISDN type #MSISDN_SMS.
*/

#define FS_FIND_MSISDN		0x01

/**
\ingroup api_fs
Search for string name.
*/

#define FS_FIND_STRING		0x02

/**
\ingroup api_fs
Find inodes of the given name in the current dir.
\par Example - Find all occurences of "My File" name


\code
u8 PROGMEM t_file_name[] = "My File";

void foo()
{
b32 file = 0;

cd(...);

do
{
file = mc_find (file, t_file_name, FS_FIND_STRING);
if(file>0)
{
// file found
}
} while(file>0);

}

\endcode
\inode
\param file	previous hit, use 0 for start.
\param name	name to look for, \ref string_alpha. Can be stored in RAM, EEPROM, PROGMEM
\param type	search type, #FS_FIND_STRING, #FS_FIND_MSISDN.
\return inode pointer if >0, -#ERR_FILE_NOT_FOUND if not found.
*/

extern b32 mc_find(b32 file, const u8 *name, u8 type);

/**
\ingroup api_fs
Detect memory card.
\return #MC_OK if OK, #MC_NOT_PRESENT or #MC_NOT_READY (=not formatted) if error.
*/

extern u8 mc_detect(void);

/** 
\ingroup api_fs
The inode() function returns pointer to the \c Inode structure, which is 
globaly allocated and usable in your code. Saves RAM and code.
\warning This structure can be used by other functions.
*/

extern Inode * inode(void);

/**
\ingroup api_fs
Returns label of the Memory Card.
\param label Pointer on the buffer where the label is returned.
*/

extern void mc_get_label(u8 label[15]);

/**
\ingroup api_fs
Set Memory Card label.
\param label New Memory Card label.
*/
extern void mc_set_label(u8 label[15]);

/**
\ingroup api_fs
Returns the number of Memory Card (DataFlash) pages.
*/

extern u16 mc_pages(void);

/**
\ingroup api_fs
Returns the number of 264bytes long blocks per page.
*/

extern u8 mc_blocks(void);

/**
\ingroup api_fs
Returns the number of free file system blocks.
*/

extern u16 mc_free(void);

#ifdef __cplusplus
}
#endif

#endif
