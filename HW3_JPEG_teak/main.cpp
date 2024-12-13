//#include "huffman.h"
#include "header/huffman.h"
#include "header/bmp.h"

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

	// getHuffmanCode(); 
	// get_adaptive_huffman_code();

	return 0;
}