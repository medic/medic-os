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

#ifndef _UART_H_
#define _UART_H_

void init_sim_port ();
void init_vars();
void init_uart(void);
void uart_irq();

void uart_send_char(u8 txValue);
u8 uart_chars(void);
u8 uart_get_char(void);
void uart_change_parity(void);

#endif
