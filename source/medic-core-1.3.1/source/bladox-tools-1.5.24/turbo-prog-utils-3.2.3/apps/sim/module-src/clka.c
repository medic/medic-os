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

//-------------------------------------------------
// Timer/Counter1 initialization
// CTC(Clear on compare mode operation)
// clocked from system clocks (XTAL1)
// output compare units are enabled
//-------------------------------------------------
void init_clka_timer (void)
{
  //disable "Input capture" interrupt
  //disable "Output compare A" interrupt
  //disable "Output compare B" interrupt
  //disable "Timer3 overflow" interrupt
  //disable "Output compare C" interrupt
  outb (ETIMSK, inb (ETIMSK) & 0xFE);
  outb (TIMSK, inb (TIMSK) & 0xC3);

  //set OCR1A=0 to generate clk/2 frequency on ORC1A pin
  // as alternative clock for SIM
  outw (OCR1A, 0x0000);

  //set mode to CTC
  outb (TCCR1A, 0x40);

  //clock source is system clock, rising edge
  //set mode to CTC with OCR1A
  outb (TCCR1B, 0x08);

  //disable Output compare units
  // do not force immediate compare
  outb (TCCR1C, 0x00);

  //set proper value to OCR1A before starting this timer
  //outw(OCR1A,....)
  //start timer ( clk/2 ouput )
  //set mode to CTC with OCR1A
  outb (TCCR1B, inb (TCCR1B) | 0x01);
}
