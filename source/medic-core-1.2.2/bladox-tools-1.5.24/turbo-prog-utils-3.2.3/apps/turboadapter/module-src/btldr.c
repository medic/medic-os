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

#define CLK_ALTERNATIVE

#ifdef CLK_ALTERNATIVE
#define DEFAULT_BTLDR_CLK_DIV (2)
#else
#define DEFAULT_BTLDR_CLK_DIV (1)
#endif

#define DEFAULT_F (372)
#define DEFAULT_D (1)
#define DEFAULT_DIVIDER (DEFAULT_F/DEFAULT_D)
u16 actualUbrr;

u8 speed_h;
u8 speed_l;

#define BTLDR_TX_SM_STATE1 (1)
#define BTLDR_TX_SM_STATE2 (2)
u8 btldrUartTxStateMachine;

#define BTLDR_UART_RX_FIFO_SIZE 8
u8 btldrUartRxFifo[BTLDR_UART_RX_FIFO_SIZE];
u8 btldrUartRxFifoNumOfChars;
u8 btldrUartRxFifoTail;
u8 btldrUartRxFifoHead;

void set_divider_default()
{
  actualUbrr = ((DEFAULT_BTLDR_CLK_DIV * DEFAULT_DIVIDER) / 8) - 1;
  outb (BTLDR_UART_UBRRH, (actualUbrr >> 8));
  outb (BTLDR_UART_UBRRL, (actualUbrr & 0xFF));
}

void set_divider(u8 dh, u8 dl)
{
u16 d=dh;
d<<=8;
d|=dl;

  actualUbrr = ((DEFAULT_BTLDR_CLK_DIV * d) / 8) - 1;
  outb (BTLDR_UART_UBRRH, (actualUbrr >> 8));
  outb (BTLDR_UART_UBRRL, (actualUbrr & 0xFF));
}

void init_vars ()
{
  btldrUartRxFifoHead = 0;
  btldrUartRxFifoTail = 0;
  btldrUartRxFifoNumOfChars = 0;

speed_h=speed_l=0;

  actualUbrr = ((DEFAULT_BTLDR_CLK_DIV * DEFAULT_DIVIDER) / 8) - 1;

  btldrUartTxStateMachine = BTLDR_TX_SM_STATE1;
}

void init_btldrUart (void)
{
  //set mode to double speed
  outb (BTLDR_UART_UCSRA, 0x02);

  // enable receive complete interrupt
  // disable transmit complete interrupt
  // enable receiver
  // disable transmitter because of RXD and TXD connected together
  // to not output log.1 to TXD and RXD
  // we enable transmitter before each char will be send
  outb (BTLDR_UART_UCSRB, 0x90);

  // asynchronous mode
  // 8 bit data, even parity, 1 stop bit
  outb (BTLDR_UART_UCSRC, 0x26);

  // set baud rate register to DEFAULT_DIVIDER
  outb (BTLDR_UART_UBRRH, (actualUbrr >> 8));
  outb (BTLDR_UART_UBRRL, (actualUbrr & 0xFF));
}

u8 btldrRxChars (void)
{
  u8 tmpRxChars;

  cli ();
  tmpRxChars = btldrUartRxFifoNumOfChars;
  sei ();
  return tmpRxChars;
}

u8 btldrRxGetChar (void)
{
  u8 tmpTail;

  cli ();
  if (btldrUartRxFifoNumOfChars > 0)
  {
    btldrUartRxFifoNumOfChars--;
    sei ();
    tmpTail = btldrUartRxFifoTail;
    if (btldrUartRxFifoTail < (BTLDR_UART_RX_FIFO_SIZE - 1))
      btldrUartRxFifoTail++;
    else
      btldrUartRxFifoTail = 0;

    return btldrUartRxFifo[tmpTail];
  }
  else
  {
    sei ();
    return 0;
  }
}

void btldrTxSendChar (u8 txValue)
{
  u8 txValueTmp;
  u8 iTmp;
  u8 interrupts;

  //store global interrupt enable bit
  interrupts = (inb (SREG) & (1 << SREG_I));

  //cli();
  txValueTmp = txValue;

  if (btldrUartTxStateMachine == BTLDR_TX_SM_STATE1)
  {
    // execute delay before transmitting char
    //do delay interval
    delayMs (3);
    btldrUartTxStateMachine = BTLDR_TX_SM_STATE2;
  }

  // disable receive complete interrupt
  // disable receiver
  outb (BTLDR_UART_UCSRB, (inb (BTLDR_UART_UCSRB) & 0x6F));

  //TXD1 pin to log.1
  sbi (BTLDR_TXD_PORT, BTLDR_TXD_PORT_PIN);

  //TXD1 pin change direction to output only when transmitting char
  sbi (BTLDR_TXD_DDR, BTLDR_TXD_DDR_DD);

  // enable transmitter
  outb (BTLDR_UART_UCSRB, (inb (BTLDR_UART_UCSRB) | 0x08));

  // send txValueTmp to UDR register
  sbi (BTLDR_UART_UCSRA, TXC);
  outb (BTLDR_UDR, txValueTmp);
  loop_until_bit_is_set (BTLDR_UART_UCSRA, TXC);
  sbi (BTLDR_UART_UCSRA, TXC);

  // disable transmitter
  outb (BTLDR_UART_UCSRB, (inb (BTLDR_UART_UCSRB) & 0xF7));

  // set TXD1 pin to log.1
  sbi (BTLDR_TXD_PORT, BTLDR_TXD_PORT_PIN);

  //TXD1 pin change direction to input to allow receiving
  cbi (BTLDR_TXD_DDR, BTLDR_TXD_DDR_DD);

  // clear TXC flag (write log.1 to it)
  sbi (BTLDR_UART_UCSRA, TXC);

  //restore global interrupt enable bit
  if (interrupts != 0)
    sei ();

  // enable receive complete interrupt
  // enable receiver
  outb (BTLDR_UART_UCSRB, (inb (BTLDR_UART_UCSRB) | 0x90));

  delayUs (30);
}

void init_turboadapter_btldr_sim_port ()
{
  led_on (LED_SIM);

  // clear RXC flag (write log.1 to it)
  sbi (BTLDR_UART_UCSRA, RXC);

  // clear TXC flag (write log.1 to it)
  sbi (BTLDR_UART_UCSRA, TXC);

  // set baud rate to enhanced value from PTS
  outb (BTLDR_UART_UBRRH, (actualUbrr >> 8));
  outb (BTLDR_UART_UBRRL, (actualUbrr & 0xFF));

#ifdef CLK_ALTERNATIVE
  //use alternative clk for sim instead  of 8mhz sysctem clk
  //4Mhz for now generated on OCR1A pin
  //SIM_CLKA_ENABLE /set pin to log.1 (active high)
  //(in BTLDR_MODE CLKA is connected)
  sbi (SIM_CLKA_ENABLE_PORT, SIM_CLKA_ENABLE_PORT_PIN);
#else
  //SIM_CLK_ENABLE /set pin to log.1 (active high)
  //(in BTLDR_MODE CLK is connected)
  sbi (SIM_CLK_ENABLE_PORT, SIM_CLK_ENABLE_PORT_PIN);
#endif

  //BTLDR_RX
  //let it set to input
  //set pin to log.1
  sbi (BTLDR_RXD_PORT, BTLDR_RXD_PORT_PIN);
  cbi (BTLDR_RXD_DDR, BTLDR_RXD_DDR_DD);

  //BTLDR_TX
  //let it set to output
  //set pin to log.0
  cbi (BTLDR_TXD_PORT, BTLDR_TXD_PORT_PIN);
  sbi (BTLDR_TXD_DDR, BTLDR_TXD_DDR_DD);

  //SIM_RST
  //change direction to output
  //set pin to log.0
  cbi (SIM_RST_PORT, SIM_RST_PORT_PIN);
  sbi (SIM_RST_DDR, SIM_RST_DDR_DD);

  //SIM_RST_ENABLE
  //set pin to log.1
  //(in SIM_MODE BTLDR_RST is connected)
  ext_sbi (EXT_SIM_RST_ENABLE_PORT, EXT_SIM_RST_ENABLE_PORT_PIN);

  //SIM_IO_ENABLE
  //set pin to log.1
  //(in SIM_MODE BTLDR_IO is connected)
  ext_sbi (EXT_SIM_IO_ENABLE_PORT, EXT_SIM_IO_ENABLE_PORT_PIN);
}

void reset_turbo_for_bootloader_sim ()
{
  //delay 10ms
  delayMs (10);

//-------------------------------------------------------------b
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

#ifdef CLK_ALTERNATIVE
  //SIM_CLKA_ENABLE /set pin to log.0 (active high)
  //(in  is disconnected at first)
  cbi (SIM_CLKA_ENABLE_PORT, SIM_CLKA_ENABLE_PORT_PIN);
#else
  //SIM_VCC_ENABLE set pin to log.0 (active low)
  cbi (SIM_VCC_ENABLE_PORT, SIM_VCC_ENABLE_PORT_PIN);
  //SIM_CLK_ENABLE /set pin to log.0 (active high)
  //(in  is disconnected at first)
  cbi (SIM_CLK_ENABLE_PORT, SIM_CLK_ENABLE_PORT_PIN);
#endif

  //delay 20ms
  delayMs (20);

  //SIM_VCC_ENABLE set pin to log.1 (active low)
  //SIM is disconnected at first
  sbi (SIM_VCC_ENABLE_PORT, SIM_VCC_ENABLE_PORT_PIN);

  //delay 50ms
  delayMs (50);

//-------------------------------------------------------------e

  //SIM_VCC_ENABLE set pin to log.0 (active low)
  cbi (SIM_VCC_ENABLE_PORT, SIM_VCC_ENABLE_PORT_PIN);

//  delayMs (500);

//-------------------------------------------------------------b
  //SIM_IO_ENABLE
  //set pin to log.1
  //(SIM_IO is connected)
  ext_sbi (EXT_SIM_IO_ENABLE_PORT, EXT_SIM_IO_ENABLE_PORT_PIN);

  //delay 1ms
  delayMs (1);

#ifdef CLK_ALTERNATIVE
  //SIM_CLKA_ENABLE /set pin to log.1 (active high)
  //(connect clk to SIM now)
  sbi (SIM_CLKA_ENABLE_PORT, SIM_CLKA_ENABLE_PORT_PIN);
#else
  //SIM_CLK_ENABLE /set pin to log.1 (active high)
  //(in BTLDR_MODE CLK is connected)
  sbi (SIM_CLK_ENABLE_PORT, SIM_CLK_ENABLE_PORT_PIN);
#endif
  //delay 1ms
  delayMs (1);

  delayMs (500);
//-------------------------------------------------------------e

  //remove any char pending in btldf rx fifo
  while (btldrRxChars () != 0)
    btldrRxGetChar ();
  delayMs (10);

  //BTLDR_TX
  //let it set to input
  //set pin to log.1
  sbi (BTLDR_TXD_PORT, BTLDR_TXD_PORT_PIN);
  cbi (BTLDR_TXD_DDR, BTLDR_TXD_DDR_DD);
}

void init_turboadapter_btldr_port (u8 what)
{
  if (what > BTLDR_PORT_BRUM_2)
    what = BTLDR_PORT_TURBO_ADAPTER;

  switch (what)
  {
    case BTLDR_PORT_TURBO_ADAPTER:	//for TurboAdapter
      break;
    case BTLDR_PORT_BRUM_1:	//for BRUM1
      sbi (BRUM1_SEL_PORT, BRUM1_SEL_PORT_PIN);
      break;
    case BTLDR_PORT_BRUM_2:	//for BRUM2
      sbi (BRUM2_SEL_PORT, BRUM2_SEL_PORT_PIN);
      break;
  }

  led_on (LED_TFM);

  //DEBUG_SS_MCU change direction to output 
  sbi (DEBUG_SS_MCU_DDR, DEBUG_SS_MCU_DDR_DD);

  // RESETn pin stays output and set it to log.1
  sbi (PROG_RESETn_PORT, PROG_RESETn_PORT_PIN);
  sbi (PROG_RESETn_DDR, PROG_RESETn_DDR_DD);

  //PROG_DEBUG_RESETn_ENABLE pin set it to log.0
  //(in BTLDR mode RESETn is connected)
  ext_cbi (EXT_PROG_DEBUG_RESETn_ENABLE_PORT,
	   EXT_PROG_DEBUG_RESETn_ENABLE_PORT_PIN);

  delayMs (200);

  // clear RXC flag (write log.1 to it)
  sbi (BTLDR_UART_UCSRA, RXC);

  // clear TXC flag (write log.1 to it)
  sbi (BTLDR_UART_UCSRA, TXC);

  // set baud rate to enhanced value from PTS
  outb (BTLDR_UART_UBRRH, (actualUbrr >> 8));
  outb (BTLDR_UART_UBRRL, (actualUbrr & 0xFF));

#ifdef CLK_ALTERNATIVE
  //PROG_DEBUG_CLKA_ENABLE set it to log.1
  //(in BTLDR_MODE CLKA is connected)
  ext_sbi (EXT_PROG_DEBUG_CLKA_ENABLE_PORT,
	   EXT_PROG_DEBUG_CLKA_ENABLE_PORT_PIN);
#else
  //PROG_DEBUG_CLK_ENABLE set it to log.1
  //(in BTLDR_MODE CLK is connected)
  ext_sbi (EXT_PROG_DEBUG_CLK_ENABLE_PORT,
	   EXT_PROG_DEBUG_CLK_ENABLE_PORT_PIN);
#endif

  delayMs (10);

  //BTLDR_RX
  //let it set to input
  //set pin to log.1
  sbi (BTLDR_RXD_PORT, BTLDR_RXD_PORT_PIN);
  cbi (BTLDR_RXD_DDR, BTLDR_RXD_DDR_DD);

  //BTLDR_TX
  //let it set to output
  //set pin to log.0
  cbi (BTLDR_TXD_PORT, BTLDR_TXD_PORT_PIN);
  sbi (BTLDR_TXD_DDR, BTLDR_TXD_DDR_DD);

  //BTLDR_ENABLE
  //set pin to log.1
  //(in BTLDR_MODE BTLDR_RX and BTLDR_TX are connected)
  ext_sbi (EXT_BTLDR_ENABLE_PORT, EXT_BTLDR_ENABLE_PORT_PIN);
}

void reset_turbo_for_bootloader ()
{
  delayMs (10);

  //PROG_DEBUG_VCC_ENABLE set it to log.0
  //(in BTLDR_MODE we must apply power after all unecessary signals were disconnected)
  ext_cbi (EXT_PROG_DEBUG_VCC_ENABLE_PORT,
	   EXT_PROG_DEBUG_VCC_ENABLE_PORT_PIN);

//EXTRA RESET-------------------------------
  delayMs (200);

  // RESETn pin stays output and set it to log.0
  cbi (PROG_RESETn_PORT, PROG_RESETn_PORT_PIN);
  sbi (PROG_RESETn_DDR, PROG_RESETn_DDR_DD);

  //PROG_DEBUG_RESETn_ENABLE pin set it to log.1
  //(in BTLDR mode RESETn is connected)
  ext_sbi (EXT_PROG_DEBUG_RESETn_ENABLE_PORT,
	   EXT_PROG_DEBUG_RESETn_ENABLE_PORT_PIN);

  delayMs (100);

  // RESETn pin stays output and set it to log.1
  sbi (PROG_RESETn_PORT, PROG_RESETn_PORT_PIN);
  sbi (PROG_RESETn_DDR, PROG_RESETn_DDR_DD);

  //PROG_DEBUG_RESETn_ENABLE pin set it to log.0
  //(in BTLDR mode RESETn is disconnected)
  ext_cbi (EXT_PROG_DEBUG_RESETn_ENABLE_PORT,
	   EXT_PROG_DEBUG_RESETn_ENABLE_PORT_PIN);

//EXTRA RESET-------------------------------

  delayMs (500);

  //remove any char pending in btldf rx fifo
  while (btldrRxChars () != 0)
    btldrRxGetChar ();

  delayMs (10);

  //BTLDR_TX
  //let it set to input
  //set pin to log.1
  sbi (BTLDR_TXD_PORT, BTLDR_TXD_PORT_PIN);
  cbi (BTLDR_TXD_DDR, BTLDR_TXD_DDR_DD);

//  delayMs (10);
}

void btldr_uart_irq ()
{
  // next BTLDR char transmit will execute delay before transmitting char
  btldrUartTxStateMachine = BTLDR_TX_SM_STATE1;

  if (btldrUartRxFifoNumOfChars == BTLDR_UART_RX_FIFO_SIZE)
  {
  }
  else
  {
    btldrUartRxFifo[btldrUartRxFifoHead] = inb (BTLDR_UDR);

    btldrUartRxFifoNumOfChars++;
    if (btldrUartRxFifoHead < (BTLDR_UART_RX_FIFO_SIZE - 1))
      btldrUartRxFifoHead++;
    else
      btldrUartRxFifoHead = 0;
  }
}

void btldr_flash_page (USB_Data * data, u8 * buf)
{
  u16 i;

  led_on (LED_NO);

  btldrTxSendChar (UCMD_BTLDR_MODE_WRITE_PAGE_FLASH);
  btldrTxSendChar (data->buf[0]);
  btldrTxSendChar (data->buf[1]);

  for (i = 0; i < 256; i++)
    btldrTxSendChar (data->buf[2 + i]);

  while (btldrRxChars () == 0);
  buf[0] = btldrRxGetChar ();

  led_on (LED_TFM);

  usb_send (NO_ERROR, 1, buf);
}

void btldr_read_page (USB_Data * data, u8 * buf)
{
  u16 i;

  led_on (LED_NO);

  btldrTxSendChar (UCMD_BTLDR_MODE_READ_PAGE_FLASH);
  btldrTxSendChar (data->buf[0]);
  btldrTxSendChar (data->buf[1]);

  for (i = 0; i < 256; i++)
  {
    while (btldrRxChars () == 0);
    buf[i] = btldrRxGetChar ();
  }
  while (btldrRxChars () == 0);
  btldrRxGetChar ();

  led_on (LED_TFM);

  usb_send (NO_ERROR, 256, buf);
}

void btldr_flash_page_eeprom (USB_Data * data, u8 * buf)
{
  u16 i;

  led_on (LED_NO);

  btldrTxSendChar (UCMD_BTLDR_MODE_WRITE_PAGE_EEPROM);
  btldrTxSendChar (data->buf[0]);
  btldrTxSendChar (data->buf[1]);

  for (i = 0; i < 8; i++)
    btldrTxSendChar (data->buf[2 + i]);

  while (btldrRxChars () == 0);
  buf[0] = btldrRxGetChar ();

  led_on (LED_TFM);

  usb_send (NO_ERROR, 1, buf);
}

void btldr_read_page_eeprom (USB_Data * data, u8 * buf)
{
  u16 i;

  led_on (LED_NO);

  btldrTxSendChar (UCMD_BTLDR_MODE_READ_PAGE_EEPROM);
  btldrTxSendChar (data->buf[0]);
  btldrTxSendChar (data->buf[1]);

  for (i = 0; i < 8; i++)
  {
    while (btldrRxChars () == 0);
    buf[i] = btldrRxGetChar ();
  }

  while (btldrRxChars () == 0);
  btldrRxGetChar ();

  led_on (LED_TFM);

  usb_send (NO_ERROR, 8, buf);
}

void btldr_sram_flash (USB_Data * data, u8 * buf)
{
  u16 page;
  u8 crc;
  u8 c;
  u32 i;
  u32 len;
  u8 tverify_error = 0;
  u8 tval;
  u8 tverify_val;

/*
->header
<-ans usb ok

->data
<-ans char data ok

<-ans char prog ok
*/

  crc = data->buf[0];

  usb_send (NO_ERROR, 0, NULL);

  len = 256;
  len *= 480;

  for (i = 0; i < len; i++)
  {
    tval = usb_get_char ();
    ext_sram_wb (i, tval);
    tverify_val = ext_sram_rb (i);
    if (tverify_val != tval)
      tverify_error = 1;
    if (i == 0)
      c = tval;
    else
      c ^= tval;
  }

  if (tverify_error)
  {
    usb_put_char_nb (BTLDR_VERIFY_ERROR);
    return;
  }
  else
  {
    if (c != crc)
    {
      usb_put_char_nb (BTLDR_CRC_ERROR);
      return;
    }
    else
      usb_put_char_nb (NO_ERROR);
  }

// prog

  for (page = 0; page < 480; page++)
  {
    led_on (LED_TFM);

    btldrTxSendChar (UCMD_BTLDR_MODE_WRITE_PAGE_FLASH);
    btldrTxSendChar (page);
    btldrTxSendChar (page >> 8);

    for (i = 0; i < 256; i++)
      btldrTxSendChar (ext_sram_rb ((u32) page * 256 + i));

    while (btldrRxChars () == 0);
    tval = btldrRxGetChar ();

    led_on (LED_NO);

    btldrTxSendChar (UCMD_BTLDR_MODE_READ_PAGE_FLASH);
    btldrTxSendChar (page);
    btldrTxSendChar (page >> 8);

    for (i = 0; i < 256; i++)
    {
      while (btldrRxChars () == 0);
      tval = btldrRxGetChar ();
      if (tval != ext_sram_rb ((u32) page * 256 + i))
	tverify_error = 1;
      if (i == 0 && page == 0)
	c = tval;
      else
	c ^= tval;
    }

    while (btldrRxChars () == 0);
    btldrRxGetChar ();

    led_on (LED_TFM);
  }

  if (tverify_error)
    usb_put_char_nb (BTLDR_VERIFY_ERROR);
  else
  {
    if (c != crc)
      usb_put_char_nb (BTLDR_CRC_ERROR);
    else
      usb_put_char_nb (NO_ERROR);
  }
}

void btldr_sram_eeprom (USB_Data * data, u8 * buf)
{
  u16 page;
  u8 crc;
  u8 c;
  u32 i;
  u32 len;
  u8 tverify_error = 0;
  u8 tval;
  u8 tverify_val;

/*
->header
<-ans usb ok

->data
<-ans char data ok

<-ans char prog ok
*/

  crc = data->buf[0];

  usb_send (NO_ERROR, 0, NULL);

  len = 0x1000;

  for (i = 0; i < len; i++)
  {
    tval = usb_get_char ();
    ext_sram_wb (i, tval);
    tverify_val = ext_sram_rb (i);
    if (tverify_val != tval)
      tverify_error = 1;
    if (i == 0)
      c = tval;
    else
      c ^= tval;
  }

  if (tverify_error)
    usb_put_char_nb (BTLDR_VERIFY_ERROR);
  else
  {
    if (c != crc)
      usb_put_char_nb (BTLDR_CRC_ERROR);
    else
      usb_put_char_nb (NO_ERROR);
  }

// prog

  for (page = 0; page < 512; page++)
  {
    led_on (LED_TFM);

    btldrTxSendChar (UCMD_BTLDR_MODE_WRITE_PAGE_EEPROM);
    btldrTxSendChar (page);
    btldrTxSendChar (page >> 8);

    for (i = 0; i < 8; i++)
      btldrTxSendChar (ext_sram_rb ((u32) page * 8 + i));

    while (btldrRxChars () == 0);
    tval = btldrRxGetChar ();

    led_on (LED_NO);

    btldrTxSendChar (UCMD_BTLDR_MODE_READ_PAGE_EEPROM);
    btldrTxSendChar (page);
    btldrTxSendChar (page >> 8);

    for (i = 0; i < 8; i++)
    {
      while (btldrRxChars () == 0);
      tval = btldrRxGetChar ();
      if (tval != ext_sram_rb ((u32) page * 8 + i))
	tverify_error = 1;
      if (i == 0 && page == 0)
	c = tval;
      else
	c ^= tval;
    }

    while (btldrRxChars () == 0);
    btldrRxGetChar ();

    led_on (LED_TFM);
  }

  if (tverify_error)
    usb_put_char_nb (BTLDR_VERIFY_ERROR);
  else
  {
    if (c != crc)
      usb_put_char_nb (BTLDR_CRC_ERROR);
    else
      usb_put_char_nb (NO_ERROR);
  }
}
