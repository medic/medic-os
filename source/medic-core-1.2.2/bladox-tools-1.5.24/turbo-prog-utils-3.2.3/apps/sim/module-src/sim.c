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

#include <config.h>
#include <tprog/tprog.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "sim.h"
#include "uart.h"
#include "clka.h"

#define ME_CMD_SELECT                   0xa4
#define ME_CMD_STATUS                   0xf2
#define ME_CMD_READ_BINARY              0xb0
#define ME_CMD_UPDATE_BINARY            0xd6
#define ME_CMD_READ_RECORD              0xb2
#define ME_CMD_UPDATE_RECORD            0xdc
#define ME_CMD_SEEK                     0xa2
#define ME_CMD_INCREASE                 0x32
#define ME_CMD_VERIFY_CHV               0x20
#define ME_CMD_CHANGE_CHV               0x24
#define ME_CMD_DISABLE_CHV              0x26
#define ME_CMD_ENABLE_CHV               0x28
#define ME_CMD_UNBLOCK_CHV              0x2c
#define ME_CMD_INVALIDATE               0x04
#define ME_CMD_REHABILITATE             0x44
#define ME_CMD_RUN_GSM_ALGORITHM        0x88
#define ME_CMD_GET_RESPONSE             0xc0
#define ME_CMD_TERMINAL_PROFILE         0x10
#define ME_CMD_ENVELOPE                 0xc2
#define ME_CMD_FETCH                    0x12
#define ME_CMD_TERMINAL_RESPONSE        0x14
#define ME_CMD_SLEEP                    0xfa

#define NX_ATR_NONE	0
#define NX_ATR_TA	10
#define NX_ATR_TB	11
#define NX_ATR_TC	12
#define NX_ATR_TD	13
#define NX_ATR_TK	14
#define NX_ATR_TS	15
#define NX_ATR_T0	16
#define NX_ATR_TCK	17

//#define ATR_MAX 16
#define ATR_MAX 33

// defines structure of ATR response from card
struct ATR
{
  u8 buf[ATR_MAX + 1];
  u8 pts;
  u8 card_TA1;
  u8 card_TC1;
  u8 card_TC2;
};

//inverse convention (ATR starts with 0x3F (0x03 direct convention))
//to handle inverse convention we have to:
//1)invert chars before transmitting and after receiving
//2)swap bits in each chars - b1=b8, b2=b7, b3=b6, .., b8=b1
//3)use opposite parity

extern u8 inverseConvention;

b8 handleAtr (struct ATR *a)
{
  u8 nx_atr = 0;
  u8 tk = 0, ta = 0, tb = 0, tc = 0, td = 0, t_proto = 0;
  u8 TK[16];
  u8 nr_tk = 0;
  u8 i_round = 1;
  u8 i_si = 0;
  u16 i = 0;
  b8 res = 0;
  u8 looping;
  u8 wasFirstAtrChar = 0;
  u8 expectTCK;
  u8 otherProtocolThanT0;
u16 cnt_a=0;
u16 cnt_b=0;
u16 cnt_c=0;

  a->card_TA1 = 0;
  a->card_TC1 = 0;
  a->card_TC2 = 0;
  a->pts = 0;
  a->buf[0] = 0;

  nx_atr = NX_ATR_TS;
  expectTCK = 0;
  otherProtocolThanT0 = 0;
  looping = 1;

  inverseConvention = 0;

  while (looping == 1)
  {
    if ((uart_chars ()) != 0)
    {
cnt_a=0;
cnt_b=0;
cnt_c=0;

      i_si = uart_get_char ();
      //check for inverse convention
      if ((i_si == 0x03) && (inverseConvention == 0)
	  && (wasFirstAtrChar == 0))
      {
	i_si = 0x3F;
	inverseConvention = 1;
	uart_change_parity ();
      }
      wasFirstAtrChar = 1;

      if (a->buf[0] < ATR_MAX)
      {
	a->buf[i + 1] = i_si;
	a->buf[0]++;
      }
      i++;
      switch (nx_atr)
      {
	case NX_ATR_TS:
	  nx_atr = NX_ATR_T0;
	  break;
	case NX_ATR_T0:
	  ta = (i_si & 16) ? 1 : 0;
	  tb = (i_si & 32) ? 1 : 0;
	  tc = (i_si & 64) ? 1 : 0;
	  td = (i_si & 128) ? 1 : 0;
	  tk = (i_si & 15);
	  nx_atr = 0;
	  i_round = 1;
	  if (ta && !nx_atr)
	    nx_atr = NX_ATR_TA;
	  if (tb && !nx_atr)
	    nx_atr = NX_ATR_TB;
	  if (tc && !nx_atr)
	    nx_atr = NX_ATR_TC;
	  if (td && !nx_atr)
	    nx_atr = NX_ATR_TD;
	  if (tk && !nx_atr)
	    nx_atr = NX_ATR_TK;
	  if (!nx_atr)
	    nx_atr = NX_ATR_TCK;
	  break;
	case NX_ATR_TA:
	  if (i_round == 1)
	  {
	    a->card_TA1 = i_si;
	    a->pts = i_si;
	  }
	  nx_atr = 0;
	  if (tb && !nx_atr)
	    nx_atr = NX_ATR_TB;
	  if (tc && !nx_atr)
	    nx_atr = NX_ATR_TC;
	  if (td && !nx_atr)
	    nx_atr = NX_ATR_TD;
	  if (tk && !nx_atr)
	    nx_atr = NX_ATR_TK;
	  if (!nx_atr)
	    nx_atr = NX_ATR_TCK;
	  break;
	case NX_ATR_TB:
	  nx_atr = 0;
	  if (tc && !nx_atr)
	    nx_atr = NX_ATR_TC;
	  if (td && !nx_atr)
	    nx_atr = NX_ATR_TD;
	  if (tk && !nx_atr)
	    nx_atr = NX_ATR_TK;
	  if (!nx_atr)
	    nx_atr = NX_ATR_TCK;
	  break;
	case NX_ATR_TC:
	  if (i_round == 1)
	    a->card_TC1 = i_si;
	  if (i_round == 2)
	    a->card_TC2 = i_si;
	  nx_atr = 0;
	  if (td && !nx_atr)
	    nx_atr = NX_ATR_TD;
	  if (tk && !nx_atr)
	    nx_atr = NX_ATR_TK;
	  if (!nx_atr)
	    nx_atr = NX_ATR_TCK;
	  break;
	case NX_ATR_TD:
	  ta = (i_si & 16) ? 1 : 0;
	  tb = (i_si & 32) ? 1 : 0;
	  tc = (i_si & 64) ? 1 : 0;
	  td = (i_si & 128) ? 1 : 0;
	  t_proto = (i_si & 15);
	  if ((t_proto != 0) && (otherProtocolThanT0 == 0))
	  {
	    otherProtocolThanT0 = 1;
	    expectTCK = 1;
	  }
	  i_round++;
	  nx_atr = 0;
	  if (ta && !nx_atr)
	    nx_atr = NX_ATR_TA;
	  if (tb && !nx_atr)
	    nx_atr = NX_ATR_TB;
	  if (tc && !nx_atr)
	    nx_atr = NX_ATR_TC;
	  if (td && !nx_atr)
	    nx_atr = NX_ATR_TD;
	  if (tk && !nx_atr)
	    nx_atr = NX_ATR_TK;
	  if (!nx_atr)
	    nx_atr = NX_ATR_TCK;
	  break;
	case NX_ATR_TK:
	  TK[nr_tk] = i_si;
	  nr_tk++;
	  if (tk != nr_tk)
	  {
	    nx_atr = NX_ATR_TK;
	  }
	  else
	  {
	    nx_atr = NX_ATR_TCK;
	    if (expectTCK == 0)
	      looping = 0;
	  }
	  break;
	case NX_ATR_TCK:
	  nx_atr = 0;
	  looping = 0;
	  break;
	default:
	  res = -1;
      }
    }
else
{
#define CNT_MAX_A 64000
#define CNT_MAX_B 10
#define CNT_MAX_C 0
if(cnt_a>=CNT_MAX_A)
{
	cnt_a=0;
	if(cnt_b>=CNT_MAX_B)
	{
		cnt_b=0;
		if(cnt_c>=CNT_MAX_C)
		{
			cnt_c=0;
			return -1;
		}
		else
			cnt_c++;
	}
	else
		cnt_b++;
}
else cnt_a++;
}
  }

  return res;
}

#define TO_SIM  0
#define FROM_SIM        1

u8 uart_get_char_blocking ()
{
  while (uart_chars () == 0);
  return uart_get_char ();
}

u16 sim_txrx (u16 tx_len, u16 rx_len, u8 * buf)
{
  u16 i;
  u8 x;
u8 y;

for(i=0;i<tx_len;i++)
{
  uart_send_char (buf[i]);
//if(i==4)y=uart_get_char_blocking();
}

//buf[0]=y;

for(i=0;i<rx_len;i++)
{
x=uart_get_char_blocking();
//buf[i]=0x30+i;
buf[i]=x;
}

  return i;
}


u16 simx (u8 cla, u8 ins, u8 p1, u8 p2, u8 p3, u8 * buf)
{
  u16 len = p3;
  u16 i;
  u8 dir = TO_SIM;
  u8 x;
  u8 invins = ~ins;

  uart_send_char (cla);
  uart_send_char (ins);
  uart_send_char (p1);
  uart_send_char (p2);
  uart_send_char (p3);

  switch (ins)
  {
/*
//TO_SIM
        case ME_CMD_SELECT:
        case ME_CMD_UPDATE_BINARY:
        case ME_CMD_UPDATE_RECORD:
        case ME_CMD_SEEK:
        case ME_CMD_INCREASE:
        case ME_CMD_VERIFY_CHV:
        case ME_CMD_CHANGE_CHV:
        case ME_CMD_DISABLE_CHV:
        case ME_CMD_ENABLE_CHV:
        case ME_CMD_UNBLOCK_CHV:
        case ME_CMD_RUN_GSM_ALGORITHM:
        case ME_CMD_TERMINAL_PROFILE:
        case ME_CMD_ENVELOPE:
        case ME_CMD_TERMINAL_RESPONSE:
                break;
*/

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

  i = 0;
X1:
  x = uart_get_char_blocking ();
  if (x == 0x60)
    goto X1;
  if (x == invins)
  {
    if (dir == TO_SIM)
      uart_send_char (buf[i]);
    else
      buf[i] = uart_get_char_blocking ();
    i++;
    if (i < len)
      goto X1;
    else
      goto X2;
  }

  if (x == ins)
    goto X4;
  if ((x & 0xF0) == 0x90 || (x & 0xF0) == 0x60)
    goto X3;			//6x xx or 9x xx
X4:
  for (; i < len; i++)
    if (dir == TO_SIM)
      uart_send_char (buf[i]);
    else
      buf[i] = uart_get_char_blocking ();
X2:
  if (dir == TO_SIM)
    i = 0;
X5:
  x = uart_get_char_blocking ();	//SW1
  if (x == 0x60)
    goto X5;
X3:
  buf[i] = x;
  i++;
  buf[i] = uart_get_char_blocking ();	//SW2
  i++;
  return i;
}

void atr ()
{
  struct ATR atrStruct;
b8 ret;

  //set RST=0
  cbi (SIM_RST_PORT, SIM_RST_PORT_PIN);
  sbi (SIM_RST_DDR, SIM_RST_DDR_DD);

  //delay 10ms
  delayMs (10);

  //SIM_IO_ENABLE
  //set pin to log.0
  //(SIM_IO is disconnected)
  ext_cbi (EXT_SIM_IO_ENABLE_PORT, EXT_SIM_IO_ENABLE_PORT_PIN);

  //delay 10ms
  delayMs (10);

  //SIM_CLK_ENABLE /set pin to log.0 (active high)
  //(in  is disconnected at first)
  cbi (SIM_CLK_ENABLE_PORT, SIM_CLK_ENABLE_PORT_PIN);
  //SIM_CLKA_ENABLE /set pin to log.0 (active high)
  //(in  is disconnected at first)
  cbi (SIM_CLKA_ENABLE_PORT, SIM_CLKA_ENABLE_PORT_PIN);

  //delay 20ms
  delayMs (20);

  //SIM_VCC_ENABLE set pin to log.1 (active low)
  //SIM is disconnected at first
  sbi (SIM_VCC_ENABLE_PORT, SIM_VCC_ENABLE_PORT_PIN);

  //delay 50ms
  delayMs (50);

  //flush rx fifo
  while (uart_chars () != 0)
    uart_get_char ();

  //SIM_VCC_ENABLE set pin to log.0 (active low)
  cbi (SIM_VCC_ENABLE_PORT, SIM_VCC_ENABLE_PORT_PIN);

  //SIM_IO_ENABLE
  //set pin to log.1
  //(SIM_IO is connected)
  ext_sbi (EXT_SIM_IO_ENABLE_PORT, EXT_SIM_IO_ENABLE_PORT_PIN);
  //set SIM_TX to input
  sbi (SIM_TXD_PORT, SIM_TXD_PORT_PIN);
  cbi (SIM_TXD_DDR, SIM_TXD_DDR_DD);

  //delay 1ms
  delayMs (1);

  //SIM_CLKA_ENABLE /set pin to log.1 (active high)
  //(connect clk to SIM now)
  sbi (SIM_CLKA_ENABLE_PORT, SIM_CLKA_ENABLE_PORT_PIN);
  //delay 1ms
  delayMs (1);

  //delay 30ms
  delayMs (30);

  //set RST=1
  sbi (SIM_RST_PORT, SIM_RST_PORT_PIN);

  ret=handleAtr (&atrStruct);
//ret=-1;
if(ret==0)
  usb_send (NO_ERROR, atrStruct.buf[0], &atrStruct.buf[1]);
else
{
atrStruct.buf[0]=0x01;
atrStruct.buf[1]=0xFF;
  usb_send (NO_ERROR, atrStruct.buf[0], &atrStruct.buf[1]);
}
}

void turbo_handler (u8 action, USB_Data * data)
{
  u8 *buf = buf_A ();

  switch (action)
  {
    case ACTION_APP_VERSION:
      data->buf[0] = VERSION_A;
      data->buf[1] = VERSION_B;
      data->buf[2] = VERSION_C;

      break;
    case ACTION_SIM_INIT:
      reg_action (ACTION_SIG_UART1_RECV);
      init_vars ();
      init_sim_port ();
      init_uart ();
      init_clka_timer ();
      usb_send (NO_ERROR, 0, NULL);
      break;
    case ACTION_SIM_CMD:
      {
	u8 ins, p1, p2, p3;
	u16 ret;
	u16 len;

	ins = data->buf[0];
	p1 = data->buf[1];
	p2 = data->buf[2];
	p3 = data->buf[3];

	ret = simx (0xa0, ins, p1, p2, p3, &data->buf[4]);
	usb_send (NO_ERROR, ret, &data->buf[4]);
      }
      break;

    case ACTION_SIM_CMDX:
      {
	u8 cla, ins, p1, p2, p3;
	u16 ret;
	u16 len;

	cla = data->buf[0];
	ins = data->buf[1];
	p1 = data->buf[2];
	p2 = data->buf[3];
	p3 = data->buf[4];

	ret = simx (cla, ins, p1, p2, p3, &data->buf[5]);
	usb_send (NO_ERROR, ret, &data->buf[5]);
      }
      break;

    case ACTION_SIM_TXRX:
      {
	u16 ret;
	u16 len_tx;
	u16 len_rx;

	len_tx = data->buf[0];
	len_tx=len_tx<<8;
	len_tx |= data->buf[1];

	len_rx = data->buf[2];
	len_rx=len_rx<<8;
	len_rx |= data->buf[3];

	ret = sim_txrx (len_tx, len_rx, &data->buf[4]);
	usb_send (NO_ERROR, ret, &data->buf[4]);
      }
      break;

    case ACTION_SIM_ATR:
      atr ();
      break;
    case ACTION_SIG_UART1_RECV:
      uart_irq ();
      break;
    default:
      break;
  }
}
