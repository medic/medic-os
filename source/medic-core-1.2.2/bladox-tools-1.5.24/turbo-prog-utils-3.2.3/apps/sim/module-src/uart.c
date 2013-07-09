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

#define DEFAULT_SIM_CLK_DIV 2
#define DEFAULT_F 372
#define DEFAULT_D 1
#define DEFAULT_DIVIDER (DEFAULT_F/DEFAULT_D)
u16 simUbrr;

#define SIM_TX_SM_STATE1 (1)
#define SIM_TX_SM_STATE2 (2)
u8 uartTxStateMachine;

#define SIM_UART_RX_FIFO_SIZE 8
u8 uartRxFifo[SIM_UART_RX_FIFO_SIZE];
u8 uartRxFifoNumOfChars;
u8 uartRxFifoTail;
u8 uartRxFifoHead;

u8 inverseConvention;

void init_vars ()
{
  uartRxFifoHead = 0;
  uartRxFifoTail = 0;
  uartRxFifoNumOfChars = 0;

  simUbrr = ((DEFAULT_SIM_CLK_DIV * DEFAULT_DIVIDER) / 8) - 1;

  uartTxStateMachine = SIM_TX_SM_STATE1;
  inverseConvention = 0;
}

void init_uart (void)
{
  //set mode to double speed
  outb (SIM_UART_UCSRA, 0x02);

  // enable receive complete interrupt
  // disable transmit complete interrupt
  // enable receiver
  // disable transmitter because of RXD and TXD connected together
  // to not output log.1 to TXD and RXD
  // we enable transmitter before each char will be send
  outb (SIM_UART_UCSRB, 0x90);

  // asynchronous mode
  // 8 bit data, even parity, 1 stop bit
  outb (SIM_UART_UCSRC, 0x26);

  // set baud rate register to DEFAULT_DIVIDER
  outb (SIM_UART_UBRRH, (simUbrr >> 8));
  outb (SIM_UART_UBRRL, (simUbrr & 0xFF));

  while (uart_chars () != 0)
    uart_get_char ();
}

u8 getInverseConventionChar (u8 directConventionChar)
{
  u8 retVal;
  u8 i;
  u8 mask;
  u8 maskInv;

  retVal = 0;
  mask = 128;
  maskInv = 1;
  for (i = 0; i < 8; i++)
  {
    if ((directConventionChar & mask) != 0)
      retVal |= maskInv;
    mask = mask >> 1;
    maskInv = maskInv << 1;
  }
  retVal = ~retVal;
  return retVal;
}

u8 uart_chars (void)
{
  u8 tmpRxChars;

  cli ();
  tmpRxChars = uartRxFifoNumOfChars;
  sei ();
  return tmpRxChars;
}

u8 uart_get_char (void)
{
  u8 tmpTail;

  cli ();
  if (uartRxFifoNumOfChars > 0)
  {
    uartRxFifoNumOfChars--;
    sei ();
    tmpTail = uartRxFifoTail;
    if (uartRxFifoTail < (SIM_UART_RX_FIFO_SIZE - 1))
      uartRxFifoTail++;
    else
      uartRxFifoTail = 0;

    return uartRxFifo[tmpTail];
  }
  else
  {
    sei ();
    return 0;
  }
}

void uart_send_char (u8 txValue)
{
  u8 txValueTmp;
  u8 iTmp;
  u8 interrupts;

  //store global interrupt enable bit
  interrupts = (inb (SREG) & (1 << SREG_I));

  //cli();
  //check if inverse convention is active
  if (inverseConvention != 0)
    txValueTmp = getInverseConventionChar (txValue);
  else
    txValueTmp = txValue;

  if (uartTxStateMachine == SIM_TX_SM_STATE1)
  {
    // execute delay before transmitting char
    //do delay interval
    delayMs (3);
    uartTxStateMachine = SIM_TX_SM_STATE2;
  }

  // disable receive complete interrupt
  // disable receiver
  outb (SIM_UART_UCSRB, (inb (SIM_UART_UCSRB) & 0x6F));

  //TXD1 pin to log.1
  sbi (SIM_TXD_PORT, SIM_TXD_PORT_PIN);

  //TXD1 pin change direction to output only when transmitting char
  sbi (SIM_TXD_DDR, SIM_TXD_DDR_DD);

  // enable transmitter
  outb (SIM_UART_UCSRB, (inb (SIM_UART_UCSRB) | 0x08));

  // send txValueTmp to UDR register
  sbi (SIM_UART_UCSRA, TXC);
  outb (SIM_UDR, txValueTmp);
  loop_until_bit_is_set (SIM_UART_UCSRA, TXC);
  sbi (SIM_UART_UCSRA, TXC);

  // disable transmitter
  outb (SIM_UART_UCSRB, (inb (SIM_UART_UCSRB) & 0xF7));

  // set TXD1 pin to log.1
  sbi (SIM_TXD_PORT, SIM_TXD_PORT_PIN);

  //TXD1 pin change direction to input to allow receiving
  cbi (SIM_TXD_DDR, SIM_TXD_DDR_DD);

  // clear TXC flag (write log.1 to it)
  sbi (SIM_UART_UCSRA, TXC);

  //restore global interrupt enable bit
  if (interrupts != 0)
    sei ();

  // enable receive complete interrupt
  // enable receiver
  outb (SIM_UART_UCSRB, (inb (SIM_UART_UCSRB) | 0x90));
}

void init_sim_port ()
{
  led_on (LED_SIM);

  // clear RXC flag (write log.1 to it)
  sbi (SIM_UART_UCSRA, RXC);

  // clear TXC flag (write log.1 to it)
  sbi (SIM_UART_UCSRA, TXC);

  // set baud rate to enhanced value from PTS
  outb (SIM_UART_UBRRH, (simUbrr >> 8));
  outb (SIM_UART_UBRRL, (simUbrr & 0xFF));

  //use alternative clk for sim instead  of 8mhz sysctem clk
  //4Mhz for now generated on OCR1A pin
  //SIM_CLKA_ENABLE /set pin to log.1 (active high)
  //(in SIM_MODE CLKA is connected)
  sbi (SIM_CLKA_ENABLE_PORT, SIM_CLKA_ENABLE_PORT_PIN);

  //SIM_RX
  //let it set to input
  //set pin to log.1
  sbi (SIM_RXD_PORT, SIM_RXD_PORT_PIN);
  cbi (SIM_RXD_DDR, SIM_RXD_DDR_DD);

  //SIM_TX
  //let it set to output
  //set pin to log.0
  cbi (SIM_TXD_PORT, SIM_TXD_PORT_PIN);
  sbi (SIM_TXD_DDR, SIM_TXD_DDR_DD);

  //SIM_RST
  //change direction to output 
  //set pin to log.0
  cbi (SIM_RST_PORT, SIM_RST_PORT_PIN);
  sbi (SIM_RST_DDR, SIM_RST_DDR_DD);

  //SIM_RST_ENABLE
  //set pin to log.1
  //(in SIM_MODE SIM_RST is connected)
  ext_sbi (EXT_SIM_RST_ENABLE_PORT, EXT_SIM_RST_ENABLE_PORT_PIN);

  //SIM_IO_ENABLE
  //set pin to log.1
  //(in SIM_MODE SIM_IO is connected)
  ext_sbi (EXT_SIM_IO_ENABLE_PORT, EXT_SIM_IO_ENABLE_PORT_PIN);
}

void uart_irq ()
{
  // next SIM char transmit will execute delay before transmitting char
  uartTxStateMachine = SIM_TX_SM_STATE1;

  // store received character to SIM RX FIFO
  if (inverseConvention != 0)
    uartRxFifo[uartRxFifoHead] = getInverseConventionChar (inb (SIM_UDR));
  else
    uartRxFifo[uartRxFifoHead] = inb (SIM_UDR);

  uartRxFifoNumOfChars++;
  if (uartRxFifoHead < (SIM_UART_RX_FIFO_SIZE - 1))
    uartRxFifoHead++;
  else
    uartRxFifoHead = 0;
}

void uart_change_parity (void)
{
  // asynchronous mode
  // odd parity
  outb (SIM_UART_UCSRC, inb (SIM_UART_UCSRC) | 0x10);
}
