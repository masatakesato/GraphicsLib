#ifndef	GEOMETRICAL_NOISE_GLSL
#define	GEOMETRICAL_NOISE_GLSL


#pragma include"../shader/WorleyNoise.glsl"




float Geometrical( vec2 p )
{
	int	int_x = int(p.x),
		int_y = int(p.y),
		i,j,
		tempx,tempy;

	float	//frac_x = x - (float)int_x,
			//frac_y = y - (float)int_y,
			//points[3][3][2],
			temp_dis,
			distance1, distance2;
	
	vec2 points_[9];


	//get the point inside the current square, and the point inside the 8 surrounding squares
	for( i=0; i<3; ++i )
	{
		for( j=0; j<3; ++j )
		{
			int idx = j*3+i;
			tempx	= int_x + i - 1;
			tempy	= int_y + j - 1;

			points_[ idx ].x	= tempx + random( tempx, tempy, 1 ) * sign( tempx );
			points_[ idx ].y	= tempy + random( tempx, tempy, 2 ) * sign( tempy );
			//points[i][j][0] = tempx + random(tempx, tempy, 1) * SIGN( tempx );
			//points[i][j][1] = tempy + random(tempx, tempy, 2) * SIGN( tempy );
		}
	}
	
	//get the distance to the closest point from x, y
	distance1 = distance2 = 99999.9;//FLT_MAX;
	
	for( i=0; i<3; ++i )
	{
		for( j=0; j<3; ++j )
		{
			int idx = j*3+i;

			temp_dis = euclidean( p, points_[idx] );
			//temp_dis = manhattan( p, points_[idx] );
			//temp_dis = chebychev( p, points_[idx] );
			//temp_dis = quadratic( p, points_[idx] );
			
			if( temp_dis < distance1 )
			{
				if( distance1 < distance2 )
					distance2 = distance1;

				distance1 = temp_dis;
			}
			else if( temp_dis < distance2 )
				distance2 = temp_dis;
		}
	}
	
	return (1.0f - distance1);//this just inverts it
	//return (distance2);
}


// TODO: ランダム性も含めたい。近傍探索の範囲を広げる必要があるか、、、
// アルゴリズム変更? 現在：セル毎に近傍の点を探索、今後：距離場テクスチャをテクスチャ領域に貼り付けていく（重なったら最小値を選択etc...）
#define DIM		3
#define DIM2	DIM*DIM
#define DIM3	DIM*DIM*DIM

uniform float g_Bin = 1.0;
uniform float g_Gap = 0.0;
uniform float g_Min = 0.48;
uniform float g_Max = 0.5;



float Geometrical( vec3 p )
{
	//p += vec3(500.0, 500.0, 500.0 );
	int	int_x	= int(p.x),
		int_y	= int(p.y),
		int_z	= int(p.z);
	int i,j,k, tempx,tempy,tempz;

	//float	frac_x	= x - (float)int_x,
	//		frac_y	= y - (float)int_y,
	//		frac_z	= z - (float)int_z;

	float	//points[3][3][3][3],
			temp_dis,
			distance1,distance2;

	vec3 points_[DIM3];
float Bin	= g_Bin;//4.0;
float Leng	= Bin;//min( Bin, 7.0 );
float Gap = g_Gap;


	//get the point inside the current cube, and the point inside the 26 surrounding cubes
	for( i=0; i<DIM; ++i )
	{
		for( j=0; j<DIM; ++j )
		{
			for( k=0; k<DIM; ++k )
			{
				tempx = int_x + i - 1;
				tempy = int_y + j - 1;
				tempz = int_z + k - 1;
				
				int idx = k*DIM2 + j*DIM+i;
				points_[idx].x = tempx + random( tempx, tempy, tempz, 1 ) * sign( tempx );
				points_[idx].y = tempy + random( tempx, tempy, tempz, 2 ) * sign( tempy );
				points_[idx].z = tempz + random( tempx, tempy, tempz, 3 ) * sign( tempz );

//########################## ルックアップテーブルをうまく使えば、レンガ模様にも拡張可能か,,,,, ##############################//
	
				//Bin	= g_Bin * random( tempy, tempy, tempy, 2 );
	float gapy = /*random( tempy, tempy, tempy, 1 ) * sign( tempx ) */ tempx * Gap /* Leng*/;
	float pos = tempy + gapy;
	float a = floor( pos/Bin ) * Bin;//min( floor( pos/Bin ), ceil( pos/Bin ) ) * Bin;//

	points_[idx].y = Leng*0.5 + a*sign( tempy );// 点と線分の距離を使ったレンガ模様のテスト2013.10.08

	points_[idx].x = tempx * sign( tempx );
	//points_[idx].y = tempy * sign( tempy );
	points_[idx].z = tempz * sign( tempz );
	
//############################################################################################################################//

				//points[i][j][k][0] = tempx + random( tempx, tempy, tempz, 1 ) * SIGN( tempx );
				//points[i][j][k][1] = tempy + random( tempx, tempy, tempz, 2 ) * SIGN( tempy );
				//points[i][j][k][2] = tempz + random( tempx, tempy, tempz, 3 ) * SIGN( tempz );

			}// end of k loop
		}// end of j loop
	}// end of i loop
	
	//get the distance to the closest point from x, y
	distance1 = distance2 = 99999.9;
	
	for( i=0; i<DIM; ++i )
	{
		for( j=0; j<DIM; ++j )
		{
			for( k=0; k<DIM; ++k )
			{
				int idx = k*DIM2 + j*DIM+i;

tempx = int_x + i - 1;
tempy = int_y + j - 1;
float gapy = /*random( tempy, tempy, tempy, 1 ) * sign( tempx )*/ tempx * Gap /* Leng*/;
//temp_dis = distance_point_line_euclidean( points_[idx]-vec3(Leng*0.5-0.5,0.0,0.0), points_[idx]+vec3(Leng*0.5-0.5,0.0,0.0), p+vec3(gapx, 0.0, 0.0) );// 点と線分距離を使ったレンガ模様のテスト2013.10.08
temp_dis = distance_point_line_chebychev( points_[idx]-vec3(0.0, Leng*0.5-0.5, 0.0), points_[idx]+vec3(0.0, Leng*0.5-0.5, 0.0), p+vec3(0.0, gapy, 0.0) );// 点と線分距離を使ったレンガ模様のテスト2013.10.08
//temp_dis = distance_point_line_manhattan( points_[idx]-vec3(Leng*0.5-0.5,0.0,0.0), points_[idx]+vec3(Leng*0.5-0.5,0.0,0.0), p+vec3(gapx, 0.0, 0.0) );// 点と線分距離を使ったレンガ模様のテスト2013.10.08

				//temp_dis = euclidean( p, points_[idx] );
				//temp_dis = manhattan( p, points_[idx] );
				//temp_dis = chebychev( p, points_[idx] );
				//temp_dis = quadratic( p, points_[idx] );
			
				if( temp_dis < distance1 )
				{
					if( distance1 < distance2 )
						distance2 = distance1;

					distance1 = temp_dis;
				}
				else if( temp_dis < distance2 )
				{
					distance2 = temp_dis;
				}

			}// end of k loop
		}// end of j loop
	}// end of i loop
	

	//return distance2 - distance1;
	return distance1;

	//return 1.0 - distance1;//this just inverts it
	//return (1.0 - distance2);
}



// fractal Brownian motion
// lacunarity: 周波数の増減率
// gain: 振幅の増減率
float Geometrical_fBm( vec2 p, int octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0;

	for( int i=0; i<octaves; ++i )
	{
		total		+= amplitude * Geometrical( p*frequency );
		amp_total	+= amplitude;

		amplitude	*= gain;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}



float Geometrical_fBm( vec2 p, float octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
	float floor_oct	= floor(octaves);

	for( int i=0; i<ceil(octaves); ++i )
	{
		float w = mix( 0.0, 1.0, octaves - float(i) );
		amplitude	*= w;
		total		+= amplitude * Geometrical( p*frequency );
		amp_total	+= amplitude;

		amplitude	*= gain/w;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}



float Geometrical_fBm( vec3 p, int octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0;

	for( int i=0; i<octaves; ++i )
	{
		total		+= amplitude * Geometrical( p*frequency );
		amp_total	+= amplitude;

		amplitude	*= gain;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}


float Geometrical_fBm( vec3 p, float octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
	float floor_oct	= floor(octaves);

	for( int i=0; i<ceil(octaves); ++i )
	{
		float w = mix( 0.0, 1.0, octaves - float(i) );
		amplitude	*= w;
		total		+= amplitude * Geometrical( p*frequency );
		amp_total	+= amplitude;

		amplitude	*= gain/w;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}



#endif GEOMETRICAL_NOISE_GLSL
