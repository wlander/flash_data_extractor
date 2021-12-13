#include <vcl.h>
#pragma hdrstop

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fstream.h>

static char fsys_buf[512];
static unsigned* ptru_fsys = (unsigned*)fsys_buf;

HANDLE openDevice(int device) {
  HANDLE handle = INVALID_HANDLE_VALUE;
  if (device <0 || device >99)
     return INVALID_HANDLE_VALUE;
   
  char _devicename[20];
  sprintf(_devicename, "\\\\.\\PhysicalDrive%d", device);
 
  // Creating a handle to disk drive using CreateFile () function ..
  handle = CreateFile(_devicename, 
    GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
    NULL, OPEN_EXISTING, 0, NULL); 
 
  return handle;
}
 
HANDLE openOutputFile(const char * filename) {
       return CreateFile ( filename,      // Open Two.txt.
            GENERIC_WRITE,          // Open for writing
            0,                      // Do not share
            NULL,                   // No security
            OPEN_ALWAYS,            // Open or create
            FILE_ATTRIBUTE_NORMAL,  // Normal file
            NULL);                  // No template file       
}
 
bool copySectorToFile(HANDLE device, HANDLE file, DWORD sector) {
    const int numberofsectors = 1;
    bool result = false;
    // Setting the pointer to point to the start of the sector we want to read ..
    SetFilePointer (device, (sector*512), NULL, FILE_BEGIN);

    DWORD dwBytesRead;
    DWORD dwBytesWritten;
    unsigned char buffer[512];
    if (ReadFile (device, buffer, 512*numberofsectors, &dwBytesRead, NULL))
    {
       result = WriteFile (file, buffer, dwBytesRead,
                 &dwBytesWritten, NULL);
    }
    return result;
}

bool GetDataToBinFile(HANDLE device) {

    HANDLE outFile = openOutputFile("file");
    int i;
    bool result = false;
    // Setting the pointer to point to the start of the sector we want to read ..
    SetFilePointer (device, 0, NULL, FILE_BEGIN);

    DWORD dwBytesRead;
    DWORD dwBytesWritten;
    unsigned char buffer[512];
    unsigned* ptr_buf = (unsigned*)&buffer;
    unsigned num_sectors = 0;

 if (outFile != INVALID_HANDLE_VALUE){

    //читаем нулевой сектор
    if (ReadFile (device, buffer, 512, &dwBytesRead, NULL))
    {
       num_sectors =  ptr_buf[2];
       result = WriteFile (outFile, buffer, dwBytesRead, &dwBytesWritten, NULL);

       memcpy(fsys_buf, buffer, 512);
    }
    num_sectors = num_sectors;
    if(num_sectors>1){
       for(i=1;i<num_sectors;i++){
                SetFilePointer (device, (i*512), NULL, FILE_BEGIN);
                if (ReadFile (device, buffer, 512, &dwBytesRead, NULL))
                {
                        result = WriteFile (outFile, buffer, dwBytesRead, &dwBytesWritten, NULL);
                }
       }
    }

    CloseHandle(outFile);
 }
 else{
    printf("Cannot open output file\n");
 }
    return result;
}

AnsiString unpack_dattim(unsigned d){
        AnsiString str_dt;

        str_dt = "h";
        str_dt +=  (d>>26); //hours
        str_dt += "m";
        str_dt +=  ((d>>18)&0x3F); //minutes
        str_dt += "_d";
        str_dt +=  ((d>>12)&0x3F); //date
        str_dt += "m";
        str_dt +=  ((d>>6)&0x3F); //month
        str_dt += "y";
        str_dt +=  (d&0x3F); //year

        return str_dt;
}

bool UnpackDataToFiles(void) {

        unsigned d;
        unsigned tim;
        unsigned dat;
        unsigned num;
        unsigned cnt_df = 0;

        float* res = (float*)&d;
        unsigned* ud =  (unsigned*)&d;
        char* cd;
        int cnt_file = 0;
        AnsiString file_name;
        AnsiString data_str;
        FILE *in = fopen("file", "rb");
        FILE *out = fopen("info.txt", "wt");
        FILE *outf;

        while(1)
        {
                fread(&d, 4, 1, in);

                if(*ud==0x7f555555){

                  fread(&num, 4, 1, in);  //2  Number of files
                  fread(&d, 4, 1, in); //3  Reserve
                  fread(&tim, 4, 1, in); //3  Time
                  fread(&dat, 4, 1, in); //4  Date

                  if(cnt_file==0){
                        fprintf(out, "Number of files in the registration data: ");
                        fprintf(out, "%d\n", num);
                        fprintf(out, "\n");
                        fprintf(out, "Number of data: ");
                        fprintf(out, "%d\n", (d*512)/4);
                        fprintf(out, "\n");
                        fprintf(out, "Date registration data: ");

                        data_str = unpack_dattim(dat);
                        data_str += "\n";

                        fprintf(out, data_str.c_str());
                        fprintf(out, "\n");
                        fprintf(out, "files information:\n");
                  }


                  if(cnt_file>0){
                        fprintf(out, "\n");
                        fprintf(out,"file:");
                        fprintf(out, "%d", cnt_file);
                        fprintf(out,"   Num: ");
                        fprintf(out, "%d\n", cnt_df);
                        cnt_df = 0;

                        data_str = unpack_dattim(dat);
                        data_str += "\n";

                        fprintf(out, data_str.c_str());
                        if(cnt_file>0) fclose(outf);
                  }

                  file_name = "file"+IntToStr(cnt_file)+".txt";
                  outf = fopen(file_name.c_str(), "wt");

                  cnt_file++;

                }

                if(feof(in))break;

                if(cnt_file>0){
                   if(cnt_df>0) fprintf(outf, "\n");
                   fprintf(outf, "%f", *res);
                }
                cnt_df++;
        }

        if(cnt_file>0){
                fprintf(out, "\n");
                fprintf(out,"file:");
                fprintf(out, "%d", cnt_file);
                fprintf(out,"   Num: ");
                fprintf(out, "%d\n", cnt_df);
                cnt_df = 0;

                data_str = unpack_dattim(dat);

                data_str += "\n";

                fprintf(out, data_str.c_str());
                if(cnt_file>0) fclose(outf);
        }

        fcloseall();

        return 0;
}
