#include	"Bitmap.h"


#include	<iostream>
using namespace std;
#pragma warning(disable : 4996)


static FILE *fp;


static WORD SetbfType(){//bfTypeの作成

	union data{//32ビット。WORDは16ビット
		BYTE byte;
		WORD word;
		DWORD double_word;
	}Four_Bytes;

	Four_Bytes.byte='M';//'M'を下位1バイトに格納
	Four_Bytes.word=Four_Bytes.word<<8;//1バイト分上位にシフト
	Four_Bytes.byte='B';//空いた下位1バイトに'B'を格納
	return Four_Bytes.word;
}

static DWORD SetbfSize(int width, int height){ return (54+width*height*3); }

Bitmap *newBitmap(int width, int height)
{

	Bitmap *bmp = new Bitmap();

	bmp->bfType=SetbfType();
	bmp->bfSize=SetbfSize(width,height);//全ヘッダーサイズ+ピクセルデータサイズ
	////////////  not used //////////////////
	bmp->bfReserved1=0;//絶対0
	bmp->bfReserved2=0;//絶対0
	/////////////////////////////////////////
	bmp->bfOffBits=54;//ピクセルデータの始まる位置(byte)
	bmp->biSize=40;//INFORMATION_HEADERのサイズ
	bmp->biWidth=width;//絵の幅
	bmp->biHeight=height;//絵の高さ
	bmp->biPlanes=1;//絶対1
	bmp->biBitCounts=24;//カラービット数 
	////////////以下は全部0の場合もあり///////////
	bmp->biCompression=0;//非圧縮
	bmp->biSizeImage=width*height*3;//画像データ部のサイズ。ピクセル数*3byte
	bmp->biXPelsPerMeter=3780;
	bmp->biYPelsPerMeter=3780;
	bmp->biClrUsed=0;
	bmp->biClrImportant=0;

	bmp->PixelData = new unsigned char[width*height*3];

	for(int i=0; i<width*height*3; i++) *(bmp->PixelData+i) = 0;

	return bmp;
}


void Bmp_Read(char *filename, Bitmap *bmp){//Bitmapの読み込み


	BYTE R,G,B, dumy;
	int i,j,f;

	if((fp =fopen(filename, "rb"))==NULL)
	{
		cout << "Cannot open file!" << endl;
		return;
	}
	/* ヘッダー情報 */
	fread(&bmp->bfType, sizeof(WORD),1,fp );
	fread(&bmp->bfSize, sizeof(DWORD), 1, fp);
	fread(&bmp->bfReserved1, sizeof(WORD), 1, fp);
	fread(&bmp->bfReserved2, sizeof(WORD), 1, fp);
	fread(&bmp->bfOffBits, sizeof(DWORD), 1, fp); 

	fread(&bmp->biSize, sizeof(DWORD), 1, fp);
	fread(&bmp->biWidth, sizeof(DWORD), 1, fp);
	fread(&bmp->biHeight, sizeof(DWORD), 1, fp);
	fread(&bmp->biPlanes, sizeof(WORD), 1, fp);
	fread(&bmp->biBitCounts, sizeof(WORD), 1, fp);
	fread(&bmp->biCompression, sizeof(DWORD), 1, fp);
	fread(&bmp->biSizeImage, sizeof(DWORD), 1, fp);
	fread(&bmp->biXPelsPerMeter, sizeof(DWORD), 1, fp);
	fread(&bmp->biYPelsPerMeter, sizeof(DWORD), 1, fp);
	fread(&bmp->biClrUsed, sizeof(DWORD), 1, fp);
	fread(&bmp->biClrImportant, sizeof(DWORD), 1, fp);

	bmp->PixelData = new unsigned char[bmp->biWidth*bmp->biHeight *3];//RGB*ピクセル数

	//ここでBitmapのデータ部分を読み込む


	for (i=(signed)bmp->biHeight-1; i>=0; i--){//縦
		for (j=0; j<(signed)bmp->biWidth; j++) {//横

			fread(&B, 1, 1, fp);
			fread(&G, 1, 1, fp);
			fread(&R, 1, 1, fp);

			*(bmp->PixelData + (i*bmp->biWidth +j)*3   ) =R;//R
			*(bmp->PixelData + (i*bmp->biWidth +j)*3 +1) =G;//G
			*(bmp->PixelData + (i*bmp->biWidth +j)*3 +2) =B;//B

		}
		f=(bmp->biWidth*3)%4;
		if(f!=0) fread(&dumy, sizeof(unsigned char), 4-f, fp);
	}//end of i loop

	fclose(fp);
}


float *Bitmap_load_texture(char *filename, unsigned int &width, unsigned int &height)
{
	BYTE R,G,B;
	int i,j,f;
	DWORD dmy;

	//printf("Reading texture: %s...\n", filename);
	width = 0;
	height= 0;

	if((fp =fopen(filename, "rb"))==NULL)
	{
		cout << "Cannot open texture file: "<< filename << endl;	
		return NULL;
	}
	/* ヘッダー情報 */
	fread(&dmy, sizeof(WORD),1,fp );
	fread(&dmy, sizeof(DWORD), 1, fp);
	fread(&dmy, sizeof(WORD), 1, fp);
	fread(&dmy, sizeof(WORD), 1, fp);
	fread(&dmy, sizeof(DWORD), 1, fp); 

	fread(&dmy, sizeof(DWORD), 1, fp);
	fread(&width, sizeof(DWORD), 1, fp);
	fread(&height, sizeof(DWORD), 1, fp);
	fread(&dmy, sizeof(WORD), 1, fp);
	fread(&dmy, sizeof(WORD), 1, fp);
	fread(&dmy, sizeof(DWORD), 1, fp);
	fread(&dmy, sizeof(DWORD), 1, fp);
	fread(&dmy, sizeof(DWORD), 1, fp);
	fread(&dmy, sizeof(DWORD), 1, fp);
	fread(&dmy, sizeof(DWORD), 1, fp);
	fread(&dmy, sizeof(DWORD), 1, fp);

	float *data = new float[width*height*3];//RGB*ピクセル数

	//ここでBitmapのデータ部分を読み込む

	for (i=(signed)height-1; i>=0; i--){//縦
		for (j=0; j<(signed)width; j++) {//横

			fread(&B, 1, 1, fp);
			fread(&G, 1, 1, fp);
			fread(&R, 1, 1, fp);
			
			// [0,255]を[0,1]にスケーリング
			*(data + (i*width +j)*3   ) = R*0.003921568627450980392156862745098f;
			*(data + (i*width +j)*3 +1) = G*0.003921568627450980392156862745098f;
			*(data + (i*width +j)*3 +2) = B*0.003921568627450980392156862745098f;

		}
		f= (4-(width*3)%4) % 4;// 4バイトで割り切れるかどうかチェック
		if(f!=0) fread(&dmy, sizeof(unsigned char), f, fp);
	}//end of i loop

	fclose(fp);

	return data;

}


void Bmp_Output(char *filename, Bitmap bmp)
{
	BYTE dumy;
	int i, j, f;

	if ((fp = fopen(filename, "wb"))==NULL) {
		cout << "writeBmp: Open error!" << endl;
		return;
	}
	/* ヘッダー情報 */
	fwrite(&bmp.bfType, sizeof(bmp.bfType), 1, fp);
	fwrite(&bmp.bfSize, sizeof(bmp.bfSize), 1, fp);
	fwrite(&bmp.bfReserved1, sizeof(bmp.bfReserved1), 1, fp);
	fwrite(&bmp.bfReserved2, sizeof(bmp.bfReserved2), 1, fp);
	fwrite(&bmp.bfOffBits, sizeof(bmp.bfOffBits), 1, fp);

	fwrite(&bmp.biSize, sizeof(bmp.biSize), 1, fp);
	fwrite(&bmp.biWidth, sizeof(bmp.biWidth), 1, fp);
	fwrite(&bmp.biHeight, sizeof(bmp.biHeight), 1, fp);
	fwrite(&bmp.biPlanes, sizeof(bmp.biPlanes), 1, fp);
	fwrite(&bmp.biBitCounts, sizeof(bmp.biBitCounts), 1, fp);
	fwrite(&bmp.biCompression, sizeof(bmp.biCompression), 1, fp);
	fwrite(&bmp.biSizeImage, sizeof(bmp.biSizeImage), 1, fp);
	fwrite(&bmp.biXPelsPerMeter, sizeof(bmp.biXPelsPerMeter), 1, fp);
	fwrite(&bmp.biYPelsPerMeter, sizeof(bmp.biYPelsPerMeter), 1, fp);
	fwrite(&bmp.biClrUsed, sizeof(bmp.biClrUsed), 1, fp);
	fwrite(&bmp.biClrImportant, sizeof(bmp.biClrImportant), 1, fp);

	for (i=(signed)bmp.biHeight-1; i>=0; i--){//縦
		for (j=0; j<(signed)bmp.biWidth; j++) {//横

			fwrite( (bmp.PixelData + (i*bmp.biWidth+j)*3 +2), 1, 1, fp);//B
			fwrite( (bmp.PixelData + (i*bmp.biWidth+j)*3 +1), 1, 1, fp);//G
			fwrite( (bmp.PixelData + (i*bmp.biWidth+j)*3   ), 1, 1, fp);//R

		}
		f=(bmp.biWidth*3)%4;
		if(f!=0) fwrite(&dumy, sizeof(unsigned char), 4-f, fp);
	}//end of i loop
	fclose(fp);
}



void Bmp_HeaderInfo(Bitmap bmp)
{/*
	printf("BMP header information...\n\n");

	printf("bfSize %d\n", bmp.bfSize);
	printf("bfReserved1 %d\n", bmp.bfReserved1);
	printf("bfReserved2 %d\n", bmp.bfReserved2);
	printf("bfOffBits %d\n", bmp.bfOffBits);

	printf("biSize %d\n", bmp.biSize);
	printf("biWidth %d\n", bmp.biWidth);
	printf("biHeight %d\n", bmp.biHeight);
	printf("biPlanes %d\n", bmp.biPlanes);
	printf("biBitCounts %d\n", bmp.biBitCounts);
	printf("biCompression %d\n", bmp.biCompression);
	printf("biSizeImage %d\n", bmp.biSizeImage);
	printf("biXPelsPerMeter %d\n", bmp.biXPelsPerMeter);
	printf("biYPelsPerMeter %d\n", bmp.biYPelsPerMeter);
	printf("biClrUsed %d\n", bmp.biClrUsed);
	printf("biClrImportant %d\n", bmp.biClrImportant);
*/
}