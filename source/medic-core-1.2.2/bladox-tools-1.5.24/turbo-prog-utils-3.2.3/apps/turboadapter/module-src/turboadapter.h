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

#ifndef _TURBO_ADAPTER_PROG_H_
#define _TURBO_ADAPTER_PROG_H_

#define APP_TURBO_ADAPTER "turboadapter.trb"

#define ACTION_TURBO_ADAPTER_BTLDR_INIT			0x80
#define ACTION_TURBO_ADAPTER_BTLDR_WRITE_PAGE		0x81
#define ACTION_TURBO_ADAPTER_BTLDR_SIM_INIT		0x82
#define ACTION_TURBO_ADAPTER_BTLDR_READ_PAGE		0x83
#define ACTION_TURBO_ADAPTER_BTLDR_WRITE_PAGE_EEPROM	0x84
#define ACTION_TURBO_ADAPTER_BTLDR_READ_PAGE_EEPROM	0x85
#define ACTION_TURBO_ADAPTER_BTLDR_SRAM_FLASH		0x86
#define ACTION_TURBO_ADAPTER_BTLDR_SRAM_EEPROM		0x87
#define ACTION_TURBO_ADAPTER_BTLDR_INIT_V3		0x88
#define ACTION_TURBO_ADAPTER_BTLDR_INIT_V4		0x89
#define ACTION_TURBO_ADAPTER_BTLDR_GET_SN		0x8a
#define ACTION_TURBO_ADAPTER_BTLDR_SET_ATTR		0x8b
#define ACTION_TURBO_ADAPTER_BTLDR_FAST_BTLDR		0x8c
#define ACTION_TURBO_ADAPTER_BTLDR_SET_ATTR_5		0x8d

#define ACTION_TURBO_ADAPTER_DBG_INIT			0x90

#define BTLDR_VERIFY_ERROR	0x11
#define BTLDR_CRC_ERROR		0x12

#define BTLDR_PORT_TURBO_ADAPTER	0x00
#define BTLDR_PORT_BRUM_1		0x01
#define BTLDR_PORT_BRUM_2		0x02
#define BTLDR_PORT_SIM			0x03

#endif
