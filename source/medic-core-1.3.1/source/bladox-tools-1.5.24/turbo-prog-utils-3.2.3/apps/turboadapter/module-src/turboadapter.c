/*
 * Turbo Programmer Utilities, turbo-prog-utils, www.bladox.com 
 *
 * Copyright (C) 2004, 2005 BLADOX, s.r.o.
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

#include <config.h>
#include <tprog/tprog.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include <turbo/btldr.h>

#include "turboadapter.h"
#include "btldr.h"
#include "dbg.h"
#include "clka.h"

void turbo_handler (u8 action, USB_Data * data)
{
  u8 *buf = buf_A ();
  u16 i;
  u8 what;

  switch (action)
  {
    case ACTION_APP_VERSION:
      data->buf[0] = VERSION_A;
      data->buf[1] = VERSION_B;
      data->buf[2] = VERSION_C;
      break;

    case ACTION_TURBO_ADAPTER_BTLDR_INIT:
      reg_action (ACTION_SIG_UART1_RECV);
      init_vars ();
      init_turboadapter_btldr_port (BTLDR_PORT_TURBO_ADAPTER);
      init_btldrUart ();
      init_clka_timer ();
      reset_turbo_for_bootloader (BTLDR_PORT_TURBO_ADAPTER);

      btldrTxSendChar (UCMD_BTLDR_MODE_INIT);
      while (btldrRxChars () == 0);
      buf[0] = btldrRxGetChar ();

      usb_send (NO_ERROR, 1, buf);
      break;

    case ACTION_TURBO_ADAPTER_BTLDR_SIM_INIT:
      reg_action (ACTION_SIG_UART1_RECV);
      init_vars ();
      init_turboadapter_btldr_sim_port ();
      init_btldrUart ();
      init_clka_timer ();
      reset_turbo_for_bootloader_sim ();

      btldrTxSendChar (UCMD_BTLDR_MODE_INIT);
      while (btldrRxChars () == 0);
      buf[0] = btldrRxGetChar ();

      usb_send (NO_ERROR, 1, buf);
      break;

    case ACTION_TURBO_ADAPTER_BTLDR_INIT_V3:
      reg_action (ACTION_SIG_UART1_RECV);
      init_vars ();
      what = data->buf[0];

      if (what == BTLDR_PORT_SIM)
	init_turboadapter_btldr_sim_port ();
      else
	init_turboadapter_btldr_port (what);

      init_btldrUart ();
      init_clka_timer ();

      if (what == BTLDR_PORT_SIM)
	reset_turbo_for_bootloader_sim ();
      else
	reset_turbo_for_bootloader ();

      btldrTxSendChar (UCMD_BTLDR_MODE_INIT);
      while (btldrRxChars () == 0);
      buf[0] = btldrRxGetChar ();

      usb_send (NO_ERROR, 1, buf);
      break;

    case ACTION_TURBO_ADAPTER_BTLDR_INIT_V4:
{
u8 s1;	// divider_h
u8 s2;	// divider_l
u8 nr_count;
u16 act;

      reg_action (ACTION_SIG_UART1_RECV);
      init_vars ();
      what = data->buf[0];
      s1 = data->buf[1];
      s2 = data->buf[2];

      if (what == BTLDR_PORT_SIM)
	init_turboadapter_btldr_sim_port ();
      else
	init_turboadapter_btldr_port (what);

      init_btldrUart ();
      init_clka_timer ();

      if (what == BTLDR_PORT_SIM)
	reset_turbo_for_bootloader_sim ();
      else
	reset_turbo_for_bootloader ();


      btldrTxSendChar (UCMD_BTLDR_MODE_SET_SPEED);
act=s1;
act<<=8;
act|=s2;
act=(act/8)-1;

      btldrTxSendChar (act>>8);
      btldrTxSendChar (act);

// set new uart speed
set_divider(s1, s2);

#define DEF_MAX_NR_COUNT	50
nr_count=0;

      while (btldrRxChars () == 0 && nr_count<DEF_MAX_NR_COUNT)
{
nr_count++;
if(what==BTLDR_PORT_SIM)
	led_on (LED_TFM);
else
	led_on(LED_SIM);

delayMs(10);
	led_on (LED_NO);
delayMs(10);
}

if(btldrRxChars()!=0 && btldrRxGetChar()==0x37)
{
led_on(4);
}
else
{
// no answer, set default speed
led_on(3);
set_divider_default();
}

      btldrTxSendChar (UCMD_BTLDR_MODE_INIT);
      while (btldrRxChars () == 0);
      buf[0] = btldrRxGetChar ();

      usb_send (NO_ERROR, 1, buf);
}
      break;

case ACTION_TURBO_ADAPTER_BTLDR_GET_SN:
{
  u8 nr_count;

  reg_action (ACTION_SIG_UART1_RECV);
  init_vars ();
  what = data->buf[0];

  if (what == BTLDR_PORT_SIM)
    init_turboadapter_btldr_sim_port ();
  else
    init_turboadapter_btldr_port (what);

  init_btldrUart ();
  init_clka_timer ();

  if (what == BTLDR_PORT_SIM)
    reset_turbo_for_bootloader_sim ();
  else
    reset_turbo_for_bootloader ();

  btldrTxSendChar (UCMD_BTLDR_MODE_GET_SN);

#define DEF_MAX_NR_COUNT	50
  nr_count = 0;

  while (btldrRxChars () == 0 && nr_count < DEF_MAX_NR_COUNT)
    {
      nr_count++;

/*
if(what == BTLDR_PORT_SIM)
{
      delayMs (10);
      led_on (LED_NO);
      delayMs (10);
}
else
*/
      delayMs (1);

      if (what == BTLDR_PORT_SIM)
	led_on (LED_TFM);
      else
	led_on (LED_SIM);

    }

  if (btldrRxChars () != 0 && (i = btldrRxGetChar ()) == 0x01)
    {
for(i=0;i<16;i++)
{
      while (btldrRxChars () == 0);
      buf[i] = btldrRxGetChar ();
}

      usb_send (NO_ERROR, 16, buf);
    }
  else
    {
// not supported or locked
      buf[0] = 0x00;
      usb_send (NO_ERROR, 1, buf);
    }
}
	break;

case ACTION_TURBO_ADAPTER_BTLDR_SET_ATTR_5:
{
  u8 nr_count;
  u8 val;
u8 len;

  reg_action (ACTION_SIG_UART1_RECV);
  init_vars ();
  what = data->buf[0];

  if (what == BTLDR_PORT_SIM)
    init_turboadapter_btldr_sim_port ();
  else
    init_turboadapter_btldr_port (what);

  init_btldrUart ();
  init_clka_timer ();

  if (what == BTLDR_PORT_SIM)
    reset_turbo_for_bootloader_sim ();
  else
    reset_turbo_for_bootloader ();

  len = data->buf[1];

  btldrTxSendChar (UCMD_BTLDR_MODE_SET_ATTR);
  nr_count = 0;

  while (btldrRxChars () == 0 && nr_count < DEF_MAX_NR_COUNT)
    {
      nr_count++;

      delayMs (10);
      led_on (LED_NO);
      delayMs (10);

      if (what == BTLDR_PORT_SIM)
	led_on (LED_TFM);
      else
	led_on (LED_SIM);
    }

  i = 0x10;

  if (btldrRxChars () != 0 && (i = btldrRxGetChar ()) == 0x01)
    {
// supported
	btldrTxSendChar (len);
      for (i = 0; i < len; i++)
	btldrTxSendChar (data->buf[i + 2]);	// data

      while (btldrRxChars () == 0);
i=btldrRxGetChar();
if(i==0x01)
{
// ok
	  buf[0] = 0x01;	//no error
	  usb_send (NO_ERROR, 1, buf);

}
       else
	{
	  buf[0] = 0x20;	//wrong key
	  usb_send (NO_ERROR, 1, buf);
	}
    }
  else
    {
// not supported or locked
      buf[0] = i;
      usb_send (NO_ERROR, 1, buf);
    }

}
	break;


case ACTION_TURBO_ADAPTER_BTLDR_SET_ATTR:
{
  u8 nr_count;
  u8 val;

  reg_action (ACTION_SIG_UART1_RECV);
  init_vars ();
  what = data->buf[0];

  if (what == BTLDR_PORT_SIM)
    init_turboadapter_btldr_sim_port ();
  else
    init_turboadapter_btldr_port (what);

  init_btldrUart ();
  init_clka_timer ();

  if (what == BTLDR_PORT_SIM)
    reset_turbo_for_bootloader_sim ();
  else
    reset_turbo_for_bootloader ();

if(data->buf[1 + 16]==0x00)
  btldrTxSendChar (UCMD_BTLDR_MODE_SET_HWD);
else
  btldrTxSendChar (UCMD_BTLDR_MODE_SET_ATTR);
  nr_count = 0;

  while (btldrRxChars () == 0 && nr_count < DEF_MAX_NR_COUNT)
    {
      nr_count++;

      delayMs (10);
      led_on (LED_NO);
      delayMs (10);

      if (what == BTLDR_PORT_SIM)
	led_on (LED_TFM);
      else
	led_on (LED_SIM);
    }

  i = 0x10;

  if (btldrRxChars () != 0 && (i = btldrRxGetChar ()) == 0x01)
    {
// supported
      for (i = 0; i < 16; i++)
	btldrTxSendChar (data->buf[i + 1]);	// key

      while (btldrRxChars () == 0);
      if (btldrRxGetChar () == 0x01)
	{
	  val = data->buf[1 + 16];
if(val==0x00) goto X_HWD;
	  btldrTxSendChar (val);	// attr
	  while (btldrRxChars () == 0);
	  if (btldrRxGetChar () == 0x01)
	    {
X_HWD:
// send attr
	      if (val == 0x00 || (val >= 0x11 && val <= 0x1f))
		{
// 1 byte
		  btldrTxSendChar (data->buf[1 + 16 + 1 + 1 + 1]);	// val
		}
	      else if (val == D_ATTR_XCONF_ATR)
		{
		  val = data->buf[1 + 16 + 1 + 1];
		  btldrTxSendChar (val);	// len
		  for (i = 0; i < val; i++)
		    btldrTxSendChar (data->buf[1 + 16 + 1 + 1 + 1 + i]);
		}
	      else if(val == D_ATTR_XCONF_MAC || val == D_ATTR_XCONF_TPIN || val == D_ATTR_XCONF_BTLDR_KEY)
		{
		  val = data->buf[1 + 16 + 1 + 1];
		  for (i = 0; i < val; i++)
		    btldrTxSendChar (data->buf[1 + 16 + 1 + 1 + 1 + i]);
		}
		else
		{
		  btldrTxSendChar (data->buf[1 + 16 + 1]);	// key_nr
		  for (i = 0; i < 16; i++)
		    btldrTxSendChar (data->buf[1 + 16 + 1 + 1 + 1 + i]);
		}

	    while (btldrRxChars () == 0);
	      buf[0] = btldrRxGetChar();
	      usb_send (NO_ERROR, 1, buf);
	    }
	  else
	    {
	      buf[0] = 0x21;	//wrong attr
	      usb_send (NO_ERROR, 1, buf);
	    }
	}
      else
	{
	  buf[0] = 0x20;	//wrong key
	  usb_send (NO_ERROR, 1, buf);
	}
    }
  else
    {
// not supported or locked
      buf[0] = i;
      usb_send (NO_ERROR, 1, buf);
    }

}
	break;

case ACTION_TURBO_ADAPTER_BTLDR_FAST_BTLDR:
{
u8 s1;	// divider_h
u8 s2;	// divider_l
u8 nr_count;
u16 act;

      reg_action (ACTION_SIG_UART1_RECV);
      init_vars ();
      what = data->buf[0];
      s1 = data->buf[1];
      s2 = data->buf[2];

      if (what == BTLDR_PORT_SIM)
	init_turboadapter_btldr_sim_port ();
      else
	init_turboadapter_btldr_port (what);

      init_btldrUart ();
      init_clka_timer ();

      if (what == BTLDR_PORT_SIM)
	reset_turbo_for_bootloader_sim ();
      else
	reset_turbo_for_bootloader ();


      btldrTxSendChar (UCMD_BTLDR_MODE_SET_SPEED);
act=s1;
act<<=8;
act|=s2;
act=(act/8)-1;

      btldrTxSendChar (act>>8);
      btldrTxSendChar (act);

// set new uart speed
set_divider(s1, s2);

#define DEF_MAX_NR_COUNT	50
nr_count=0;

      while (btldrRxChars () == 0 && nr_count<DEF_MAX_NR_COUNT)
{
nr_count++;
if(what==BTLDR_PORT_SIM)
	led_on (LED_TFM);
else
	led_on(LED_SIM);

delayMs(10);
	led_on (LED_NO);
delayMs(10);
}

if(btldrRxChars()!=0 && btldrRxGetChar()==0x37)
{
led_on(4);
}
else
{
// no answer, set default speed
led_on(3);
set_divider_default();
}




btldrTxSendChar (UCMD_BTLDR_MODE_FAST_BTLDR);
nr_count=0;

while (btldrRxChars () == 0 && nr_count<DEF_MAX_NR_COUNT)
{
nr_count++;

delayMs(10);
	led_on (LED_NO);
delayMs(10);

if(what==BTLDR_PORT_SIM)
	led_on (LED_TFM);
else
	led_on(LED_SIM);
}

i=0x10;

if(btldrRxChars()!=0 && (i=btldrRxGetChar())==0x01)
{
// supported
for(i=0;i<16;i++)
btldrTxSendChar(data->buf[i+3]);

      while (btldrRxChars () == 0);

      buf[0] = btldrRxGetChar ();
      usb_send (NO_ERROR, 1, buf);
}
else
{
// not supported or locked
buf[0]=i;
      usb_send (NO_ERROR, 1, buf);
}

}

	break;


    case ACTION_SIG_UART1_RECV:
      btldr_uart_irq ();
      break;

    case ACTION_TURBO_ADAPTER_BTLDR_WRITE_PAGE:
      btldr_flash_page (data, buf);
      break;
    case ACTION_TURBO_ADAPTER_BTLDR_READ_PAGE:
      btldr_read_page (data, buf);
      break;
    case ACTION_TURBO_ADAPTER_BTLDR_WRITE_PAGE_EEPROM:
      btldr_flash_page_eeprom (data, buf);
      break;
    case ACTION_TURBO_ADAPTER_BTLDR_READ_PAGE_EEPROM:
      btldr_read_page_eeprom (data, buf);
      break;

    case ACTION_TURBO_ADAPTER_BTLDR_SRAM_FLASH:
      btldr_sram_flash (data, buf);
      break;
    case ACTION_TURBO_ADAPTER_BTLDR_SRAM_EEPROM:
      btldr_sram_eeprom (data, buf);
      break;

// following is "dbg" related

    case ACTION_TURBO_ADAPTER_DBG_INIT:
      reg_action (ACTION_SIG_SPI);
      init_turboadapter_dbg_port ();
      usb_send (NO_ERROR, 0, NULL);
      break;

    case ACTION_SIG_SPI:
      usb_put_char_nb (inb (SPDR));
#if 0
      {
	u8 x = inb (SPDR);

	usb_put_char_nb (x);
	led_on (x % 8);
      }
#endif
      break;

    default:
      break;
  }
}
