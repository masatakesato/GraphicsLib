// Include Files
#pragma include "exp/GPUErosion/GPUErosion_Common.glsl"


// Uniform Locations
uniform sampler2D		g_TexWaterLevel;


// Random Accessible Buffers
layout( binding = 0 )	uniform writeonly image2D	g_TexDest;


// Constant Variables
#define	g_TexSize	g_ErosionAttribs.TexSize;


// Workgroup Layout



// Main Kernel
void main()
{
	ivec2 texelPos	= ivec2( gl_GlobalInvocationID.xy );
	if( texelPos.x >= int(g_TexSize.x) || texelPos.y >= int(g_TexSize.y) )
		return;

	Vec2i pos = {x, y};

	const Vec4f* oldoutFlow = (Vec4f *)m_WaterFlow(x,y);
	Vec4f outFlow;

	for( int n=0; n<numNs; ++n )
	{
		Vec2i n_pos;
		Add( n_pos, pos, Ns[n] );

		//============== 近傍セルの水位差から圧力を計算 ==============//
		float curr_water_lv = texelFetch( g_TexWaterLevel, , 0 );//*(float *)m_WaterLevel( pos.x, pos.y );
		float neighbor_water_lv = *(float *)m_WaterLevel( n_pos.x, n_pos.y );

		// posセルの高さを計算する.現状1レイヤーだけ
		float h		= curr_water_lv + *(float *)m_TerrainHeight( pos.x, pos.y );

		// neighborセルの高さを計算する
		float n_h	= neighbor_water_lv + *(float *)m_TerrainHeight( n_pos.x, n_pos.y );

		// 圧力差分を計算する
		//float dP	= m_G * ( h - n_h );
		//float accel	= min( dP / ( m_Length ), m_Length );

		//============== 圧力をもとに水の加速度を計算 ================//
		float accel	= m_G * ( h - n_h ) / m_Length;

		//================== 加速度から流量を計算 ====================//

		// ダンピングを加えてシミュレーション安定させる & 流出(正の値)の場合だけ値を登録する
		outFlow.xyzw[n]	= max( DAMPING * oldoutFlow->xyzw[n] + m_timeStep * m_C * accel, 0.0f );// 論文中の(1)

	}// end of neighbor loop



	//============================			============================//

	// 現在の水量
	float maxWater	= m_Length * m_Length * *(float *)m_WaterLevel(x,y);
	// 流出させたい水量
	float waterOut	= ( outFlow.x + outFlow.y + outFlow.z + outFlow.w ) * m_timeStep;

	if( waterOut == 0.0f )
	{
		InitVec( outFlow, 0.0f, 0.0f, 0.0f, 0.0f );
	}
	else
	{
		float waterScaleFactor = maxWater / waterOut;
		if( waterScaleFactor < 1.0f )	Scale( outFlow, waterScaleFactor );
	}

	*(Vec4f *)m_WaterFlow(x,y) = outFlow;

}
