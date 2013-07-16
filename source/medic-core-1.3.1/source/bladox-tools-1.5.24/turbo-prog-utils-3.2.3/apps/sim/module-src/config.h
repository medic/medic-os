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


#ifndef _CONFIG_TURBO_
#define _CONFIG_TURBO_

#define VERSION_A       3
#define VERSION_B       2
#define VERSION_C       0

#define DEBUG

#define CONFIG_LANG_EN
#define CONFIG_LANG_DE
#define CONFIG_LANG_IT
#define CONFIG_LANG_FR
#define CONFIG_LANG_ES
#define CONFIG_LANG_NL
#define CONFIG_LANG_SE
#define CONFIG_LANG_DK
#define CONFIG_LANG_PT
#define CONFIG_LANG_FI
#define CONFIG_LANG_NO
#define CONFIG_LANG_EL
#define CONFIG_LANG_TR
#define CONFIG_LANG_HU
#define CONFIG_LANG_PL
#define CONFIG_LANG_UN
#define CONFIG_LANG_CZ
#define CONFIG_LANG_HE
#define CONFIG_LANG_AR
#define CONFIG_LANG_RU
#define CONFIG_LANG_IS

//-------------------------------------------------------------------
// PINS
//-------------------------------------------------------------------

#define SIM_RST_DDR DDRD
#define SIM_RST_DDR_DD DD4
#define SIM_RST_PORT PORTD
#define SIM_RST_PORT_PIN PORT4

#define SIM_VCC_ENABLE_DDR DDRB
#define SIM_VCC_ENABLE_DDR_DD DD4
#define SIM_VCC_ENABLE_PORT PORTB
#define SIM_VCC_ENABLE_PORT_PIN PORT4

#define SIM_CLKA_DDR DDRB
#define SIM_CLKA_DDR_DD DD5
#define SIM_CLKA_PORT PORTB
#define SIM_CLKA_PORT_PIN PORT5

#define SIM_CLKA_ENABLE_DDR DDRG
#define SIM_CLKA_ENABLE_DDR_DD DD3
#define SIM_CLKA_ENABLE_PORT PORTG
#define SIM_CLKA_ENABLE_PORT_PIN PORT3

#define SIM_CLK_ENABLE_DDR DDRG
#define SIM_CLK_ENABLE_DDR_DD DD4
#define SIM_CLK_ENABLE_PORT PORTG
#define SIM_CLK_ENABLE_PORT_PIN PORT4

#define SIM_RXD_DDR DDRD
#define SIM_RXD_DDR_DD DD2
#define SIM_RXD_PORT PORTD
#define SIM_RXD_PORT_PIN PORT2

#define SIM_TXD_DDR DDRD
#define SIM_TXD_DDR_DD DD3
#define SIM_TXD_PORT PORTD
#define SIM_TXD_PORT_PIN PORT3

//-------------------------------------------------------------------
// UARTs
//-------------------------------------------------------------------
#define SIM_SIG_UART_RECV SIG_UART1_RECV
#define SIM_UDR UDR1
#define SIM_UART_UCSRA UCSR1A
#define SIM_UART_UCSRB UCSR1B
#define SIM_UART_UCSRC UCSR1C
#define SIM_UART_UBRRH UBRR1H
#define SIM_UART_UBRRL UBRR1L

//-------------------------------------------------------------------
// EXT PORTS
//-------------------------------------------------------------------
#define ENABLES1_WRITE_PORT 3

//-------------------------------------------------------------------
// EXT PINS 
//-------------------------------------------------------------------

#define EXT_SIM_IO_ENABLE_PORT ENABLES1_WRITE_PORT
#define EXT_SIM_IO_ENABLE_PORT_PIN 5

#define EXT_SIM_RST_ENABLE_PORT ENABLES1_WRITE_PORT
#define EXT_SIM_RST_ENABLE_PORT_PIN 6

#endif
