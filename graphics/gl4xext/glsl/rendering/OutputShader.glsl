#ifndef OUTPUT_SHADER_GLSLH
#define	OUTPUT_SHADER_GLSLH




uniform float	g_Contrast;
uniform float	g_Brightness;

uniform float	g_Threshold;
uniform float	g_Smoothness;

uniform float	g_Invert;


// BrightnessContrast
float BrightnessContrast( float value, float contrast, float brightness )
{
	float result = value;
	//pixelColor.rgb /= pixelColor.a;

	// Apply contrast.
	value	= ((value - 0.5) * max(contrast, 0)) + 0.5;//pixelColor.rgb = ((pixelColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f;

	// Apply brightness.
	value	+= brightness;//pixelColor.rgb += Brightness;

	// Return final pixel color.
	//pixelColor.rgb *= pixelColor.a;

	return value;
}




// Threshold Node
float Threshold( float value, float threshold, float smoothness )
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
float Invert( float value )
{
	return 1.0 - value;
}




// Blend. 後で考える
//float Blend( float a, float b, int mode )
//{
//
//
//
//
//}






#endif	// FUNCTIONS_GLSLH //
