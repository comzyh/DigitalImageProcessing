/*
* @Author: Comzyh
* @Date:   2015-04-19 20:24:41
* @Last Modified by:   Comzyh
* @Last Modified time: 2015-04-20 02:25:34
*/
//索伯算子
#include <iostream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cmath>
#include "bmp.h"
using namespace std;
const double PI = acos(-1.0);

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
//索伯算子
void Edge_Detection_Sobel(ArrayReader image_in, ArrayReader sobel_g, ArrayReader sobel_t)
{
    //本程序坐标,第一维增大方向为x,第二维增大方向为y
    static const int sobel_x[3][3] =
    {
        { -1, -2, -1},
        {  0,  0,  0},
        {  1,  2,  1}
    };
    static const int sobel_y[3][3] =
    {
        { -1, 0, 1},
        { -2, 0, 2},
        { -1, 0, 1}
    };
    const double pi_8 = PI / 8.0;
    const double pi_4 = PI / 4.0;
    for (int x = 0; x < image_in.height - 3; x++)
        for (int y = 0; y < image_in.width - 3; y ++)
        {
            int gx = 0;
            int gy = 0;
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                {
                    gx += sobel_x[i][j] * image_in[x + i][y + j];
                    gy += sobel_y[i][j] * image_in[x + i][y + j];
                }
            double t = atan2(gy, gx);
            t += pi_8;
            if (t < 0)
                t = 2 * PI + t;
            sobel_t[x + 1][y + 1] = ((int)(t / pi_4));
            sobel_t[x + 1][y + 1] = min(255,sobel_t[x + 1][y + 1] * 32);
            gx /= 8;
            gy /= 8;
            sobel_g[x + 1][y + 1] = min((uint8_t)255, (uint8_t)sqrt((double)gx * gx + (double)gy * gy));
        }
}
int main(int argc, char const *argv[])
{
    char filename_in[256] = "Edge_Detection_in.bmp";
    char filename_out[256] = "Edge_Detection_Soble_out.bmp";
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
    ArrayReader sobel_g = ArrayReader(bmpinfo.biHeight, bmpinfo.biWidth, new uint8_t[bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8]);
    ArrayReader sobel_t = ArrayReader(bmpinfo.biHeight, bmpinfo.biWidth, new uint8_t[bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8]);
    fread(image_in.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_in);

    //EdgeDetection
    Edge_Detection_Sobel(image_in, sobel_g,sobel_t);

    //output
    fseek(file_out, 0L, 0);
    fseek(file_in, 0L, 0);
    uint8_t *header = new uint8_t[bmp.bfOffBit];
    fread(header, bmp.bfOffBit, 1, file_in);
    fwrite(header, bmp.bfOffBit, 1, file_out);
    // fwrite(sobel_g.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_out);
    fwrite(sobel_t.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_out);
    return 0;
}
