#ifndef BMP_READER_H
#define _CRT_SECURE_NO_WARNINGS //fopen ���� �ذ�
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0


#include<iostream>
#include <stdio.h> // FILE*, fseek, fread �� ���


typedef unsigned int int32;
typedef short int16;
typedef unsigned char bmp_byte;

struct BMPFileHeader {
    int16 bfType;
    int32 bfSize;
    int16 bfReserved1;
    int16 bfReserved2;
    int32 bfOffBits;
};

struct BMPBitMapInfoHeader {
    int32 biSize;
    int32 biWidth;
    int32 biHeight;
    int16 biPlanes;
    int16 biBitCount;
    int32 biCompression;
    int32 biSizeImage;
    int32 biXPelsPerMeter;
    int32 biYPelsPerMeter;
    int32 biClrUsed;
    int32 biClrImportant;
};

void parse_header(FILE* imageFile);
void ReadImage(const char* fileName, const char* outfileName, bmp_byte** pixels, int32* width, int32* height, int32* bytesPerPixel, FILE*& imageFile, FILE*& OUT); // (�����̸�, �ȼ��� ���� �迭, ���α��� ���� ���� ��ġ, ���α��� ���� ���� ��ġ, �ȼ��� ����Ʈ �� ���� ��ġ;
void WriteImage(bmp_byte* pixels, int32 width, int32 height, int32 bytesPerPixel, FILE*& outputFile);


#endif // BMP_READER_H
// int main()
// {
//     byte* pixels;
//     int32 width;
//     int32 height;
//     int32 bytesPerPixel;
//     FILE* imageFile; //���� ������ 
//     FILE* outputFile;
//     ReadImage("Lion.bmp", "Lion_out.bmp", &pixels, &width, &height, &bytesPerPixel, imageFile, outputFile);
//     WriteImage(pixels, width, height, bytesPerPixel, outputFile);
//     delete[] pixels;

//     return 0;
// }