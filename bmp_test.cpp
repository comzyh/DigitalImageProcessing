/*
* @Author: Comzyh
* @Date:   2014-11-05 23:06:14
* @Last Modified by:   Comzyh
* @Last Modified time: 2014-11-06 04:09:13
*/
//尝试BMP文件的读取
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "bmp.h"
using namespace std;

#pragma pack(push,2)//手动元素对齐
int main(int argc, char const *argv[])
{
    char path[256];
    if (argc >= 1)
        strcpy(path, argv[1]);
    else
        return 0;
    printf("%s\n", path );
    FILE *file = fopen(path, "rb");
    fseek(file, 0L, 0);
    BMPFileHeader bmp;
    BMPInfoHeader bmpinfo;
    fread(&bmp, 14, 1, file);
    fread(&bmpinfo, 40, 1, file);
    fseek(file, bmp.bfOffBit, 0);
    for (int r = 0; r < bmpinfo.biHeight; r++ )
    {
        for (int c = 0; c < bmpinfo.biWidth; c++)
        {
            int32_t p = 0;
            fread(&p, 3, 1, file);
            printf("%06x ", p);
        }
        printf("\n");
    }
    return 0;
}
