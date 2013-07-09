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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/timeb.h>

#ifdef WIN32_MINGW
#include <conio.h>
#endif

#include <tprog/tproglib.h>
#include "../module-src/turboadapter.h"

void help ()
{
  printf ("Print Turbo Adapter debug\n");
  printf ("Usage: dbg [args] [file]\n");
  printf ("-a application\tuse application, default is %s\n",
	  APP_TURBO_ADAPTER);
  printf ("-d device\tuse USB device, default is %s\n", USBDEVICE);
  printf ("-h\t\thelp\n");
  printf ("-t\t\tdisplay time stamps\n");
  printf ("-v\t\tverbose\n");
  exit (-1);
}

int main (int argc, char **argv)
{
  int result;
  u8 *dev = USBDEVICE;
  u8 *app_name = APP_TURBO_ADAPTER;
  u8 answer[2048];
  u16 answerLen;
  u16 i;
  u16 l;
  int fdstdin;
  int flags;
  int bytes_read;
  char buf[1024];
  int len;
  FILE *fe;
  u8 *filename = NULL;
  u8 c;
  u8 show_time = 0;
  u8 was_nl;
  u8 first_time = 0;
  struct timeb tp;
  struct timeb tp_start;
  u16 max_len = 0;

  do
  {
    result = getopt (argc, argv, "hd:a:vt");
    switch (result)
    {
      case 'a':
	app_name = optarg;
	break;
      case 'd':
	dev = optarg;
	break;
      case 'h':
	help ();
	break;
      case 'v':
	verbose_inc ();
	break;
      case 't':
	show_time = 1;
	break;
    }
  }
  while (result > 0);

  if (argc - optind == 1)
    filename = argv[optind];
  if (filename)
  {
    fe = fopen (filename, "w");
    if (fe == NULL)
    {
      printf ("Cannot open %s for writing\n", filename);
      exit (-1);
    }
  }

  open_device (dev);
  if (select_app (app_name) != NO_ERROR)
  {
    close_device ();
    fprintf (stderr, "Cannot open application %s\n", app_name);
    exit (-1);
  }

#ifndef WIN32_MINGW
  fdstdin = fileno (stdin);
  flags = fcntl (fdstdin, F_GETFL, 0);
  fcntl (fdstdin, F_SETFL, flags | O_NONBLOCK);
  len = sizeof (buf);
  while ((bytes_read = read (fdstdin, buf, len)) != -1);
#endif

  usb_send (ACTION_TURBO_ADAPTER_DBG_INIT, 0, NULL, &answerLen, answer);
  printf ("Turbo Adapter debug mode initialized, %s\n", app_name);
  printf ("Press ENTER to quit\n");
  fflush (stdout);

  was_nl = 0;
#ifndef WIN32_MINGW
  while ((bytes_read = read (fdstdin, buf, len)) == -1)
#else
  while (!kbhit ())
#endif
  {
    if ((l = usb_chars ()) != 0)
    {
//if(l>max_len)max_len=l; printf ("L: %d %d\n", max_len, l);
      for (i = 0; i < l; i++)
      {
	c = usb_get_char ();
	if (show_time && was_nl)
	{
	  if (first_time == 0)
	  {
	    ftime (&tp_start);
	    first_time = 1;
	  }
	  ftime (&tp);
	  printf ("%d.%.3d\t", tp.time - tp_start.time, tp.millitm);
	  if (filename)
	    fprintf (fe, "%d.%.3d\t", tp.time - tp_start.time, tp.millitm);
	}
	printf ("%c", c);
	if (show_time && c == '\n')
	  was_nl = 1;
	else
	  was_nl = 0;
	if (filename)
	  fputc (c, fe);
      }
      fflush (stdout);
    }
  }

  if (filename)
    fclose (fe);

  close_device ();
}
