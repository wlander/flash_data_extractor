#include <vcl.h>
#pragma hdrstop

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fstream.h>
#include <data_extr.h>

int main(int argc, char *argv[]) {
  int i;
  HANDLE hDevice = openDevice(1);
  if (hDevice != INVALID_HANDLE_VALUE) {
    printf("device opened ok\n");

    GetDataToBinFile(hDevice);
    UnpackDataToFiles();
    
    CloseHandle(hDevice);
  } else {
    printf("Cannot open device specified\n");
  }
  return EXIT_SUCCESS;
}

