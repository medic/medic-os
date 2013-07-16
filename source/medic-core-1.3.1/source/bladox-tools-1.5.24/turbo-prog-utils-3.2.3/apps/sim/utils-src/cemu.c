/*
 * Turbo Programmer Utilities, turbo-prog-utils, www.bladox.com 
 *
 * Copyright (C) 2004 BLADOX, s.r.o.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "config.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

#include <tfslib.h>
#include <tproglib.h>
#include <cable.h>
#include <admin.h>

#include "../module-src/sim.h"

#ifdef WIN32_MINGW
#include <windows.h>
#endif

/*
 * cemu.c - cable utils emulation with Turbo Programmer
 *
 * Copyright (C) 2003 BLADOX, s.r.o.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

u8 ver;
u16 ota_cnt;

u8 sim_atr (u8 * buf)
{
  u16 len;

  usb_send (ACTION_SIM_ATR, 0, NULL, &len, buf);

  return len;
}

#define TO_SIM  	0
#define FROM_SIM        1

u16 sim (u8 ins, u8 p1, u8 p2, u8 p3, u8 * buf)
{
  u16 len = p3;
  u16 i;
  u8 dir = TO_SIM;
  u16 ret;
  u8 txBuf[2048];

  switch (ins)
  {
//FROM_SIM
    case ME_CMD_STATUS:
    case ME_CMD_READ_BINARY:
    case ME_CMD_READ_RECORD:
    case ME_CMD_GET_RESPONSE:
    case ME_CMD_FETCH:
      dir = FROM_SIM;
      break;
    default:
      break;
/*
        case ME_CMD_INVALIDATE:
        case ME_CMD_REHABILITATE:
        case ME_CMD_SLEEP:
*/
  }

  if (len == 0 && !(ins == ME_CMD_INVALIDATE || ins == ME_CMD_REHABILITATE))
    len = 0x100;
  if (dir == FROM_SIM)
    len = 0;

  //GSM command
  txBuf[0] = ins;
  txBuf[1] = p1;
  txBuf[2] = p2;
  txBuf[3] = p3;

  for (i = 0; i < len; i++)
    txBuf[4 + i] = buf[i];

/*
printf(">SIM(%d):\t a0 ", 4+len);
for(i=0;i<len+4;i++)printf("%02x ", txBuf[i]);
printf("\n");
*/

  usb_send (ACTION_SIM_CMD, 4 + len, txBuf, &len, buf);

/*
printf("<SIM(%d):\t ", len);
for(i=0;i<len;i++)printf("%02x ", buf[i]);
printf("\n");
printf("\n");
*/

  return len;
}

#define PROGMEM
u8 PROGMEM ef_sms_path[] = { 0x3f, 0x00, 0x7f, 0x10, 0x6f, 0x3c };

typedef struct _Glob_Mem
{
  u8 path[4 * 2];
  u8 depth;
  u16 size;
  u8 rec_len;
}
Glob_Mem;

Glob_Mem *glob_mem;

u16 sim_select (u8 * path, u8 len)
{
  u16 ret;
  u8 buf[200];
  u8 i = 0;

  while (i < len)
  {
    buf[0] = path[i * 2];
    buf[1] = path[i * 2 + 1];
    ret = sim (ME_CMD_SELECT, 0, 0, 2, buf);
    ret = ((u16) buf[0] << 8) | buf[1];
    if ((ret & 0x9f00) != 0x9f00)
      break;
    i++;
  }

  return ret;
}

void sim_read_bin (u16 size)
{
  u16 off;
  u16 i;
  u16 j;
  u16 ret;
  u16 display_len = 0x100;
  u8 buf[1000];

  printf ("Data: ");
  for (off = 0; off < size; off += display_len)
  {
    if (off + display_len > size)
      display_len = size - off;
    ret = sim (ME_CMD_READ_BINARY, off >> 8, off, display_len, buf);
    if (ret > 2)
    {
      ret -= 2;
      for (i = 0; i < ret; i++)
      {
	if (i % 16 == 0)
	  printf ("\n\t");
	printf ("%02X ", buf[i]);
      }
    }
    else
    {
      printf ("Read ERROR %02X%02X\n", buf[0], buf[1]);
      return;
    }
  }
  printf ("\n");
}

void sim_read_rec (u16 nr_rec, u16 rec_len)
{
  u8 i;
  u8 j;
  u16 ret;
  u8 buf[1000];

  for (i = 1; i <= nr_rec; i++)
  {
    printf ("Record %d:", i);
    ret = sim (ME_CMD_READ_RECORD, i, 0x04, rec_len, buf);
    if (ret > 2)
    {
      ret -= 2;
      for (j = 0; j < rec_len; j++)
      {
	if (j % 16 == 0)
	  printf ("\n\t");
	printf ("%02X ", buf[j]);
      }
    }
    else
    {
      printf ("Read ERROR %02X%02X\n", buf[0], buf[1]);
    }
    printf ("\n");
  }
}

u8 cable_sms_head[] =
  { 0x02, 0x91, 0x00, 0x11, 0x00, 0x00, 0x81, 0x00, 0x04, 0xFF };

b8 ch_write (u8 * s, u32 l)
{
  u8 buf[1000];
  u8 i, j;
  u16 ret;

//+CMGW, return 0 or pos i

  for (i = 0; i < 0xb0; i++)
    buf[i] = 0xff;
  buf[0] = 0x07;
  memcpy (buf + 1, cable_sms_head, sizeof (cable_sms_head));
  buf[1 + sizeof (cable_sms_head)] = l;
  for (i = 0; i < l; i++)
    buf[sizeof (cable_sms_head) + i + 2] = s[i];

  if (ver)
  {
    printf ("WRITE\n");
    for (j = 0; j < 0xb0; j++)
    {
      printf ("%02X ", buf[j]);
    }
    printf ("\n");
  }

  ret = sim (ME_CMD_UPDATE_RECORD, 0x01, 0x04, 0xB0, buf);
/*
  for (j = 0; j < ret; j++)
  {
    printf ("%02X ", buf[j]);
  }
  printf ("\n");
*/
  return 1;
}

u8 ch_read (u8 i2, u8 * s)
{
  u8 buf[1000];
  u8 i, j, l;
  u16 ret;

//+CMGR

  if (ver)
    printf ("READ\n");
  ret = sim (ME_CMD_READ_RECORD, 0x01, 0x04, 0xb0, buf);
  if (ver)
  {
    for (j = 0; j < ret; j++)
    {
      printf ("%02X ", buf[j]);
    }
    printf ("\n");
  }

  l = buf[1 + sizeof (cable_sms_head)];
//  printf ("LEN %d\n", l);
  for (i = 0; i < l; i++)
    s[i] = buf[2 + sizeof (cable_sms_head) + i];
//  printf ("ERR %02X\n", s[0]);

//+CMGD
  if (ver)
    printf ("DELETE\n");
  for (i = 0; i < 240; i++)
    buf[i] = 0xff;
  buf[0] = 0x00;

  ret = sim (ME_CMD_UPDATE_RECORD, 0x01, 0x04, 0xb0, buf);
/*
  for (j = 0; j < ret; j++)
  {
    printf ("%02X ", buf[j]);
  }
  printf ("\n");
  printf ("READ\n");
  ret = sim (ME_CMD_READ_RECORD, 0x01, 0x04, 0xb0, buf);
  for (j = 0; j < ret; j++)
  {
    printf ("%02X ", buf[j]);
  }
  printf ("\n");
*/
  return l;
}

b8 app_open ()
{
  u8 i, j;
  u8 *t;
  u8 res[1000];
  b32 f;
  b8 err;

  i = NO_ERROR;

  i = 0;
  t = res;
  *t = CABLE_UPLOAD_APP;
  t++;
  *t = CABLE_UPLOAD_APP_START;
  t++;

  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    err = res[0];
  }
  return err;
}

b8 app_append (u8 * data, u16 pos, u8 l)
{
  u8 i = NO_ERROR;
  u8 res[1000];
  u8 *t = res;
  b8 err;

  *t = CABLE_UPLOAD_APP;
  t++;
  *t = CABLE_UPLOAD_APP_CONT;
  t++;
  *t = l;
  t++;
  for (i = 0; i < l; i++)
  {
    *t = data[pos + i];
    t++;
  }
  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    err = res[0];
  }
  return err;
}

b8 app_close (u8 * name)
{
  u8 i = NO_ERROR;
  u8 res[1000];
  u8 *t = res;
  u8 l = strlen (name);
  b8 err;

  *t = CABLE_UPLOAD_APP;
  t++;
  *t = CABLE_UPLOAD_APP_END;
  t++;

  for (i = 0; i < l; i++)
  {
    *t = name[i];
    t++;
  }
  *t = '\0';
  t++;
  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    err = res[0];
  }
  return err;
}

#ifndef CABLE_TWIN_APDU_T1
#define CABLE_TWIN_APDU_T1 0x12
#endif

b8 cable_twin_t1_apdu (u8 tx_len, u8 *buf)
{
  u8 i, j, k, l;
  u8 *t;
  u8 res[1000];
  b8 err;

  i = 0;
  t = res;
  *t = CABLE_TWIN_APDU_T1;
  t++;
*t++=tx_len;
printf("Twin TX: ");
for(i=0;i<tx_len;i++)
{
printf("%02X ", buf[i]);
*t++=buf[i];
}
printf("\n");

  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    j = err;

    err = res[0];
    if (err == NO_ERROR)
    {

printf("Twin RX: ");

for(i=1;i<j;i++)
printf("%02X ", res[i]);

printf("\n");
    }
  }
  return err;
}


b8 cable_twin_txrx (u8 tx_len, u8 *buf, u8 rx_len)
{
  u8 i, j, k, l;
  u8 *t;
  u8 res[1000];
  b8 err;

  i = 0;
  t = res;
#ifndef CABLE_TWIN_TXRX
#define CABLE_TWIN_TXRX 0x13
#endif
  *t = CABLE_TWIN_TXRX;
  t++;
*t++=tx_len;
*t++=rx_len;
printf("Twin TX: ");
for(i=0;i<tx_len;i++)
{
printf("%02X ", buf[i]);
*t++=buf[i];
}
printf("\n");

  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    j = err;

    err = res[0];
    if (err == NO_ERROR)
    {

printf("Twin RX: ");

for(i=1;i<j;i++)
printf("%02X ", res[i]);

printf("\n");
    }
  }
  return err;
}

u8 cmp_edc(u8 *buf, u8 len)
{
u8 i;
u8 x=0;
for(i=0;i<len;i++)x=x^buf[i];
//printf("EDC: %02X\n", x);
return x;
}


b8 cable_twin_on ()
{
  u8 i, j, k, l;
  u8 *t;
  u8 res[1000];
  b8 err;

  i = 0;
  t = res;
#ifndef CABLE_TWIN_ON
#define CABLE_TWIN_ON 0x11
#endif
  *t = CABLE_TWIN_ON;
  t++;

  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    j = err;



    err = res[0];
    if (err == NO_ERROR)
    {

printf("Twin ATR: ");

for(i=1;i<j;i++)
printf("%02X ", res[i]);

printf("\n");

#if 0
{
// T1 
u8 buf[1000];

i=0;

#if 0
// fused
buf[i++]=0xff;
buf[i++]=0x11;
buf[i++]=0x14;
buf[i++]=0xfa;
#endif

// unfused
buf[i++]=0xff;
buf[i++]=0x01;
buf[i++]=0xfe;

//buf[i++]=cmp_edc(&buf[0], i);
cable_twin_txrx(i, buf, 3);
}
#endif

#if 0
{
// T1-2 
u8 buf[1000];

i=0;
buf[i++]=0x00;
buf[i++]=0xc1;
buf[i++]=0x01;
buf[i++]=0xfe;
buf[i++]=0x3e;

//buf[i++]=cmp_edc(&buf[0], i);
cable_twin_txrx(i, buf, 5);
}
#endif


#if 0
{
u8 buf[1000];

i=0;
buf[i++]=0x00;  // ICC cmd CT->ICC
buf[i++]=0x00;  // PCB
buf[i++]=0x0e;

buf[i++]=0x00;
buf[i++]=0xa4;
buf[i++]=0x04;
buf[i++]=0x00;
buf[i++]=0x09;
buf[i++]=0xa0;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x01;
buf[i++]=0x67;
buf[i++]=0x41;
buf[i++]=0x30;
buf[i++]=0x00;
buf[i++]=0xff;

#if 0
// SEC DOMAIN
buf[i++]=0x00;
buf[i++]=0xa0;
buf[i++]=0x04;
buf[i++]=0x00;
buf[i++]=0x08;

buf[i++]=0xa0;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x03;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
#endif

buf[i++]=cmp_edc(&buf[0], i);

cable_twin_txrx(i, buf, 0x19);

//cable_twin_t1_apdu(i, buf);
}
#endif

{
u8 buf[1000];

i=0;
//buf[i++]=0x00;  // ICC cmd CT->ICC
//buf[i++]=0x00;  // PCB
//buf[i++]=0x0e;

buf[i++]=0x00;
buf[i++]=0xa4;
buf[i++]=0x04;
buf[i++]=0x00;
buf[i++]=0x09;
buf[i++]=0xa0;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x01;
buf[i++]=0x67;
buf[i++]=0x41;
buf[i++]=0x30;
buf[i++]=0x00;
buf[i++]=0xff;

#if 0
// SEC DOMAIN
buf[i++]=0x00;
buf[i++]=0xa0;
buf[i++]=0x04;
buf[i++]=0x00;
buf[i++]=0x08;

buf[i++]=0xa0;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x03;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
#endif

//buf[i++]=cmp_edc(&buf[0], i);

//cable_twin_txrx(i, buf, 0x19);

cable_twin_t1_apdu(i, buf);
}

{
u8 buf[1000];

i=0;
//buf[i++]=0x00;  // ICC cmd CT->ICC
//buf[i++]=0x00;  // PCB
//buf[i++]=0x0e;

buf[i++]=0x00;
buf[i++]=0xa4;
buf[i++]=0x04;
buf[i++]=0x00;
buf[i++]=0x09;
buf[i++]=0xa0;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x01;
buf[i++]=0x67;
buf[i++]=0x41;
buf[i++]=0x30;
buf[i++]=0x00;
buf[i++]=0xff;

#if 0
// SEC DOMAIN
buf[i++]=0x00;
buf[i++]=0xa0;
buf[i++]=0x04;
buf[i++]=0x00;
buf[i++]=0x08;

buf[i++]=0xa0;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x03;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x00;
#endif

//buf[i++]=cmp_edc(&buf[0], i);

//cable_twin_txrx(i, buf, 0x19);

cable_twin_t1_apdu(i, buf);
}
    }
  }
  return err;
}


b8 cable_info ()
{
  u8 i, j, k, l;
  u8 *t;
  u8 res[1000];
  b8 err;

  i = 0;
  t = res;
#ifndef CABLE_INFO
#define CABLE_INFO 0x10
#endif
  *t = CABLE_INFO;
  t++;

  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    j = err;
    err = res[0];
    if (err == NO_ERROR)
    {
      i = 1;
#ifndef CABLE_INFO_VERSION
#define CABLE_INFO_VERSION      0x01
#endif
      while (i < j)
      {
#ifndef CABLE_INFO_SN
#define CABLE_INFO_SN           0x02
#endif

	if (res[i] == CABLE_INFO_VERSION)
	{
	  i++;
	  printf ("Kernel Version %d.%d.%d.%d\n", res[i + 1], res[i + 2],
		  res[i + 3], res[i + 4]);
	  i += res[i] + 1;
	}
	else
	if (res[i] == CABLE_INFO_SN)
	{
	  i++;
	  printf ("Serial Number ");
	  l = res[i];
	  for (k = 0; k < l; k++)
	    printf ("%c", res[i + 1 + k]);
	  printf ("\n");
	  i += l + 1;
	}
	else
	{
	  printf ("Unknown Tag %02x: ", res[i]);
	  i++;
	  l = res[i];
	  for (k = 0; k < l; k++)
	    printf ("%02x", res[i + 1 + k]);
	  printf ("\n");
	  i += l + 1;
	}
      }
    }
  }
  return err;
}

b8 cable_reset ()
{
  u8 i, j;
  u8 *t;
  u8 res[1000];
  b8 err;

  i = 0;
  t = res;
#ifndef CABLE_RESET_DEFAULT
#define CABLE_RESET_DEFAULT 0x0e
#endif
  *t = CABLE_RESET_DEFAULT;
  t++;

  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    err = res[0];
  }
  return err;
}

b8 cable_clear_apps ()
{
  u8 i, j;
  u8 *t;
  u8 res[1000];
  b8 err;

  i = 0;
  t = res;
#ifndef CABLE_CLEAR_APPS
#define CABLE_CLEAR_APPS 0x0d
#endif
  *t = CABLE_CLEAR_APPS;
  t++;

  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    err = res[0];
  }
  return err;
}

void do_stk_init ()
{
  u16 ret;
  u8 buf[255];
  u8 i = 0;
  u8 j = 0;

// TERMINAL PROFILE
  u8 t_p[] = { 0x7f, 0xff, 0xff, 0xff,
    0x7f, 0x0f, 0x00, 0xdf, 0x7f, 0x00, 0x00,
    0x1f, 0x23, 0x00, 0x00, 0x00, 0x03
  };
  memcpy (buf, t_p, sizeof (t_p));
  ret = sim (ME_CMD_TERMINAL_PROFILE, 0, 0, sizeof (t_p), buf);

  ret = sim (ME_CMD_FETCH, 0, 0, buf[1], buf);
  u8 t_r1[] =
    { 0x81, 0x03, 0x05, 0x25, 0x00, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01,
0x00 };
  memcpy (buf, t_r1, sizeof (t_r1));
  ret = sim (ME_CMD_TERMINAL_RESPONSE, 0, 0, sizeof (t_r1), buf);

  ret = sim (ME_CMD_FETCH, 0, 0, buf[1], buf);
  u8 t_r2[] =
    { 0x81, 0x03, 0x05, 0x05, 0x00, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01,
0x00 };
  memcpy (buf, t_r2, sizeof (t_r2));
  ret = sim (ME_CMD_TERMINAL_RESPONSE, 0, 0, sizeof (t_r2), buf);
}

int do_ota_msg(u8 flags, u8 len, u8 *msg, u8 *res)
{
  u16 ret;
  u8 i = 0;
  u8 j = 0;
  u8 s[1000];
  u8 buf[1000];

/*
example
FROM ME:  a0 c2 00 00 28
APDU ENVELOPE
FROM ME:  d1 26 82 02 83 81 06 07 91 24 60 80 00 65 18 8b 17 24 0c 91 24 70 47 70 64 96 7f f6 90 30 10 91 73 75 40 04 4f 54 41 10
BER_TLV_ME_TO_SIM_SMS_PP_DOWNLOAD
T_DEVICE_ID
  SRC 83
  DST 81
T_ADDRESS
  Len: 07
  TON/NPI 91
  Dialing number: 420608005681
T_SMS_TPDU
SMS TYPE TP_MTI_DELIVER
*/

  u8 b_h[] = {
    0xd1, 0x81, 0x26, 0x82, 0x02, 0x83, 0x81, 0x06, 0x07, 0x91, 0x24, 0x10, 0x32, 0x54, 0x76, 0x98,
    0x8b, 0x81, 0x00,
    0x24, 0x0c, 0x91, 0x24, 0x70, 0x98, 0x21, 0x43, 0x65,
    0x7f, 0xf6,
    0x90, 0x30, 0x10, 0x91, 0x73, 0x75, 0x40
  };

  s[0] = 0;
  s[1] = 0;			//crc
  s[2] = len + 6;		//len
  s[3] = 0;			//rnd
  s[4] = flags;			//flags
  s[5] = 0;
  s[6] = 0;			//rnd
  s[7] = ota_cnt;			//cnt_l
  s[8] = ota_cnt>>8;		//cnt_h

  for (i = 0; i < len; i++)
    s[9 + i] = msg[i];

  for (i = 0; i < s[2] + 1; i++)
    s[1] ^= s[2 + i];

  len = 140;

  printf (">SMS(%d):\t", s[2] + 3);
  for (i = 0; i < s[2] + 3; i++)
    printf ("%02x ", s[i]);
  printf ("\n");

  j = 0;
  for (i = 0; i < sizeof (b_h); i++)
    buf[j++] = b_h[i];
  buf[j++] = len;
  for (i = 0; i < len; i++)
    buf[j++] = s[i];
  buf[18] = j - 18;
  buf[2] = j - 3;

  ret = sim (ME_CMD_ENVELOPE, 0, 0, j, buf);
  ret = ((u16) buf[0] << 8) | buf[1];
  if (buf[0] == 0x9f)
    {
      ret = sim (ME_CMD_GET_RESPONSE, 0, 0, buf[1], buf);
      buf[0] = buf[ret - 2];
      buf[1] = buf[ret - 1];
    }

X_F:
  if (buf[0] == 0x91)
    {
      u8 err, l, k;
      ret = sim (ME_CMD_FETCH, 0, 0, buf[1], buf);
i=5;
if(buf[1]==0x81)i++;
i=buf[i];

  u8 t_r2[] =
    { 0x81, 0x03, 0x01, 0x13, 0x00, 0x82, 0x02, 0x81, 0x88, 0x83, 0x01,
0x00 };
t_r2[3]=i;
  ret = sim (ME_CMD_TERMINAL_RESPONSE, 0, 0, sizeof (t_r2), t_r2);
if(i!=0x13)
{
      buf[0] = t_r2[ret - 2];
      buf[1] = t_r2[ret - 1];

goto X_F;
}


      j = buf[28];
      for (i = 0; i < j; i++)
	res[i] = buf[29 + i];

      printf ("<SMS(%d):\t", j);
      for (i = 0; i < j; i++)
	  printf ("%02x ", res[i]);
      printf ("\n\n");
    }

  return j;
}

b8 ota_info ()
{
  u8 res[255];
  u8 s[255];
  u8 i = 0;
  u8 j = 0;
  u8 err, k, l;

  s[0] = 0x10;
  j = do_ota_msg (0x80, 1, s, res);

  err = res[0];
  if (err == NO_ERROR)
    {
      i = 1;
#ifndef CABLE_INFO_VERSION
#define CABLE_INFO_VERSION      0x01
#endif
      while (i < j)
	{
#ifndef CABLE_INFO_SN
#define CABLE_INFO_SN           0x02
#endif

	  if (res[i] == CABLE_INFO_VERSION)
	    {
	      i++;
	      printf ("Kernel Version:\t %d.%d.%d.%d\n", res[i + 1], res[i + 2],
		      res[i + 3], res[i + 4]);
	      i += res[i] + 1;
	    }
	  else if (res[i] == CABLE_INFO_SN)
	    {
	      i++;
	      printf ("Serial Number:\t ");
	      l = res[i];
	      for (k = 0; k < l; k++)
		printf ("%c", res[i + 1 + k]);
	      printf ("\n");
	      i += l + 1;
	    }
	  else if (res[i] == ADMIN_INFO_COUNTER)
	    {
	      i++;
	      printf ("Counter:\t ");
	      l = res[i];
ota_cnt=res[i + 1 + 0];
ota_cnt|=(u16)res[i + 1 + 1]<<8;
	      for (k = 0; k < l; k++)
		printf ("%02x", res[i + 1 + k]);
	      printf ("\n");
	      i += l + 1;
	    }

	  else if (res[i] == ADMIN_INFO_APP)
	    {
		u16 x;
	      i++;
	      printf ("App:\t\t ");
	      l = res[i + 1 + 7];
	      for (k = 0; k < l; k++)
		printf ("%c", res[i + 1 + 8+k]);
	      printf ("\t");
	      printf ("PS: %02x ", res[i + 1 + 0]);
		x=res[i + 1 + 1]; x<<=8; x|=res[i + 1 + 2];
	      printf ("PL: %04x ", x);
		x=res[i + 1 + 3]; x<<=8; x|=res[i + 1 + 4];
	      printf ("RS: %04x ", x);
		x=res[i + 1 + 5]; x<<=8; x|=res[i + 1 + 6];
	      printf ("RL: %04x ", x);
	      printf ("\n");
	      i += res[i] + 1;
	    }
	  else
	    {
switch(res[i])
{
case 0x0d:
	  printf ("BTLDR Conf:\t ");
	break;
case 0x0c:
	  printf ("Nonce:\t\t ");
	break;
case 0x0b:
	  printf ("Hardware:\t ");
	break;
case 0x03:
	  printf ("BTLDR Ver:\t ");
	break;
case 0x04:
	  printf ("Term Profile:\t ");
	break;
case 0x05:
	  printf ("ATR:\t\t ");
	break;
case 0x06:
	  printf ("SIM Info:\t ");
	break;
default:
	      printf ("Unknown Tag %02x:\t ", res[i]);
	break;
}
	      i++;
	      l = res[i];
	      for (k = 0; k < l; k++)
		printf ("%02x", res[i + 1 + k]);
	      printf ("\n");
	      i += l + 1;
	    }
	}
    }
printf("\n");

  return 0;
}

b8 ota_rm_app (u8 d)
{
  u8 res[255];
  u8 s[255];
  u8 i = 0;
  u8 j = 0;
  u8 err, k, l;

ota_info();

  s[0] = ADMIN_REMOVE_APP;
  s[1] = 1;
  s[2] = d;
  do_ota_msg (0x80, 3, s, res);
  ota_cnt++;

  return 0;
}

b8 ota_btldr (u8 len, u8 *in)
{
  u8 res[255];
  u8 s[255];
  u8 i = 0;
  u8 j = 0;
  u8 err, k, l;

printf("D: ");
for(i=0;i<len;i++)
printf("%02x ", s[i]);
printf("\n");
//return 0;

ota_info();

  s[0] = ADMIN_BTLDR_SET;
s[1]=len;
for(i=0;i<len;i++)s[2+i]=in[i];
  do_ota_msg (0x80, 2+len, s, res);

  return 0;
}


b8 ota_reset ()
{
  u8 res[255];
  u8 s[255];
  u8 i = 0;
  u8 j = 0;
  u8 err, k, l;

ota_info();

  s[0] = CABLE_RESET_DEFAULT;
  do_ota_msg (0x00, 1, s, res);

  return 0;
}

b8 ota_app (u8 * s_buf, u16 s_size, u8 *s_name)
{
  u8 res[255];
  u8 s[255];
  u8 i = 0;
  u8 j = 0;
  u8 err, k, l;
  b8 f;
  u16 len, pos;

  ota_info ();

  s[0] = CABLE_UPLOAD_APP;
  s[1] = CABLE_UPLOAD_APP_START;
  do_ota_msg (0x80, 2, s, res);
  ota_cnt++;
  if (res[0] != 0)
    goto E_RR;

  pos = 0;

  while (pos < s_size)
    {
      len = s_size - pos;
      if (len > 128)
	len = 128;
      j = 0;
      s[j++] = CABLE_UPLOAD_APP;
      s[j++] = CABLE_UPLOAD_APP_CONT;
      s[j++] = len;
      for (i = 0; i < len; i++)
	s[j++] = s_buf[pos + i];
      do_ota_msg (0x80, j, s, res);
      ota_cnt++;
      if (res[0] != 0)
	goto E_RR;

      pos += 128;
      printf ("Uploaded %d/%d bytes\n", (pos > s_size) ? s_size : pos,
	      s_size);
    }
  j = 0;
  s[j++] = CABLE_UPLOAD_APP;
  s[j++] = CABLE_UPLOAD_APP_END;
//  s[j++] = len;
  for (i = 0; i < strlen (s_name) + 1; i++)
    s[j++] = s_name[i];
  do_ota_msg (0x80, j, s, res);
  ota_cnt++;

E_RR:
  return res[0];
}


b8 ota_sms(int len, u8 * s)
{
 u16 ret;
 u8 buf[255];
 u8 i = 0;
u8 j=0;


// TERMINAL PROFILE
u8 t_p[]={0x7f, 0xff, 0xff, 0xff,
0x7f, 0x0f, 0x00, 0xdf, 0x7f, 0x00, 0x00,
0x1f, 0x23, 0x00, 0x00, 0x00, 0x03};
memcpy(buf, t_p, sizeof(t_p));
ret = sim (ME_CMD_TERMINAL_PROFILE, 0, 0, sizeof(t_p), buf);

ret = sim (ME_CMD_FETCH, 0, 0, buf[1], buf);
u8 t_r1[]={0x81, 0x03, 0x09, 0x25, 0x00, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
memcpy(buf, t_r1, sizeof(t_r1));
ret = sim (ME_CMD_TERMINAL_RESPONSE, 0, 0, sizeof(t_r1), buf);

ret = sim (ME_CMD_FETCH, 0, 0, buf[1], buf);
u8 t_r2[]={0x81, 0x03, 0x05, 0x05, 0x00, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
memcpy(buf, t_r2, sizeof(t_r2));
ret = sim (ME_CMD_TERMINAL_RESPONSE, 0, 0, sizeof(t_r2), buf);

// ENVELOPE
/*
example
FROM ME:  a0 c2 00 00 28
APDU ENVELOPE
FROM ME:  d1 26 82 02 83 81 06 07 91 24 60 80 00 65 18 8b 17 24 0c 91 24 70 47 70 64 96 7f f6 90 30 10 91 73 75 40 04 4f 54 41 10
BER_TLV_ME_TO_SIM_SMS_PP_DOWNLOAD
T_DEVICE_ID
  SRC 83
  DST 81
T_ADDRESS
  Len: 07
  TON/NPI 91
  Dialing number: 420608005681
T_SMS_TPDU
SMS TYPE TP_MTI_DELIVER
*/

u8 b_h[]={
0xd1, 0x26, 0x82, 0x02, 0x83, 0x81, 0x06, 0x07, 0x91, 0x24, 0x10, 0x32, 0x54, 0x76, 0x98, 
0x8b, 0x00,  
0x24, 0x0c, 0x91, 0x24, 0x70, 0x98, 0x21, 0x43, 0x65, 
0x7f, 0xf6, 
0x90, 0x30, 0x10, 0x91, 0x73, 0x75, 0x40 
//04 4f 54 41 10
};

// optional GET RESPONSE
// FETCH
// TERMINAL RESPONSE

s[0]=0;
s[1]=0;	//crc
s[2]=10;	//len
s[3]=0;	//rnd
//s[4]=0x40|0x80;	//flags
s[4]=0x80;	//flags
s[5]=0;
s[6]=0;	//rnd
s[7]=0;	//cnt_l
s[8]=0;	//cnt_h
s[9]=0x10;	// cmd
s[10]=0;
s[11]=0;
s[12]=0;

for(i=0;i<s[2]+1;i++)s[1]^=s[2+i];

len=140;

printf(">SMS(%d):\t",s[2]+3);
for(i=0;i<s[2]+3;i++)
printf("%02X ", s[i]);
printf("\n");

j=0;
for(i=0;i<sizeof(b_h);i++) buf[j++]=b_h[i];
buf[j++]=len;
for(i=0;i<len;i++) buf[j++]=s[i];
buf[16]=j-16;
buf[1]=j-2;

ret = sim (ME_CMD_ENVELOPE, 0, 0, j, buf);
ret = ((u16) buf[0] << 8) | buf[1];
if(buf[0]==0x9f)
{
ret = sim (ME_CMD_GET_RESPONSE, 0, 0, buf[1], buf);
buf[0]=buf[ret-2];
buf[1]=buf[ret-1];
}

if(buf[0]==0x91)
{
u8 * res;
u8 err,l,k;
ret = sim (ME_CMD_FETCH, 0, 0, buf[1], buf);

    j = buf[28];
res=&buf[29];
printf("<SMS(%d):\t",j);
for(i=0;i<j;i++)
printf("%02X ", res[i]);
printf("\n\n");

    err = res[0];
    if (err == NO_ERROR)
    {
      i = 1;
#ifndef CABLE_INFO_VERSION
#define CABLE_INFO_VERSION      0x01
#endif
      while (i < j)
      {
#ifndef CABLE_INFO_SN
#define CABLE_INFO_SN           0x02
#endif

	if (res[i] == CABLE_INFO_VERSION)
	{
	  i++;
	  printf ("Kernel Version %d.%d.%d.%d\n", res[i + 1], res[i + 2],
		  res[i + 3], res[i + 4]);
	  i += res[i] + 1;
	}
	else
	if (res[i] == CABLE_INFO_SN)
	{
	  i++;
	  printf ("Serial Number ");
	  l = res[i];
	  for (k = 0; k < l; k++)
	    printf ("%c", res[i + 1 + k]);
	  printf ("\n");
	  i += l + 1;
	}
	else
	{
	  printf ("Unknown Tag %02x: ", res[i]);
	  i++;
	  l = res[i];
	  for (k = 0; k < l; k++)
	    printf ("%02x", res[i + 1 + k]);
	  printf ("\n");
	  i += l + 1;
	}
      }
    }

}

return 0;
}

b8 cable_lock ()
{
  u8 i, j;
  u8 *t;
  u8 res[1000];
  b8 err;

  i = 0;
  t = res;
#ifndef CABLE_LOCK_TURBO
#define CABLE_LOCK_TURBO 0x0f
#endif
  *t = CABLE_LOCK_TURBO;
  t++;

  err = ch_write (res, t - res);
  if (err > 0)
  {
    err = ch_read (err, res);
    err = res[0];
  }
  return err;
}

void print_result (b8 r)
{
//printf("RESULT: ");fflush(stdout);
  printf ("\n");
  switch (r)
  {
    case -1:
      printf ("Mobile Phone/Serial Cable Communication Error\n");
      break;
    case NO_ERROR:
      printf ("OK. No Error\n");
      break;
    case ERR_NO_PROGMEM:
      printf ("ERROR: Not Enough PROGMEM\n");
      break;
    case ERR_NO_RAM:
      printf ("ERROR: Not Enough RAM\n");
      break;
    case ERR_UNKNOWN:
      printf ("ERROR: Unknown Error\n");
      break;
    case ERR_NO_APP_SLOT:
      printf ("ERROR: No Application Slot\n");
      break;
    case ERR_MC_NOT_PRESENT:
      printf ("ERROR: Memory Card not present\n");
      break;
    case ERR_MC_NOT_READY:
      printf ("ERROR: Not Enough EEPROM\n");
      break;
    case ERR_BAD_FILE:
      printf ("ERROR: Bad File\n");
      break;
#ifndef ERR_TURBO_LOCKED
#define ERR_TURBO_LOCKED                0x48
#endif
    case ERR_TURBO_LOCKED:
      printf ("ERROR: Turbo Locked\n");
      break;
    default:
      printf ("Unknown Error\n");
      break;
  }
}

/*
ret 0 if unblocked
ret 1 if blocked
*/

u8 unblock (u8 * puk, u8 * pin)
{
  u8 buf[1000];
  u8 i;
  u8 l_pin = strlen (pin);
  u8 l_puk = strlen (puk);
  u16 ret;

  if (l_pin > 8)
  {
    printf ("PIN longer than 8 chars ERROR\n");
    return 1;
  }

  if (l_puk > 8)
  {
    printf ("PUK longer than 8 chars ERROR\n");
    return 1;
  }

  for (i = 0; i < 8; i++)
  {
    if (i < l_pin)
    {
      if (pin[i] < '0' || pin[i] > '9')
      {
	printf ("PIN wrong char '%c' ERROR\n", pin[i]);
	return 1;
      }
      buf[i + 8] = pin[i];
    }
    else
      buf[i + 8] = 0xff;

    if (i < l_puk)
    {
      if (puk[i] < '0' || puk[i] > '9')
      {
	printf ("PUK wrong char '%c' ERROR\n", puk[i]);
	return 1;
      }
      buf[i] = puk[i];
    }
    else
      buf[i] = 0xff;
  }

// SELECT 3f00
// STATUS
// VERIFY
  ret = sim (ME_CMD_UNBLOCK_CHV, 0x00, 0x00, 0x10, buf);
  printf ("Unblock answer: ");
  for (i = 0; i < ret; i++)
    printf ("%02X ", buf[i]);
  printf ("\n");
  if (buf[0] != 0x90 || buf[1] != 0x00)
  {
    printf ("Wrong PUK ERROR\n");
    return 1;
  }
// STATUS
  printf ("Unlocked with PUK %s OK\n", puk);
  printf ("New PIN is %s\n", pin);
  return 0;
}

/*
ret 0 if unlocked
ret 1 if locked
*/

u8 unlock (u8 * pin, u8 what)
{
  u8 buf[1000];
  u8 i;
  u8 l = strlen (pin);
  u16 ret;

  if (l > 8)
  {
    if (what == 0x01)
      printf ("PIN longer than 8 chars ERROR\n");
    else
      printf ("CHV2 longer than 8 chars ERROR\n");
    return 1;
  }

  for (i = 0; i < 8; i++)
  {
    if (i < l)
    {
      if (pin[i] < '0' || pin[i] > '9')
      {
	if (what == 0x01)
	  printf ("PIN wrong char '%c' ERROR\n", pin[i]);
	else
	  printf ("CHV2 wrong char '%c' ERROR\n", pin[i]);
	return 1;
      }
      buf[i] = pin[i];
    }
    else
      buf[i] = 0xff;
  }

// SELECT 3f00
// STATUS
// VERIFY
  ret = sim (ME_CMD_VERIFY_CHV, 0x00, what, 0x08, buf);
  printf ("Unlock answer: ");
  for (i = 0; i < ret; i++)
    printf ("%02X ", buf[i]);
  printf ("\n");
  if (buf[0] != 0x90 || buf[1] != 0x00)
  {
    if (what == 0x01)
      printf ("Wrong PIN ERROR\n");
    else
      printf ("Wrong CHV2 ERROR\n");
    return 1;
  }
// STATUS
  if (what == 0x01)
    printf ("Unlocked with PIN %s OK\n", pin);
  else
    printf ("Unlocked with CHV2 %s OK\n", pin);
  return 0;
}

void help ()
{
  printf ("Usage: cemu [args]\n");
  printf
    ("One of the --app, --info, --lock, --reset, --rm-apps, --remove-app, --btldr has to bee given.\n");
  printf ("--app file\tTurbo: upload application\n");
  printf ("--info\t\tTurbo: display version (requires kernel>=1.0.0.35)\n");
  printf ("--lock\t\tTurbo: lock (requires kernel>=1.0.0.34)\n");
  printf ("--reset\t\tTurbo: reset (requires kernel>=1.0.0.34)\n");
  printf ("--rm-apps\tTurbo: remove applications\n");
  printf ("--btldr settings\tTurbo: tune bootloader settings, only with --ota\n");
  printf ("--remove-app x\tTurbo: remove applications #PS, see --info, only with --ota\n\n");

  printf ("--ota\tUse OTA SMS - DATADOWNLOAD protocol\n");
  printf ("--sms file.sms\tsend file as OTA SMS - DATADOWNLOAD\n\n");

  printf ("-a application\tuse application, default is %s\n", APP_SIM);
  printf ("-r\t\tprint ATR only and exit\n");
  printf ("-d device\tuse USB device\n");
  printf ("--pin pin\tuse PIN to unlock\n");
  printf ("--puk pin\tuse PUK to set a new PIN, --pin has to be set\n");
  printf ("-t\t\ttwin\n");
  printf ("-h\t\thelp\n");
  printf ("-v\t\tverbose\n");
  exit (-1);
}

int main (int argc, char **argv)
{
  u16 i;
  int len;
  int result;
  u8 *dev = USBDEVICE;
  u8 buf[1000];
  u8 atr_only = 0;
  u8 *pin = NULL;
  u8 *puk = NULL;
  u8 locked = 0;
  u8 *app_name = APP_SIM;
  u8 answer[2048];
  u16 answerLen;
  u8 cmd_given = 0;
  u8 cmd = 0;
  u8 *file_app = NULL;
  u8 err = 0;
u8 twin=0;
u8 use_ota=0;

ota_cnt=0;

#define CMD_INFO	0x01
#define CMD_RESET	0x02
#define CMD_LOCK	0x03
#define CMD_RMAPPS	0x04
#define CMD_APP		0x05
#define CMD_SMS		0x06
#define CMD_RM_APP	0x07
#define CMD_BTLDR	0x08

  ver = 0;

/*
info
reset
lock
rm-apps
app
*/

  while (1)
  {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"pin", 1, 0, 0},
      {"puk", 1, 0, 0},
      {"info", 0, 0, 0},
      {"lock", 0, 0, 0},
      {"reset", 0, 0, 0},
      {"rm-apps", 0, 0, 0},
      {"app", 1, 0, 0},
      {"sms", 1, 0, 0},
      {"ota", 0, 0, 0},
      {"remove-app", 1, 0, 0},
      {"btldr", 1, 0, 0},
      {0, 0, 0, 0}
    };

    result = getopt_long (argc, argv, "hd:ra:vt", long_options, &option_index);
    if (result == -1)
      break;
    switch (result)
    {
      case 0:
	switch (option_index)
	{
	  case 0:
	    pin = optarg;
	    break;
	  case 1:
	    puk = optarg;
	    break;
	  case 2:
	    cmd = CMD_INFO;
	    cmd_given++;
	    break;
	  case 3:
	    cmd = CMD_LOCK;
	    cmd_given++;
	    break;
	  case 4:
	    cmd = CMD_RESET;
	    cmd_given++;
	    break;
	  case 5:
	    cmd = CMD_RMAPPS;
	    cmd_given++;
	    break;
	  case 6:
	    cmd = CMD_APP;
	    file_app = optarg;
	    cmd_given++;
	    break;
	  case 7:
	    cmd = CMD_SMS;
	    file_app = optarg;
	    cmd_given++;
	    break;
	  case 8:
	    use_ota=1;
	    break;
	  case 9:
  	    use_ota=1;
	    cmd = CMD_RM_APP;
	    file_app = optarg;
	    cmd_given++;
	    break;
	  case 10:
  	    use_ota=1;
	    cmd = CMD_BTLDR;
	    file_app = optarg;
	    cmd_given++;
	    break;

	}
	break;
      case 'd':
	dev = optarg;
	break;
      case 'r':
	atr_only = 1;
	break;
      case 't':
	twin = 1;
	break;
      case 'a':
	app_name = optarg;
	break;
      case 'v':
	verbose_inc ();
	ver++;
	break;
      case 'h':
      default:
	help ();
	break;
    }
  }

  if (cmd_given != 1 && twin==0)
  {
    help ();
  }

  glob_mem = malloc (sizeof (Glob_Mem));
  glob_mem->depth = 0;

  if (puk && pin == NULL)
  {
    printf ("--pin missing\n");
    help ();
  }

  open_device (dev);
  if (select_app (app_name) != NO_ERROR)
  {
    close_device ();
    fprintf (stderr, "Cannot open application %s\n", app_name);
    exit (-1);
  }

  usb_send (ACTION_SIM_INIT, 0, NULL, &answerLen, answer);
  printf ("SIM mode initialized, %s\n", app_name);

  len = sim_atr (buf);
  if (len == 1)
  {
    printf ("NO SIM\n");
    err = 1;
    goto X_END;
  }

  printf ("SIM_ATR: ");
  for (i = 0; i < len; i++)
    printf ("%02X ", buf[i]);
  printf ("\n");


  {
    u8 buf[1000];

    buf[0] = 0x7f;
    buf[1] = 0x20;
    sim (ME_CMD_SELECT, 0, 0, 0x02, buf);
    sim (ME_CMD_STATUS, 0, 0, 0x17, buf);
/*
    buf[0] = 0x6f;
    buf[1] = 0xae;
    sim (ME_CMD_SELECT, 0, 0, 0x02, buf);
    sim (ME_CMD_READ_BINARY, 0, 0, 0x01, buf);
    buf[0] = 0x3f;
    buf[1] = 0x00;
    sim (ME_CMD_SELECT, 0, 0, 0x02, buf);
    buf[0] = 0x7f;
    buf[1] = 0x20;
    sim (ME_CMD_SELECT, 0, 0, 0x02, buf);
    buf[0] = 0x6f;
    buf[1] = 0x05;
    sim (ME_CMD_SELECT, 0, 0, 0x02, buf);
    sim (ME_CMD_GET_RESPONSE, 0, 0, 0x0e, buf);
    sim (ME_CMD_READ_BINARY, 0, 0, 0x05, buf);
    sim (ME_CMD_STATUS, 0, 0, 0x17, buf);
*/
  }

  if (puk)
    locked = unblock (puk, pin);

  if (pin && locked == 0)
    locked = unlock (pin, 1);

if(twin && atr_only)
{
    printf ("\n");
    sim_select (ef_sms_path, 3);
	print_result (cable_twin_on ());
goto X_END;
}


  if (atr_only == 0 && locked == 0)
  {
    printf ("\n");
    sim_select (ef_sms_path, 3);
    if(use_ota)do_stk_init();

    switch (cmd)
    {
      case CMD_INFO:
if(use_ota)
	print_result (ota_info ());
else
	print_result (cable_info ());
	break;
      case CMD_RESET:
if(use_ota)
	print_result (ota_reset ());
else
	print_result (cable_reset ());
	break;
      case CMD_LOCK:
	print_result (cable_lock ());
	break;
      case CMD_RMAPPS:
	print_result (cable_clear_apps ());
	break;
      case CMD_RM_APP:
	{
	int d;
	sscanf(file_app, "%x", &d);
	print_result (ota_rm_app (d));
	}
	break;
      case CMD_BTLDR:
	{
	int i;
	int d;
	u8 l=strlen(file_app);

	u8 bufx[1000];

	strcpy(bufx, file_app);

	for(i=0;i<l/2;i++)
	{
	sscanf(&bufx[(l/2-1-i)*2], "%x", &d);
	bufx[(l/2-1-i)*2]='\0';
	buf[l/2-1-i]=d;
	}
	print_result (ota_btldr (strlen(file_app)/2, buf));
	}
	break;
      case CMD_SMS:
{
	  u8 *s_name;
	  int ret;
	  u8 *s_buf;
	  struct stat _st;
	  int s_size;
	  FILE *fs;

	  s_name = file_app;
	  ret = stat (s_name, &_st);
	  if (ret < 0)
	  {
	    fprintf (stderr, "Cannot stat %s\n", s_name);
	    exit (-1);
	  }

	  s_size = _st.st_size;
	  s_buf = malloc (s_size);

#ifndef WIN32_MINGW
	  fs = fopen (s_name, "rb");
	  for (i = 0; i < s_size; i++)
	    s_buf[i] = fgetc (fs);
	  fclose (fs);
#else
	  {
	    HANDLE *f1;
	    int n_in;

	    f1 =
	      CreateFile (s_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0,
			  NULL);

	    if (f1 == INVALID_HANDLE_VALUE)
	    {
	      printf ("Cannot open file %s (error %d)\n", s_name,
		      GetLastError ());
	      exit (-1);
	    }

	    ReadFile (f1, s_buf, s_size, (DWORD *) (&n_in), NULL);

	    CloseHandle (f1);
	  }
#endif

	print_result (ota_sms (s_size, s_buf));
}
	break;
      case CMD_APP:
	{
	  b8 f;
	  u16 pos, len;
	  u8 *s_name;
	  int s_size;
	  struct stat _st;
	  u8 *s_buf;
	  int ret;
	  FILE *fs;

	  //TODO terminal profile???

	  s_name = file_app;
	  ret = stat (s_name, &_st);
	  if (ret < 0)
	  {
	    fprintf (stderr, "Cannot stat %s\n", s_name);
	    exit (-1);
	  }
	  s_size = _st.st_size;
	  s_buf = malloc (s_size);

#ifndef WIN32_MINGW
	  fs = fopen (s_name, "rb");
	  for (i = 0; i < s_size; i++)
	    s_buf[i] = fgetc (fs);
	  fclose (fs);
#else
	  {
	    HANDLE *f1;
	    int n_in;

	    f1 =
	      CreateFile (s_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0,
			  NULL);

	    if (f1 == INVALID_HANDLE_VALUE)
	    {
	      printf ("Cannot open file %s (error %d)\n", s_name,
		      GetLastError ());
	      exit (-1);
	    }

	    ReadFile (f1, s_buf, s_size, (DWORD *) (&n_in), NULL);

	    CloseHandle (f1);
	  }
#endif

if(use_ota)
{
	print_result (ota_app (s_buf, s_size, s_name));
}
else
{

	  f = app_open ();
	  if (f != 0)
	    goto E_RR;

	  pos = 0;

	  while (pos < s_size)
	  {
	    len = s_size - pos;
	    if (len > 128)
	      len = 128;
	    f = app_append (s_buf, pos, len);
	    if (f != 0)
	      goto E_RR;
	    pos += 128;
	    printf ("Uploaded %d/%d bytes\n", (pos > s_size) ? s_size : pos,
		    s_size);
	  }
	  f = app_close (s_name);

	E_RR:
	  print_result (f);
	}
}

	break;
    }
  }

X_END:
  close_device ();
  if (err)
    exit (-1);
}
