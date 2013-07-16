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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <tprog/tproglib.h>
#include "../module-src/turboadapter.h"
#include "../module-src/btldr.h"

#include <include/turbo/btldr.h>

#ifdef WIN32_MINGW
#include <windows.h>
#endif

#define TARGET_USB_BRUM		4

void help ()
{
  printf ("Usage: btldr [args] [file]\n");
  printf ("-a application\tuse application, default is %s\n",
	  APP_TURBO_ADAPTER);
  printf ("-d device\tuse USB device, default is %s\n", USBDEVICE);
  printf ("-e file\t\tupload EEPROM image too\n");
  printf ("-l\t\tuse old (slow) protocol\n");
  printf
    ("-m module\ttarget is BRUM number 1 or 2 instead of TurboAdapter(=0), 3 is USB BRUM\n");
  printf ("-s\t\tuse SIM connector\n");
  printf ("-i\t\tget SN (on devices with btldr fw version >=4\n");
  printf ("-b settings\tune bootloader, in hex (on devices with btldr fw version >=5\n");
  printf ("-k key[16]\tuse btldr_key, hex notation, eg. 404142....4f (on devices with btldr fw version ==4)\n");
  printf ("-x attr\tset attribute X, in hex (on devices with btldr fw version ==4\n");

  printf ("-y value\tset Y, in hex (on devices with btldr fw version ==4\n");
  printf ("-n key\t\tset key N - for key arrays, in hex (on devices with btldr fw version ==4\n");
  printf ("-f\t\tfast btldr\n");
  printf ("-h\t\thelp\n");
  printf ("-v\t\tverbose\n");
  printf ("-t\t\ttest, verify content of memory\n");

printf("\n-x values:\n");
printf("\t\tD_ATTR_CONF_BTLDR_LOCKED        0x11\n");
printf("\t\tD_ATTR_CONF_OTA_ALLOWED         0x12\n");
printf("\t\tD_ATTR_CONF_FAKE_ATR            0x13\n");
printf("\t\tD_ATTR_CONF_RESET_LOCKED        0x14\n");
printf("\t\tD_ATTR_CONF_APP_LOCKED          0x15\n");
printf("\t\tD_ATTR_CONF_SN_LOCKED           0x16\n");
printf("\t\tD_ATTR_CONF_TPIN_LOCKED         0x17\n");
printf("\t\tD_ATTR_XCONF_MAC                0x40\n");
printf("\t\tD_ATTR_XCONF_TPIN               0x41\n");
printf("\t\tD_ATTR_XCONF_BTLDR_KEY          0x42\n");
printf("\t\tD_ATTR_XCONF_PASSWD             0x43\n");
printf("\t\tD_ATTR_XCONF_READ_KEY           0x44\n");
printf("\t\tD_ATTR_XCONF_DE_KEY             0x45\n");
printf("\t\tD_ATTR_XCONF_D_KEY              0x46\n");
printf("\t\tD_ATTR_XCONF_E_KEY              0x47\n");
printf("\t\tD_ATTR_XCONF_ATR                0x48\n");

  exit (-1);
}

int main (int argc, char **argv)
{
  int result;
  u8 *dev = USBDEVICE;
  u8 *app_name = APP_TURBO_ADAPTER;
  u8 answer[2048];
  u16 answerLen;
  u32 i, i2;
  u16 l;
  u16 flashPageNumber;
  int fdstdin;
  int flags;
  int bytes_read;
  u8 buf[1024];
  u8 verbuf[1024];
  int len;
  FILE *fe;
  u8 *filename = NULL;
  u8 *efile = NULL;
  u8 c;
  u8 buf2[0x20000];
  u8 buf3[0x2000];
  u8 verify = 0;
  u8 was_error = 0;
  u8 old_prot = 0;
  u8 target = 0;
  u8 gen_target = BLADOX_DEVICE_TPROG;
int j;
u8 btldr_len;
u8 btldr_buf[1000];

#define D_ACT_REFLASH		0x00
#define D_ACT_ATTR		0x01
#define D_ACT_SN		0x02
#define D_ACT_FAST_BTLDR	0x03
#define D_ACT_ATTR_5		0x04

u8 act=D_ACT_REFLASH;
int attr_x;
u8 attr_y[1000];
int key_n=0;
u8 k[1000];
u8 * a_y;
u8 k_set=0;
u8 a_y_len=0;

  do
  {
    result = getopt (argc, argv, "hd:a:vse:tlm:ifx:y:n:k:b:");
    switch (result)
    {
      case 'b':
act=D_ACT_ATTR_5;
if(strlen(optarg)<32)
{
fprintf(stderr, "-b key must be at least 32 chars, i.e. 16 bytes in hex\n");
exit(-1);
}
{
int d;
       u8 bufx[1000];

        strcpy(bufx, optarg);

l=strlen(optarg);
        for(i=0;i<l/2;i++)
        {
        sscanf(&bufx[(l/2-1-i)*2], "%x", &d);
        bufx[(l/2-1-i)*2]='\0';
        btldr_buf[l/2-1-i]=d;
        }
btldr_len=strlen(optarg)/2;
}
	break;

      case 'k':
k_set=1;
if(strlen(optarg)!=32)
{
fprintf(stderr, "-k key must be 32 chars, i.e. 16 bytes in hex\n");
exit(-1);
}
strcpy(buf, optarg);
for(i=0;i<16;i++)
{
int x;
sscanf(&buf[(15-i)*2], "%x", &x);
buf[(15-i)*2]='\0';
k[15-i]=x;
}
printf("KEY: ");
for(i=0;i<16;i++)printf("%02x", k[i]);
printf("\n");
	break;

      case 'a':
	app_name = optarg;
	break;
      case 'd':
	dev = optarg;
	break;
      case 'e':
	efile = optarg;
	break;
      case 'l':
	old_prot = 1;
	break;
      case 'm':
	target = atoi (optarg);
	if (target == 3)
	  target = TARGET_USB_BRUM;
	break;
      case 's':
	target = BTLDR_PORT_SIM;
	break;
      case 't':
	verify = 1;
	break;
      case 'v':
	verbose_inc ();
	break;
case 'i':
act=D_ACT_SN;
	break;
case 'f':
act=D_ACT_FAST_BTLDR;
	break;
case 'x':
act=D_ACT_ATTR;
sscanf(optarg, "%x", &attr_x);
printf("X %02x\n",attr_x);
	break;
case 'y':
a_y=optarg;
strcpy(buf, optarg);
j=strlen(optarg)/2;
if(j*2!=strlen(optarg))
{
printf("-y has to be in hex, even length\n");
help();
}
for(i=0;i<j;i++)
{
int x;
sscanf(&buf[(j-1-i)*2], "%x", &x);
buf[(j-1-i)*2]='\0';
attr_y[j-1-i]=x;
}
printf("Y: ");
for(i=0;i<j;i++)printf("%02x", attr_y[i]);
a_y_len=j;
printf("\n");
	break;
case 'n':
sscanf(optarg, "%d", &key_n);
	break;
      case 'h':
	help ();
	break;
    }
  }
  while (result > 0);

if(k_set==0 && (act==D_ACT_FAST_BTLDR || act==D_ACT_ATTR))
{
printf("-k is needed\n");
help();
}

  if ((act==D_ACT_REFLASH || act== D_ACT_FAST_BTLDR)&& argc - optind != 1)
    help ();

  if (target > TARGET_USB_BRUM)
    help ();

  if (target == TARGET_USB_BRUM)
  {
    gen_target = BLADOX_DEVICE_USBBRUM;
    old_prot = 1;
  }

  open_device_gen (dev, gen_target);

  if (select_app (app_name) != NO_ERROR)
  {
    close_device ();
    fprintf (stderr, "Cannot open application %s\n", app_name);
    exit (-1);
  }


if(act==D_ACT_ATTR_5)
{
i=0;
answer[i++] = target;
answer[i++] = btldr_len;
printf("ATTR: ");
for(j=0;j<btldr_len;j++)
{
printf("%02x ", btldr_buf[j]);
answer[i++]=btldr_buf[j];
}
printf("\n");

usb_send (ACTION_TURBO_ADAPTER_BTLDR_SET_ATTR_5, i, answer, &answerLen, answer);

printf("ANS: %02x ", answer[0]);
printf("\n");

if(answer[0]==0x20)
{
printf("Wrong key\n");
}
else
if(answer[0]==0x01)
{
printf("OK\n");
printf("%02x ", answer[0]);
printf("\n");
}
else
{
printf("Unsupported or locked.\n");
}
}
else
if(act==D_ACT_ATTR)
{
i=0;
answer[i++] = target;
for(j=0;j<16;j++)answer[i++]=k[j];
answer[i++] = attr_x;
answer[i++] = key_n;
answer[i++] = a_y_len;
for(j=0;j<a_y_len;j++)answer[i++]=attr_y[j];

usb_send (ACTION_TURBO_ADAPTER_BTLDR_SET_ATTR, i, answer, &answerLen, answer);

printf("ANS: %02x ", answer[0]);
printf("\n");

if(answer[0]==0x20)
{
printf("Wrong key -k\n");
}
else
if(answer[0]==0x21)
{
printf("Wrong -x\n");
}
else
if(answer[0]==0x01)
{
printf("OK\n");
printf("%02x ", answer[0]);
printf("\n");
}
else
{
printf("Unsupported or locked.\n");
}
}
else
if(act==D_ACT_SN)
{
answer[0] = target;
    usb_send (ACTION_TURBO_ADAPTER_BTLDR_GET_SN, 1, answer, &answerLen, answer);

if(answerLen>1)
{
printf("BTLDR VERSION: %d\n", answer[1]);
printf("SN: ");
for(i=2;i<answerLen;i++)
printf("%c", answer[i]);
//printf("%02x ", answer[i]);
printf("\n");
}
else
{
printf("Unsupported or locked.\n");
}

}
else
if(act==D_ACT_REFLASH || act==D_ACT_FAST_BTLDR)
{
  filename = argv[optind];

#ifndef WIN32_MINGW
  fe = fopen (filename, "rb");
  if (fe == NULL)
  {
    close_device ();
    printf ("Cannot open %s for reading\n", filename);
    exit (-1);
  }

  for (i2 = 0; i2 < 0x1E000; i2++)
    buf2[i2] = 0xFF;

  i2 = 0;
  while (!feof (fe))
  {
    buf2[i2] = fgetc (fe);
    i2++;
  }
  fclose (fe);

  if (efile)
  {
    fe = fopen (efile, "rb");
    if (fe == NULL)
    {
    close_device ();
      printf ("Cannot open %s for reading\n", efile);
      exit (-1);
    }

    for (i2 = 0; i2 < 0x1000; i2++)
      buf3[i2] = 0xFF;

    i2 = 0;
    while (!feof (fe))
    {
      buf3[i2] = fgetc (fe);
      i2++;
    }
    fclose (fe);
  }
#else
  HANDLE *f1;
  int n_in;

  f1 =
    CreateFile (argv[optind], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

  if (f1 == INVALID_HANDLE_VALUE)
  {
    close_device ();
    printf ("Cannot open file %s (error %d)\n", argv[optind],
	    GetLastError ());
    exit (-1);
  }

  for (i2 = 0; i2 < 0x1E000; i2++)
    buf2[i2] = 0xFF;

  ReadFile (f1, buf2, 0x1E000, (DWORD *) (&n_in), NULL);

  CloseHandle (f1);

  if (efile)
  {
    f1 = CreateFile (efile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (f1 == INVALID_HANDLE_VALUE)
    {
    close_device ();
      printf ("Cannot open file %s (error %d)\n", efile, GetLastError ());
      exit (-1);
    }

    for (i2 = 0; i2 < 0x1000; i2++)
      buf3[i2] = 0xFF;

    ReadFile (f1, buf3, 0x1000, (DWORD *) (&n_in), NULL);

    CloseHandle (f1);
  }
#endif


  printf ("Turbo Adapter/BRUM bootloader mode initialized, %s\n", app_name);

if(act==D_ACT_REFLASH)
{
  printf ("BOOTLOADER is erasing FLASH and EEPROM, wait for approx. 50s\n");
  answer[0] = target;

//u16 divider=372;
u16 divider=16;		// 16 is min
answer[1]= divider>>8;
answer[2]=divider;

  answerLen = 1;
  if (target == TARGET_USB_BRUM)
    usb_send (UCMD_BTLDR_MODE_INIT, 0, answer, &answerLen, answer);
  else
    usb_send (ACTION_TURBO_ADAPTER_BTLDR_INIT_V4, 3, answer, &answerLen,
	      answer);
}
else
{
// fast
  answer[0] = target;

//u16 divider=372;
u16 divider=16;		// 16 is min
answer[1]= divider>>8;
answer[2]=divider;
for(i=0;i<16;i++)answer[3+i]=k[i];

usb_send (ACTION_TURBO_ADAPTER_BTLDR_FAST_BTLDR, 3+16, answer, &answerLen, answer);
printf("ANSWER: ");
for(i=0;i<answerLen;i++)printf("%02x ", answer[i]);
printf("\n");

if(answer[0]!=0x36)
{
printf("Unsupported or locked.\n");
goto X_ERROR;
}
}

  printf ("BOOTLOADER is updating FLASH\n");

  if (old_prot)
  {
    for (i2 = 0; i2 < 0x1E000; i2 += 256)
    {
      printf ("Page %4d/(0-%4d)\r", i2 / 256, 0x1E0 - 1);
      fflush (stdout);
      flashPageNumber = i2 / 256;	//address(last page before 4kB bootloader)= 0x1DF00 bytes(0xEF00 words) (page=256B)
      answer[0] = (flashPageNumber & 0xFF);	//low byte
      answer[1] = (flashPageNumber >> 8);	//high byte
      for (i = 0; i < 256; i++)
      {
	answer[2 + i] = buf2[i2 + i];
      }

      answerLen = 1;
      if (target == TARGET_USB_BRUM)
	usb_send (UCMD_BTLDR_MODE_WRITE_PAGE_FLASH, 2 + 256, answer,
		  &answerLen, answer);
      else
	usb_send (ACTION_TURBO_ADAPTER_BTLDR_WRITE_PAGE, 2 + 256, answer,
		  &answerLen, answer);

      if (verify)
      {
	answer[0] = (flashPageNumber & 0xFF);	//low byte
	answer[1] = (flashPageNumber >> 8);	//high byte

	answerLen = 257;
	if (target == TARGET_USB_BRUM)
	{
	  usb_send (UCMD_BTLDR_MODE_READ_PAGE_FLASH, 2, answer, &answerLen,
		    verbuf);
	  answerLen = 256;
	}
	else
	  usb_send (ACTION_TURBO_ADAPTER_BTLDR_READ_PAGE, 2, answer,
		    &answerLen, verbuf);

	if (answerLen != 256)
	{
	  printf ("ERROR: verify wrong answer len %d\n", answerLen);
	  was_error = 1;
	}

	for (i = 0; i < answerLen; i++)
	{
	  if (verbuf[i] != buf2[i2 + i])
	  {
	    printf ("ERROR: verify FLASH at pos %06X: %02X, should be %02X\n",
		    i2 + i, verbuf[i], buf2[i2 + i]);
	    was_error = 1;
	  }
	}
      }
    }
    printf ("\n");

    if (efile)
    {
      printf ("BOOTLOADER is updating EEPROM\n");
      for (i2 = 0; i2 < 0x1000; i2 += 8)
      {
	printf ("Page %4d/(0-%4d)\r", i2 / 8, 511);
	fflush (stdout);
	flashPageNumber = i2 / 8;	//address page len 8b
	answer[0] = (flashPageNumber & 0xFF);	//low byte
	answer[1] = (flashPageNumber >> 8);	//high byte
	for (i = 0; i < 8; i++)
	  answer[2 + i] = buf3[i2 + i];

	answerLen = 1;
	if (target == TARGET_USB_BRUM)
	  usb_send (UCMD_BTLDR_MODE_WRITE_PAGE_EEPROM, 2 + 8, answer,
		    &answerLen, answer);
	else
	  usb_send (ACTION_TURBO_ADAPTER_BTLDR_WRITE_PAGE_EEPROM, 2 + 8,
		    answer, &answerLen, answer);

	if (verify)
	{
	  answer[0] = (flashPageNumber & 0xFF);	//low byte
	  answer[1] = (flashPageNumber >> 8);	//high byte

	  answerLen = 1 + 8;
	  if (target == TARGET_USB_BRUM)
	  {
	    usb_send (UCMD_BTLDR_MODE_READ_PAGE_EEPROM, 2, answer, &answerLen,
		      verbuf);
	    answerLen = 8;
	  }
	  else
	    usb_send (ACTION_TURBO_ADAPTER_BTLDR_READ_PAGE_EEPROM, 2, answer,
		      &answerLen, verbuf);
	  if (answerLen != 8)
	  {
	    printf ("ERROR: verify wrong answer len %d\n", answerLen);
	    was_error = 1;
	  }

	  for (i = 0; i < answerLen; i++)
	  {
	    if (verbuf[i] != buf3[i2 + i])
	    {
	      printf
		("ERROR: verify EEPROM at pos %04X: %02X, should be %02X\n",
		 i2 + i, verbuf[i], buf3[i2 + i]);
	      was_error = 1;
	    }
	  }
	}
      }
      printf ("\n");
    }
  }
  else
  {
    u8 crc;

    was_error = 0;

    for (i2 = 0; i2 < 0x1E000; i2++)
    {
      if (i2 == 0)
	crc = buf2[i2];
      else
	crc ^= buf2[i2];
    }
    answer[0] = crc;
    usb_send (ACTION_TURBO_ADAPTER_BTLDR_SRAM_FLASH, 1, answer, &answerLen,
	      answer);

    printf (">Copying FLASH image to programmer SRAM\n");
    for (i2 = 0; i2 < 0x1E000; i2++)
      usb_put_char (buf2[i2]);

    printf ("<Copying FLASH image to programmer SRAM finished\n");

    c = usb_get_char ();
    if (c == BTLDR_CRC_ERROR)
    {
      printf ("SRAM CRC ERROR\n");
      was_error = 1;
      goto X_ERROR;
    }
    if (c == BTLDR_VERIFY_ERROR)
    {
      printf ("SRAM VERIFY ERROR\n");
      was_error = 1;
      goto X_ERROR;
    }

    printf (">Writing to FLASH memory...\n");
    c = usb_get_char ();
    printf ("<Writing to FLASH finished\n");

    if (c == BTLDR_CRC_ERROR)
    {
      printf ("FLASH CRC ERROR\n");
      was_error = 1;
    }
    if (c == BTLDR_VERIFY_ERROR)
    {
      printf ("FLASH VERIFY ERROR\n");
      was_error = 1;
    }

    if (was_error == 0 && efile)
    {
      printf ("BOOTLOADER is updating EEPROM\n");

      for (i2 = 0; i2 < 0x1000; i2++)
      {
	if (i2 == 0)
	  crc = buf3[i2];
	else
	  crc ^= buf3[i2];
      }
      answer[0] = crc;
      usb_send (ACTION_TURBO_ADAPTER_BTLDR_SRAM_EEPROM, 1, answer, &answerLen,
		answer);

      printf (">Copying EEPROM image to programmer SRAM\n");
      for (i2 = 0; i2 < 0x1000; i2++)
	usb_put_char (buf3[i2]);
      printf ("<Copying EEPROM image to programmer SRAM finished\n");

      c = usb_get_char ();
      if (c == BTLDR_CRC_ERROR)
      {
	printf ("SRAM CRC ERROR\n");
	was_error = 1;
	goto X_ERROR;
      }
      if (c == BTLDR_VERIFY_ERROR)
      {
	printf ("SRAM VERIFY ERROR\n");
	was_error = 1;
	goto X_ERROR;
      }

      printf (">Writing to EEPROM memory...\n");
      c = usb_get_char ();
      printf ("<Writing to EEPROM finished\n");

      if (c == BTLDR_CRC_ERROR)
      {
	printf ("EEPROM CRC ERROR\n");
	was_error = 1;
      }
      if (c == BTLDR_VERIFY_ERROR)
      {
	printf ("EEPROM VERIFY ERROR\n");
	was_error = 1;
      }
    }
  }
}


X_ERROR:
  close_device ();
  if (was_error)
  {
    printf ("ERROR!\n");
    exit (-1);
  }
}
