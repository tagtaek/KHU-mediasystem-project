#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <bitset>
#include <string>
#include <algorithm>

typedef unsigned char bmp_byte;
typedef unsigned int int32;
typedef short int16;

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

void ReadHuffmanTable(std::ifstream& file, std::map<int, std::string>& table) {
    size_t tableSize;
    file.read(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));
    for (size_t i = 0; i < tableSize; ++i) {
        int key;
        size_t codeLength;
        file.read(reinterpret_cast<char*>(&key), sizeof(key));
        file.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
        std::string code(codeLength, '0');
        file.read(&code[0], codeLength);
        table[key] = code;
    }
}

void ReadACHuffmanTable(std::ifstream& file, std::map<std::string, std::string>& huffmanTable) {
    size_t tableSize;
    file.read(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));

    for (size_t i = 0; i < tableSize; ++i) {
        size_t keyLength, codeLength;
        file.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength)); // 키 길이
        std::string key(keyLength, '0');
        file.read(&key[0], keyLength); // 키 읽기

        file.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength)); // 코드 길이
        std::string code(codeLength, '0');
        file.read(&code[0], codeLength); // 허프만 코드 읽기

        huffmanTable[key] = code; // 테이블에 추가
    }
}

void ReadCompressedData(std::ifstream& file, std::string& data) {
    size_t bitLength;
    file.read(reinterpret_cast<char*>(&bitLength), sizeof(bitLength));
    std::string bytes((bitLength + 7) / 8, '\0');
    file.read(&bytes[0], bytes.size());
    for (char byte : bytes) {
        data += std::bitset<8>(byte).to_string();
    }
    data = data.substr(0, bitLength); // 패딩 제거
}

int DecodeAmplitude(const std::string& bits, int size) {
    if (size == 0) return 0;
    int value = std::bitset<32>(bits).to_ulong();
    if (bits[0] == '0') { // 음수 값 처리
        value -= (1 << size) - 1;
    }
    return value;
}

void DecodeDPCMValues(const std::string& compressedData, const std::map<int, std::string>& dcHuffmanTable, std::vector<int>& dcValues) {
    std::map<std::string, int> reverseTable;
    for (const auto& [key, value] : dcHuffmanTable) {
        reverseTable[value] = key;
    }

    size_t index = 0;
    int previousDC = 0;

    while (index < compressedData.size()) {
        std::string huffmanCode;
        while (reverseTable.find(huffmanCode) == reverseTable.end() && index < compressedData.size()) {
            huffmanCode += compressedData[index++];
        }

        int size = reverseTable[huffmanCode];
        std::string amplitudeBits = compressedData.substr(index, size);
        index += size;

        int dpcmValue = DecodeAmplitude(amplitudeBits, size);
        previousDC += dpcmValue;
        dcValues.push_back(previousDC);
    }
}

void DecodeACValues(const std::string& compressedData, const std::map<std::string, std::string>& acHuffmanTable, std::vector<std::vector<int>>& acBlocks) {
    // TODO: Implement AC decoding similar to DC decoding
}

void PerformDequantization(int quantizedBlock[8][8], double dequantizedBlock[8][8]) {
    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            dequantizedBlock[u][v] = quantizedBlock[u][v] * quantizationTable[u][v];
        }
    }
}

void PerformIDCT(double dequantizedBlock[8][8], bmp_byte* block) {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            double sum = 0.0;
            for (int u = 0; u < 8; ++u) {
                for (int v = 0; v < 8; ++v) {
                    double Cu = (u == 0) ? 1 / sqrt(2.0) : 1.0;
                    double Cv = (v == 0) ? 1 / sqrt(2.0) : 1.0;
                    sum += Cu * Cv * dequantizedBlock[u][v] *
                           cos((2 * x + 1) * u * M_PI / 16) *
                           cos((2 * y + 1) * v * M_PI / 16);
                }
            }
            sum *= 0.25;
            block[y * 8 + x] = std::clamp(static_cast<int>(round(sum)), 0, 255);
        }
    }
}

void DecodeImage(const std::string& compressedFile, const std::string& originalFile, const std::string& outputFile) {
    std::ifstream compressed(compressedFile, std::ios::binary);
    if (!compressed.is_open()) {
        std::cerr << "Error: Unable to open compressed file!" << std::endl;
        return;
    }

    std::ifstream original(originalFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);

    if (!original.is_open() || !output.is_open()) {
        std::cerr << "Error: Unable to open original/output file!" << std::endl;
        return;
    }

    // BMP 헤더 복사
    char header[54];
    original.read(header, sizeof(header));
    original.close();
    output.write(header, sizeof(header));

    // 양자화 테이블 읽기
    int quantizationTable[8][8];
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            compressed.read(reinterpret_cast<char*>(&quantizationTable[i][j]), sizeof(int));
        }
    }

    // 허프만 테이블 읽기
    std::map<int, std::string> dcHuffmanTable;
    ReadHuffmanTable(compressed, dcHuffmanTable);

    std::map<std::string, std::string> acHuffmanTable;
    ReadACHuffmanTable(compressed, acHuffmanTable);

    // DC 및 AC 데이터 읽기
    std::string compressedDCData, compressedACData;
    ReadCompressedData(compressed, compressedDCData);
    ReadCompressedData(compressed, compressedACData);

    compressed.close();

    // DC 값 디코딩
    std::vector<int> dcValues;
    DecodeDPCMValues(compressedDCData, dcHuffmanTable, dcValues);

    // TODO: AC 값 디코딩 및 재구성

    // TODO: 복원된 데이터로 BMP 이미지를 완성
}

int main() {
    DecodeImage("compressed_Lena.dat", "HW3_Lena.bmp", "HW3_Lena_decoded2.bmp");
    return 0;
}


// g++ -std=c++17 decoder.cpp -o decoder && ./decoder