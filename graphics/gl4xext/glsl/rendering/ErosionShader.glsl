


uniform sampler2D	g_TerrainHeight;	// 地面の高さ
uniform sampler2D	g_TerrainNormal;	// 地形法線

uniform sampler2D	g_WaterLevel;		// 水の高さ
uniform sampler2D	g_WaterNormal;		// 水面法線

uniform sampler2D	g_Flow;				// セル間の流量
uniform sampler2D	g_DP;				// 隣接セルとの水圧差 r:+x, g:-x, b:+y, a:-y
uniform sampler2D	g_Accelation;		// 加速度
	

uniform sampler2D	g_Velocity;					// 流速(m_timeStep時間あたり)
uniform sampler2D	g_SedimentTransportCapacity;// 運搬可能な土砂の容量
uniform sampler2D	g_ActualSediment;			// 流送土砂の量
uniform sampler2D	g_RegolithThickness;		// 流送土砂の量



const vec4 g_LightIntensity = vec4( 1.0, 1.0, 1.0, 0.0 );
const vec3 lightdir = normalize( vec3(0, 0, 1) );
const vec4 g_TerrainColor	= vec4( 1.0, 0.866, 0.701, 0.0 );
const vec4 g_WaterColor		= vec4( 0.101, 0.368, 0.584, 0.0 );
const vec4 g_SedimentColor	= vec4( 1.0, 0.0, 0.0, 0.0 );





#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Uniform location


// Vertex Shader Output
out vec2		vTexCoord;


// Vertex Shader
void main( void )
{
	gl_Position = vec4( POSITION, 1.0 );
	vTexCoord	= TEXCOORD;
}


#endif




#ifdef _FRAGMENT_SHADER_

// Fragment Shader Input
in vec2		vTexCoord;

out vec4	fragColor0;


// Fragment Shader
void main( void )
{
	float terrain_height	= texture( g_TerrainHeight, vTexCoord.xy ).x;
	vec3 terrain_normal		= texture( g_TerrainNormal, vTexCoord.xy ).xyz;

	float water_level		= texture( g_WaterLevel, vTexCoord.xy ).x;
	vec3 water_normal		= texture( g_WaterNormal, vTexCoord.xy ).xyz;

	float sediment			= texture( g_ActualSediment, vTexCoord.xy ).x;
	float regolith			= texture( g_RegolithThickness, vTexCoord.xy ).x;


	float terrain_ldotn	= max( dot( terrain_normal, lightdir ), 0.0 );
	vec4 terr_color		= g_TerrainColor * terrain_ldotn * g_LightIntensity * 1.0 / ( 1.0 + exp(-terrain_height*0.01) );
	
	// calc transparent water color
const vec3 viewVec = vec3( 0.0, 0.0, 1.0 );
const float water_shiness = 100.0;
	vec3 halfVec = normalize( lightdir + viewVec );
	float specular = pow( max(dot(water_normal, halfVec), 0.0), water_shiness );

	float water_opacity	= clamp( water_level*0.5, 0.0, 0.9 );
	vec4 water_attn = vec4(0.2, 0.05, 0.001, 0.0) * water_level;
	vec4 water_color	= max( terr_color - water_attn, vec4(0.0)) + g_LightIntensity * specular;

	//fragColor0 = terr_color;
	fragColor0 = mix( mix( terr_color, water_color, water_opacity ), g_SedimentColor, sediment+regolith );//vec4( vTexCoord.xy, 0.0, 0.0 );//vec4(vNormal, 0.0);//
}

#endif
