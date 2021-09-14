#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include"BasicShape.h"
#define Neighbor 0.5

#define PI 3.141592//円周率

struct Fish{//ID is FishDest array's index
 double x,y,z,
	    Fx,Fy,Fz,
		M,r,g,b;
 int ID,ClockWise;
};

struct Object{
 double x,y,z,
		Gravity_Field,Distance_Limit,Dead_Line,
		k;
};

struct FishDest{
 double x,y,z,k,c,Cx,Cy,Cz;
};

void ForceToDest(Fish *fish, int Num, FishDest *fd, int NumFD){     
  double Distance, dx,dy,dz;
  int id;
 for(int i=0; i<Num; i++){
	id=(fish+i)->ID;
    dx = (fd+id)->x -(fish+i)->x;//魚から目的地までの距離x
	dy = (fd+id)->y -(fish+i)->y;//魚から目的地までの距離y
	dz = (fd+id)->z -(fish+i)->z;//魚から目的地までの距離z
	Distance =sqrt( pow(dx,2)+pow(dy,2)+pow(dz,2) );////魚から目的地までの距離の長さ

    (fish+i)->Fx +=(fd+id)->k*dx;//Distance;
    (fish+i)->Fy +=(fd+id)->k*dy;//Distance;
    (fish+i)->Fz +=(fd+id)->k*dz;//Distance;

 }

}
/******************** method to evade clush**************************/   
void InGroundEffect(Fish *fish,int Num, double Height_Limit, double k){
 
  for(int i=0; i<Num; i++){
	  if( (fish+i)->y<=Height_Limit ){
		  (fish+i)->Fy+=k*( Height_Limit-(fish+i)->y );
	   if( (fish+i)->y<=0){
		    DrawShimen((fish+i)->x,(fish+i)->y,(fish+i)->z,
				       (fish+i)->M,(fish+i)->M,(fish+i)->M,
					    0,1,1);
	   } 
	  }
  }    

}//end of method
/****************** ignoring fishes in the back ******************************/
void EvadeFront(Fish *fish, int in, int out, double k, double Distance_Limit){
  double Distance, FLength, x,y,z;//x,y,zはinを基準にしたベクトル
  Distance=sqrt( pow( (fish+out)->x-(fish+in)->x,2) 
	            +pow( (fish+out)->y-(fish+in)->y,2)
				+pow( (fish+out)->z-(fish+in)->z,2) );
    x=( (fish+out)->x-(fish+in)->x )/Distance;
    y=( (fish+out)->y-(fish+in)->y )/Distance;
	z=( (fish+out)->z-(fish+in)->z )/Distance;

  FLength=sqrt( pow((fish+in)->Fx,2)+pow((fish+in)->Fy,2)+pow((fish+in)->Fz,2) );
  if(x*(fish+in)->Fx/FLength +y*(fish+in)->Fy/FLength +z*(fish+in)->Fz/FLength >-0.5){
	   (fish+in)->Fx +=k*(fish+out)->M *((fish+in)->x-(fish+out)->x)/(Distance) *(Distance_Limit-Distance);  //#declare fish[out][3]=25*(fish[in][0]-fish[out][0])*(Distance_Limit-Distance); 
       (fish+in)->Fy +=k*(fish+out)->M *((fish+in)->y-(fish+out)->y)/(Distance) *(Distance_Limit-Distance);  //#declare fish[out][4]=25*(fish[in][1]-fish[out][1])*(Distance_Limit-Distance);
	   (fish+in)->Fz +=k*(fish+out)->M *((fish+in)->z-(fish+out)->z)/(Distance) *(Distance_Limit-Distance); //#declare fish[out][5]=25*(fish[in][2]-fish[out][2])*(Distance_Limit-Distance);
  }

  FLength=sqrt( pow((fish+out)->Fx,2)+pow((fish+out)->Fy,2)+pow((fish+out)->Fz,2) );
  if(x*(fish+out)->Fx/FLength +y*(fish+out)->Fy/FLength +z*(fish+out)->Fz/FLength <0.5){
       (fish+out)->Fx +=k*(fish+in)->M *((fish+out)->x-(fish+in)->x)/(Distance) *(Distance_Limit-Distance);  //#declare fish[out][3]=25*(fish[out][0]-fish[in][0])*(Distance_Limit-Distance); 
       (fish+out)->Fy +=k*(fish+in)->M *((fish+out)->y-(fish+in)->y)/(Distance) *(Distance_Limit-Distance);  //#declare fish[out][4]=25*(fish[out][1]-fish[in][1])*(Distance_Limit-Distance);
	   (fish+out)->Fz +=k*(fish+in)->M *((fish+out)->z-(fish+in)->z)/(Distance) *(Distance_Limit-Distance); //#declare fish[out][5]=25*(fish[out][2]-fish[in][2])*(Distance_Limit-Distance);
  }
}
/****************** method to evade collision each other************************/   
void EvasiveAction(Fish *fish, int Num, double Distance_Limit, double k){

  double Distance;//=sqrt(pow(fish[out][0]-fish[in][0],2)+pow(fish[out][1]-fish[in][1],2)+pow(fish[out][2]*fish[in][2],2)); 

  for(int out=0; out<Num-1; out++){
	  for(int in=out+1; in<Num; in++){
		  Distance=sqrt( pow( (fish+out)->x-(fish+in)->x, 2 )
			            +pow( (fish+out)->y-(fish+in)->y, 2 )
					    +pow( (fish+out)->z-(fish+in)->z, 2) );  
		  if(Distance<Distance_Limit && Distance!=0){
			  if(Distance<Neighbor){
				glColor3d(0,1,1);
			     DrawShimen((fish+out)->x,(fish+out)->y,(fish+out)->z,0.5,0.5,0.5,0,1,1);
			  }
            EvadeFront(fish,in,out,k,Distance_Limit);
		  }//end of if
	  }//end of in loop
  }//end of out loop
   
} 
/********************** change direction *************************/

void ChangeDirection(Fish *fish, int j, Object *object, int i){
   double FLength=sqrt( pow((fish+j)->Fx,2)+pow((fish+j)->Fz,2) );//force length
   double Distance =sqrt( pow((object+i)->x-(fish+j)->x,2)+pow((object+i)->z-(fish+j)->z,2) );//魚から柱中心までの距離
   double Radius =(object+i)->Distance_Limit +(object+i)->Dead_Line;//回避の目安とする柱の半径
   double TangentLen= sqrt( pow(Distance,2)-pow(Radius,2) );//魚から接線ベクトルの接点までの距離。
   double Expx=(fish+j)->x+(fish+j)->Fx*TangentLen/FLength;//魚のベクトルを接線ベクトルの長さに引き伸ばしたx
   double Expz=(fish+j)->z+(fish+j)->Fz*TangentLen/FLength;//魚のベクトルを接線ベクトルの長さに引き伸ばしたz

if( pow(Expx-(object+i)->x,2)+pow(Expz-(object+i)->z,2) <pow(Radius,2) /*|| (fish+j)->ClockWise !=0*/ ){  
   
   DrawShimen((fish+j)->x,(fish+j)->y,(fish+j)->z,(fish+j)->M*0.5,(fish+j)->M*0.5,(fish+j)->M*0.5,1,0,0);
    double vx=(fish+j)->z-(object+i)->z;//魚と柱中心を結ぶ直線の法線ベクトルx成分
    double vz=(object+i)->x-(fish+j)->x;//魚と柱中心を結ぶ直線の法線ベクトルz成分
    double VLength=Distance*Radius/TangentLen;//法線ベクトルの長さ
	double Smoothing=1.01*pow( ((object+i)->Gravity_Field-Distance)/( (object+i)->Gravity_Field-Radius ), 1);
    double scale=Distance/( (fish+j)->Fx*((object+i)->x-(fish+j)->x)/Distance+(fish+j)->Fz*((object+i)->z-(fish+j)->z)/Distance );
	
 if((fish+j)->ClockWise==0){
	 if( (fish+j)->Fx*vx+(fish+j)->Fz*vz<0 ){ (fish+j)->ClockWise=2;}
	 else{ (fish+j)->ClockWise=1;}
  } 

  if((fish+j)->ClockWise==2){ vx=-vx; vz=-vz;}

	(fish+j)->Fx*=scale; (fish+j)->Fz*=scale;
    vx=vx/Distance*VLength;//vxの長さを調整 ok
    vz=vz/Distance*VLength;//vzの長さを調整 ok
	 
	 vx=( (object+i)->x+vx)-( (fish+j)->x+(fish+j)->Fx );
	 vz=( (object+i)->z+vz)-( (fish+j)->z+(fish+j)->Fz );

      vx=Smoothing*vx +(fish+j)->x+(fish+j)->Fx-(object+i)->x;
      vz=Smoothing*vz +(fish+j)->z+(fish+j)->Fz-(object+i)->z;
      VLength=sqrt(vx*vx+vz*vz);

  (fish+j)->Fx=( (object+i)->x+vx-(fish+j)->x )*FLength/sqrt( pow(VLength,2)+pow(Distance,2) );  
  (fish+j)->Fz=( (object+i)->z+vz-(fish+j)->z )*FLength/sqrt( pow(VLength,2)+pow(Distance,2) );
   
}
else{(fish+j)->ClockWise=0; }

}
/********************** evade objects ****************************/
void EvadeObject(Fish *fish, int Num, Object *object, int NumObj){  
 
  double Distance=0; 

  for(int i=0; i<NumObj; i++){
	 for(int j=0; j<Num; j++){
       Distance=sqrt( pow((object+i)->x-(fish+j)->x,2)
                     +pow((object+i)->z-(fish+j)->z,2) );
       if(Distance<= (object+i)->Gravity_Field)
	             ChangeDirection(fish,j,object,i); 
	 }//end of j loop
  }//end of i loop
}   
/***************************** update position *********************************/
void RefreshPosition(Fish *fish, int Num, double interval){
   double ax,ay,az;

	for(int i=0; i<Num; i++){
	  ax=(fish+i)->Fx/(fish+i)->M;
	  ay=(fish+i)->Fy/(fish+i)->M;
	  az=(fish+i)->Fz/(fish+i)->M;

	  (fish+i)->x +=0.5*ax*pow(interval,2);
	  (fish+i)->y +=0.5*ay*pow(interval,2);
	  (fish+i)->z +=0.5*az*pow(interval,2);
	}
}

void MoveDestination(FishDest *fd, int NumFD, double interval){

for(int i=0; i<NumFD; i++){
  if((fd+i)->c>=360)
    (fd+i)->c=0;
}

  fd->x=48.88*cos(PI*fd->c/180.0)+fd->Cx;
  fd->y=fd->Cy;
  fd->z=48.88*sin(PI*fd->c/180.0)+fd->Cz;
  fd->c+=4*interval;

  (fd+1)->x=45.88*cos(PI*(fd+1)->c/180.0)+(fd+1)->Cx;
  (fd+1)->y=(fd+1)->Cy;//*cos(PI*c/45.0);
  (fd+1)->z=16.88*sin(-PI*(fd+1)->c/180.0)+(fd+1)->Cz;
  (fd+1)->c+=6*interval;

  (fd+2)->x=46.88*cos(PI*(fd+2)->c/180.0)+(fd+2)->Cx;
  (fd+2)->y=(fd+1)->Cy*cos(PI*(fd+2)->c/45.0);
  (fd+2)->z=46.88*sin(-PI*(fd+2)->c/180.0)+(fd+2)->Cz;
  (fd+2)->c+=5.5*interval;

  (fd+3)->x=26.88*cos(PI*(fd+3)->c/180.0)+(fd+3)->Cx;
  (fd+3)->y=(fd+3)->Cy;//*cos(PI*c/45.0);
  (fd+3)->z=45.88*sin(PI*(fd+3)->c/180.0)+(fd+3)->Cz;
  (fd+3)->c+=10*interval;

}
/****************************** Drawing ****************************************/
void DrawFish(Fish *fish, int Num, int f, bool AutoCamera){

  double Distance=0;
 for(int i=0; i<Num; i++){
	if(i==f && AutoCamera==true)
		 DrawShimen((fish+i)->x,(fish+i)->y,(fish+i)->z,(fish+i)->M,(fish+i)->M,(fish+i)->M,
			        1,1,1);
 Distance=sqrt(pow((fish+i)->Fx,2)+ pow((fish+i)->Fy,2)+pow((fish+i)->Fz,2));
 glBegin(GL_LINES);
    glColor3d((fish+i)->r,(fish+i)->g,(fish+i)->b);
    glVertex3d((fish+i)->x,(fish+i)->y,(fish+i)->z);
	glVertex3d((fish+i)->x+(fish+i)->Fx/Distance*1.2,
		       (fish+i)->y+(fish+i)->Fy/Distance*1.2,
			   (fish+i)->z+(fish+i)->Fz/Distance*1.2);
 glEnd();
 }
}

void DrawObject(Object *object, int NumObj){
 for(int i=0; i<NumObj; i++){
		 DrawColumn( (object+i)->x, (object+i)->y, (object+i)->z,
			         (object+i)->Distance_Limit,25,(object+i)->Distance_Limit,0.6,0.8,0.6);
}


}

/******************************* Initialize ************************************/
void InitForce(Fish *fish, int Num){
 
 for(int i=0; i<Num; i++){    
   (fish+i)->Fx=0;
   (fish+i)->Fy=0;
   (fish+i)->Fz=0;
 }
}

void InitFishPosition(Fish *fish, int Num, int NumFD){
  srand(time(0));
  double x=0,y=0,z=0,ID=0,M=0;
for(int i=0; i<Num; i++){	
	x=( (rand()%1000)-500 )*0.1;
	y=( (rand()%1000) )*0.05;
	z=( (rand()%1000)-500 )*0.1;
	ID=rand()%NumFD;
	M=( (rand()%1000)-500 )*0.0008+1.0;
	
  (fish+i)->x=x;    
  (fish+i)->y=y;
  (fish+i)->z=z;
  (fish+i)->ID=ID;
  (fish+i)->M=M;
  (fish+i)->r=0.7; (fish+i)->g=0.7; (fish+i)->b=0.1;
  (fish+i)->ClockWise=0;
}


}

