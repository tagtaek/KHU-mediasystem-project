#define _CRT_SECURE_NO_WARNINGS //fopen ���� �ذ�
#include "huffman.h"
#include <stdio.h> // FILE*, fseek, fread �� ���
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

void parse_header(FILE* imageFile) {

    BMPFileHeader fileHeader;
    fseek(imageFile, 0, SEEK_SET);
    fread(&(fileHeader.bfType), 2, 1, imageFile);
    fread(&(fileHeader.bfSize), 4, 1, imageFile);
    fread(&(fileHeader.bfReserved1), 2, 1, imageFile);
    fread(&(fileHeader.bfReserved2), 2, 1, imageFile);
    fread(&(fileHeader.bfOffBits), 4, 1, imageFile);

    BMPBitMapInfoHeader bitMapInfoHeader;
    fread(&(bitMapInfoHeader.biSize), 4, 1, imageFile);
    fread(&(bitMapInfoHeader.biWidth), 4, 1, imageFile);
    fread(&(bitMapInfoHeader.biHeight), 4, 1, imageFile);
    fread(&(bitMapInfoHeader.biPlanes), 2, 1, imageFile);
    fread(&(bitMapInfoHeader.biBitCount), 2, 1, imageFile);

    fread(&(bitMapInfoHeader.biCompression), 4, 1, imageFile);
    fread(&(bitMapInfoHeader.biSizeImage), 4, 1, imageFile);
    fread(&(bitMapInfoHeader.biXPelsPerMeter), 4, 1, imageFile);
    fread(&(bitMapInfoHeader.biYPelsPerMeter), 4, 1, imageFile);
    fread(&(bitMapInfoHeader.biClrUsed), 4, 1, imageFile);
    fread(&(bitMapInfoHeader.biClrImportant), 4, 1, imageFile);

    std::cout << "=====File Header=====" << std::endl;
    std::cout << "bfType " << (char)((fileHeader.bfType & 0xFF)) << " " << (char)((fileHeader.bfType) >> 8) << std::endl;
    std::cout << "bfSize " << fileHeader.bfSize << std::endl;
    std::cout << "bfReserved1 " << fileHeader.bfReserved1 << std::endl;
    std::cout << "bfReserved2 " << fileHeader.bfReserved2 << std::endl;
    std::cout << "bfOffBits " << fileHeader.bfOffBits << std::endl;

    std::cout << "=====Bitmap Info Header===== " << std::endl;
    std::cout << "biSize " << bitMapInfoHeader.biSize << std::endl;
    std::cout << "biWidth " << bitMapInfoHeader.biWidth << std::endl;
    std::cout << "biHeight " << bitMapInfoHeader.biHeight << std::endl;
    std::cout << "biPlanes " << bitMapInfoHeader.biPlanes << std::endl;
    std::cout << "biBitCount " << bitMapInfoHeader.biBitCount << std::endl;

    std::cout << "biCompression " << bitMapInfoHeader.biCompression << std::endl;
    std::cout << "biSizeImage " << bitMapInfoHeader.biSizeImage << std::endl;
    std::cout << "biXPelsPerMeter " << bitMapInfoHeader.biXPelsPerMeter << std::endl;
    std::cout << "biYPelsPerMeter " << bitMapInfoHeader.biYPelsPerMeter << std::endl;
    std::cout << "biClrUsed " << bitMapInfoHeader.biClrUsed << std::endl;
    std::cout << "biClrImportant " << bitMapInfoHeader.biClrImportant << std::endl;

    return;
}

void ReadImage(const char* fileName, const char* outfileName, bmp_byte** pixels, int32* width, int32* height, int32* bytesPerPixel, FILE*& imageFile, FILE*& OUT) // (�����̸�, �ȼ��� ���� �迭, ���α��� ���� ���� ��ġ, ���α��� ���� ���� ��ġ, �ȼ��� ����Ʈ �� ���� ��ġ
{

    imageFile = fopen(fileName, "rb");//������ ���̳ʸ� ���� ����

    parse_header(imageFile);

    int32 dataOffset; //������ ���� ��ġ �ּҰ� 
    int32 LookUpTable = 0;
    fseek(imageFile, HEADER_SIZE + INFO_HEADER_SIZE - 8, SEEK_SET); //fseek(���Ϻ���,�̵�byte,������ġ)
    fread(&LookUpTable, 4, 1, imageFile); //fread(�޸��ּ�,ũ��,����,���Ϻ���)
    fseek(imageFile, 0, SEEK_SET);

    OUT = fopen(outfileName, "wb");

    int header = 0;
    if (LookUpTable)
        header = HEADER_SIZE + INFO_HEADER_SIZE + 1024;
    else
        header = HEADER_SIZE + INFO_HEADER_SIZE;
    for (int i = 0; i < header; i++) // ���� BMP ���Ͽ��� ����� ���̺� �̾Ƽ� ���ο� BMP ������ ����� ����
    {
        int get = getc(imageFile);
        putc(get, OUT);
    }

    fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET); //fseek(���Ϻ���,�̵�byte,������ġ)
    fread(&dataOffset, 4, 1, imageFile); //fread(�޸��ּ�,ũ��,����,���Ϻ���)
    fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
    fread(width, 4, 1, imageFile);
    fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
    fread(height, 4, 1, imageFile);
    int16 bitsPerPixel;
    fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
    fread(&bitsPerPixel, 2, 1, imageFile);
    *bytesPerPixel = ((int32)bitsPerPixel) / 8; //3 bytes per pixel when color, 1 byte per pixel when grayscale


    int paddedRowSize = (int)(((*width * *bytesPerPixel) + 3) / 4.0f) * 4; //4�� ����� ������ִ� ����
    int unpaddedRowSize = (*width) * (*bytesPerPixel);
    int totalSize = unpaddedRowSize * (*height);

    *pixels = new bmp_byte[totalSize];
    int i = 0;
    bmp_byte* currentRowPointer = *pixels + ((*height - 1) * unpaddedRowSize);
    for (i = 0; i < *height; i++)
    {
        fseek(imageFile, dataOffset + (i * paddedRowSize), SEEK_SET);
        fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
        currentRowPointer -= unpaddedRowSize;
    }


    fclose(imageFile);

}

void WriteImage(bmp_byte* pixels, int32 width, int32 height, int32 bytesPerPixel, FILE*& outputFile)
{
    int paddedRowSize = (int)(((width * bytesPerPixel) + 3) / 4.0f) * 4;
    int unpaddedRowSize = width * bytesPerPixel;
    for (int i = 0; i < height; i++)
    {
        int pixelOffset = ((height - i) - 1) * unpaddedRowSize;
        fwrite(&pixels[pixelOffset], 1, paddedRowSize, outputFile);
    }
    fclose(outputFile);
}

int main()
{
    bmp_byte* pixels;
    int32 width;
    int32 height;
    int32 bytesPerPixel;
    FILE* imageFile; //���� ������ 
    FILE* outputFile;
    ReadImage("HW3_Lena.bmp", "HW3_Lena_out.bmp", &pixels, &width, &height, &bytesPerPixel, imageFile, outputFile);
    WriteImage(pixels, width, height, bytesPerPixel, outputFile);
    delete[] pixels;

    getHuffmanCode();

    return 0;
}
//test
//test