// **************************************************************************
//
//
// File:         FM/AM Radiomodule SI4735 with I2C interface
//
// **************************************************************************

#ifndef __SI4735_LIB_H
#define __SI4735_LIB_H

#define SI4735_ADDRESS	0x22
#define INIT_DELAY		100

extern char g_fm_debug;

char si4735_init(void);

#endif // __SI4735_LIB_H

