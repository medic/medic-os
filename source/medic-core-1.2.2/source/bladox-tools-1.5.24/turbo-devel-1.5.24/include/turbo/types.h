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
#define TURBO_LIBC

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
\ingroup api_i18n 
Used for string localization. 
\sa locale()
*/

typedef struct _lc_char
{
	/** language */
	u8 lang;
	/** sizeof following string */
	u8 len;
	/** string */
	const u8 s[];
}
lc_char;

/** 
\ingroup api_i18n 
Used for list ocalization. 
\sa locale_list()
*/

typedef struct _lc_list
{
	/** language */
	u8 lang;
	/** pointer on items */
	const u8 * s;
}
lc_list;

struct _SCtx;

/**
\ingroup api_spider
Spider node callback, it handles #APP_ENTER and #APP_LEAVE actions.
*/

typedef u8 (t_spider_cb)(struct _SCtx * ctx, u8 action);

/**
\ingroup api_fs
File handling callback, see reg_file_type(). Action can be #FILE_SELECT, #FILE_DETAIL, #FILE_DELETE, #FILE_MOVE, #FILE_RENAME.
*/

typedef u8 (t_file_cb)(u32 file, u8 file_action);


/** 
\ingroup api_spider
Spider node structure for RAM and EEPROM stored nodes. 
\attention See the difference to
SNodeP, where text is <b>const lc_char *</b>. Here it is just <b>u8 *</b>.
*/

typedef struct _SNode 
{
/** pointer on text label of the node.*/
u8 * text;
/** callback called when spider enters and leaves given node.*/
t_spider_cb * cb;
/** convenience pointer. */
void * p;
} SNode;

/** 
\ingroup api_spider
PROGMEM stored spider node structure. 
*/

typedef struct _SNodeP
{
/** pointer on text label of the node.*/
const lc_char * text;
/** callback called when spider enters and leaves given node. */
t_spider_cb * cb;
/** convenience pointer. */
const void * p;
} PROGMEM SNodeP;

/**
\ingroup api_spider
RAM and EEPROM stored spider edge. Compared to to PROGMEM stored SEdgeP there
are two extra fields: \c next and \c source.
*/

typedef struct _SEdge 
{
/** edges are linked into list, last has to be NULL. */
const struct _SEdge * next;
/** (#MEM_F_R or #MEM_F_P or #MEM_F_E or #MEM_F_D)|(#MEM_T_R or #MEM_T_P or #MEM_T_E or #MEM_T_D) memory indicator of \b from and \b to nodes. */
u8 source;	/* hi-halfbyte f, lo-halfbyte t */
/** pointer on \b from node. */
const SNode * f;
/** pointer on \b to node. */
const SNode * t;
} SEdge;

/** 
\ingroup api_spider
Structure of PROGMEM stored edge. Such edges are stored as NULL terminated array.
*/

typedef struct _SEdgeP
{
/** pointer on \b from node (stored in PROGMEM). */
const SNodeP * f;
/** pointer on \b to node (stored in PROGMEM). */
const SNodeP * t;
} PROGMEM SEdgeP;

/**
\ingroup api_spider
Linked list of SParent structures keeps the spider path. \c f is useful to access the parent of the current node.   
*/

typedef struct _SParent
{
struct _SParent * next;
u8 source;	/* hi-halfbyte f, lo-halfbyte t */
SNode * f;
SNode * t;
u8    n_index;
u16 item_from;
u16 pref_item;
} SParent;

/** 
\ingroup api_spider
Spider context structure. Keeps internal state of spider.
*/

typedef struct _SCtx
{
/** pointer on list of edges stored in RAM */
SEdge	* eR;
/** pointer on array of edges stored in PROGMEM */
SEdgeP  * eP;
/** pointer on list of edges stored in EEPROM */
SEdge   * eE;
/** pointer on directory inode pointer */
b32     eD;
u8 n_index;
SEdge   * e;	/* current edge */
/** current node */
SNode 	* n;	
/** pointer on path */
SParent * parent;
/** pointer on callback for Memory Card Nodes */
t_spider_cb * df_fnc;
u8 e_source;
u8 n_source;
/** index of default node */
u8 n_default;
u8 order;
u8 flag;
/** presenation style for Memory Card Nodes, #SPIDER_STYLE_LS, #SPIDER_STYLE_LS_L, #SPIDER_STYLE_LS_AL, #SPIDER_STYLE_UCS2 */
u8 style;
u16 item_from;
u16 pref_item;
} SCtx;

/**
Twofish key structure holding the internal state. Set up by twofish_setup().
\ingroup api_security
*/
typedef struct _Twofish_key {
	u32 K[40];
	u8 S[32], start;
} Twofish_key;


/**
\ingroup api_actions
Turbo handler type definition.
*/
  
typedef void (t_turbo_main)(u8 action, void *data);

/** 
\ingroup api_actions
Structure passed with #ACTION_MENU_SELECTION action to application.
inserted with insert_menu() (max. 4 items can be inserted by one application).
*/

typedef struct _Menu_selection_data {
/** item 	counted from 0,... with respect to the order menu items were */
	u8 item;
/** envelope	pointer on ENVELOPE in buf_B() */
	u8 * envelope;
} Menu_selection_data;

/** 
\ingroup api_actions
Structure passed with #ACTION_FILE_APDU action to application.
*/

typedef struct _File_apdu_data
{
/** APDU INS, i.e. command (SELECT, etc.) \ref cf_etsi_11_11. */
u8 ins;
/** APDU P1 */
u8 p1;
/** APDU P2 */
u8 p2;
/** APDU P3 */
u8 p3;
/** data of len P3 */
u8 * data;
/** current DF */
u16 df;
/** current EF */
u16 ef;
/** INS of previous APDU, #ME_CMD_SELECT, #ME_CMD_SEEK, #ME_CMD_RUN_GSM_ALGORITHM, #ME_CMD_INCREASE (used for #ME_CMD_GET_RESPONSE handling). */
u8 prev_ins;
} File_apdu_data;

typedef struct _Net_conf {
u8 bearer[6];
u8 * apn_addr;
u8 * user;
u8 * password;
u8 local_ipv4[4];
} Net_conf;

/** 
Network socket structure used for open_channel_raw().
\ingroup api_stk 
*/

typedef struct _Sockaddr {
/** #IPPROTO_UDP or #IPPROTO_TCP */
u8 proto;
/** destination port */
u16 port;
/** destination IPv4 address */
u8 addr[4];
} Sockaddr;

/**
Turbo SMS Access Control List.
\ingroup api_tsms
*/

typedef struct _TSMS_ACL
{
/**
Or'ed #TSMS_ACL_BLOCKED, #TSMS_ACL_EVERYONE
*/
u8 general;
/**
Access List itself implemeted as list of edges, each "To" node of these edges holds the allowed number.
*/
SEdge * access_list;
} TSMS_ACL;

#ifdef __cplusplus
}
#endif

#endif
