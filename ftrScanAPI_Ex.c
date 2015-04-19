#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <unistd.h>

#include <time.h>

//#include <ftrScanAPI.h>
#include "ftrScanAPI.h" // modified

#define FILE_NAME_FORMAT "frame_%d.bmp"

typedef struct tagBITMAPINFOHEADER{
        unsigned long int	biSize;
        long int			biWidth;
        long int			biHeight;
        unsigned short int  biPlanes;
        unsigned short int	biBitCount;
        unsigned long int	biCompression;
        unsigned long int	biSizeImage;
        long int			biXPelsPerMeter;
        long int			biYPelsPerMeter;
        unsigned long int   biClrUsed;
        unsigned long int	biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
        unsigned char	rgbBlue;
        unsigned char	rgbGreen;
        unsigned char	rgbRed;
        unsigned char	rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO, *PBITMAPINFO;

typedef struct tagBITMAPFILEHEADER {
        unsigned short int	bfType;
        unsigned long int	bfSize;
        unsigned short int	bfReserved1;
        unsigned short int	bfReserved2;
        unsigned long int	bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

int write_bmp_file(unsigned char *pImage, int width, int height, int imgNum)
{
	BITMAPINFO *pDIBHeader;
	BITMAPFILEHEADER  bmfHeader;
	int iCyc;

	// allocate memory for a DIB header
	if( (pDIBHeader = (BITMAPINFO *)malloc( sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * 255 )) == NULL )
	{
		printf("Alloc memory failed! - Unable to write to file!!\n");
		return -1;
	}
	memset( (void *)pDIBHeader, 0, sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * 255 );
	// fill the DIB header
	pDIBHeader->bmiHeader.biSize          = sizeof( BITMAPINFOHEADER );
	pDIBHeader->bmiHeader.biWidth         = width;
	pDIBHeader->bmiHeader.biHeight        = height;
	pDIBHeader->bmiHeader.biPlanes        = 1;
	pDIBHeader->bmiHeader.biBitCount      = 8;		// 8bits gray scale bmp
	pDIBHeader->bmiHeader.biCompression   = 0;		// BI_RGB = 0;
	// initialize logical and DIB grayscale
	for( iCyc = 0; iCyc < 256; iCyc++ )
	{
		pDIBHeader->bmiColors[iCyc].rgbBlue = pDIBHeader->bmiColors[iCyc].rgbGreen = pDIBHeader->bmiColors[iCyc].rgbRed = (unsigned char)iCyc;
	}
	// set BITMAPFILEHEADER structure
	//((char *)(&bmfHeader.bfType))[0] = 'B';
	//((char *)(&bmfHeader.bfType))[1] = 'M';
	bmfHeader.bfType = 0x42 + 0x4D * 0x100;
	bmfHeader.bfSize = 14 + sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * 255 + width * height;	//sizeof( BITMAPFILEHEADER ) = 14
	bmfHeader.bfOffBits = 14 + pDIBHeader->bmiHeader.biSize + sizeof( RGBQUAD ) * 256;

	time_t t1;
	time(&t1);
	char fileName[250];
	//sprintf(fileName, FILE_NAME_FORMAT, t1);
	sprintf(fileName, FILE_NAME_FORMAT, imgNum);

	//write to file
	FILE *fp;
	fp = fopen(fileName, "wb");
	if( fp == NULL )
	{
		printf("Failed to write to file\n");
		free( pDIBHeader );
		return -1;
	}
	//fwrite( (void *)&bmfHeader, 1, sizeof(BITMAPFILEHEADER), fp );
	fwrite( (void *)&bmfHeader.bfType, sizeof(unsigned short int), 1, fp );
	fwrite( (void *)&bmfHeader.bfSize, sizeof(unsigned long int), 1, fp );
	fwrite( (void *)&bmfHeader.bfReserved1, sizeof(unsigned short int), 1, fp );
	fwrite( (void *)&bmfHeader.bfReserved2, sizeof(unsigned short int), 1, fp );
	fwrite( (void *)&bmfHeader.bfOffBits, sizeof(unsigned long int), 1, fp );
	//fwrite( (void *)pDIBHeader, 1, sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * 255, fp );
	fwrite( (void *)&pDIBHeader->bmiHeader.biSize, sizeof(unsigned long int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biWidth, sizeof(long int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biHeight, sizeof(long int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biPlanes, sizeof(unsigned short int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biBitCount, sizeof(unsigned short int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biCompression, sizeof(unsigned long int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biSizeImage, sizeof(unsigned long int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biXPelsPerMeter, sizeof(long int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biYPelsPerMeter, sizeof(long int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biClrUsed, sizeof(unsigned long int), 1, fp);
	fwrite( (void *)&pDIBHeader->bmiHeader.biClrImportant, sizeof(unsigned long int), 1, fp);
	for( iCyc=0; iCyc<256; iCyc++ )
	{
		fwrite( (void *)&pDIBHeader->bmiColors[iCyc].rgbBlue, sizeof(unsigned char), 1, fp );
		fwrite( (void *)&pDIBHeader->bmiColors[iCyc].rgbGreen, sizeof(unsigned char), 1, fp );
		fwrite( (void *)&pDIBHeader->bmiColors[iCyc].rgbRed, sizeof(unsigned char), 1, fp );
		fwrite( (void *)&pDIBHeader->bmiColors[iCyc].rgbReserved, sizeof(unsigned char), 1, fp );
	}
	//
   // copy fingerprint image
	unsigned char *cptrData;
	unsigned char *cptrDIBData;
	unsigned char *pDIBData;

	pDIBData = (unsigned char *)malloc( height * width);
	memset( (void *)pDIBData, 0, height * width );

	cptrData = pImage + (height - 1) * width;
	cptrDIBData = pDIBData;
	for( iCyc = 0; iCyc < height; iCyc++ )
	{
		memcpy( cptrDIBData, cptrData, width );
		cptrData = cptrData - width;
		cptrDIBData = cptrDIBData + width;
	}
	fwrite( (void *)pDIBData, 1, width * height, fp );
	fclose(fp);
	printf("Fingerprint image is written to file: %s.\n", fileName);
	free( pDIBData );
	free( pDIBHeader );
	return 0;
}

void PrintErrorMessage( unsigned long nErrCode )
{
	printf("Failed to obtain image. ");

	char stError[64];

    switch( nErrCode )
	{
    case 0:
        strcpy( stError, "OK" );
        break;
    case FTR_ERROR_EMPTY_FRAME:	// ERROR_EMPTY
        strcpy( stError, "- Empty frame -" );
        break;
    case FTR_ERROR_MOVABLE_FINGER:
        strcpy( stError, "- Movable finger -" );
        break;
    case FTR_ERROR_NO_FRAME:
        strcpy( stError, "- Fake finger -" );
        break;
    case FTR_ERROR_HARDWARE_INCOMPATIBLE:
        strcpy( stError, "- Incompatible hardware -" );
        break;
    case FTR_ERROR_FIRMWARE_INCOMPATIBLE:
        strcpy( stError, "- Incompatible firmware -" );
        break;
    case FTR_ERROR_INVALID_AUTHORIZATION_CODE:
        strcpy( stError, "- Invalid authorization code -" );
        break;
    default:
        sprintf( stError, "Unknown return code - %lu", nErrCode );
	}
	printf("%s\n", stError);
}

int main(int argc, char *argv[])
{
	void *hDevice;
	FTRSCAN_IMAGE_SIZE ImageSize;
	unsigned char *pBuffer;
	int i;

	hDevice = ftrScanOpenDevice();
	if( hDevice == NULL )
	{
		printf("Failed to open device!\n");
		return -1;
	}

	if( !ftrScanGetImageSize( hDevice, &ImageSize ) )
	{
		printf("Failed to get image size\n");
		ftrScanCloseDevice( hDevice );
		return -1;
	}
	else
	{
		int k = 0;
		do {
			printf("Image size is %d\n", ImageSize.nImageSize);
			pBuffer = (unsigned char *)malloc( ImageSize.nImageSize );
			printf("Please put your finger on the scanner:\n");

			while(1)
			{
				ftrScanSetDiodesStatus(hDevice, (unsigned int)100/2, 0); // green led ON, red led OFF
				if( ftrScanIsFingerPresent( hDevice, NULL ) )
					break;
				//for(i=0; i<100; i++);	//sleep
				usleep(500);
			}
			printf("Capturing fingerprint ......\n");
			while(1)
			{
				if( ftrScanGetFrame(hDevice, pBuffer, NULL) )
				{
					printf("Done!\nWriting to file......\n");
					write_bmp_file( pBuffer, ImageSize.nWidth, ImageSize.nHeight, k );
					break;
				}
				else
				{
					PrintErrorMessage( ftrScanGetLastError() );
					for(i=0; i<100; i++);
				}
			}
			free( pBuffer );

			sleep(2);
			k++;
		} while ( k < 8);
	}

        ftrScanSetDiodesStatus(hDevice, 0, 0); // green led OFF, red led OFF
	ftrScanCloseDevice( hDevice );
	return 0;
}
