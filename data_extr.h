#ifndef DATA_EXTR__H
#define DATA_EXTR__H

#include <vcl.h>
#pragma hdrstop

#include <windows.h>

HANDLE openDevice(int device);
 
HANDLE openOutputFile(const char * filename);
 
bool copySectorToFile(HANDLE device, HANDLE file, DWORD sector);

bool GetDataToBinFile(HANDLE device);

AnsiString unpack_dattim(unsigned d);

bool UnpackDataToFiles(void);

#endif