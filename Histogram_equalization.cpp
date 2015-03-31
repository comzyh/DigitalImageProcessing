/*
* @Author: Comzyh
* @Date:   2015-03-30 01:47:36
* @Last Modified by:   Comzyh
* @Last Modified time: 2015-03-30 16:34:08
*/
#include <cstdio>
#include <cstring>
#include <iostream>
#include "bmp.h"
using namespace std;
int main(int argc, char const *argv[])
{
    char filename_in[256] = "DigitalImageProcessing_in.bmp";
    char filename_out[256] = "Histogram_equalization_out.bmp";
    if (argc >= 2)
        strcpy(filename_in, argv[1]);
    if (argc >= 3)
        strcpy(filename_out, argv[2]);
    FILE *file_in = fopen(filename_in, "rb");
    FILE *file_out = fopen(filename_out, "wb+");
    fseek(file_in, 0L, 0);
    BMPFileHeader bmp;
    BMPInfoHeader bmpinfo;
    fread(&bmp, 14, 1, file_in);
    fread(&bmpinfo, 40, 1, file_in);
    fseek(file_in, bmp.bfOffBit, 0);

    uint8_t *image = new uint8_t[bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8];
    fread(image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_in);
    int *histogram = new int[1 << bmpinfo.biBitCount];
    memset(histogram, 0, sizeof(int) * (1 << bmpinfo.biBitCount));
    for (int i = 0; i < bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8; i++)
        histogram[image[i]] ++ ;
    int *cdf = new int[1 << bmpinfo.biBitCount];
    cdf[0] = histogram[0];
    for (int i = 1; i < (1 << bmpinfo.biBitCount); i++)
        cdf[i] = cdf [i - 1] + histogram[i];
    int cdf_min = 0x3f3f3f3f;
    for (int i = 0; i < (1 << bmpinfo.biBitCount); i++)
        if (cdf[i] != 0)
            cdf_min = min(cdf_min, cdf[i]);
    for (int i = 0; i < bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8; i++)
        image[i] = 1.0 * (cdf[image[i]]  - cdf_min)  / (( bmpinfo.biHeight * bmpinfo.biWidth ) - cdf_min) * ((1 << bmpinfo.biBitCount ) - 1);
    fseek(file_out, 0L, 0);
    fseek(file_in, 0L, 0);
    uint8_t *header = new uint8_t[bmp.bfOffBit];
    fread(header, bmp.bfOffBit, 1, file_in);
    fwrite(header, bmp.bfOffBit, 1, file_out);
    fwrite(image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_out);
    return 0;
}
//Thanks
//Sample image in.bmp
//“Unequalized Hawkes Bay NZ”作者original Phillip Capper, modified by User:Konstable - modified Hawkes Bay NZ.jpg。来自维基共享资源 - https://commons.wikimedia.org/wiki/File:Unequalized_Hawkes_Bay_NZ.jpg#/media/File:Unequalized_Hawkes_Bay_NZ.jpg根据知识共享 署名 2.0授权
