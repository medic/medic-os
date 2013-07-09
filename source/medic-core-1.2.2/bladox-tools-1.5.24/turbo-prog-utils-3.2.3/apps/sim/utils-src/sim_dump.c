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

#include <tfslib.h>
#include <tproglib.h>

#include "../module-src/sim.h"

/*
 * simsh.c - SIM Shell Turbo application
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

/*

Included standards:
11.11 DF_TELECOM, DF_GSM
11.19 DF_FP-CTS
   
TODO
- other etsi docs, files: dcs1800
- select arbitrary file

df_dcs1800, df_ota
df_eia_tia_553
df_iridium
df_globst
df_ico
df_aces

Cryptoflex:
0000 EF_CHV1
0001 EF_Key_Int
0002 EF_ICC_SN
0011 EF_Key_Ext
0012 EF_RSA_PRI
0100 EF_CHV2
1012 EF_RSA_PUB

7F24 TDMA
7F25 CDMA
7F20:5F40 PCS1900
7FE0 DF_EIRENE

R-UIM
3F00:7F20:5F40:4F80:  System ID
3F00:7F20:5F40:4F81:  Group ID
3F00:7F20:5F40:4F82:  Initial Paging Channel
3F00:7F20:5F40:4F83:  Call Count
3F00:7F20:5F40:4F84:  Negative/Forbidden SID List
3F00:7F20:5F40:4F85:  Positive/Favored SID List
3F00:7F20:5F40:4F88:  Mobile Identification Number
3F00:7F20:5F40:4F89:  Access Overload Class
3F00:7F20:5F40:4F8B:  SIM Electronic Serial Number
3F00:7F20:5F40:4F8D:  Registration Threshold
3F00:7F20:5F40:4F93:  AMPS Usage Indicators

3F00:7F24:6FB0:  Home System OperatorCode and Mobile Country Code
3F00:7F24:6FB1:  ANSI-136 Usage Indicators
3F00:7F24:6FB2:  Alpha Tags
3F00:7F20:6FB3:  Cooperative SOC List
3F00:7F20:6FB4:  IRDB Version Table
3F00:7F20:6FB5:  Partner SID List
3F00:7F20:6FB6:  Partner SOC List
3F00:7F20:6FB7:  Favored SOC List
3F00:7F20:6FB8:  Forbidden SOC List
3F00:7F20:6FB9:  IRDB Parameter
3F00:7F20:6FBA:  Band and Sub-Band Priority Order
3F00:7F20:6FBB:  NAM Private and Residential System IDs
3F00:7F20:6FBC:  Test Registration Private and Regidential System IDs
3F00:7F20:6FBD:  Reg Accept Private and Regidential System IDs
3F00:7F20:6FBE:  SS Feature Code Table
3F00:7F20:6FBF:  Triggered Scan Timers

3F00:7F24:6FC0:  GSM-ANSI-136 SIM Phase ID 
3F00:7F24:6FC1:  Teleservice Server Address Info
3F00:7F24:6FC2:  Band History Table
3F00:7F24:6FC3:  DCCH History Table
3F00:7F24:6FC4:  Additional User Registration Parameters
3F00:7F24:6FC5:  User Group ID Info
3F00:7F24:6FC6:  
3F00:7F24:6FC7:  Directory Number Info
3F00:7F24:6FC8:  Mobile's IP address information for the MS
3F00:7F24:6FC9:  Application Gateway Circuit Switched Parameters
3F00:7F24:6FCA:  Application Provisioning Data
3F00:7F24:6FCB:  Application Gateway IP Address Data
3F00:7F24:6FCC:  
3F00:7F24:6FCD:  Message Encryption Algorithm Map 
3F00:7F24:6FCE:  R-UIM Protocol Version
3F00:7F24:6FCF:  R-UIM Server Table

3F00:7F24:6FD0:  SOC-Depersonalization Control Key
3F00:7F24:6FD1:  EGPRS-136 PSPs
3F00:7F24:6FD2:  Test Registration EGPRS-136 PSPs
3F00:7F24:6FD3:  Accept EGPRS-136 PSPs
3F00:7F24:6FD4:  Virtual Teleservice Server Addess Info

*/

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

  usb_send (ACTION_SIM_CMD, 4 + len, txBuf, &len, buf);
  return len;
}

u16 simx (u8 cla, u8 ins, u8 p1, u8 p2, u8 p3, u8 * buf)
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
  txBuf[0] = cla;
  txBuf[1] = ins;
  txBuf[2] = p1;
  txBuf[3] = p2;
  txBuf[4] = p3;

  for (i = 0; i < len; i++)
    txBuf[5 + i] = buf[i];

  usb_send (ACTION_SIM_CMDX, 5 + len, txBuf, &len, buf);
  return len;
}


#define PROGMEM

typedef struct _Glob_Mem
{
  u8 path[4 * 2];
  u8 depth;
  u16 size;
  u8 rec_len;
}
Glob_Mem;

Glob_Mem *glob_mem;

struct _SCtx;
typedef u8 (t_spider_cb) (struct _SCtx * ctx, u8 action);

typedef struct _SNodeP
{
  const lc_char *text;
  t_spider_cb *cb;
  const void *p;
} PROGMEM SNodeP;

typedef struct _SEdgeP
{
  const SNodeP *f;
  const SNodeP *t;
} PROGMEM SEdgeP;

typedef struct _SCtx SCtx;

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

u8 sh_df (SCtx * ctx, u8 action)
{
}

u8 sh_ef (SCtx * ctx, u8 action)
{
}

/* *INDENT-OFF* */

lc_char PROGMEM lc_mf[]={
        LC_EN("MF 3F00")
        LC_END
};

lc_char PROGMEM lc_df_gsm[]={
        LC_EN("DF 7F20 GSM")
        LC_END
};

lc_char PROGMEM lc_df_telecom[]={
        LC_EN("DF 7F10 TELECOM")
        LC_END
};

lc_char PROGMEM lc_ef_elp[]={
        LC_EN("EF 2F05 ELP")
        LC_END
};

lc_char PROGMEM lc_ef_iccid[]={
        LC_EN("EF 2FE2 ICCID")
        LC_END
};

lc_char PROGMEM lc_ef_adn[]={
        LC_EN("EF 6F3A ADN")
        LC_END
};

lc_char PROGMEM lc_ef_fdn[]={
        LC_EN("EF 6F3B FDN")
        LC_END
};

lc_char PROGMEM lc_ef_sms[]={
        LC_EN("EF 6F3C SMS")
        LC_END
};

lc_char PROGMEM lc_ef_ccp[]={
        LC_EN("EF 6F3D CCP")
        LC_END
};

lc_char PROGMEM lc_ef_msisdn[]={
        LC_EN("EF 6F40 MSISDN")
        LC_END
};

lc_char PROGMEM lc_ef_smsp[]={
        LC_EN("EF 6F42 SMSP")
        LC_END
};

lc_char PROGMEM lc_ef_smss[]={
        LC_EN("EF 6F43 SMSS")
        LC_END
};

lc_char PROGMEM lc_ef_lnd[]={
        LC_EN("EF 6F44 LND")
        LC_END
};

lc_char PROGMEM lc_ef_smsr[]={
        LC_EN("EF 6F47 SMSR")
        LC_END
};

lc_char PROGMEM lc_ef_sdn[]={
        LC_EN("EF 6F49 SDN")
        LC_END
};

lc_char PROGMEM lc_ef_ext1[]={
        LC_EN("EF 6F4A EXT1")
        LC_END
};

lc_char PROGMEM lc_ef_ext2[]={
        LC_EN("EF 6F4B EXT2")
        LC_END
};

lc_char PROGMEM lc_ef_ext3[]={
        LC_EN("EF 6F4C EXT3")
        LC_END
};

lc_char PROGMEM lc_ef_bdn[]={
        LC_EN("EF 6F4D BDN")
        LC_END
};

lc_char PROGMEM lc_ef_ext4[]={
        LC_EN("EF 6F4E EXT4")
        LC_END
};

lc_char PROGMEM lc_ef_lp[]={
        LC_EN("EF 6F05 LP")
        LC_END
};

lc_char PROGMEM lc_ef_imsi[]={
        LC_EN("EF 6F07 IMSI")
        LC_END
};

lc_char PROGMEM lc_ef_kc[]={
        LC_EN("EF 6F20 Kc")
        LC_END
};

lc_char PROGMEM lc_ef_plmnsel[]={
        LC_EN("EF 6F30 PLMNsel")
        LC_END
};

lc_char PROGMEM lc_ef_hplmn[]={
        LC_EN("EF 6F31 HPLMN")
        LC_END
};

lc_char PROGMEM lc_ef_acmmax[]={
        LC_EN("EF 6F37 ACMmax")
        LC_END
};

lc_char PROGMEM lc_ef_sst[]={
        LC_EN("EF 6F38 SST")
        LC_END
};

lc_char PROGMEM lc_ef_acm[]={
        LC_EN("EF 6F39 ACM")
        LC_END
};

lc_char PROGMEM lc_ef_gid1[]={
        LC_EN("EF 6F3E GID1")
        LC_END
};

lc_char PROGMEM lc_ef_gid2[]={
        LC_EN("EF 6F3F GID2")
        LC_END
};

lc_char PROGMEM lc_ef_puct[]={
        LC_EN("EF 6F41 PUCT")
        LC_END
};

lc_char PROGMEM lc_ef_cbmi[]={
        LC_EN("EF 6F45 CBMI")
        LC_END
};

lc_char PROGMEM lc_ef_spn[]={
        LC_EN("EF 6F46 SPN")
        LC_END
};

lc_char PROGMEM lc_ef_cbmid[]={
        LC_EN("EF 6F48 CBMID")
        LC_END
};

lc_char PROGMEM lc_ef_bcch[]={
        LC_EN("EF 6F74 BCCH")
        LC_END
};

lc_char PROGMEM lc_ef_acc[]={
        LC_EN("EF 6F78 ACC")
        LC_END
};

lc_char PROGMEM lc_ef_fplmn[]={
        LC_EN("EF 6F7B FPLMN")
        LC_END
};

lc_char PROGMEM lc_ef_loci[]={
        LC_EN("EF 6F7E LOCI")
        LC_END
};

lc_char PROGMEM lc_ef_ad[]={
        LC_EN("EF 6FAD AD")
        LC_END
};

lc_char PROGMEM lc_ef_phase[]={
        LC_EN("EF 6FAE PHASE")
        LC_END
};

lc_char PROGMEM lc_ef_vgcs[]={
        LC_EN("EF 6FB1 VGCS")
        LC_END
};

lc_char PROGMEM lc_ef_vgcss[]={
        LC_EN("EF 6FB2 VGCSS")
        LC_END
};

lc_char PROGMEM lc_ef_vbs[]={
        LC_EN("EF 6FB3 VBS")
        LC_END
};

lc_char PROGMEM lc_ef_vbss[]={
        LC_EN("EF 6FB4 VBSS")
        LC_END
};

lc_char PROGMEM lc_ef_emlpp[]={
        LC_EN("EF 6FB5 eMLPP")
        LC_END
};

lc_char PROGMEM lc_ef_aaem[]={
        LC_EN("EF 6FB6 AAeM")
        LC_END
};

lc_char PROGMEM lc_ef_ecc[]={
        LC_EN("EF 6FB7 ECC")
        LC_END
};

lc_char PROGMEM lc_ef_cbmir[]={
        LC_EN("EF 6F50 CBMIR")
        LC_END
};

lc_char PROGMEM lc_ef_nia[]={
        LC_EN("EF 6F51 NIA")
        LC_END
};

lc_char PROGMEM lc_ef_kcgprs[]={
        LC_EN("EF 6F52 KcGPRS")
        LC_END
};

lc_char PROGMEM lc_ef_locigprs[]={
        LC_EN("EF 6F53 LOCIGPRS")
        LC_END
};

lc_char PROGMEM lc_ef_sume[]={
        LC_EN("EF 6F54 SUME")
        LC_END
};

lc_char PROGMEM lc_ef_plmnwact[]={
        LC_EN("EF 6F60 PLMNwAcT")
        LC_END
};

lc_char PROGMEM lc_ef_oplmnwact[]={
        LC_EN("EF 6F61 OPLMNwAcT")
        LC_END
};

lc_char PROGMEM lc_ef_hplmnwact[]={
        LC_EN("EF 6F62 HPLMNwAcT")
        LC_END
};

lc_char PROGMEM lc_ef_cpbcch[]={
        LC_EN("EF 6F63 CPBCCH")
        LC_END
};

lc_char PROGMEM lc_ef_invscan[]={
        LC_EN("EF 6F64 INVSCAN")
        LC_END
};

lc_char PROGMEM lc_ef_img[]={
        LC_EN("EF 4F20 IMG")
        LC_END
};

lc_char PROGMEM lc_ef_sai[]={
        LC_EN("EF 4F30 SAI")
        LC_END
};

lc_char PROGMEM lc_ef_sll[]={
        LC_EN("EF 4F31 SLL")
        LC_END
};

lc_char PROGMEM lc_ef_mexe_st[]={
        LC_EN("EF 4F40 MExE-ST")
        LC_END
};

lc_char PROGMEM lc_ef_orpk[]={
        LC_EN("EF 4F41 ORPK")
        LC_END
};

lc_char PROGMEM lc_ef_arpk[]={
        LC_EN("EF 4F42 ARPK")
        LC_END
};

lc_char PROGMEM lc_ef_tprpk[]={
        LC_EN("EF 4F43 TPRPK")
        LC_END
};

lc_char PROGMEM lc_df_graphics[]={
        LC_EN("DF 5F50 GRAPHICS")
        LC_END
};

lc_char PROGMEM lc_df_solsa[]={
        LC_EN("DF 5F70 SoLSA")
        LC_END
};

lc_char PROGMEM lc_df_mexe[]={
        LC_EN("DF 5F3C MExE")
        LC_END
};

lc_char PROGMEM lc_df_cts[]={
        LC_EN("DF 5F60 CTS")
        LC_END
};

lc_char PROGMEM lc_df_is_41[]={
        LC_EN("DF 7F22 IS-41")
        LC_END
};

lc_char PROGMEM lc_df_fp_cts[]={
        LC_EN("DF 7F23 FP-CTS")
        LC_END
};

lc_char PROGMEM lc_ef_rplmnat[]={
        LC_EN("EF 6F65 RPLMNAT")
        LC_END
};

lc_char PROGMEM lc_ef_chv1[]={
        LC_EN("EF 0000 CHV1")
        LC_END
};

lc_char PROGMEM lc_ef_keyint[]={
        LC_EN("EF 0001 KeyInt")
        LC_END
};

lc_char PROGMEM lc_ef_iccsn[]={
        LC_EN("EF 0002 ICC SN")
        LC_END
};

lc_char PROGMEM lc_ef_keyext[]={
        LC_EN("EF 0011 KeyExt")
        LC_END
};

lc_char PROGMEM lc_ef_rsapri[]={
        LC_EN("EF 0012 RSAPRI")
        LC_END
};

lc_char PROGMEM lc_ef_chv2[]={
        LC_EN("EF 0100 CHV2")
        LC_END
};

lc_char PROGMEM lc_ef_rsapub[]={
        LC_EN("EF 1012 RSAPUB")
        LC_END
};

lc_char PROGMEM lc_ef_dir[]={
        LC_EN("EF 2F00 DIR")
        LC_END
};

lc_char PROGMEM lc_ef_atr[]={
        LC_EN("EF 2F01 ATR")
        LC_END
};

lc_char PROGMEM lc_ef_info[]={
        LC_EN("EF 2FFF INFO")
        LC_END
};

lc_char PROGMEM lc_df_dcs1800[]={
        LC_EN("DF 7F21 DCS1800")
        LC_END
};

lc_char PROGMEM lc_df_ota[]={
        LC_EN("DF 7F40 OTA")
        LC_END
};

lc_char PROGMEM lc_ef_ifpsi[]={
        LC_EN("EF 6F07 IFPSI")
        LC_END
};

lc_char PROGMEM lc_ef_cts_info[]={
        LC_EN("EF 6F38 CTS-INFO")
        LC_END
};

lc_char PROGMEM lc_ef_cts_sndn[]={
        LC_EN("EF 6F41 CTS-SNDN")
        LC_END
};

lc_char PROGMEM lc_ef_cts_ccp[]={
        LC_EN("EF 6F3D CTS-CCP")
        LC_END
};

lc_char PROGMEM lc_ef_cts_ext[]={
        LC_EN("EF 6F4A CTS-EXT")
        LC_END
};

lc_char PROGMEM lc_ef_pplmn[]={
        LC_EN("EF 6F7B PPLMN")
        LC_END
};

lc_char PROGMEM lc_ef_cts_fprip[]={
        LC_EN("EF 4F01 CTS-FPRIP")
        LC_END
};

/* *INDENT-ON* */

u16 PROGMEM file_mf[] = { 0x3f00 };
u16 PROGMEM file_df_dcs1800[] = { 0x7f21 };
u16 PROGMEM file_df_is_41[] = { 0x7f22 };
u16 PROGMEM file_df_ota[] = { 0x7f40 };
u16 PROGMEM file_ef_chv1[] = { 0x0000 };
u16 PROGMEM file_ef_keyint[] = { 0x0001 };
u16 PROGMEM file_ef_iccsn[] = { 0x0002 };
u16 PROGMEM file_ef_keyext[] = { 0x0011 };
u16 PROGMEM file_ef_rsapri[] = { 0x0012 };
u16 PROGMEM file_ef_chv2[] = { 0x0100 };
u16 PROGMEM file_ef_rsapub[] = { 0x1012 };
u16 PROGMEM file_ef_dir[] = { 0x2f00 };
u16 PROGMEM file_ef_atr[] = { 0x2f01 };
u16 PROGMEM file_ef_elp[] = { 0x2f05 };
u16 PROGMEM file_ef_iccid[] = { 0x2fe2 };
u16 PROGMEM file_ef_info[] = { 0x2fff };

u16 PROGMEM file_df_gsm[] = { 0x7f20 };
u16 PROGMEM file_ef_lp[] = { 0x6f05 };
u16 PROGMEM file_ef_imsi[] = { 0x6f07 };
u16 PROGMEM file_ef_kc[] = { 0x6f20 };
u16 PROGMEM file_ef_plmnsel[] = { 0x6f30 };
u16 PROGMEM file_ef_hplmn[] = { 0x6f31 };
u16 PROGMEM file_ef_acmmax[] = { 0x6f37 };
u16 PROGMEM file_ef_sst[] = { 0x6f38 };
u16 PROGMEM file_ef_acm[] = { 0x6f39 };
u16 PROGMEM file_ef_gid1[] = { 0x6f3e };
u16 PROGMEM file_ef_gid2[] = { 0x6f3f };
u16 PROGMEM file_ef_puct[] = { 0x6f41 };
u16 PROGMEM file_ef_cbmi[] = { 0x6f45 };
u16 PROGMEM file_ef_spn[] = { 0x6f46 };
u16 PROGMEM file_ef_cbmid[] = { 0x6f48 };
u16 PROGMEM file_ef_bcch[] = { 0x6f74 };
u16 PROGMEM file_ef_acc[] = { 0x6f78 };
u16 PROGMEM file_ef_fplmn[] = { 0x6f7b };
u16 PROGMEM file_ef_loci[] = { 0x6f7e };
u16 PROGMEM file_ef_ad[] = { 0x6fad };
u16 PROGMEM file_ef_phase[] = { 0x6fae };
u16 PROGMEM file_ef_vgcs[] = { 0x6fb1 };
u16 PROGMEM file_ef_vgcss[] = { 0x6fb2 };
u16 PROGMEM file_ef_vbs[] = { 0x6fb3 };
u16 PROGMEM file_ef_vbss[] = { 0x6fb4 };
u16 PROGMEM file_ef_emlpp[] = { 0x6fb5 };
u16 PROGMEM file_ef_aaem[] = { 0x6fb6 };
u16 PROGMEM file_ef_ecc[] = { 0x6fb7 };
u16 PROGMEM file_ef_cbmir[] = { 0x6f50 };
u16 PROGMEM file_ef_nia[] = { 0x6f51 };
u16 PROGMEM file_ef_kcgprs[] = { 0x6f52 };
u16 PROGMEM file_ef_locigprs[] = { 0x6f53 };
u16 PROGMEM file_ef_sume[] = { 0x6f54 };
u16 PROGMEM file_ef_plmnwact[] = { 0x6f60 };
u16 PROGMEM file_ef_oplmnwact[] = { 0x6f61 };
u16 PROGMEM file_ef_hplmnwact[] = { 0x6f62 };
u16 PROGMEM file_ef_cpbcch[] = { 0x6f63 };
u16 PROGMEM file_ef_invscan[] = { 0x6f64 };
u16 PROGMEM file_ef_rplmnat[] = { 0x6f65 };
u16 PROGMEM file_df_cts[] = { 0x5f60 };
u16 PROGMEM file_ef_cts_fprip[] = { 0x4f01 };
u16 PROGMEM file_df_mexe[] = { 0x5f3c };
u16 PROGMEM file_ef_mexe_st[] = { 0x4f40 };
u16 PROGMEM file_ef_orpk[] = { 0x4f41 };
u16 PROGMEM file_ef_arpk[] = { 0x4f42 };
u16 PROGMEM file_ef_tprpk[] = { 0x4f43 };
u16 PROGMEM file_df_solsa[] = { 0x5f70 };
u16 PROGMEM file_ef_sai[] = { 0x4f30 };
u16 PROGMEM file_ef_sll[] = { 0x4f31 };

u16 PROGMEM file_df_telecom[] = { 0x7f10 };
u16 PROGMEM file_ef_adn[] = { 0x6f3a };
u16 PROGMEM file_ef_fdn[] = { 0x6f3b };
u16 PROGMEM file_ef_sms[] = { 0x6f3c };
u16 PROGMEM file_ef_ccp[] = { 0x6f3d };
u16 PROGMEM file_ef_msisdn[] = { 0x6f40 };
u16 PROGMEM file_ef_smsp[] = { 0x6f42 };
u16 PROGMEM file_ef_smss[] = { 0x6f43 };
u16 PROGMEM file_ef_lnd[] = { 0x6f44 };
u16 PROGMEM file_ef_smsr[] = { 0x6f47 };
u16 PROGMEM file_ef_sdn[] = { 0x6f49 };
u16 PROGMEM file_ef_ext1[] = { 0x6f4a };
u16 PROGMEM file_ef_ext2[] = { 0x6f4b };
u16 PROGMEM file_ef_ext3[] = { 0x6f4c };
u16 PROGMEM file_ef_bdn[] = { 0x6f4d };
u16 PROGMEM file_ef_ext4[] = { 0x6f4e };
u16 PROGMEM file_df_graphics[] = { 0x5f50 };
u16 PROGMEM file_ef_img[] = { 0x4f20 };

u16 PROGMEM file_df_fp_cts[] = { 0x7f23 };
u16 PROGMEM file_ef_ifpsi[] = { 0x6f07 };
u16 PROGMEM file_ef_cts_info[] = { 0x6f38 };
u16 PROGMEM file_ef_cts_sndn[] = { 0x6f41 };
u16 PROGMEM file_ef_cts_ccp[] = { 0x6f3d };
u16 PROGMEM file_ef_cts_ext[] = { 0x6f4a };
u16 PROGMEM file_ef_pplmn[] = { 0x6f7b };

SNodeP n_mf = { lc_mf, sh_df, file_mf };
SNodeP n_ef_elp = { lc_ef_elp, sh_ef, file_ef_elp };
SNodeP n_ef_iccid = { lc_ef_iccid, sh_ef, file_ef_iccid };
SNodeP n_ef_chv1 = { lc_ef_chv1, sh_ef, file_ef_chv1 };
SNodeP n_ef_keyint = { lc_ef_keyint, sh_ef, file_ef_keyint };
SNodeP n_ef_iccsn = { lc_ef_iccsn, sh_ef, file_ef_iccsn };
SNodeP n_ef_keyext = { lc_ef_keyext, sh_ef, file_ef_keyext };
SNodeP n_ef_rsapri = { lc_ef_rsapri, sh_ef, file_ef_rsapri };
SNodeP n_ef_chv2 = { lc_ef_chv2, sh_ef, file_ef_chv2 };
SNodeP n_ef_rsapub = { lc_ef_rsapub, sh_ef, file_ef_rsapub };
SNodeP n_ef_dir = { lc_ef_dir, sh_ef, file_ef_dir };
SNodeP n_ef_atr = { lc_ef_atr, sh_ef, file_ef_atr };
SNodeP n_ef_info = { lc_ef_info, sh_ef, file_ef_info };

SNodeP n_df_telecom = { lc_df_telecom, sh_df, file_df_telecom };
SNodeP n_ef_adn = { lc_ef_adn, sh_ef, file_ef_adn };
SNodeP n_ef_fdn = { lc_ef_fdn, sh_ef, file_ef_fdn };
SNodeP n_ef_sms = { lc_ef_sms, sh_ef, file_ef_sms };
SNodeP n_ef_ccp = { lc_ef_ccp, sh_ef, file_ef_ccp };
SNodeP n_ef_msisdn = { lc_ef_msisdn, sh_ef, file_ef_msisdn };
SNodeP n_ef_smsp = { lc_ef_smsp, sh_ef, file_ef_smsp };
SNodeP n_ef_smss = { lc_ef_smss, sh_ef, file_ef_smss };
SNodeP n_ef_lnd = { lc_ef_lnd, sh_ef, file_ef_lnd };
SNodeP n_ef_smsr = { lc_ef_smsr, sh_ef, file_ef_smsr };
SNodeP n_ef_sdn = { lc_ef_sdn, sh_ef, file_ef_sdn };
SNodeP n_ef_ext1 = { lc_ef_ext1, sh_ef, file_ef_ext1 };
SNodeP n_ef_ext2 = { lc_ef_ext2, sh_ef, file_ef_ext2 };
SNodeP n_ef_ext3 = { lc_ef_ext3, sh_ef, file_ef_ext3 };
SNodeP n_ef_bdn = { lc_ef_bdn, sh_ef, file_ef_bdn };
SNodeP n_ef_ext4 = { lc_ef_ext4, sh_ef, file_ef_ext4 };

SNodeP n_df_gsm = { lc_df_gsm, sh_df, file_df_gsm };
SNodeP n_ef_lp = { lc_ef_lp, sh_ef, file_ef_lp };
SNodeP n_ef_imsi = { lc_ef_imsi, sh_ef, file_ef_imsi };
SNodeP n_ef_kc = { lc_ef_kc, sh_ef, file_ef_kc };
SNodeP n_ef_plmnsel = { lc_ef_plmnsel, sh_ef, file_ef_plmnsel };
SNodeP n_ef_hplmn = { lc_ef_hplmn, sh_ef, file_ef_hplmn };
SNodeP n_ef_acmmax = { lc_ef_acmmax, sh_ef, file_ef_acmmax };
SNodeP n_ef_sst = { lc_ef_sst, sh_ef, file_ef_sst };
SNodeP n_ef_acm = { lc_ef_acm, sh_ef, file_ef_acm };
SNodeP n_ef_gid1 = { lc_ef_gid1, sh_ef, file_ef_gid1 };
SNodeP n_ef_gid2 = { lc_ef_gid2, sh_ef, file_ef_gid2 };
SNodeP n_ef_puct = { lc_ef_puct, sh_ef, file_ef_puct };
SNodeP n_ef_cbmi = { lc_ef_cbmi, sh_ef, file_ef_cbmi };
SNodeP n_ef_spn = { lc_ef_spn, sh_ef, file_ef_spn };
SNodeP n_ef_cbmid = { lc_ef_cbmid, sh_ef, file_ef_cbmid };
SNodeP n_ef_bcch = { lc_ef_bcch, sh_ef, file_ef_bcch };
SNodeP n_ef_acc = { lc_ef_acc, sh_ef, file_ef_acc };
SNodeP n_ef_fplmn = { lc_ef_fplmn, sh_ef, file_ef_fplmn };
SNodeP n_ef_loci = { lc_ef_loci, sh_ef, file_ef_loci };
SNodeP n_ef_ad = { lc_ef_ad, sh_ef, file_ef_ad };
SNodeP n_ef_phase = { lc_ef_phase, sh_ef, file_ef_phase };
SNodeP n_ef_vgcs = { lc_ef_vgcs, sh_ef, file_ef_vgcs };
SNodeP n_ef_vgcss = { lc_ef_vgcss, sh_ef, file_ef_vgcss };
SNodeP n_ef_vbs = { lc_ef_vbs, sh_ef, file_ef_vbs };
SNodeP n_ef_vbss = { lc_ef_vbss, sh_ef, file_ef_vbss };
SNodeP n_ef_emlpp = { lc_ef_emlpp, sh_ef, file_ef_emlpp };
SNodeP n_ef_aaem = { lc_ef_aaem, sh_ef, file_ef_aaem };
SNodeP n_ef_ecc = { lc_ef_ecc, sh_ef, file_ef_ecc };
SNodeP n_ef_cbmir = { lc_ef_cbmir, sh_ef, file_ef_cbmir };
SNodeP n_ef_nia = { lc_ef_nia, sh_ef, file_ef_nia };
SNodeP n_ef_kcgprs = { lc_ef_kcgprs, sh_ef, file_ef_kcgprs };
SNodeP n_ef_locigprs = { lc_ef_locigprs, sh_ef, file_ef_locigprs };
SNodeP n_ef_sume = { lc_ef_sume, sh_ef, file_ef_sume };
SNodeP n_ef_plmnwact = { lc_ef_plmnwact, sh_ef, file_ef_plmnwact };
SNodeP n_ef_oplmnwact = { lc_ef_oplmnwact, sh_ef, file_ef_oplmnwact };
SNodeP n_ef_hplmnwact = { lc_ef_hplmnwact, sh_ef, file_ef_hplmnwact };
SNodeP n_ef_cpbcch = { lc_ef_cpbcch, sh_ef, file_ef_cpbcch };
SNodeP n_ef_invscan = { lc_ef_invscan, sh_ef, file_ef_invscan };
SNodeP n_ef_rplmnat = { lc_ef_rplmnat, sh_ef, file_ef_rplmnat };

SNodeP n_df_fp_cts = { lc_df_fp_cts, sh_df, file_df_fp_cts };
SNodeP n_ef_ifpsi = { lc_ef_ifpsi, sh_ef, file_ef_ifpsi };
SNodeP n_ef_cts_info = { lc_ef_cts_info, sh_ef, file_ef_cts_info };
SNodeP n_ef_cts_sndn = { lc_ef_cts_sndn, sh_ef, file_ef_cts_sndn };
SNodeP n_ef_cts_ccp = { lc_ef_cts_ccp, sh_ef, file_ef_cts_ccp };
SNodeP n_ef_cts_ext = { lc_ef_cts_ext, sh_ef, file_ef_cts_ext };
SNodeP n_ef_pplmn = { lc_ef_pplmn, sh_ef, file_ef_pplmn };

SNodeP n_df_is_41 = { lc_df_is_41, sh_df, file_df_is_41 };
SNodeP n_df_dcs1800 = { lc_df_dcs1800, sh_df, file_df_dcs1800 };
SNodeP n_df_ota = { lc_df_ota, sh_df, file_df_ota };

SNodeP n_df_graphics = { lc_df_graphics, sh_df, file_df_graphics };
SNodeP n_ef_img = { lc_ef_img, sh_ef, file_ef_img };

SNodeP n_df_mexe = { lc_df_mexe, sh_df, file_df_mexe };
SNodeP n_ef_mexe_st = { lc_ef_mexe_st, sh_ef, file_ef_mexe_st };
SNodeP n_ef_orpk = { lc_ef_orpk, sh_ef, file_ef_orpk };
SNodeP n_ef_arpk = { lc_ef_arpk, sh_ef, file_ef_arpk };
SNodeP n_ef_tprpk = { lc_ef_tprpk, sh_ef, file_ef_tprpk };

SNodeP n_df_cts = { lc_df_cts, sh_df, file_df_cts };
SNodeP n_ef_cts_fprip = { lc_ef_cts_fprip, sh_ef, file_ef_cts_fprip };

SNodeP n_df_solsa = { lc_df_solsa, sh_df, file_df_solsa };
SNodeP n_ef_sai = { lc_ef_sai, sh_ef, file_ef_sai };
SNodeP n_ef_sll = { lc_ef_sll, sh_ef, file_ef_sll };

/* *INDENT-OFF* */
SEdgeP simsh_edges_p[] = {
  {&n_mf, &n_df_telecom},
  {&n_mf, &n_df_gsm},
  {&n_mf, &n_df_dcs1800},
  {&n_mf, &n_df_is_41},
  {&n_mf, &n_df_fp_cts},
  {&n_mf, &n_df_ota},
  {&n_mf, &n_ef_chv1},
  {&n_mf, &n_ef_keyint},
  {&n_mf, &n_ef_iccsn},
  {&n_mf, &n_ef_keyext},
  {&n_mf, &n_ef_rsapri},
  {&n_mf, &n_ef_chv2},
  {&n_mf, &n_ef_rsapub},
  {&n_mf, &n_ef_elp},
  {&n_mf, &n_ef_iccid},
  {&n_mf, &n_ef_dir},
  {&n_mf, &n_ef_atr},
  {&n_mf, &n_ef_info},
  {&n_df_gsm, &n_df_mexe},
  {&n_df_mexe, &n_ef_mexe_st},
  {&n_df_mexe, &n_ef_orpk},
  {&n_df_mexe, &n_ef_arpk},
  {&n_df_mexe, &n_ef_tprpk},
  {&n_df_gsm, &n_df_cts},
  {&n_df_gsm, &n_df_solsa},
  {&n_df_solsa, &n_ef_sai},
  {&n_df_solsa, &n_ef_sll},
  {&n_df_gsm, &n_ef_lp},
  {&n_df_gsm, &n_ef_imsi},
  {&n_df_gsm, &n_ef_kc},
  {&n_df_gsm, &n_ef_plmnsel},
  {&n_df_gsm, &n_ef_hplmn},
  {&n_df_gsm, &n_ef_acmmax},
  {&n_df_gsm, &n_ef_sst},
  {&n_df_gsm, &n_ef_acm},
  {&n_df_gsm, &n_ef_gid1},
  {&n_df_gsm, &n_ef_gid2},
  {&n_df_gsm, &n_ef_puct},
  {&n_df_gsm, &n_ef_cbmi},
  {&n_df_gsm, &n_ef_spn},
  {&n_df_gsm, &n_ef_cbmid},
  {&n_df_gsm, &n_ef_bcch},
  {&n_df_gsm, &n_ef_acc},
  {&n_df_gsm, &n_ef_fplmn},
  {&n_df_gsm, &n_ef_loci},
  {&n_df_gsm, &n_ef_ad},
  {&n_df_gsm, &n_ef_phase},
  {&n_df_gsm, &n_ef_vgcs},
  {&n_df_gsm, &n_ef_vgcss},
  {&n_df_gsm, &n_ef_vbs},
  {&n_df_gsm, &n_ef_vbss},
  {&n_df_gsm, &n_ef_emlpp},
  {&n_df_gsm, &n_ef_aaem},
  {&n_df_gsm, &n_ef_ecc},
  {&n_df_gsm, &n_ef_cbmir},
  {&n_df_gsm, &n_ef_nia},
  {&n_df_gsm, &n_ef_kcgprs},
  {&n_df_gsm, &n_ef_locigprs},
  {&n_df_gsm, &n_ef_sume},
  {&n_df_gsm, &n_ef_plmnwact},
  {&n_df_gsm, &n_ef_oplmnwact},
  {&n_df_gsm, &n_ef_hplmnwact},
  {&n_df_gsm, &n_ef_cpbcch},
  {&n_df_gsm, &n_ef_invscan},
  {&n_df_gsm, &n_ef_rplmnat},
  {&n_df_telecom, &n_df_graphics},
  {&n_df_graphics, &n_ef_img},
  {&n_df_telecom, &n_ef_adn},
  {&n_df_telecom, &n_ef_fdn},
  {&n_df_telecom, &n_ef_sms},
  {&n_df_telecom, &n_ef_ccp},
  {&n_df_telecom, &n_ef_msisdn},
  {&n_df_telecom, &n_ef_smsp},
  {&n_df_telecom, &n_ef_smss},
  {&n_df_telecom, &n_ef_lnd},
  {&n_df_telecom, &n_ef_smsr},
  {&n_df_telecom, &n_ef_sdn},
  {&n_df_telecom, &n_ef_ext1},
  {&n_df_telecom, &n_ef_ext2},
  {&n_df_telecom, &n_ef_ext3},
  {&n_df_telecom, &n_ef_bdn},
  {&n_df_telecom, &n_ef_ext4},
  {&n_df_fp_cts, &n_ef_ifpsi},
  {&n_df_fp_cts, &n_ef_cts_info},
  {&n_df_fp_cts, &n_ef_cts_sndn},
  {&n_df_fp_cts, &n_ef_cts_ccp},
  {&n_df_fp_cts, &n_ef_cts_ext},
  {&n_df_fp_cts, &n_ef_pplmn},
  {&n_df_fp_cts, &n_ef_ad},
  {&n_df_cts, &n_ef_cts_fprip},
  NULL
};
/* *INDENT-ON* */

void dump_file (SNodeP * node, u8 depth)
{
  u8 *t = node->text;
  u16 *f = node->p;
  u16 ret;
  u8 buf[1000];
  u16 i;
  u16 len;

  t++;
  t++;
  printf ("FILE: %s\n", t);
  glob_mem->path[depth * 2] = (*f) >> 8;
  glob_mem->path[depth * 2 + 1] = *f;
  glob_mem->depth++;

  printf ("SELECT ");
  for (i = 0; i < 2 * glob_mem->depth; i++)
  {
    if (i != 0 && i % 2 == 0)
      printf (":");
    printf ("%02X", glob_mem->path[i]);
  }
  printf ("\n");

  ret = sim_select (glob_mem->path, glob_mem->depth);
  if ((ret & 0x9f00) != 0x9f00)
  {
    glob_mem->depth--;
    printf ("File Does Not Exist\n\n");
    return;
  }
  else
  {
    printf ("GET_RESPONSE ");
    ret = sim (ME_CMD_GET_RESPONSE, 0, 0, ret, buf);
    if (ret > 2)
      ret -= 2;
    for (i = 0; i < ret; i++)
      printf ("%02X ", buf[i]);
    printf ("\n");

    if (buf[6] == 0x01 || buf[6] == 0x02)
    {
// MF or DF
      printf ("Free:\t\t %d\n", (((u16) buf[2]) << 8) | buf[3]);
      printf ("#DF:\t\t %d\n", buf[14]);
      printf ("#EF:\t\t %d\n", buf[15]);
      if (buf[6] == 0x01)
      {
// MF
	printf ("C13s:\t\t %d\n", buf[13]);
	printf ("#CHVs:\t\t %02X\n", buf[16]);
	printf ("CHV1 STS:\t %02X\n", buf[18]);
	printf ("UNBL CHV1 STS:\t %02X\n", buf[19]);
	printf ("CHV2 STS:\t %02X\n", buf[20]);
	printf ("UNBL CHV1 STS:\t %02X\n", buf[21]);

	if (ret > 23)
	{
	  printf ("Admin:\t\t ");
	  for (i = 23; i < ret; i++)
	    printf ("%02X ", buf[i]);
	  printf ("\n");
	}
      }

      printf ("\n");
      i = 0;
      while (simsh_edges_p[i].f != NULL)
      {
	if (simsh_edges_p[i].f == node)
	  dump_file (simsh_edges_p[i].t, depth + 1);
	i++;
      }
    }
    else if (buf[6] == 0x04)
    {
//EF
      if (buf[13] == 0x00)
	printf ("Transparent file\n");
      else if (buf[13] == 0x01)
	printf ("Linear file\n");
      else if (buf[13] == 0x03)
	printf ("Cyclic file\n");
      else
	printf ("Unknown EF file\n");

      printf ("Size:\t\t %d\n", ((u16) buf[2] << 8) | buf[3]);
      printf ("Access:\t\t %02X %02X %02X\n", buf[8], buf[9], buf[10]);
      printf ("Status:\t\t %02X\n", buf[11]);
      if (buf[13] == 0x03)
	printf ("B8:\t\t %02X\n", buf[7]);

      if (buf[13] == 0x03 || buf[13] == 0x01)
      {
	printf ("Record len:\t %d\n", buf[14]);
	printf ("Nr. records:\t %d\n",
		(((u16) buf[2] << 8) | buf[3]) / buf[14]);
	sim_read_rec ((((u16) buf[2] << 8) | buf[3]) / buf[14], buf[14]);
      }
      else
	sim_read_bin (((u16) buf[2] << 8) | buf[3]);

      printf ("\n");
    }
    else
    {
//unknow file type
    }
  }

  glob_mem->depth--;
  return;
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
  printf ("Usage: sim_dump [args]\n");
  printf ("-a application\tuse application, default is %s\n", APP_SIM);
  printf ("-r\t\tprint ATR only and exit\n");
  printf ("-d device\tuse USB device\n");
  printf ("--pin pin\tuse PIN to unlock\n");
  printf ("--puk pin\tuse PUK to set a new PIN, --pin has to be set\n");
  printf ("-h\t\thelp\n");
  printf ("-v\t\tverbose\n");
  exit (-1);
}

u8 cmp_edc(u8 *buf, u8 len)
{
u8 i;
u8 x=0;
for(i=0;i<len;i++)x=x^buf[i];
printf("EDC: %02X\n", x);
return x;
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
  u8 err = 0;
u8 seq=0;

  while (1)
  {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"pin", 1, 0, 0},
      {"puk", 1, 0, 0},
      {0, 0, 0, 0}
    };

    result = getopt_long (argc, argv, "hd:ra:v", long_options, &option_index);
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
	}
	break;
      case 'd':
	dev = optarg;
	break;
      case 'r':
	atr_only = 1;
	break;
      case 'a':
	app_name = optarg;
	break;
      case 'v':
	verbose_inc ();
	break;
      case 'h':
      default:
	help ();
	break;
    }
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



#if 1
  if (puk)
    locked = unblock (puk, pin);

  if (pin && locked == 0)
    locked = unlock (pin, 1);

  if (atr_only == 0 && locked == 0)
  {
    printf ("\n");
    dump_file (&n_mf, 0);
  }
#endif

#if 0
// T0
//sleep(1);

i=0;

buf[i++]=0x00;
buf[i++]=0x03;
buf[i++]=0x00;
buf[i++]=0x03;	// 3 ok

buf[i++]=0xff;
buf[i++]=0x00;
buf[i++]=0xff;

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");

#if 0
i=0;

buf[i++]=0x00;
buf[i++]=0x03;
buf[i++]=0x00;
buf[i++]=0x00;	// 3 ok

buf[i++]=0xff;
buf[i++]=0x00;
buf[i++]=0xff;

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");
#endif

#endif

#if 0
// T1
//sleep(1);

i=0;

buf[i++]=0x00;
buf[i++]=0x03;
buf[i++]=0x00;
buf[i++]=0x03;	// 3 ok

buf[i++]=0xff;
buf[i++]=0x01;
buf[i++]=0xff;

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");

#if 0
i=0;

buf[i++]=0x00;
buf[i++]=0x03;
buf[i++]=0x00;
buf[i++]=0x00;	// 3 ok

buf[i++]=0xff;
buf[i++]=0x00;
buf[i++]=0xff;

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");
#endif

#endif


#if 0
// T0 SELECT ROOT
i=0;
//sleep(1);
buf[i++]=0x00;
buf[i++]=5;
buf[i++]=0x00;
buf[i++]=0x01;

/*
buf[i++]=0x01;	// ICC cmd CT->ICC
buf[i++]=0x00;	// PCB
buf[i++]=21;
*/

buf[i++]=0x00;
buf[i++]=0xa0;
buf[i++]=0x04;
buf[i++]=0x00;
buf[i++]=0x10;


  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");


i=0;
//sleep(1);
buf[i++]=0x00;
buf[i++]=0x10;
buf[i++]=0x00;
buf[i++]=0x02;

buf[i++]=0x8c;
buf[i++]=0xcf;
buf[i++]=0x24;
buf[i++]=0x63;
buf[i++]=0xb7;
buf[i++]=0xb5;
buf[i++]=0x35;
buf[i++]=0x0c;
buf[i++]=0xad;
buf[i++]=0xab;
buf[i++]=0x79;
buf[i++]=0x1a;
buf[i++]=0xcb;
buf[i++]=0x9e;
buf[i++]=0x67;
buf[i++]=0x75;

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");



#endif


#if 0
//SELECT ROOT
i=0;
sleep(1);
buf[i++]=0x00;
buf[i++]=25;
buf[i++]=0x00;
buf[i++]=0x06;

buf[i++]=0x01;	// ICC cmd CT->ICC
buf[i++]=0x00;	// PCB
buf[i++]=21;

buf[i++]=0x00;
buf[i++]=0xa0;
buf[i++]=0x04;
buf[i++]=0x00;
buf[i++]=0x10;

buf[i++]=0x8c;
buf[i++]=0xcf;
buf[i++]=0x24;
buf[i++]=0x63;
buf[i++]=0xb7;
buf[i++]=0xb5;
buf[i++]=0x35;
buf[i++]=0x0c;
buf[i++]=0xad;
buf[i++]=0xab;
buf[i++]=0x79;
buf[i++]=0x1a;
buf[i++]=0xcb;
buf[i++]=0x9e;
buf[i++]=0x67;
buf[i++]=0x75;

buf[i++]=cmp_edc(&buf[4], i-4);
printf("%02X %02X\n", buf[i-1], buf[i-1]^buf[i-1]);

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");

#endif


#if 0
//SELECT IDENTIFY

//sleep(1);

i=0;
buf[i++]=0x00;
buf[i++]=18;
buf[i++]=0x00;
buf[i++]=0x06;

buf[i++]=0x01;	// ICC cmd CT->ICC
buf[i++]=0x00;	// PCB
buf[i++]=14;

buf[i++]=0x00;
buf[i++]=0xa0;
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
//buf[i++]=0x00;

buf[i++]=cmp_edc(&buf[4], i-4);
printf("%02X %02X\n", buf[i-1], buf[i-1]^buf[i-1]);

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");

#endif


#if 0
//GET STATUS 
i=0;
buf[i++]=0x00;
buf[i++]=11;
buf[i++]=0x00;
buf[i++]=0x06;

buf[i++]=0x01;	// ICC cmd CT->ICC
buf[i++]=0x00;	// PCB
buf[i++]=0x07;

buf[i++]=0x80;
buf[i++]=0xf2;
buf[i++]=0xe0;
buf[i++]=0x00;
buf[i++]=0x02;
buf[i++]=0x4f;
buf[i++]=0x00;

buf[i++]=cmp_edc(&buf[4], i-4);

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX_1: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");
#endif


#if 0
//READ CPLC
i=0;
buf[i++]=0x00;
buf[i++]=0x09;
buf[i++]=0x00;
buf[i++]=0x06;

buf[i++]=0x01;	// ICC cmd CT->ICC
buf[i++]=0x00;	// PCB
buf[i++]=0x05;

buf[i++]=0x00;
buf[i++]=0xca;
buf[i++]=0x9f;
buf[i++]=0x7f;
buf[i++]=0x00;

buf[i++]=cmp_edc(&buf[4], i-4);

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");

#endif

#if 0
//SELECT SEC DOMAIN
i=0;
buf[i++]=0x00;
buf[i++]=17;
buf[i++]=0x00;
buf[i++]=0x06;

buf[i++]=0x01;	// ICC cmd CT->ICC
buf[i++]=0x00;	// PCB
buf[i++]=13;

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
//buf[i++]=0x00;

buf[i++]=cmp_edc(&buf[4], i-4);
printf("%02X %02X\n", buf[i-1], buf[i-1]^buf[i-1]);

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");

#endif


#if 0
i=0;
buf[i++]=0x00;
buf[i++]=5;	//15
buf[i++]=0x00;
buf[i++]=0x03;	// 7 ok

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

buf[i++]=0x00;

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX_1: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");

#if 0
i=0;
buf[i++]=0x00;
buf[i++]=10;	//15
buf[i++]=0x00;
buf[i++]=0x01;	// 7 ok


buf[i++]=0xa0;
buf[i++]=0x00;
buf[i++]=0x00;
buf[i++]=0x01;
buf[i++]=0x67;
buf[i++]=0x41;
buf[i++]=0x30;
buf[i++]=0x00;
buf[i++]=0xff;

buf[i++]=0x00;

  usb_send (ACTION_SIM_TXRX, i, buf, &answerLen, answer);

  printf ("RX_2: ");
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", answer[i]);
  printf ("\n");
#endif
#endif

#if 0
buf[0]=0x3f;
buf[1]=0x00;
answerLen=simx(0x00, 0xa4, 0x00, 0x04, 0x02, buf);
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", buf[i]);
  printf ("\n");

answerLen=simx(0x80, 0xF2, 0x01, 0x01, 0xff, buf);
  for (i = 0; i < answerLen; i++)
    printf ("%02X ", buf[i]);
  printf ("\n");
#endif

//    dump_file (&n_mf, 0);

X_END:
  close_device ();
  if (err)
    exit (-1);
}
