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

#include "turboadapter.h"

void init_turboadapter_dbg_port ()
{
  led_on (LED_DBG);

  //PROG_DEBUG_SSn_ENABLE  pin set it to log.1
  //(in DEBUG MODE SSn is connected)
  ext_sbi (EXT_PROG_DEBUG_SSn_ENABLE_PORT,
	   EXT_PROG_DEBUG_SSn_ENABLE_PORT_PIN);

  //PROG_DEBUG_MOSI_ENABLE  pin set it to log.1
  //(in DEBUG MODE MOSI is connected)
  ext_sbi (EXT_PROG_DEBUG_MOSI_ENABLE_PORT,
	   EXT_PROG_DEBUG_MOSI_ENABLE_PORT_PIN);

  //PROG_DEBUG_MISO_ENABLE  pin set it to log.1
  //(in DEBUG MODE MOSI is connected)
  ext_sbi (EXT_PROG_DEBUG_MISO_ENABLE_PORT,
	   EXT_PROG_DEBUG_MISO_ENABLE_PORT_PIN);

  //PROG_DEBUG_SCK_ENABLE pin set it to log.1
  //(in DEBUG mode SCK is connected)
  ext_sbi (EXT_PROG_DEBUG_SCK_ENABLE_PORT,
	   EXT_PROG_DEBUG_SCK_ENABLE_PORT_PIN);

  // enable SPI, set SPI SLAVE mode
  outb (SPCR, 0xC3);
}
