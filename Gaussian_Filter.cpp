/*
* @Author: Comzyh
* @Date:   2015-04-19 20:24:41
* @Last Modified by:   Comzyh
* @Last Modified time: 2015-04-19 23:20:48
*/

#include <iostream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "bmp.h"
using namespace std;
const int LOG[5][5] =
{
    {  0,  0, -1,  0,  0},
    {  0, -1, -2, -1,  0},
    { -1, -2, 16, -2, -1},
    {  0, -1, -2, -1,  0},
    {  0,  0, -1,  0,  0},
};
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
inline int abs(int x)
{
    return x > 0 ? x : -x;
}
void Gaussian_filter(ArrayReader image_in, ArrayReader image_out)
{
    static const int Gaussian[5][5] =
    {
        {2,  4,  5,  4, 2},
        {4,  9, 12,  9, 4},
        {5, 12, 15, 12, 5},
        {4,  9, 12,  9, 4},
        {2,  4,  5,  4, 2},
    };
    for (int x = 0; x < image_in.height; x++)
        for (int y = 0; y < image_in.width; y++)
        {
            int sum = 0;
            int pp = 0;
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    if (x + i - 2 >= 0 && x + i - 2 < image_in.height && y + j - 2 >= 0 && y + j - 2 < image_in.width)
                    {
                        pp += Gaussian[i][j];
                        sum += Gaussian[i][j] * image_in[x + i - 2][y + j - 2];
                    }
            image_out[x][y] = sum / pp;
        }
}
int main(int argc, char const *argv[])
{
    char filename_in[256] = "Gaussian_filter_in.bmp";
    char filename_out[256] = "Gaussian_filter_out.bmp";
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

    ArrayReader image_in = ArrayReader(bmpinfo.biHeight, bmpinfo.biWidth, new uint8_t[bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8]);
    ArrayReader image_out = ArrayReader(bmpinfo.biHeight, bmpinfo.biWidth, new uint8_t[bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8]);
    fread(image_in.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_in);

    //EdgeDetection
    Gaussian_filter(image_in, image_out);

    //output
    fseek(file_out, 0L, 0);
    fseek(file_in, 0L, 0);
    uint8_t *header = new uint8_t[bmp.bfOffBit];
    fread(header, bmp.bfOffBit, 1, file_in);
    fwrite(header, bmp.bfOffBit, 1, file_out);
    fwrite(image_out.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_out);
    return 0;
}
