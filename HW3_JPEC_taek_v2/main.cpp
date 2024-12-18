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
#include <bitset>


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

//-------------------DC---------------------

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
    int count = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        std::cout << values[i] << " ";
        count++;
    }
    std::cout << "\n";
    cout << count;
}

// DPCM 값의 사이즈별 빈도를 계산하는 함수
void CalculateDCFrequency(const std::vector<int>& dpcmValues, std::map<int, int>& frequencyMap) {
    for (int value : dpcmValues) {
        frequencyMap[value]++;
    }
}
// dpcm된 값들의 size 계산
int CalculateSize(int value) {
    if (value == 0) return 0; // DPCM 값이 0일 경우 SIZE도 0
    return static_cast<int>(std::log2(std::abs(value)) + 1);
}

// // dpcm을 sigend 이진수로 변환
// std::string EncodeAmplitude(int value, int size) {
//     if (value == 0) return ""; // 0일 경우 처리 필요 없음

//     std::string result;

//     if (value > 0) {
//         // 양수 값: 절댓값을 그대로 이진수 변환
//         result = std::bitset<32>(value).to_string().substr(32 - size);
//     } else {
//         // 음수 값: 절댓값을 1의 보수로 변환
//         int absValue = -value;
//         int inverted = (1 << size) - 1 - absValue; // 음수는 크기만큼 1의 보수를 적용
//         result = std::bitset<32>(inverted).to_string().substr(32 - size);
//     }

//     return result;
// }
//2
std::string EncodeAmplitude(int value, int size) {
    // 1. 절댓값 계산
    int absoluteValue = std::abs(value);

    // 2. 절댓값을 이진수 문자열로 변환
    std::string binaryRepresentation = std::bitset<32>(absoluteValue).to_string(); // 32비트로 변환

    // 필요한 비트만 추출 (size 길이만큼)
    binaryRepresentation = binaryRepresentation.substr(32 - size);

    // 3. 음수인 경우 1의 보수로 반전
    if (value < 0) {
        for (char& bit : binaryRepresentation) {
            bit = (bit == '0') ? '1' : '0'; // 비트를 반전
        }
    }

    return binaryRepresentation;
}

std::string EncodeDPCMValues(const std::vector<int>& dpcmValues, const std::map<int, std::string>& dcHuffmanTable) {
    std::string encodedData; // 결과 문자열을 저장할 변수

    for (const int value : dpcmValues) {
        // 1. SIZE 계산
        int size = CalculateSize(value);

        // 2. SIZE를 허프만 코딩
        auto huffmanCodeIt = dcHuffmanTable.find(size);
        if (huffmanCodeIt == dcHuffmanTable.end()) {
            std::cerr << "Error: Huffman code for size " << size << " not found!" << std::endl;
            continue;
        }
        std::string huffmanCode = huffmanCodeIt->second;

        // 3. DPCM 값을 크기만큼 이진화
        std::string amplitudeBits = EncodeAmplitude(value, size);

        //  // 디버깅용 출력문 추가
        // std::cout << "DPCM Value: " << value 
        //           << ", SIZE: " << size 
        //           << ", Huffman Code: " << huffmanCode 
        //           << ", Amplitude Bits: " << amplitudeBits 
        //           << std::endl;

        // 4. 허프만 코드 + 이진화된 값을 결합
        encodedData += huffmanCode + amplitudeBits;
    }

    return encodedData;
}


//--------------------------AC-----------------------------

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
    int count = 0;
    for (const std::pair<int, int>& pair : rlcResults) {
        std::cout << "(" << pair.first << ", " << pair.second << ") ";
        count++;
    }
    std::cout << "\n";
    cout << count;
}

// AC 빈도 계산
void CalculateACFrequency(const std::vector<std::pair<int, int>>& rlcResults, std::map<std::string, int>& acFrequency) {
    for (const auto& pair : rlcResults) {
        int skip = pair.first;
        int value = pair.second;
        int size = (value == 0) ? 0 : (int)log2(abs(value)) + 1;

        std::string key = std::to_string(skip) + "/" + std::to_string(size);
        acFrequency[key]++;
    }
}

std::string EncodeACValues(const std::vector<std::pair<int, int>>& rlcResults, const std::map<std::string, std::string>& acHuffmanTable) {
    std::string encodedData; // 결과 문자열 저장

    for (const auto& pair : rlcResults) {
        int skip = pair.first;    // RLC에서 skip 값
        int value = pair.second;  // RLC에서 값
        int size = (value == 0) ? 0 : (int)log2(abs(value)) + 1;

        // 1. 허프만 키 생성 ("skip/size")
        std::string key = std::to_string(skip) + "/" + std::to_string(size);

        // 2. 허프만 코드 가져오기
        auto huffmanCodeIt = acHuffmanTable.find(key);
        if (huffmanCodeIt == acHuffmanTable.end()) {
            std::cerr << "Error: Huffman code for key " << key << " not found!" << std::endl;
            continue;
        }
        std::string huffmanCode = huffmanCodeIt->second;

        // 3. Value를 크기(size)만큼 이진화
        std::string amplitudeBits = EncodeAmplitude(value, size);

        // 4. 허프만 코드 + 이진화된 값을 결합
        encodedData += huffmanCode + amplitudeBits;
    }

    return encodedData;
}

// ---------------Write data-------------------------------

// Quantization Table 저장
void WriteQuantizationTable(std::ofstream& outFile, const int quantizationTable[8][8]) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            outFile.write(reinterpret_cast<const char*>(&quantizationTable[i][j]), sizeof(int));
        }
    }
}

void WriteHuffmanTable(std::ofstream& outFile, const std::map<int, std::string>& huffmanTable) {
    size_t size = huffmanTable.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size)); // 테이블 크기 저장
    
    for (const auto& entry : huffmanTable) {
        outFile.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first)); // 심볼 (int)
        size_t codeLength = entry.second.size();
        outFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));   // 코드 길이
        outFile.write(entry.second.c_str(), codeLength); // 허프만 코드
    }
}

void WriteCompressedData(std::ofstream& outFile, const std::string& compressedData) {
    size_t bitLength = compressedData.size(); //DC또는 AC비트스트림의 전체비트길이를 먼저 저장!!!
    outFile.write(reinterpret_cast<const char*>(&bitLength), sizeof(bitLength)); // 전체 비트 길이 저장

    // 비트 데이터를 바이트 단위로 변환하여 저장
    for (size_t i = 0; i < bitLength; i += 8) {
        std::string byteString = compressedData.substr(i, 8); // 8비트씩 잘라서 처리
        while (byteString.size() < 8) byteString += "0";     // 마지막 바이트 패딩
        unsigned char byte = std::bitset<8>(byteString).to_ulong(); // 비트셋을 바이트로 변환
        outFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }
}

void WriteHuffmanTable(std::ofstream& outFile, const std::map<std::string, std::string>& huffmanTable) {
    size_t size = huffmanTable.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size)); // 테이블 크기 저장
    
    for (const auto& entry : huffmanTable) {
        size_t keyLength = entry.first.size();
        outFile.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength)); // 키 길이
        outFile.write(entry.first.c_str(), keyLength); // 심볼 (string)
        
        size_t codeLength = entry.second.size();
        outFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength)); // 코드 길이
        outFile.write(entry.second.c_str(), codeLength); // 허프만 코드
    }
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
    std::map<std::string, int> acFrequency; // AC 빈도 맵 생성


    // 각 블록에 대해 수행
    for (size_t i = 0; i < blocks.size(); ++i) {
        
        double dctBlock[8][8] = {0};
        // DCT 수행
        PerformDCT(blocks[i], dctBlock);

        // //DCT 결과 출력 (옵션)
        // std::cout << "Block " << i << " DCT Result:\n";
        // PrintDCTBlock(dctBlock);
        // std::cout << "----------------------\n";

        // 양자화 결과 저장
        int quantizedBlock[8][8] = {0};
        PerformQuantization(dctBlock, quantizedBlock);

        // //양자화 결과 출력
        // std::cout << "Block " << i << " Quantized Result:\n";
        // PrintQuantizedBlock(quantizedBlock);
        // std::cout << "----------------------\n";

        std::vector<int> zigzagArray;
        zigzagArray.clear();
        ZigzagScan(quantizedBlock, zigzagArray);

        // //zigzagScan 결과 출력
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
        std::vector<std::pair<int, int>> blockRLCResults; // 각 블록의 RLC 결과
        PerformRLC(acValues, blockRLCResults);

        // // RLC 결과 출력
        // std::cout << "Block " << i << " RLC Result:\n";
        // PrintRLCResults(blockRLCResults);
        // std::cout << "----------------------\n";

        // RLC 결과를 전체 AC 데이터에 합침
        rlcResults.insert(rlcResults.end(), blockRLCResults.begin(), blockRLCResults.end());
    }
    // //dc발류 출력
    // for (size_t i = 0; i < dcValues.size(); ++i) {
    //    std::cout << " DC[" << i << "]: " << dcValues[i] ;
    // }

    // DPCM 수행
    std::vector<int> dpcmValues;
    PerformDPCM(dcValues, dpcmValues);

    // //dpcm발류 출력
    // for (size_t i = 0; i < dpcmValues.size(); ++i) {
    //    std::cout << " DPCM[" << i << "]: " << dpcmValues[i] ;
    // }


//     for (size_t i = 0; i < dpcmValues.size(); ++i) {
//     std::cout << "DPCM[" << i << "]: " << dpcmValues[i] << std::endl;
// }
    // //DPCM 결과 출력 ---------정상!!!!
    // std::cout << "DPCM Result:\n";
    // PrintDCValues(dpcmValues);
    // std::cout << "----------------------\n";

    // std::string compressedDCData; // 최종 압축 데이터를 저장할 문자열
    std::map<int, int> frequencyMap;
    for (const int value : dpcmValues) {
        // 1. DPCM 값의 SIZE 계산
        int size = CalculateSize(value);
        frequencyMap[size]++;
    }

    // DC허프만 테이블 생성
    std::map<int, std::string> dcHuffmanTable;
    getDCHuffmanCode(frequencyMap, dcHuffmanTable);
    // std::cout << "\nGenerated Huffman Table for DC:\n";
    // for (const auto& entry : dcHuffmanTable) {
    // std::cout << "Size: " << entry.first << " -> Code: " << entry.second << std::endl;

    // 압축된 DC데이터
    std::string compressedDCData = EncodeDPCMValues(dpcmValues, dcHuffmanTable);
    // 결과 출력
    // std::cout << "Compressed DC Data (Huffman Encoded):" << std::endl;
    // std::cout << compressedDCData << std::endl;

    // AC 빈도 계산 및 허프만 테이블 생성
    std::map<std::string, int> acFrequencyMap;
    CalculateACFrequency(rlcResults, acFrequencyMap);
    std::map<std::string, std::string> acHuffmanTable;
    getACHuffmanCode(acFrequencyMap, acHuffmanTable);

    // AC 데이터 엔트로피 코딩
    std::string compressedACData = EncodeACValues(rlcResults, acHuffmanTable);
    // // 결과 출력
    // std::cout << "Compressed AC Data (Huffman Encoded):" << std::endl;
    // std::cout << compressedACData << std::endl;

    // 압축된 데이터 저장
    std::ofstream compressedFile("compressed_Lena.dat", std::ios::binary);
    if (compressedFile.is_open()) {
        WriteQuantizationTable(compressedFile, quantizationTable); // 양자화 테이블
        WriteHuffmanTable(compressedFile, dcHuffmanTable);        // DC 허프만 테이블
        WriteHuffmanTable(compressedFile, acHuffmanTable);        // AC 허프만 테이블
        WriteCompressedData(compressedFile, compressedDCData);   // DC 데이터
        WriteCompressedData(compressedFile, compressedACData);   // AC 데이터
        compressedFile.close();
        std::cout << "Compressed data saved to 'compressed_data.dat'." << std::endl;
    } else {
        std::cerr << "Error: Unable to save compressed data!" << std::endl;
    }

    return 0;
}

// 살행 명령어 : 
// g++ -std=c++11 main.cpp -o main && ./main