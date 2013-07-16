#include <tproglib.h>
#include "device.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define UCMD_INIT_MODE_ENTER 		(0x80)
#define UCMD_INIT_MODE_EXIT 		(0x81)
#define UCMD_INIT_MODE_VERSION 		(0x82)
#define UCMD_INIT_MODE_UPLOAD_APP 	(0x83)
#define UCMD_INIT_MODE_REMOVE_APP 	(0x84)
#define UCMD_INIT_MODE_LIST_APPS 	(0x85)
#define UCMD_INIT_MODE_INIT_EEPROM 	(0x86)
#define UCMD_INIT_MODE_READ_PAGE 	(0x87)
#define UCMD_INIT_MODE_WRITE_PAGE 	(0x88)
#define UCMD_INIT_MODE_APP_ACTION 	(0x89)

u8 __tproglib_verbose = 0;
u8 __tproglib_target = 0;

u8 verbose_inc ()
{
  __tproglib_verbose++;
}

u8 __app = 0;

u8 select_app_nr (u8 app_nr)
{
  __app = app_nr;
}

u8 select_app (u8 * app_name)
{
  u8 nr_apps;
  u16 av_name;
  u8 *av;
  u8 *p;
  u16 i, j, len;
  u8 name[100];
  u8 buf[2048];
  u8 result;
  u8 sizeof_App_vector = 15;
  u8 ver_prog_A;

  if (__tproglib_target != BLADOX_DEVICE_TPROG)
    return NO_ERROR;

  usb_put_char (UCMD_INIT_MODE_VERSION);
while((ver_prog_A = usb_get_char ())==0);
  usb_get_char ();
  usb_get_char ();
  usb_get_char ();

//printf("Programmer application list:\n");

  usb_put_char (UCMD_INIT_MODE_LIST_APPS);

  result = usb_get_char ();
  len = usb_get_char ();
  len <<= 8;
  len += usb_get_char ();

  for (i = 0; i < len; i++)
    buf[i] = usb_get_char ();

  nr_apps = buf[6];
  p = &buf[7];

  if (ver_prog_A >= 3)
    sizeof_App_vector = 18;

  for (i = 0; i < nr_apps; i++)
  {
    av = p;
    if (ver_prog_A >= 3)
    {
      av_name = (((u16) av[16]) << 8) | av[17];
    }
    else
      av_name = (((u16) av[13]) << 8) | av[14];

    p += sizeof_App_vector;

    if (av_name)
    {
      j = 0;
      while (*p)
      {
	name[j++] = *p;
	p++;
      }
      name[j++] = '\0';
      p++;
      if (strcmp (name, app_name) == 0)
      {
	__app = i;
	return NO_ERROR;
      }
    }
  }
  __app = 0;
  return ERR_NOT_FOUND;
}

u8 usb_send (u8 action, u16 txlen, u8 * txbuf, b16 * rxlen, u8 * rxbuf)
{
  u16 i;
  u16 l;
  u8 ret;
  u8 c;

  if (__tproglib_target == BLADOX_DEVICE_TPROG)
  {
    usb_put_char (UCMD_INIT_MODE_APP_ACTION);
    usb_put_char (__app);
    usb_put_char (action);
    usb_put_char (txlen >> 8);
    usb_put_char (txlen);

    if (txlen != 0)
      for (i = 0; i < txlen; i++)
	usb_put_char (txbuf[i]);

    c = usb_get_char ();

    if (c == 0x04)
    {
      ret = usb_get_char ();
      l = usb_get_char ();
      l = (l << 8) + usb_get_char ();

      if (l)
      {
	if (rxbuf)
	{
	  for (i = 0; i < l; i++)
	    rxbuf[i] = usb_get_char ();
	  if (rxlen)
	    *rxlen = l;
	}
	else
	{
//printf("USB_PROTOCOL_ERROR - data and no buffer\n");
	  for (i = 0; i < l; i++)
	    usb_get_char ();
	  if (rxlen)
	    *rxlen = 0;
	}
      }
      else
      {
	if (rxlen)
	  *rxlen = 0;
      }
    }
    else
    {
      printf ("USB_REC_ERROR\n");
      return 1;
    }
  }
  else
  {
    if (action)
      usb_put_char (action);
//  usb_put_char (txlen >> 8);
//  usb_put_char (txlen);

    if (txlen != 0)
      for (i = 0; i < txlen; i++)
	usb_put_char (txbuf[i]);

    if (*rxlen >= 0)
    {
// predefined len
      l = *rxlen;

      if (rxbuf)
      {
	for (i = 0; i < l; i++)
	  rxbuf[i] = usb_get_char ();
	if (rxlen)
	  *rxlen = l;
      }
      else
      {
//printf("USB_PROTOCOL_ERROR - data and no buffer\n");
	for (i = 0; i < l; i++)
	  usb_get_char ();
	if (rxlen)
	  *rxlen = 0;
      }
    }
    else
    {
//unknown <0
      ret = usb_get_char ();
      l = usb_get_char ();
      l = (l << 8) + usb_get_char ();

      if (l)
      {
	if (rxbuf)
	{
	  rxbuf[0] = ret;
	  rxbuf[1] = l >> 8;
	  rxbuf[2] = l;
	  for (i = 0; i < l; i++)
	    rxbuf[i + 3] = usb_get_char ();
	  if (rxlen)
	    *rxlen = l;
	}
	else
	{
//printf("USB_PROTOCOL_ERROR - data and no buffer\n");
	  for (i = 0; i < l; i++)
	    usb_get_char ();
	  if (rxlen)
	    *rxlen = 0;
	}
      }
    }

  }

  return ret;
}

u8 usb_put_char (u8 c)
{
  deviceTxSendChar (c);
  if (__tproglib_verbose > 1)
  {
    printf ("P: %02X\n", c);
    fflush (stdout);
  }
}

u8 usb_put_data (u8 * d, u16 len)
{
  deviceTxSendData (d, len);
  if (__tproglib_verbose > 1)
  {
    u16 i;

    for (i = 0; i < len; i++)
      printf ("P: %02X\n", d[i]);
    fflush (stdout);
  }
}

u8 usb_get_char (void)
{
  u8 c;

  while (deviceRxIsChar () == 0);
  deviceRxGetChar (&c);
  if (__tproglib_verbose > 1)
  {
    printf ("G: %02X\n", c);
    fflush (stdout);
  }
  return c;
}

u8 usb_get_char_nb (void)
{
  u8 c;

  deviceRxGetChar (&c);
  return c;
}

u8 enter_app_mode()
{
  if (__tproglib_verbose > 0)
    printf ("Entering Application Mode\n");
  usb_put_char (UCMD_INIT_MODE_ENTER);
  usb_get_char ();

  if (__tproglib_verbose > 0)
    printf ("Application Mode entered\n");

  return 0;
}

u8 open_device (u8 * dev)
{
  __tproglib_target = BLADOX_DEVICE_TPROG;

#ifndef WIN32_MINGW
  sleep (1);
#endif

  if (__tproglib_verbose > 0)
    printf ("Initializing device\n");
  deviceInit (dev);
  if (__tproglib_verbose > 0)
    printf ("Device initialized\n");

enter_app_mode();
  return 0;
}

u8 open_device_gen (u8 * dev, u8 target)
{
  __tproglib_target = target;

#ifndef WIN32_MINGW
  sleep (1);
#endif

  if (target == BLADOX_DEVICE_TPROG)
    return open_device (dev);

  if (__tproglib_verbose > 0)
    printf ("Initializing device\n");
  deviceInit (dev);
  if (__tproglib_verbose > 0)
    printf ("Device initialized\n");

  return 0;
}

u8 close_app()
{
  if (__tproglib_target == BLADOX_DEVICE_TPROG)
  {
    usb_put_char (UCMD_INIT_MODE_EXIT);
    usb_get_char ();
    if (__tproglib_verbose > 0)
      printf ("Application Mode left\n");
  }

return 0;
}

u8 close_device ()
{
  close_app();
  deviceClose ();
  return 0;
}

u8 no_exit_close_device ()
{
  deviceClose ();
  return 0;
}

u8 usb_chars ()
{
  return deviceRxIsChar ();
}
