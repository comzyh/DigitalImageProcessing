/*
* @Author: Comzyh
* @Date:   2015-04-19 20:24:41
* @Last Modified by:   Comzyh
* @Last Modified time: 2015-04-20 06:27:35
*/

#include <iostream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cmath>
#include "bmp.h"
using namespace std;
const double PI = acos(-1.0);
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
struct ArrayBuffer: ArrayReader
{
    ArrayBuffer(int height, int width): ArrayReader(height, width, NULL)
    {
        image = new uint8_t[height * width];
    }
    ~ArrayBuffer()
    {
        delete image;
    }
    inline void memset(char val)
    {
        std::memset(image, val, sizeof(uint8_t) * height * width);
    }
};
inline int abs(int x)
{
    return x > 0 ? x : -x;
}
void Edge_Detection_LOG(ArrayReader image_in, ArrayReader image_out)
{
    memset(image_out.image, 0, sizeof(uint8_t) * image_out.height * image_out.width);
    for (int x = 0; x + 5 < image_in.height; x++)
        for (int y = 0; y + 5 < image_in.width; y++ )
        {
            int ans = 0;
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    ans += LOG[i][j] * image_in[x + i][y + j];
            ans = min(255, abs(ans));
            image_out[x + 2][y + 2] = ans;
        }

}
long long hough[120][4000];
int main(int argc, char const *argv[])
{
    char filename_in[256] = "Road.bmp";
    char filename_out[256] = "Road_out.bmp";
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

    ArrayBuffer image_in = ArrayBuffer(bmpinfo.biHeight, bmpinfo.biWidth);
    ArrayBuffer image_log = ArrayBuffer(bmpinfo.biHeight, bmpinfo.biWidth);
    image_log.memset(0);
    ArrayBuffer image_out = ArrayBuffer(bmpinfo.biHeight, bmpinfo.biWidth);
    image_out.memset(0);
    fread(image_in.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_in);

    //EdgeDetection
    Edge_Detection_LOG(image_in, image_log);

    //HoughTransform
    memset(hough, 0, sizeof(hough));
    for (int t = 0; t < 120; t++)
    {
        double tt = (t - 60) / 360.0 * 2 * PI; // 120~240
        if (tt < 0)
            tt += PI;
        double costt = cos(tt);
        double sintt = sin(tt);
        for (int x = 0; x < image_log.height; x++)
            for (int y = 0; y < image_log.width; y++)
            {
                int p = x * costt + y * sintt;
                hough[t][p + 2000] += image_log[x][y];
            }
    }
    for (int t = 0; t < 120; t++)
        for (int p = -2000; p < 2000; p++)
            if (hough[t][p + 2000] > 100000)
            {
                double tt = (t - 60) / 360.0 * 2 * PI; // 120~240
                if (tt < 0)
                    tt += PI;
                double costt = cos(tt);
                double sintt = sin(tt);
                printf("%4d %4d %lld\n", t, p, hough[t][p + 2000] );
                for (int x = 0; x < image_log.height; x++)
                    for (int y = 0; y < image_log.width; y++)
                        if ((int)(x * costt + y * sintt)== p)
                            image_out[x][y] = 255;
            }

    //output
    fseek(file_out, 0L, 0);
    fseek(file_in, 0L, 0);
    uint8_t *header = new uint8_t[bmp.bfOffBit];
    fread(header, bmp.bfOffBit, 1, file_in);
    fwrite(header, bmp.bfOffBit, 1, file_out);
    fwrite(image_out.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_out);
    return 0;
}
