/*
* @Author: Comzyh
* @Date:   2015-04-19 20:24:41
* @Last Modified by:   Comzyh
* @Last Modified time: 2015-04-20 02:08:26
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
const int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
const int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
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
            sobel_t[x + 1][y + 1] = ((int)(t / pi_4)) % 4;
            // sobel_t[x + 1][y + 1] = min(255,sobel_t[x + 1][y + 1] * 64);
            gx /= 8;
            gy /= 8;
            sobel_g[x + 1][y + 1] = min((uint8_t)255, (uint8_t)sqrt((double)gx * gx + (double)gy * gy));
        }
}
void Edge_Detection_Canny(ArrayReader image_in, ArrayReader image_out,double T_Low_ratio = 0.5,double T_High_ratio = 0.99999)
{
    ArrayBuffer gauss = ArrayBuffer(image_in.height, image_out.width);
    memset(image_out.image, 0, sizeof(uint8_t) * image_out.height * image_out.width);
    //setp-1 高斯滤镜
    Gaussian_filter(image_in, gauss);
    ArrayBuffer sobel_g = ArrayBuffer(image_in.height, image_in.width);
    sobel_g.memset(0);
    ArrayBuffer sobel_t = ArrayBuffer(image_in.height, image_in.width);
    //sobel_t
    //0: x =  1, y = 0
    //1: x =  1, y = 1
    //2: x =  0, y = 1
    //3: x = -1, y = 1

    //setp-2 取梯度
    Edge_Detection_Sobel(image_in, sobel_g, sobel_t);


    //setp-3 非极大值抑制
    ArrayBuffer suppressed = ArrayBuffer(image_in.height, image_in.width);
    suppressed.memset(0);
    for (int x = 1; x + 1 < image_in.height; x++)
        for (int y = 1; y + 1 < image_in.width; y++ )
        {
            int d = sobel_t[x][y];
            if (sobel_g[x][y] > sobel_g[x + dx[d]][y + dy[d]] &&
                    sobel_g[x][y] > sobel_g[x - dx[d]][y - dy[d]])
                suppressed[x][y] = sobel_g[x][y];
        }

    //自适应双阈值增强(基于直方图)
    int histogram[256];
    memset(histogram, 0, sizeof(histogram));
    for (int x = 0; x < image_in.height; x++)
        for (int y = 0; y < image_in.width; y++ )
            histogram[suppressed[x][y]] ++;
    int all_histogram = image_in.height * image_in.width - histogram[0]; //不计入

    uint8_t T_Low = 0;
    uint8_t T_High = 0;
    int sum_histogram = 0;
    for (int i = 1; i < 256; i++)
    {
        sum_histogram += histogram[i];
        if (T_Low == 00 && 1.0 * sum_histogram / all_histogram > T_Low_ratio)
            T_Low = i;
        if (T_High == 0 && 1.0 * sum_histogram / all_histogram >= T_High_ratio)
            T_High = i;
        // printf("i=%4d,ratio=%lf\n", i, 1.0 * sum_histogram / all_histogram);
    }
    printf("T_Low=%4d, T_High=%4d\n", T_Low, T_High);
    uint8_t T_diff = T_High - T_Low;
    for (int x = 1; x + 1 < image_in.height; x++)
        for (int y = 1; y + 1 < image_in.width; y++ )
            if (suppressed[x][y] >= T_Low && suppressed[x][y] <= T_High)
                image_out[x][y] = min((uint8_t)255, (uint8_t)((suppressed[x][y] - T_Low) / (double)T_diff * 255));
    // memcpy(image_out.image, suppressed.image, sizeof(uint8_t)*image_in.height * image_in.width);

}
int main(int argc, char const *argv[])
{
    char filename_in[256] = "Edge_Detection_in.bmp";
    char filename_out[256] = "Edge_Detection_out.bmp";
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
    Edge_Detection_Canny(image_in, image_out,0.5,1.0);

    //output
    fseek(file_out, 0L, 0);
    fseek(file_in, 0L, 0);
    uint8_t *header = new uint8_t[bmp.bfOffBit];
    fread(header, bmp.bfOffBit, 1, file_in);
    fwrite(header, bmp.bfOffBit, 1, file_out);
    fwrite(image_out.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_out);
    return 0;
}
