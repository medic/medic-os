/*
 * Copyright (c) 2004 BLADOX, s.r.o.
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE BLADOX OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.  
 */

#ifndef __TPROG_LIB_
#define __TPROG_LIB_

#define LIBTFS

#include "types.h"
//#include "consts.h"
#include "err.h"

// action send if not zero
// if rxlen==0 -> 2 first bytes expected to be len to receive
u8 usb_send(u8 action, u16 txlen, u8 *txbuf, b16 *rxlen, u8 *rxbuf);
u8 usb_get_char(void);
u8 usb_get_char_nb(void);
u8 usb_chars(void);
u8 usb_put_char(u8 c);
u8 usb_put_data(u8 *d, u16 len);
u8 select_app_nr(u8 app_nr);
u8 select_app(u8 *app_name);
u8 close_app();
u8 enter_app_mode();

u8 open_device (u8 *dev);

#define BLADOX_DEVICE_TPROG	0x00
#define BLADOX_DEVICE_USBBRUM	0x01

u8 open_device_gen (u8 *dev, u8 target);

u8 close_device(void);
u8 no_exit_close_device(void);

u8 verbose_inc();

#ifndef WIN32_MINGW
#define USBDEVICE "/dev/ttyUSB0"
#else
#define USBDEVICE "COM3"
#endif

u8 apacketSendChar(u8 command, u8 ch);
u8 apacketGetApacket(u8 **packet);

#endif
