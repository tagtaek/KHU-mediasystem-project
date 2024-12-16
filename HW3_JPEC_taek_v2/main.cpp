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
#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <fstream>

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

// 강의자료에 제시된 Quantization Table (8x8)
int quantizationTable[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
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

// 8x8 macroblock으로 나누는 함수
void DivideIntoBlocks(bmp_byte* pixels, int32 width, int32 height, int32 bytesPerPixel, std::vector<std::vector<bmp_byte> >& blocks) {
    int blockWidth = width / 8;
    int blockHeight = height / 8;

    for (int blockRow = 0; blockRow < blockHeight; ++blockRow) {
        for (int blockCol = 0; blockCol < blockWidth; ++blockCol) {
            std::vector<bmp_byte> block(64); // 8x8 block

            for (int y = 0; y < 8; ++y) {
                for (int x = 0; x < 8; ++x) {
                    int pixelRow = blockRow * 8 + y;
                    int pixelCol = blockCol * 8 + x;
                    int pixelIndex = (pixelRow * width + pixelCol) * bytesPerPixel;

                    block[y * 8 + x] = pixels[pixelIndex]; // 1차원 배열에서 픽셀 값 가져오기
                }
            }

            blocks.push_back(block);

             // 로그 출력: 블록 번호와 블록 데이터
            // std::cout << "Block (" << blockRow << ", " << blockCol << "):" << std::endl;
            // for (int i = 0; i < 8; ++i) {
            //     for (int j = 0; j < 8; ++j) {
            //         std::cout << (int)block[i * 8 + j] << " "; // 픽셀 값을 출력
            //     }
            //     std::cout << std::endl;
            // }
            // std::cout << "----------------------" << std::endl;
        }
    }
    // 전체 블록 개수 출력
    std::cout << "Total Blocks: " << blocks.size() << std::endl;
}

// DCT를 수행하는 함수
void PerformDCT(const std::vector<bmp_byte>& block, double dctBlock[8][8]) {
    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            double sum = 0.0;

            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    double pixel = block[y * 8 + x];
                    sum += pixel *
                           cos((2 * x + 1) * u * M_PI / 16) *
                           cos((2 * y + 1) * v * M_PI / 16);
                }
            }

            double Cu = (u == 0) ? 1 / sqrt(2.0) : 1.0;
            double Cv = (v == 0) ? 1 / sqrt(2.0) : 1.0;
            dctBlock[u][v] = 0.25 * Cu * Cv * sum; // DCT 변환 결과
        }
    }
}

// DCT 수행 결과를 확인하기 위한 DCTBlock print function
void PrintDCTBlock(const double dctBlock[8][8]) {
    std::cout << "DCT Result:\n";
    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            std::cout << dctBlock[u][v] << " ";
        }
        std::cout << "\n";
    }
}

// 양자화 함수
void PerformQuantization(double dctBlock[8][8], int quantizedBlock[8][8]) {
    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            quantizedBlock[u][v] = round(dctBlock[u][v] / quantizationTable[u][v]);
        }
    }
}

// 양자화 결과 출력
void PrintQuantizedBlock(int quantizedBlock[8][8]) {
    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            std::cout << quantizedBlock[u][v] << " ";
        }
        std::cout << std::endl;
    }
}

// zigzagscan을 수행하는 함수
void ZigzagScan(int quantizedBlock[8][8], std::vector<int>& zigzagArray) {
    const int zigzagOrder[8][8] = {
        {0, 1, 5, 6, 14, 15, 27, 28},
        {2, 4, 7, 13, 16, 26, 29, 42},
        {3, 8, 12, 17, 25, 30, 41, 43},
        {9, 11, 18, 24, 31, 40, 44, 53},
        {10, 19, 23, 32, 39, 45, 52, 54},
        {20, 22, 33, 38, 46, 51, 55, 60},
        {21, 34, 37, 47, 50, 56, 59, 61},
        {35, 36, 48, 49, 57, 58, 62, 63}
    };

    zigzagArray.resize(64);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            zigzagArray[zigzagOrder[i][j]] = quantizedBlock[i][j];
        }
    }
}

// zigzagScan 결과 확인용 함수
void PrintZigzagArray(const std::vector<int>& zigzagArray) {
    for (size_t i = 0; i < zigzagArray.size(); ++i) {
        std::cout << zigzagArray[i] << " ";
        if ((i + 1) % 8 == 0) std::cout << "\n"; // 8개 단위로 줄바꿈
    }
}

// DPCM 수행 함수
void PerformDPCM(const std::vector<int>& dcValues, std::vector<int>& dpcmValues) {
    if (dcValues.empty()) return;

    dpcmValues.resize(dcValues.size());
    dpcmValues[0] = dcValues[0]; // 첫 번째 DC는 그대로 유지

    for (size_t i = 1; i < dcValues.size(); ++i) {
        dpcmValues[i] = dcValues[i] - dcValues[i - 1];
    }
}

// DPCM 수행 후 DC 값 확인용 출력 함수
void PrintDCValues(const std::vector<int>& values) {
    for (size_t i = 0; i < values.size(); ++i) {
        std::cout << values[i] << " ";
    }
    std::cout << "\n";
}

// DPCM 값의 빈도를 계산하는 함수
void CalculateDCFrequency(const std::vector<int>& dpcmValues, std::map<int, int>& frequencyMap) {
    for (int value : dpcmValues) {
        frequencyMap[value]++;
    }
}

// RLC 수행 함수
void PerformRLC(const std::vector<int>& acValues, std::vector<std::pair<int, int> >& rlcResults) {
    int skipCount = 0;

    for (size_t i = 1; i < acValues.size(); ++i) { // DC를 제외하고 처리
        if (acValues[i] == 0) {
            skipCount++;
            if (skipCount == 15) { // 최대 15개의 연속된 0
                rlcResults.push_back(std::make_pair(15, 0));
                skipCount = 0;
            }
        } else {
            rlcResults.push_back(std::make_pair(skipCount, acValues[i]));
            skipCount = 0;
        }
    }

    // 블록의 끝을 (0, 0)으로 마무리
    rlcResults.push_back(std::make_pair(0, 0));
}

// RLC 결과 출력 함수
void PrintRLCResults(const std::vector<std::pair<int, int> >& rlcResults) {
    for (const std::pair<int, int>& pair : rlcResults) {
        std::cout << "(" << pair.first << ", " << pair.second << ") ";
    }
    std::cout << "\n";
}

// AC 빈도 계산
void CalculateACFrequency(const std::vector<std::pair<int, int>>& rlcResults, std::map<std::pair<int, int>, int>& acFrequencyMap) {
    for (const auto& pair : rlcResults) {
        acFrequencyMap[pair]++;
    }
}

// @@@@ 압축된 데이터 쓰기 @@@@
// Quantization Table 저장
void WriteQuantizationTable(std::ofstream& outFile, const int quantizationTable[8][8]) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            outFile.write(reinterpret_cast<const char*>(&quantizationTable[i][j]), sizeof(int));
        }
    }
}

// 허프만 테이블 저장 (DC 및 AC 모두 지원)
template<typename KeyType>
void WriteHuffmanTable(std::ofstream& outFile, const std::map<KeyType, std::string>& huffmanTable) {
    size_t size = huffmanTable.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size)); // 테이블 크기 저장
    
    for (const auto& entry : huffmanTable) {
        outFile.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first)); // 심볼 또는 (Run/Size)
        size_t codeLength = entry.second.size();
        outFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));   // 코드 길이
        outFile.write(entry.second.c_str(), codeLength); // 허프만 코드
    }
}

// 압축 데이터 저장
void WriteCompressedData(std::ofstream& outFile, const std::string& compressedData) {
    size_t length = compressedData.size();
    outFile.write(reinterpret_cast<const char*>(&length), sizeof(length)); // 데이터 길이
    outFile.write(compressedData.c_str(), length); // 데이터 저장
}

// 압축률률 계산 및 비교 함수
void CompareFileSize(const std::string& originalFile, const std::string& compressedFile) {
    std::ifstream original(originalFile, std::ios::binary | std::ios::ate);
    std::ifstream compressed(compressedFile, std::ios::binary | std::ios::ate);

    if (original.is_open() && compressed.is_open()) {
        size_t originalSize = original.tellg();
        size_t compressedSize = compressed.tellg();
        double compressionRatio = (double)compressedSize / originalSize * 100;

        std::cout << "Original File Size: " << originalSize << " bytes" << std::endl;
        std::cout << "Compressed File Size: " << compressedSize << " bytes" << std::endl;
        std::cout << "Compression Ratio: " << compressionRatio << " %" << std::endl;

        original.close();
        compressed.close();
    } else {
        std::cerr << "Error opening files for comparison!" << std::endl;
    }
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
    // WriteImage(pixels, width, height, bytesPerPixel, outputFile); // 이미지 그냥 다시 write

    std::vector<std::vector<bmp_byte> > blocks;
    DivideIntoBlocks(pixels, width, height, bytesPerPixel, blocks);

    delete[] pixels;

    std::vector<int> dcValues; // DC값 저장
    std::vector<std::pair<int, int> > rlcResults; // RLC 결과 저장

    // huffman 수행을 위한 빈도 맵
    std::map<int, int> DCfrequencyMap;
    std::map<std::pair<int, int>, int> acFrequencyMap;

    // 각 블록에 대해 DCT 수행
    for (size_t i = 0; i < blocks.size(); ++i) {
        double dctBlock[8][8] = {0};
        PerformDCT(blocks[i], dctBlock);

        // DCT 결과 출력 (옵션)
        // std::cout << "Block " << i << " DCT Result:\n";
        // PrintDCTBlock(dctBlock);
        // std::cout << "----------------------\n";

        // 양자화 결과 저장
        int quantizedBlock[8][8] = {0};
        PerformQuantization(dctBlock, quantizedBlock);

        // 양자화 결과 출력
        // std::cout << "Block " << i << " Quantized Result:\n";
        // PrintQuantizedBlock(quantizedBlock);
        // std::cout << "----------------------\n";

        std::vector<int> zigzagArray;
        ZigzagScan(quantizedBlock, zigzagArray);

        // zigzagScan 결과 출력
        // std::cout << "Block " << i << " Zigzag Scanned Result:\n";
        // PrintZigzagArray(zigzagArray);
        // std::cout << "----------------------\n";

         // DC 값 추출 (zigzagArray의 첫 번째 값)
        if (!zigzagArray.empty()) {
        dcValues.push_back(zigzagArray[0]);
        }

        // AC 값만 추출 (DC 제외)
        std::vector<int> acValues(zigzagArray.begin() + 1, zigzagArray.end());

        // RLC 수행
        rlcResults.clear(); // 초기화
        PerformRLC(acValues, rlcResults);
        CalculateACFrequency(rlcResults, acFrequencyMap);

        // std::cout << "\nAC Huffman Codes:\n";
        // getHuffmanCode(acFrequency);
        // RLC 결과 출력
        // std::cout << "Block " << i << " RLC Result:\n";
        // PrintRLCResults(rlcResults);
        // std::cout << "----------------------\n";
    }
    
    // DPCM 수행
    std::vector<int> dpcmValues;
    PerformDPCM(dcValues, dpcmValues);
    CalculateDCFrequency(dpcmValues, DCfrequencyMap);

    // DPCM 결과 출력
    // std::cout << "DPCM Result:\n";
    // PrintDCValues(dpcmValues);
    // std::cout << "----------------------\n";

    // 8x8 블록 테스트 데이터 (임의의 예제 데이터)
    // std::vector<bmp_byte> testBlock = {
    //     52, 55, 61, 66, 70, 61, 64, 73,
    //     63, 59, 55, 90, 109, 85, 69, 72,
    //     62, 59, 68, 113, 144, 104, 66, 73,
    //     63, 58, 71, 122, 154, 106, 70, 69,
    //     67, 61, 68, 104, 126, 88, 68, 70,
    //     79, 65, 60, 70, 77, 68, 58, 75,
    //     85, 71, 64, 59, 55, 61, 65, 83,
    //     87, 79, 69, 68, 65, 76, 78, 94
    // };

    // double dctBlock[8][8] = {0};
    // PerformDCT(testBlock, dctBlock);
    // PrintDCTBlock(dctBlock); // 결과 출력

    std::map<int, std::string> dcHuffmanTable; // DC Huffman Table
    std::map<std::pair<int, int>, std::string> acHuffmanTable; // AC Huffman Table
    std::string compressedDCData, compressedACData; // 압축된 데이터

    // DC 값 허프만 코드 생성
    std::cout << "\nDC Huffman Codes:\n";
    getHuffmanCode(DCfrequencyMap, dcHuffmanTable);

    // AC 값 허프만 코드 생성
    std::cout << "AC Huffman Codes:\n";
    getHuffmanCodeForAC(acFrequencyMap, acHuffmanTable);

    // DC 및 AC 데이터 압축 코드 (생략) 후 결과 저장
    std::ofstream outFile("compressed_Lena.dat", std::ios::binary);
    if (outFile.is_open()) {
        WriteQuantizationTable(outFile, quantizationTable);
        WriteHuffmanTable(outFile, dcHuffmanTable);
        WriteHuffmanTable(outFile, acHuffmanTable);
        WriteCompressedData(outFile, compressedDCData);
        WriteCompressedData(outFile, compressedACData);
        outFile.close();
        std::cout << "Compression completed successfully and saved to 'compressed_image.dat'." << std::endl;
    } else {
        std::cerr << "Error opening file for writing!" << std::endl;
    }

    return 0;
}

// 살행 명령어 : 
// g++ -std=c++11 main.cpp -o main && ./main