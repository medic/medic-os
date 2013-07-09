#ifndef _UTILS_USB_H_
#define _UTILS_USB_H_

int deviceInit ();
int deviceClose ();
int deviceRxIsChar ();
int deviceRxGetChar (u8 * rxChar);
int deviceRxFlushFifo ();
int deviceTxSendChar (u8 txChar);
int deviceTxSendData (u8 * data, u16 len);

#endif
