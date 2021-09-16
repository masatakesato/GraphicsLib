#include	"WorleyNoiseCPU.h"

#include	<stdlib.h>
//#include	<math.h>
//#include	<float.h>

#include	<oreore/mathlib/GraphicsMath.h>



namespace OreOreLib
{


#ifndef SIGN
#define	SIGN(n)	( (n > 0) - (n < 0) )
#endif


#ifndef DEFAULT_OFFSET
#define DEFAULT_OFFSET 5000.0f
#endif


// TODO: オレオレライブラリに登録
static float smoothstep( float edge0, float edge1, float x )
{
    // Scale, bias and saturate x to 0..1 range
    x = Saturate((x - edge0)/(edge1 - edge0)); 
    // Evaluate polynomial
    return x*x*(3 - 2*x);
}

static float lerp( float a, float b, float t )
{
	if( t<a )	return a;
	if( b<t )	return b;
	return		a + t * (b - a);
}



// BrightnessContrast
static float BrightnessContrast( float value, float contrast, float brightness )
{
	float result = value;
	//pixelColor.rgb /= pixelColor.a;

	// Apply contrast.
	value	= ((value - 0.5f) * Max(contrast, 0.0f)) + 0.5;//pixelColor.rgb = ((pixelColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f;

	// Apply brightness.
	value	+= brightness;//pixelColor.rgb += Brightness;

	// Return final pixel color.
	//pixelColor.rgb *= pixelColor.a;

	return value;
}


	
static float Threshold( float value, float threshold, float smoothness )
{
//								threshold
//	|----------------------------*-----------|
// 0.0										1.0

	// threshold未満の値は0、それ以外は1にする
	
	// smoothnessで、thresholdを基点にして値を線形補間する
	//	smoothnessが0.0: 補間なし
	//	smoothnessが0.5: threshold-0.5, threshold+0.5
	const float start	= threshold - smoothness * 0.5;
	//const float end		= threshold + smoothness * 0.5;
	float lerp_value = smoothness>0.0 ? (value - start) / smoothness : float(value>threshold);
	
	return lerp_value;
}


// Invert 
static float Invert( float value )
{
	return 1.0 - value;
}




WorleyNoiseCPU::WorleyNoiseCPU()
{
	int i, j, k;

	for( i=0; i<256; ++i )
		permutations[i]	= i;

	for( i=0; i<256; ++i )
	{
		j				= permutations[i];
		k				= ranInt();
		permutations[i]	= permutations[k];
		permutations[k]	= j;
	}// end of i loop

	
	m_NoiseParam.Scale			= 1.0f;
	m_NoiseParam.Angle			= 0.0f;
	m_NoiseParam.Stretch		= 0.0f;
	m_NoiseParam.Octaves		= 1.0f;
	m_NoiseParam.Lacunarity		= 1.5f;
	m_NoiseParam.Gain			= 0.5f;

	m_OutputParam.Contrast		= 1.0f;
	m_OutputParam.Brightness	= 0.0f;
	m_OutputParam.Threshold		= 0.5f;
	m_OutputParam.Smoothness	= 1.0f;
	m_OutputParam.Invert		= false;
}


WorleyNoiseCPU::~WorleyNoiseCPU()
{
	Release();
}


void WorleyNoiseCPU::Release()
{
	m_NoiseParam.Scale			= 1.0f;
	m_NoiseParam.Angle			= 0.0f;
	m_NoiseParam.Stretch		= 0.0f;
	m_NoiseParam.Octaves		= 1.0f;
	m_NoiseParam.Lacunarity		= 1.5f;
	m_NoiseParam.Gain			= 0.5f;

	m_OutputParam.Contrast		= 1.0f;
	m_OutputParam.Brightness	= 0.0f;
	m_OutputParam.Threshold		= 0.8f;
	m_OutputParam.Smoothness	= 0.5f;
	m_OutputParam.Invert		= false;
}




float WorleyNoiseCPU::noise( float x, float y )
{
	int	int_x = (int)x,
		int_y = (int)y,
		i,j, tempx,tempy;

	float	//frac_x = x - (float)int_x,
			//frac_y = y - (float)int_y,
			points[3][3][2],
			temp_dis,
			distance1,distance2;
	
	//get the point inside the current square, and the point inside the 8 surrounding squares
	for (i = 0;i<3;++i)
	{
		for (j = 0; j < 3; ++j)
		{
			tempx = int_x + i - 1;
			tempy = int_y + j - 1;
			points[i][j][0] = tempx + random(tempx, tempy, 1) * SIGN( tempx );
			points[i][j][1] = tempy + random(tempx, tempy, 2) * SIGN( tempy );
		}
	}
	
	//get the distance to the closest point from x, y
	distance1 = distance2 = FLT_MAX;
	
	for (i = 0; i < 3; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			temp_dis = euclidean(x,y,points[i][j][0],points[i][j][1]);
			//temp_dis = manhattan(x,y,points[i][j][0],points[i][j][1]);
			//temp_dis = chebychev(x,y,points[i][j][0],points[i][j][1]);
			//temp_dis = quadratic(x,y,points[i][j][0],points[i][j][1]);
			
			if (temp_dis < distance1)
			{
				if (distance1 < distance2)
					distance2 = distance1;

				distance1 = temp_dis;
			}
			else if (temp_dis < distance2)
				distance2 = temp_dis;
		}
	}
	
	return distance1;//this just inverts it
	//return (distance2);
}


float WorleyNoiseCPU::noise( float x, float y, float z )
{
	int	int_x	= (int)x,
		int_y	= (int)y,
		int_z	= (int)z;
	int i,j,k, tempx,tempy,tempz;

	//float	frac_x	= x - (float)int_x,
	//		frac_y	= y - (float)int_y,
	//		frac_z	= z - (float)int_z;

	float	points[3][3][3][3],
			temp_dis,
			distance1,distance2;
	
	//get the point inside the current cube, and the point inside the 26 surrounding cubes
	for( i=0; i<3; ++i )
	{
		for( j=0; j<3; ++j )
		{
			for( k=0; k<3; ++k )
			{
				tempx = int_x + i - 1;
				tempy = int_y + j - 1;
				tempz = int_z + k - 1;
				
				points[i][j][k][0] = tempx + random( tempx, tempy, tempz, 1 ) * SIGN( tempx );
				points[i][j][k][1] = tempy + random( tempx, tempy, tempz, 2 ) * SIGN( tempy );
				points[i][j][k][2] = tempz + random( tempx, tempy, tempz, 3 ) * SIGN( tempz );

			}// end of k loop
		}// end of j loop
	}// end of i loop
	
	//get the distance to the closest point from x, y
	distance1 = distance2 = FLT_MAX;
	
	for( i=0; i<3; ++i )
	{
		for( j=0; j<3; ++j )
		{
			for( k=0; k<3; ++k )
			{
				temp_dis = euclidean( x, y, z, points[i][j][k][0], points[i][j][k][1], points[i][j][k][2] );
				//temp_dis = manhattan(x,y,points[i][j][0],points[i][j][1]);
				//temp_dis = chebychev(x,y,points[i][j][0],points[i][j][1]);
				//temp_dis = quadratic(x,y,points[i][j][0],points[i][j][1]);
			
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
	
	return distance1;
	//return (distance2);
}




float WorleyNoiseCPU::fBm( float x, float y )
{
	//float amplitude = 1.0f;
	//float total		= 0.0f;
	//float amp_total = 0.0f;
	//Vec2f pp		= { x, y };

 //   int i;
 //    
 //   for( i=0; i<octaves; ++i )
 //   {
	//	total		+= amplitude * noise( pp.x, pp.y );
	//	amp_total	+= amplitude;

 //       amplitude	*= gain;
 //       Scale( pp, lacunarity );
 //   }

 //   return total / amp_total;


	float amplitude = 1.0;
	//float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
	float floor_oct	= floor(m_NoiseParam.Octaves);

	const Vec4f p	= { x, y, 0.0f, 1.0f };
	Vec4f pp	= { 0.0f, 0.0f, 0.0f, 1.0f };
	Multiply( pp, m_MatTransform, p );


	for( int i=0; i<(int)ceil(m_NoiseParam.Octaves); ++i )
	{
		float w = lerp( 0.0, 1.0, m_NoiseParam.Octaves - float(i) );
		amplitude	*= w;
		total		+= amplitude * noise( pp.x, pp.y );
		amp_total	+= amplitude;

		amplitude	*= m_NoiseParam.Gain/w;
		//frequency	*= lacunarity;
		Scale( pp, m_NoiseParam.Lacunarity );
	}

	return total / amp_total;
}




float WorleyNoiseCPU::fBm( float x, float y, float z )
{
	//float amplitude = 1.0f;
	//float total		= 0.0f;
	//float amp_total = 0.0f;
	//Vec3f pp		= { x, y, z };

 //   int i;
 //    
 //   for( i=0; i<octaves; ++i )
 //   {
	//	total		+= amplitude * noise( pp.x, pp.y, pp.z );
	//	amp_total	+= amplitude;

 //       amplitude	*= gain;
 //       Scale( pp, lacunarity );		
 //   }

 //   return total / amp_total;

	float amplitude = 1.0;
	//float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
	float floor_oct	= floor(m_NoiseParam.Octaves);

	const Vec4f p	= { x, y, z, 1.0f };
	Vec4f pp	= { 0.0f, 0.0f, 0.0f, 1.0f };
	Multiply( pp, m_MatTransform, p );


	for( int i=0; i<(int)ceil(m_NoiseParam.Octaves); ++i )
	{
		float w = lerp( 0.0, 1.0, m_NoiseParam.Octaves - float(i) );
		amplitude	*= w;
		total		+= amplitude * noise( pp.x, pp.y, pp.z );
		amp_total	+= amplitude;

		amplitude	*= m_NoiseParam.Gain/w;
		//frequency	*= lacunarity;
		Scale( pp, m_NoiseParam.Lacunarity );
	}

	return total / amp_total;
}


float WorleyNoiseCPU::fBmOutput( float x, float y )
{
	float n = fBm(x,y);

	n	= BrightnessContrast( n, m_OutputParam.Contrast, m_OutputParam.Brightness );
	n	= Clamp( Threshold( n, m_OutputParam.Threshold, m_OutputParam.Smoothness ), 0.0f, 1.0f );
	n	= m_OutputParam.Invert==1.0 ? Invert(n) : n;

	return n;
}


float WorleyNoiseCPU::fBmOutput( float x, float y, float z )
{
	float n = fBm(x,y,z);
	
	n	= BrightnessContrast( n, m_OutputParam.Contrast, m_OutputParam.Brightness );
	n	= Clamp( Threshold( n, m_OutputParam.Threshold, m_OutputParam.Smoothness ), 0.0f, 1.0f );
	n	= m_OutputParam.Invert==1.0 ? Invert(n) : n;

	return n;
}



float WorleyNoiseCPU::random(int x, int y, int z)
{
	int r1 = permutations[ (x + permutations[(y + permutations[z & 255]) & 255]) & 255 ];

	return((float)r1/256.0f);
}


float WorleyNoiseCPU::random(int x, int y, int z, int w)
{
	int r1 = //permutations[ (x + permutations[(y + permutations[z & 255]) & 255]) & 255 ];
	
	permutations[ (x + permutations[(y + permutations[(z + permutations[w & 255]) & 255]) & 255]) & 255 ];

	return((float)r1/256.0f);
}



int WorleyNoiseCPU::ranInt()
{
	int r;	
	r = rand();
    return (r&255);
}


float WorleyNoiseCPU::euclidean(float x1, float y1, float x2, float y2)
{
	float	dif_x = x1-x2,
			dif_y = y1-y2;

	return sqrt(dif_x * dif_x + dif_y * dif_y);
}


float WorleyNoiseCPU::euclidean( float x1, float y1, float z1, float x2, float y2, float z2 )
{
	float	dif_x = x1-x2,
			dif_y = y1-y2,
			dif_z = z1-z2;

	return sqrt(dif_x * dif_x + dif_y * dif_y + dif_z * dif_z);
}



float WorleyNoiseCPU::manhattan(float x1, float y1, float x2, float y2)
{
	float	dif_x = x1 - x2,
			dif_y = y1 - y2;

	return (fabs(dif_x) + fabs(dif_y));
}


float WorleyNoiseCPU::chebychev(float x1, float y1, float x2, float y2)
{
	float	dif_x = fabs(x1 - x2),
			dif_y = fabs(y1 - y2);

	if (dif_x > dif_y)
		return (dif_x);
	else
		return (dif_y);

}


float WorleyNoiseCPU::quadratic(float x1, float y1, float x2, float y2)
{
	float	dif_x = x1 - x2,
			dif_y = y1 - y2;

	return (dif_x * dif_x + dif_x * dif_y + dif_y * dif_y);
}


void WorleyNoiseCPU::UpdateTransformMatrix()
{
	// トランスフォーム行列を計算
	static Quatf	quat;
	static Mat4f	matRotScale, matScale, matRotation, matTranslation;
	
	MatScale( matScale, m_NoiseParam.Scale, m_NoiseParam.Scale/Max(m_NoiseParam.Stretch+1.0f, 1.0f), m_NoiseParam.Scale );

	InitQuat( quat, m_NoiseParam.Angle, 0.0f, 0.0f, 1.0f );
	Quat2Mat( matRotation, quat );
	
	Multiply( matRotScale, matScale, matRotation );

	MatTranslation( matTranslation, DEFAULT_OFFSET, DEFAULT_OFFSET, DEFAULT_OFFSET );

	// m_MatTransform = [ matTranslation ] * [ matRotation ] * [ matScale ] * [ 頂点座標 ]. 回転,拡大縮小が終わった後で平行移動する
	Multiply( m_MatTransform, matTranslation, matRotScale );
}



}// end of namespace