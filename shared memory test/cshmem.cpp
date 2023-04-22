// Must ruun as administrator.
// via https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>

#define BUF_SIZE 256
// Note: originally Global\\MyFileMappingObject.
// Not sure what global does but needs admin
TCHAR szName[]=TEXT("ReimaginingBreath");
TCHAR szMsg[]=TEXT("YOssage from first process.");

int _tmain()
{
   int temp = 12;
   int *point2;
   point2 = &temp;
   
   HANDLE hMapFile;
   LPCTSTR pBuf;

   for (int i = 0; i < 100087; i++) {
      hMapFile = CreateFileMapping(
                  INVALID_HANDLE_VALUE,    // use paging file
                  NULL,                    // default security
                  PAGE_READWRITE,          // read/write access
                  0,                       // maximum object size (high-order DWORD)
                  BUF_SIZE,                // maximum object size (low-order DWORD)
                  szName);                 // name of mapping object

      if (hMapFile == NULL)
      {
         _tprintf(TEXT("Could not create file mapping object (%d).\n"),
               GetLastError());
         return 1;
      }
      pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                           FILE_MAP_ALL_ACCESS, // read/write permission
                           0,
                           0,
                           BUF_SIZE);

      if (pBuf == NULL)
      {
         _tprintf(TEXT("Could not map view of file (%d).\n"),
               GetLastError());

         CloseHandle(hMapFile);

         return 1;
      }

      


      CopyMemory((PVOID)pBuf, point2, sizeof(int));
      // _getch();

      
      std::cout << temp << std::endl;
      temp++;
   }

   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);
   
   return 0;
}