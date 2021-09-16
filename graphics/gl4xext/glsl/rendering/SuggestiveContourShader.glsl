// Suggestive Contours

uniform sampler2D	g_Texture;	// texture
uniform float		g_Scale;	// color scale


#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Uniform location


// Vertex Shader Output
out vec2		vTexCoord;


// Vertex Shader
void main(void)
{
	gl_Position = vec4( POSITION, 1.0 );//
	vTexCoord	= TEXCOORD;
}


#endif




#ifdef _FRAGMENT_SHADER_

// Fragment Shader Input
in vec2		vTexCoord;

out vec4	outColor;


// Fragment Shader
//void main (void)
//{
//	vec4 color = texture( g_Texture, vTexCoord.xy ).xyzw;
//	outColor = color;
//}




// texture
//uniform sampler2D color;
// radius for valley detection
uniform int radius = 2;
// rendered image width
//uniform int renderwidth = 512;
uniform vec4 g_ScreenSize = vec4( 1280.0, 720.0, 1.0/1280.0, 1.0/720.0 );


float intensity(in vec4 color)
{
	return sqrt((color.x*color.x)+(color.y*color.y)+(color.z*color.z));
}

vec3 simple_edge_detection( in vec2 step, in vec2 center )
{
	// let's learn more about our center pixel
	float center_intensity = intensity( texture(g_Texture, center) );
	// counters we need
	int darker_count = 0;
	float max_intensity = center_intensity;
	// 一定距離内の近傍ピクセルを調べる
	for(int i = -radius; i <= radius; i++)
	{
		for(int j = -radius; j<= radius; j++)
		{
			vec2 current_location = center + vec2(i*step.x, j*step.y);
			float current_intensity = intensity(texture( g_Texture, current_location) );
			if(current_intensity < center_intensity)// 中心ピクセルよりも暗い場合
			{
				darker_count++;
			}
			if(current_intensity > max_intensity)	// 中心ピクセルよりも明るい場合
			{
				max_intensity = current_intensity;
			}
		}
	}
	// do we have a valley pixel?
	if((max_intensity - center_intensity) > 0.1*radius)// 0.01って何?
	{
		if(darker_count/(radius*radius) < (1-(1/radius)))
		{
			return vec3(0.1,0.5,0.0); // yep, it's a valley pixel.
		}
	}
	return vec3(1.0,1.0,1.0); // no, it's not.

}

void main(void)
{
	vec2 step = g_ScreenSize.zw;//1.0/renderwidth;
	vec2 center_color = vTexCoord;
	
//	float center_intensity = intensity( texture(g_Texture, center_color) );
//	if( center_intensity==0.0 )	discard;

	outColor.rgb = simple_edge_detection( step, center_color );
    outColor.a = 0.0;
}





#endif
