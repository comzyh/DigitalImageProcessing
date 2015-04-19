/*
* @Author: Comzyh
* @Date:   2015-04-19 20:24:41
* @Last Modified by:   Comzyh
* @Last Modified time: 2015-04-19 22:47:19
*/

#include <iostream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "bmp.h"
using namespace std;
const int Laplacian[3][3] =
{
    { -1, -1, -1},
    { -1,  8, -1},
    { -1, -1, -1}
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
void Edge_Detection_Laplacian(ArrayReader image_in, ArrayReader image_out)
{
    memset(image_out.image, 0, sizeof(uint8_t) * image_out.height * image_out.width);
    for (int x = 0; x + 3 < image_in.height; x++)
        for (int y = 0; y + 3 < image_in.width; y++ )
        {
            int ans = 0;
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    ans += Laplacian[i][j] * image_in[x + i][y + j];
            ans = min(255, abs(ans));
            image_out[x + 1][y + 1] = ans;
        }

}
int main(int argc, char const *argv[])
{
    char filename_in[256] = "Edge_Detection_in.bmp";
    char filename_out[256] = "Edge_Detection_Laplacian_out.bmp";
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
    Edge_Detection_Laplacian(image_in, image_out);

    //output
    fseek(file_out, 0L, 0);
    fseek(file_in, 0L, 0);
    uint8_t *header = new uint8_t[bmp.bfOffBit];
    fread(header, bmp.bfOffBit, 1, file_in);
    fwrite(header, bmp.bfOffBit, 1, file_out);
    fwrite(image_out.image, bmpinfo.biHeight * bmpinfo.biWidth * bmpinfo.biBitCount / 8, 1, file_out);
    return 0;
}
