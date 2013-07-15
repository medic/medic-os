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

#ifndef _BTLDR_H_
#define _BTLDR_H_

void init_turboadapter_btldr_port (u8 what);
void init_turboadapter_btldr_sim_port ();
void btldr_uart_irq();
void init_vars();
void init_btldrUart(void);
void btldr_flash_page (USB_Data * data, u8 * buf);
void btldr_read_page (USB_Data * data, u8 * buf);
void btldr_flash_page_eeprom (USB_Data * data, u8 * buf);
void btldr_read_page_eeprom (USB_Data * data, u8 * buf);
void reset_turbo_for_bootloader ();
void reset_turbo_for_bootloader_sim ();

void btldr_sram_flash (USB_Data * data, u8 * buf);
void btldr_sram_eeprom (USB_Data * data, u8 * buf);

void btldrTxSendChar(unsigned char txValue);
u8 btldrRxChars(void);
u8 btldrRxGetChar(void);

void set_divider(u8 dh, u8 dl);
void set_divider_default();

#endif
