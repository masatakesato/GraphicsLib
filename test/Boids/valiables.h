#include"Fish.h"

/*********************** Fish Definition ************************/
int Num =100;
struct Fish fish[100]={0};
struct Fish *f=fish;
/********************** Object Definition ***********************/
int NumObj =6;

struct Object object[6]={//x,y,z,GravityField,Distance_Limit,Dead_Line,k 
	{-20, 0.0, 4.0, 10, 3.5, 2, 1.0},
    { 17, 0.0, 20, 12.5, 4.5, 2, 1.2},
    { 20, 0.0,-16, 9, 4, 2, 1.8},
	{ 3.0, 0.0, -2.0, 8.5, 3.5, 2, 30},
	{ -5.0, 0.0, -20.0, 10, 2.5, 2, 2.6},
	{ -10.0, 0.0, 20.0, 8, 3, 2, 2.0},
};
struct Object *o=object;
/******************* Fish Destination Definition *****************/
int NumFD =4;

struct FishDest fishdest[4]={//x,y,z,k,c,Cx,Cy,Cz
    {0,0,0,  2.5,1.5,   0, 0, 0},
    {0,0,0,  2.75,0.8,  -3, 15, 0},
	{0,0,0,  3.0,1.2,  1, 2, 0},
    {0,0,0,  5.2,1.5,  16,5,0},
};
struct FishDest *fd=fishdest;
/********************** Distance Limit Definition ***************/

double Height_Limit=10.5;
double Distance_Limit=5.5;//for fish
double InGroundK=15.0;
double FishK=25.0;
/*****************************************************************/
/*********************** Other Control Variables ***************************/
double a=50;
double b=70;
double scale=85;
double interval=0.03;
int i=0;
bool AutoCamera=false;
bool pause=false;
