#ifndef BITMAP_H
#define BITMAP_H



typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;


typedef struct pixel{

	unsigned char R,G,B;

} pixel;



typedef struct Bitmap
{
   WORD    bfType;//2byte
   DWORD   bfSize;//4byte
   WORD    bfReserved1;//2byte
   WORD    bfReserved2;//2byte
   DWORD   bfOffBits;//4byte
   DWORD   biSize;//4byte
   DWORD   biWidth;//4byte
   DWORD   biHeight;//4byte
   WORD    biPlanes;//2byte
   WORD    biBitCounts; //2byte
   DWORD   biCompression;//4byte
   DWORD   biSizeImage;//4byte
   DWORD   biXPelsPerMeter;//4byte
   DWORD   biYPelsPerMeter;//4byte
   DWORD   biClrUsed;//4byte
   DWORD   biClrImportant;//4byte

   unsigned char *PixelData;

} Bitmap;



extern Bitmap *newBitmap(int width, int height);
extern void Bmp_Read(char *filename, Bitmap *bmp);
extern float *Bitmap_load_texture(char *filename, unsigned int &width, unsigned int &height);
extern void Bmp_Output(char *filename, Bitmap bmp);
extern void BmpHeaderInfo(Bitmap bmp);


/////////////////////////////////////////////////////////////////////////////////////////////////////
#define SetRGB(x,y,r,g,b,bmp)                          \
	   if(r>255)r=255; if(g>255)g=255; if(b>255)b=255; \
           bmp.ImageData[y][x].R=(int)r;               \
           bmp.ImageData[y][x].G=(int)g;               \
           bmp.ImageData[y][x].B=(int)b;





#endif /* BITMAP_H */