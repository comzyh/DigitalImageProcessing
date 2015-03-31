/*
* @Author: Comzyh
* @Date:   2015-03-30 16:05:37
* @Last Modified by:   Comzyh
* @Last Modified time: 2015-03-30 16:46:43
*/

#include <iostream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "bmp.h"
using namespace std;
struct ArrayReader
{
    uint8_t *image;
    int height, width;
    ArrayReader() {}
    ArrayReader(int height, int width, uint8_t *image)
    {
        this -> height = height;
        this -> width = width;
        this -> image = image;
    }
    uint8_t *operator[](const int x)
    {
        return image + x * width;
    }
    const uint8_t *operator[](const int x)const
    {
        return image + x * width;
    }
};
uint8_t middle(const int *histogram, int window, int biBitCount)
{
    int middle_index = (window * window + 1) / 2;
    int sum = 0;
    for (int i = 0; i < ( 1 << biBitCount); i++)
    {
        sum += histogram[i];
        if (sum >= middle_index)
            return i;
    }
}
int main(int argc, char const *argv[])
{
    char filename_in[256] = "DigitalImageProcessing_in.bmp";
    char filename_out[256] = "Median_Filter_out.bmp";
    int window =  21;
    if ( argc >= 2)
        sscanf(argv[1], "%d", &window);
    if (argc >= 3)
        strcpy(filename_in, argv[2]);
    if (argc >= 4)
        strcpy(filename_out, argv[3]);
    int hwo = window / 2; // half window offset
    FILE *file_in = fopen(filename_in, "rb");
    FILE *file_out = fopen(filename_out, "wb+");
    fseek(file_in, 0L, 0);
    BMPFileHeader bmp;
    BMPInfoHeader bmpinfo;
    fread(&bmp, 14, 1, file_in);
    fread(&bmpinfo, 40, 1, file_in);
    fseek(file_in, bmp.bfOffBit, 0);

    ArrayReader image_in = ArrayReader(bmpinfo.biHeight, bmpinfo.biWidth, new uint8_t[bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8]);
    ArrayReader image_out = ArrayReader(bmpinfo.biHeight, bmpinfo.biWidth, new uint8_t[bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8]);
    fread(image_in.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_in);
    memcpy(image_out.image,image_in.image,bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8);
    //Processing
    int *histogram = new int[1 << bmpinfo.biBitCount];
    for (int x = 0; x < bmpinfo.biHeight - window; x++) // row
    {
        memset(histogram,0,sizeof(int) * (1 << bmpinfo.biBitCount));
        for (int i = 0;i< window ;i++)
        	for (int j = 0; j < window;j++)
        		histogram[image_in[x+i][j]] ++;
        image_out[x + hwo][hwo] = middle(histogram,window,bmpinfo.biBitCount);
        for (int y = 1 ; y < bmpinfo.biWidth - window;y ++) //column
        {
        	for (int i=0;i<window;i++)
        	{
        		histogram[image_in[x+i][y-1]] --;
        		histogram[image_in[x+i][y+window -1]]++;
        	}
        	image_out[x+hwo][y+hwo] = middle(histogram,window,bmpinfo.biBitCount);
        }
    }

    //Output
    fseek(file_out, 0L, 0);
    fseek(file_in, 0L, 0);
    uint8_t *header = new uint8_t[bmp.bfOffBit];
    fread(header, bmp.bfOffBit, 1, file_in);
    fwrite(header, bmp.bfOffBit, 1, file_out);
    fwrite(image_out.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_out);


    return 0;
}
