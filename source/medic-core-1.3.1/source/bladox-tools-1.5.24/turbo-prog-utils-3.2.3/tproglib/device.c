#include <tproglib.h>
#include "device.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef WIN32_MINGW
#include <termios.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#define BAUDRATE B9600
#define BAUDRATE B38400
//#define BAUDRATE B115200

int deviceFd;
int deviceBufChars;
int deviceBufPointer;

#ifndef WIN32_MINGW
struct termios deviceOldtio, deviceNewtio;
#endif

#ifdef MINGW
#define WIN32_MINGW
#endif

#ifdef WIN32_MINGW
#include <windows.h>
HANDLE hPort;
#endif

u8 deviceBuf[2048];

//-------------------------------------------------
// Initialize usb device
// returns 0 if OK
//-------------------------------------------------
int deviceInit (u8 * dev)
{
#ifdef WIN32_MINGW
  DCB dcb;

  hPort =
    CreateFile (dev, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0,
		NULL);

  if (hPort == INVALID_HANDLE_VALUE)
  {
    printf ("Cannot open IO port %s (error %d). Use format \\\\.\\COM#\n",
	    dev, GetLastError ());
    exit (-1);
  }

  GetCommState (hPort, &dcb);
//  dcb.BaudRate = CBR_19200;
  dcb.BaudRate = CBR_38400;
  dcb.fParity = FALSE;
  dcb.fOutxCtsFlow = FALSE;
  dcb.fOutxDsrFlow = FALSE;
  dcb.fDtrControl = DTR_CONTROL_DISABLE;
  dcb.fDsrSensitivity = FALSE;
  dcb.fOutX = FALSE;
  dcb.fInX = FALSE;
  dcb.fRtsControl = RTS_CONTROL_DISABLE;
  dcb.fAbortOnError = FALSE;
  dcb.ByteSize = 8;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  SetCommState (hPort, &dcb);

#else
  deviceFd = open (dev, O_RDWR | O_NOCTTY);

  if (deviceFd < 0)
  {
    perror (dev);
    exit (-1);
  }
#endif

  deviceBufChars = 0;
  deviceBufPointer = 0;

#ifndef WIN32_MINGW
  tcgetattr (deviceFd, &deviceOldtio);	/* save current port settings */

  bzero (&deviceNewtio, sizeof (deviceNewtio));
  deviceNewtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  deviceNewtio.c_iflag = IGNPAR;
  deviceNewtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  deviceNewtio.c_lflag = 0;

  deviceNewtio.c_cc[VTIME] = 0;	/* inter-character timer unused */
  deviceNewtio.c_cc[VMIN] = 0;	/* non-blocking read */

  tcflush (deviceFd, TCIFLUSH);
  tcsetattr (deviceFd, TCSANOW, &deviceNewtio);
#endif

  return 0x00;
}

//-------------------------------------------------
// Closes usb device
// returns 0 if OK
//-------------------------------------------------
int deviceClose ()
{
#ifdef WIN32_MINGW
  CloseHandle (hPort);
#else
  tcsetattr (deviceFd, TCSANOW, &deviceOldtio);
  close (deviceFd);
#endif
  return 0x00;
}

//-------------------------------------------------
// Check if there is a char in USB receive FIFO
// returns 0 if no char in receiving FIFO
//-------------------------------------------------
int deviceRxIsChar ()
{
  u16 i;

  if (deviceBufChars != 0)
    return deviceBufChars;
  else
  {
    // if there are any data in receive FIFO
#ifdef WIN32_MINGW
    int n_in;

    ReadFile (hPort, deviceBuf, 1, (DWORD *) (&n_in), NULL);
    deviceBufChars = n_in;
    if (deviceBufChars != 0)
#else
    //if ((deviceBufChars = read (deviceFd, deviceBuf, 2048)) != 0)
/*
Max buffer size from 2048->200 because of Linux FTDI bug? 
on 2.4.19 2048 is OK, on 2.6.7 it stops reading
*/
    if ((deviceBufChars = read (deviceFd, deviceBuf, 200)) != 0)
#endif
    {
//printf("X: %02X\n", deviceBuf[0]);fflush(stdout);
      deviceBufPointer = 0;
      return deviceBufChars;
    }
    // no data in receive FIFO
    else
      return 0;
  }
}

//-------------------------------------------------
// Gets char from USB iface
// returns 0 if no char in receiving FIFO
//-------------------------------------------------
int deviceRxGetChar (u8 * rxChar)
{

  if (deviceBufChars != 0)
  {
    *rxChar = deviceBuf[deviceBufPointer];
    deviceBufChars--;
    if (deviceBufChars > 0)
      deviceBufPointer++;
    else
      deviceBufPointer = 0;
    return 1;
  }
  else
    return 0;
}

//-------------------------------------------------
// Flushes USB receive FIFO
// returns 0
//-------------------------------------------------
int deviceRxFlushFifo ()
{
  u8 tmpData;

  while (deviceRxIsChar () != 0)
    deviceRxGetChar (&tmpData);
  return 0;
}

//-------------------------------------------------
// Sends char to USB iface BLOCKING
// returns 0 if char could not be transmitted
// (transmit FIFO was full)
//-------------------------------------------------
int deviceTxSendChar (u8 txChar)
{
  u8 tmpData;
  int n_out;

  tmpData = txChar;
#ifdef WIN32_MINGW
  WriteFile (hPort, &tmpData, 1, (DWORD *) (&n_out), NULL);
#else
  write (deviceFd, &tmpData, 1);
//fsync(deviceFd);
#endif
#if 0
printf("S1\n");fflush(stdout);
sleep(1);
printf("S2\n");fflush(stdout);
#endif
  return 1;
}

//-------------------------------------------------
// Sends data buffer to USB iface BLOCKING
// returns 0 if char could not be transmitted
// (transmit FIFO was full)
//-------------------------------------------------
int deviceTxSendData (u8 * data, u16 len)
{
  write (deviceFd, data, len);
  return 1;
}
