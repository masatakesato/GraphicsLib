
// Unfiorm Locations
uniform int	g_Pass = 0;
uniform sampler2D uInpTex;
uniform sampler2D uInpTex2;
uniform sampler2D uInpTex3;


const vec2 texSize = textureSize(uInpTex, 0);//vec2(textureSize(uInpTex, 0));
#define SMAA_RT_METRICS vec4(1.0/texSize,texSize)
#define SMAA_GLSL_4 1

//#define SMAA_PRESET_LOW 1
//#define SMAA_PRESET_MEDIUM 1
#define SMAA_PRESET_ULTRA 1




//-----------------------------------------------------------------------------
// SMAA Presets

/**
 * Note that if you use one of these presets, the following configuration
 * macros will be ignored if set in the "Configurable Defines" section.
 */

#if defined(SMAA_PRESET_LOW)
#define SMAA_THRESHOLD 0.15
#define SMAA_MAX_SEARCH_STEPS 4
#define SMAA_DISABLE_DIAG_DETECTION
#define SMAA_DISABLE_CORNER_DETECTION
#elif defined(SMAA_PRESET_MEDIUM)
#define SMAA_THRESHOLD 0.1
#define SMAA_MAX_SEARCH_STEPS 8
#define SMAA_DISABLE_DIAG_DETECTION
#define SMAA_DISABLE_CORNER_DETECTION
#elif defined(SMAA_PRESET_HIGH)
#define SMAA_THRESHOLD 0.1
#define SMAA_MAX_SEARCH_STEPS 16
#define SMAA_MAX_SEARCH_STEPS_DIAG 8
#define SMAA_CORNER_ROUNDING 25
#elif defined(SMAA_PRESET_ULTRA)
#define SMAA_THRESHOLD 0.05
#define SMAA_MAX_SEARCH_STEPS 32
#define SMAA_MAX_SEARCH_STEPS_DIAG 16
#define SMAA_CORNER_ROUNDING 25
#endif



//-----------------------------------------------------------------------------
// Configurable Defines

/**
 * SMAA_THRESHOLD specifies the threshold or sensitivity to edges.
 * Lowering this value you will be able to detect more edges at the expense of
 * performance. 
 *
 * Range: [0, 0.5]
 *   0.1 is a reasonable value, and allows to catch most visible edges.
 *   0.05 is a rather overkill value, that allows to catch 'em all.
 *
 *   If temporal supersampling is used, 0.2 could be a reasonable value, as low
 *   contrast edges are properly filtered by just 2x.
 */
#ifndef SMAA_THRESHOLD
#define SMAA_THRESHOLD 0.1
#endif

/**
 * SMAA_DEPTH_THRESHOLD specifies the threshold for depth edge detection.
 * 
 * Range: depends on the depth range of the scene.
 */
#ifndef SMAA_DEPTH_THRESHOLD
#define SMAA_DEPTH_THRESHOLD (0.1 * SMAA_THRESHOLD)
#endif

/**
 * SMAA_MAX_SEARCH_STEPS specifies the maximum steps performed in the
 * horizontal/vertical pattern searches, at each side of the pixel.
 *
 * In number of pixels, it's actually the double. So the maximum line length
 * perfectly handled by, for example 16, is 64 (by perfectly, we meant that
 * longer lines won't look as good, but still antialiased).
 *
 * Range: [0, 112]
 */
#ifndef SMAA_MAX_SEARCH_STEPS
#define SMAA_MAX_SEARCH_STEPS 16
#endif

/**
 * SMAA_MAX_SEARCH_STEPS_DIAG specifies the maximum steps performed in the
 * diagonal pattern searches, at each side of the pixel. In this case we jump
 * one pixel at time, instead of two.
 *
 * Range: [0, 20]
 *
 * On high-end machines it is cheap (between a 0.8x and 0.9x slower for 16 
 * steps), but it can have a significant impact on older machines.
 *
 * Define SMAA_DISABLE_DIAG_DETECTION to disable diagonal processing.
 */
#ifndef SMAA_MAX_SEARCH_STEPS_DIAG
#define SMAA_MAX_SEARCH_STEPS_DIAG 8
#endif

/**
 * SMAA_CORNER_ROUNDING specifies how much sharp corners will be rounded.
 *
 * Range: [0, 100]
 *
 * Define SMAA_DISABLE_CORNER_DETECTION to disable corner processing.
 */
#ifndef SMAA_CORNER_ROUNDING
#define SMAA_CORNER_ROUNDING 25
#endif

/**
 * If there is an neighbor edge that has SMAA_LOCAL_CONTRAST_FACTOR times
 * bigger contrast than current edge, current edge will be discarded.
 *
 * This allows to eliminate spurious crossing edges, and is based on the fact
 * that, if there is too much contrast in a direction, that will hide
 * perceptually contrast in the other neighbors.
 */
#ifndef SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR
#define SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR 2.0
#endif

/**
 * Predicated thresholding allows to better preserve texture details and to
 * improve performance, by decreasing the number of detected edges using an
 * additional buffer like the light accumulation buffer, object ids or even the
 * depth buffer (the depth buffer usage may be limited to indoor or short range
 * scenes).
 *
 * It locally decreases the luma or color threshold if an edge is found in an
 * additional buffer (so the global threshold can be higher).
 *
 * This method was developed by Playstation EDGE MLAA team, and used in 
 * Killzone 3, by using the light accumulation buffer. More information here:
 *     http://iryoku.com/aacourse/downloads/06-MLAA-on-PS3.pptx 
 */
#ifndef SMAA_PREDICATION
#define SMAA_PREDICATION 0
#endif

/**
 * Threshold to be used in the additional predication buffer. 
 *
 * Range: depends on the input, so you'll have to find the magic number that
 * works for you.
 */
#ifndef SMAA_PREDICATION_THRESHOLD
#define SMAA_PREDICATION_THRESHOLD 0.01
#endif

/**
 * How much to scale the global threshold used for luma or color edge
 * detection when using predication.
 *
 * Range: [1, 5]
 */
#ifndef SMAA_PREDICATION_SCALE
#define SMAA_PREDICATION_SCALE 2.0
#endif

/**
 * How much to locally decrease the threshold.
 *
 * Range: [0, 1]
 */
#ifndef SMAA_PREDICATION_STRENGTH
#define SMAA_PREDICATION_STRENGTH 0.4
#endif

/**
 * Temporal reprojection allows to remove ghosting artifacts when using
 * temporal supersampling. We use the CryEngine 3 method which also introduces
 * velocity weighting. This feature is of extreme importance for totally
 * removing ghosting. More information here:
 *    http://iryoku.com/aacourse/downloads/13-Anti-Aliasing-Methods-in-CryENGINE-3.pdf
 *
 * Note that you'll need to setup a velocity buffer for enabling reprojection.
 * For static geometry, saving the previous depth buffer is a viable
 * alternative.
 */
#ifndef SMAA_REPROJECTION
#define SMAA_REPROJECTION 0
#endif

/**
 * SMAA_REPROJECTION_WEIGHT_SCALE controls the velocity weighting. It allows to
 * remove ghosting trails behind the moving object, which are not removed by
 * just using reprojection. Using low values will exhibit ghosting, while using
 * high values will disable temporal supersampling under motion.
 *
 * Behind the scenes, velocity weighting removes temporal supersampling when
 * the velocity of the subsamples differs (meaning they are different objects).
 *
 * Range: [0, 80]
 */
#ifndef SMAA_REPROJECTION_WEIGHT_SCALE
#define SMAA_REPROJECTION_WEIGHT_SCALE 30.0
#endif

/**
 * On some compilers, discard cannot be used in vertex shaders. Thus, they need
 * to be compiled separately.
 */
//#ifndef SMAA_INCLUDE_VS
//#define SMAA_INCLUDE_VS 1
//#endif
//#ifndef SMAA_INCLUDE_PS
//#define SMAA_INCLUDE_PS 1
//#endif

//-----------------------------------------------------------------------------
// Texture Access Defines

#ifndef SMAA_AREATEX_SELECT
#if defined(SMAA_HLSL_3)
#define SMAA_AREATEX_SELECT(sample) sample.ra
#else
#define SMAA_AREATEX_SELECT(sample) sample.rg
#endif
#endif

#ifndef SMAA_SEARCHTEX_SELECT
#define SMAA_SEARCHTEX_SELECT(sample) sample.r
#endif

#ifndef SMAA_DECODE_VELOCITY
#define SMAA_DECODE_VELOCITY(sample) sample.rg
#endif

//-----------------------------------------------------------------------------
// Non-Configurable Defines

#define SMAA_AREATEX_MAX_DISTANCE 16
#define SMAA_AREATEX_MAX_DISTANCE_DIAG 20
#define SMAA_AREATEX_PIXEL_SIZE (1.0 / float2(160.0, 560.0))
#define SMAA_AREATEX_SUBTEX_SIZE (1.0 / 7.0)
#define SMAA_SEARCHTEX_SIZE float2(66.0, 33.0)
#define SMAA_SEARCHTEX_PACKED_SIZE float2(64.0, 16.0)
#define SMAA_CORNER_ROUNDING_NORM (float(SMAA_CORNER_ROUNDING) / 100.0)

//-----------------------------------------------------------------------------
// Porting Functions

#if defined(SMAA_HLSL_3)
#define SMAATexture2D sampler2D
#define SMAASampleLevelZero(tex, coord) tex2Dlod(tex, float4(coord, 0.0, 0.0))
#define SMAASampleLevelZeroPoint(tex, coord) tex2Dlod(tex, float4(coord, 0.0, 0.0))
#define SMAASampleLevelZeroOffset(tex, coord, offset) tex2Dlod(tex, float4(coord + offset * SMAA_RT_METRICS.xy, 0.0, 0.0))
#define SMAASample(tex, coord) tex2D(tex, coord)
#define SMAASamplePoint(tex, coord) tex2D(tex, coord)
#define SMAASampleOffset(tex, coord, offset) tex2D(tex, coord + offset * SMAA_RT_METRICS.xy)
#define SMAA_FLATTEN [flatten]
#define SMAA_BRANCH [branch]
#endif
#if defined(SMAA_HLSL_4) || defined(SMAA_HLSL_4_1)
SamplerState LinearSampler { Filter = MIN_MAG_LINEAR_MIP_POINT; AddressU = Clamp; AddressV = Clamp; };
SamplerState PointSampler { Filter = MIN_MAG_MIP_POINT; AddressU = Clamp; AddressV = Clamp; };
#define SMAATexture2D Texture2D
#define SMAASampleLevelZero(tex, coord) tex.SampleLevel(LinearSampler, coord, 0)
#define SMAASampleLevelZeroPoint(tex, coord) tex.SampleLevel(PointSampler, coord, 0)
#define SMAASampleLevelZeroOffset(tex, coord, offset) tex.SampleLevel(LinearSampler, coord, 0, offset)
#define SMAASample(tex, coord) tex.Sample(LinearSampler, coord)
#define SMAASamplePoint(tex, coord) tex.Sample(PointSampler, coord)
#define SMAASampleOffset(tex, coord, offset) tex.Sample(LinearSampler, coord, offset)
#define SMAA_FLATTEN [flatten]
#define SMAA_BRANCH [branch]
#define SMAATexture2DMS2(tex) Texture2DMS<float4, 2> tex
#define SMAALoad(tex, pos, sample) tex.Load(pos, sample)
#if defined(SMAA_HLSL_4_1)
#define SMAAGather(tex, coord) tex.Gather(LinearSampler, coord, 0)
#endif
#endif
#if defined(SMAA_GLSL_3) || defined(SMAA_GLSL_4)
#define SMAATexture2D sampler2D
#define SMAASampleLevelZero(tex, coord) textureLod(tex, coord, 0.0)
#define SMAASampleLevelZeroPoint(tex, coord) textureLod(tex, coord, 0.0)
#define SMAASampleLevelZeroOffset(tex, coord, offset) textureLodOffset(tex, coord, 0.0, offset)
#define SMAASample(tex, coord) texture(tex, coord)
#define SMAASamplePoint(tex, coord) texture(tex, coord)
#define SMAASampleOffset(tex, coord, offset) texture(tex, coord, offset)
#define SMAA_FLATTEN
#define SMAA_BRANCH
#define lerp(a, b, t) mix(a, b, t)
#define saturate(a) clamp(a, 0.0, 1.0)
#if defined(SMAA_GLSL_4)
#define SMAAMad(a, b, c) (a * b + c)//#define SMAAMad(a, b, c) fma(a, b, c)
#define SMAAGather(tex, coord) textureGather(tex, coord)
#else
#define mad(a, b, c) (a * b + c)
#endif
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define int2 ivec2
#define int3 ivec3
#define int4 ivec4
#define bool2 bvec2
#define bool3 bvec3
#define bool4 bvec4
#endif

#if !defined(SMAA_HLSL_3) && !defined(SMAA_HLSL_4) && !defined(SMAA_HLSL_4_1) && !defined(SMAA_GLSL_3) && !defined(SMAA_GLSL_4) && !defined(SMAA_CUSTOM_SL)
#error you must define the shading language: SMAA_HLSL_*, SMAA_GLSL_* or SMAA_CUSTOM_SL
#endif

//-----------------------------------------------------------------------------
// Misc functions

/**
 * Gathers current pixel, and the top-left neighbors.
 */
float3 SMAAGatherNeighbours( float2 texcoord, float4 offset[3], SMAATexture2D tex )
{
    #ifdef SMAAGather
    return SMAAGather(tex, texcoord + SMAA_RT_METRICS.xy * float2(-0.5, -0.5)).grb;
    #else
    float P = SMAASamplePoint(tex, texcoord).r;
    float Pleft = SMAASamplePoint(tex, offset[0].xy).r;
    float Ptop  = SMAASamplePoint(tex, offset[0].zw).r;
    return float3(P, Pleft, Ptop);
    #endif
}

/**
 * Adjusts the threshold by means of predication.
 */
float2 SMAACalculatePredicatedThreshold( float2 texcoord, float4 offset[3], SMAATexture2D predicationTex )
{
    float3 neighbours = SMAAGatherNeighbours(texcoord, offset, predicationTex );
    float2 delta = abs(neighbours.xx - neighbours.yz);
    float2 edges = step(SMAA_PREDICATION_THRESHOLD, delta);
    return SMAA_PREDICATION_SCALE * SMAA_THRESHOLD * (1.0 - SMAA_PREDICATION_STRENGTH * edges);
}

/**
 * Conditional move:
 */
void SMAAMovc(bool2 cond, inout float2 variable, float2 value)
{
    SMAA_FLATTEN if (cond.x) variable.x = value.x;
    SMAA_FLATTEN if (cond.y) variable.y = value.y;
}

void SMAAMovc(bool4 cond, inout float4 variable, float4 value)
{
    SMAAMovc(cond.xy, variable.xy, value.xy);
    SMAAMovc(cond.zw, variable.zw, value.zw);
}


#ifdef _VERTEX_SHADER_
//-----------------------------------------------------------------------------
// Vertex Shaders

/**
 * Edge Detection Vertex Shader
 */
void SMAAEdgeDetectionVS(float2 texcoord,
                         inout float4 offset[3])
{
    offset[0] = texcoord.xyxy + SMAA_RT_METRICS.xyxy * float4(-1.0, 0.0, 0.0, -1.0);	//mad(SMAA_RT_METRICS.xyxy, float4(-1.0, 0.0, 0.0, -1.0), texcoord.xyxy);
    offset[1] = texcoord.xyxy + SMAA_RT_METRICS.xyxy * float4( 1.0, 0.0, 0.0,  1.0);	//mad(SMAA_RT_METRICS.xyxy, float4( 1.0, 0.0, 0.0,  1.0), texcoord.xyxy);
    offset[2] = texcoord.xyxy + SMAA_RT_METRICS.xyxy * float4(-2.0, 0.0, 0.0, -2.0);	//mad(SMAA_RT_METRICS.xyxy, float4(-2.0, 0.0, 0.0, -2.0), texcoord.xyxy);
}

/**
 * Blend Weight Calculation Vertex Shader
 */
void SMAABlendingWeightCalculationVS( float2 texcoord, inout float2 pixcoord, inout float4 offset[3] )
{
    pixcoord = texcoord * SMAA_RT_METRICS.zw;

    // We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
    offset[0] = texcoord.xyxy + SMAA_RT_METRICS.xyxy * float4(-0.25, -0.125,  1.25, -0.125);	//mad(SMAA_RT_METRICS.xyxy, float4(-0.25, -0.125,  1.25, -0.125), texcoord.xyxy);
    offset[1] = texcoord.xyxy + SMAA_RT_METRICS.xyxy * float4(-0.125, -0.25, -0.125,  1.25);	//mad(SMAA_RT_METRICS.xyxy, float4(-0.125, -0.25, -0.125,  1.25), texcoord.xyxy);

    // And these for the searches, they indicate the ends of the loops:
    offset[2] = float4(offset[0].xz, offset[1].yw) + 
				float4(-2.0, 2.0, -2.0, 2.0) *
				SMAA_RT_METRICS.xxyy * float(SMAA_MAX_SEARCH_STEPS);
		//mad(SMAA_RT_METRICS.xxyy,
                //    float4(-2.0, 2.0, -2.0, 2.0) * float(SMAA_MAX_SEARCH_STEPS),
                //    float4(offset[0].xz, offset[1].yw));
}

/**
 * Neighborhood Blending Vertex Shader	// 処理内容の変更。
 */
//void SMAANeighborhoodBlendingVS( float2 texcoord,
//                                inout float4 offset )
//{
//	offset = texcoord.xyxy + SMAA_RT_METRICS.xyxy * float4( 1.0, 0.0, 0.0,  1.0);	//mad(SMAA_RT_METRICS.xyxy, float4( 1.0, 0.0, 0.0,  1.0), texcoord.xyxy);
//
//}


void SMAANeighborhoodBlendingVS( float2 texcoord,
                                 inout float4 offset[3] )
{ 
    offset[0] = texcoord.xyxy + SMAA_RT_METRICS.xyxy * float4(-1.0, 0.0, 0.0, -1.0);
    offset[1] = texcoord.xyxy + SMAA_RT_METRICS.xyxy * float4( 1.0, 0.0, 0.0,  1.0);
}


#endif // SMAA_INCLUDE_VS


#ifdef _FRAGMENT_SHADER_
//-----------------------------------------------------------------------------
// Edge Detection Pixel Shaders (First Pass)

/**
 * Luma Edge Detection
 *
 * IMPORTANT NOTICE: luma edge detection requires gamma-corrected colors, and
 * thus 'colorTex' should be a non-sRGB texture.
 */
float2 SMAALumaEdgeDetectionPS(float2 texcoord,
                               float4 offset[3],
                               SMAATexture2D colorTex
                               #if SMAA_PREDICATION
                               , SMAATexture2D predicationTex
                               #endif
                               ) {
    // Calculate the threshold:
    #if SMAA_PREDICATION
    float2 threshold = SMAACalculatePredicatedThreshold(texcoord, offset, predicationTex );
    #else
    float2 threshold = float2(SMAA_THRESHOLD, SMAA_THRESHOLD);
    #endif

    // Calculate lumas:
    float3 weights = float3(0.2126, 0.7152, 0.0722);
    float L = dot(SMAASamplePoint(colorTex, texcoord).rgb, weights);

    float Lleft = dot(SMAASamplePoint(colorTex, offset[0].xy).rgb, weights);
    float Ltop  = dot(SMAASamplePoint(colorTex, offset[0].zw).rgb, weights);

    // We do the usual threshold:
    float4 delta;
    delta.xy = abs(L - float2(Lleft, Ltop));
    float2 edges = step(threshold, delta.xy);

    // Then discard if there is no edge:
    if (dot(edges, float2(1.0, 1.0)) == 0.0)
        discard;

    // Calculate right and bottom deltas:
    float Lright = dot(SMAASamplePoint(colorTex, offset[1].xy).rgb, weights);
    float Lbottom  = dot(SMAASamplePoint(colorTex, offset[1].zw).rgb, weights);
    delta.zw = abs(L - float2(Lright, Lbottom));

    // Calculate the maximum delta in the direct neighborhood:
    float2 maxDelta = max(delta.xy, delta.zw);

    // Calculate left-left and top-top deltas:
    float Lleftleft = dot(SMAASamplePoint(colorTex, offset[2].xy).rgb, weights);
    float Ltoptop = dot(SMAASamplePoint(colorTex, offset[2].zw).rgb, weights);
    delta.zw = abs(float2(Lleft, Ltop) - float2(Lleftleft, Ltoptop));

    // Calculate the final maximum delta:
    maxDelta = max(maxDelta.xy, delta.zw);
    float finalDelta = max(maxDelta.x, maxDelta.y);

    // Local contrast adaptation:
    edges.xy *= step(finalDelta, /*SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR */ delta.xy);

    return edges;
}

/**
 * Color Edge Detection
 *
 * IMPORTANT NOTICE: color edge detection requires gamma-corrected colors, and
 * thus 'colorTex' should be a non-sRGB texture.
 */
float2 SMAAColorEdgeDetectionPS(float2 texcoord,
                                float4 offset[3],
                                SMAATexture2D colorTex
                                #if SMAA_PREDICATION
                                , SMAATexture2D predicationTex
                                #endif
                                ) {
    // Calculate the threshold:
    #if SMAA_PREDICATION
    float2 threshold = SMAACalculatePredicatedThreshold(texcoord, offset, predicationTex);
    #else
    float2 threshold = float2(SMAA_THRESHOLD, SMAA_THRESHOLD);
    #endif

    // Calculate color deltas:
    float4 delta;
    float3 C = SMAASamplePoint(colorTex, texcoord).rgb;

    float3 Cleft = SMAASamplePoint(colorTex, offset[0].xy).rgb;
    float3 t = abs(C - Cleft);
    delta.x = max(max(t.r, t.g), t.b);

    float3 Ctop  = SMAASamplePoint(colorTex, offset[0].zw).rgb;
    t = abs(C - Ctop);
    delta.y = max(max(t.r, t.g), t.b);

    // We do the usual threshold:
    float2 edges = step(threshold, delta.xy);

    // Then discard if there is no edge:
    if (dot(edges, float2(1.0, 1.0)) == 0.0)
        discard;

    // Calculate right and bottom deltas:
    float3 Cright = SMAASamplePoint(colorTex, offset[1].xy).rgb;
    t = abs(C - Cright);
    delta.z = max(max(t.r, t.g), t.b);

    float3 Cbottom  = SMAASamplePoint(colorTex, offset[1].zw).rgb;
    t = abs(C - Cbottom);
    delta.w = max(max(t.r, t.g), t.b);

    // Calculate the maximum delta in the direct neighborhood:
    float2 maxDelta = max(delta.xy, delta.zw);

    // Calculate left-left and top-top deltas:
    float3 Cleftleft  = SMAASamplePoint(colorTex, offset[2].xy).rgb;
    t = abs(C - Cleftleft);
    delta.z = max(max(t.r, t.g), t.b);

    float3 Ctoptop = SMAASamplePoint(colorTex, offset[2].zw).rgb;
    t = abs(C - Ctoptop);
    delta.w = max(max(t.r, t.g), t.b);

    // Calculate the final maximum delta:
    maxDelta = max(maxDelta.xy, delta.zw);
    float finalDelta = max(maxDelta.x, maxDelta.y);

    // Local contrast adaptation:
    edges.xy *= step(finalDelta, SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR * delta.xy);

    return edges;
}

/**
 * Depth Edge Detection
 */
float2 SMAADepthEdgeDetectionPS(float2 texcoord,
                                float4 offset[3],
                                SMAATexture2D depthTex ) {
    float3 neighbours = SMAAGatherNeighbours(texcoord, offset, depthTex );
    float2 delta = abs(neighbours.xx - float2(neighbours.y, neighbours.z));
    float2 edges = step(SMAA_DEPTH_THRESHOLD, delta);

    if (dot(edges, float2(1.0, 1.0)) == 0.0)
        discard;

    return edges;
}

//-----------------------------------------------------------------------------
// Diagonal Search Functions

#if !defined(SMAA_DISABLE_DIAG_DETECTION)

/**
 * Allows to decode two binary values from a bilinear-filtered access.
 */
float2 SMAADecodeDiagBilinearAccess(float2 e) {
    // Bilinear access for fetching 'e' have a 0.25 offset, and we are
    // interested in the R and G edges:
    //
    // +---G---+-------+
    // |   x o R   x   |
    // +-------+-------+
    //
    // Then, if one of these edge is enabled:
    //   Red:   (0.75 * X + 0.25 * 1) => 0.25 or 1.0
    //   Green: (0.75 * 1 + 0.25 * X) => 0.75 or 1.0
    //
    // This function will unpack the values (mad + mul + round):
    // wolframalpha.com: round(x * abs(5 * x - 5 * 0.75)) plot 0 to 1
    e.r = e.r * abs(5.0 * e.r - 5.0 * 0.75);
    return round(e);
}

float4 SMAADecodeDiagBilinearAccess(float4 e) {
    e.rb = e.rb * abs(5.0 * e.rb - 5.0 * 0.75);
    return round(e);
}

/**
 * These functions allows to perform diagonal pattern searches.
 */

float SMAASearchDiag1(SMAATexture2D edgesTex, float2 texcoord, float2 dir, float c)
{
	texcoord += dir * SMAA_RT_METRICS.xy;
	float2 e = float2(0.0, 0.0);
	float i;
	for (i = 0.0; i < float(SMAA_MAX_SEARCH_STEPS_DIAG); i++)
	{
		e.rg = SMAASampleLevelZero(edgesTex, texcoord).rg;
		SMAA_FLATTEN if (dot(e, float2(1.0, 1.0)) < 1.9) break;
		texcoord += dir * SMAA_RT_METRICS.xy;
	}
	return i + float(e.g > 0.9) * c;
}



float SMAASearchDiag2(SMAATexture2D edgesTex, float2 texcoord, float2 dir, float c)
{
	texcoord += dir * SMAA_RT_METRICS.xy;
	float2 e = float2(0.0, 0.0);
	float i;
	for (i = 0.0; i < float(SMAA_MAX_SEARCH_STEPS_DIAG); i++)
	{
		e.g = SMAASampleLevelZero(edgesTex, texcoord).g;
		e.r = SMAASampleLevelZeroOffset(edgesTex, texcoord, int2(1, 0)).r;
		SMAA_FLATTEN if (dot(e, float2(1.0, 1.0)) < 1.9) break;
		texcoord += dir * SMAA_RT_METRICS.xy;
	}
	return i + float(e.g > 0.9) * c;
}




/** 
 * Similar to SMAAArea, this calculates the area corresponding to a certain
 * diagonal distance and crossing edges 'e'.
 */
float2 SMAAAreaDiag(SMAATexture2D areaTex, float2 dist, float2 e, float offset )
{
    float2 texcoord = float(SMAA_AREATEX_MAX_DISTANCE_DIAG) * e + dist;

    // We do a scale and bias for mapping to texel space:
    texcoord = SMAA_AREATEX_PIXEL_SIZE * texcoord + ( 0.5 * SMAA_AREATEX_PIXEL_SIZE ); 

    // Diagonal areas are on the second half of the texture:
    texcoord.x += 0.5;

    // Move to proper place, according to the subpixel offset:
    texcoord.y += SMAA_AREATEX_SUBTEX_SIZE * offset;

    // Do it!
	//return SMAA_AREATEX_SELECT(SMAASampleLevelZero(areaTex, texcoord));
	return SMAASampleLevelZero(areaTex, texcoord).rg;
}

/**
 * This searches for diagonal patterns and returns the corresponding weights.
 */

float2 SMAACalculateDiagWeights(SMAATexture2D edgesTex, SMAATexture2D areaTex, float2 texcoord, float2 e, int4 subsampleIndices)
{
	float2 weights = float2(0.0, 0.0);

	float2 d;
	d.x = e.r > 0.0? SMAASearchDiag1(edgesTex, texcoord, float2(-1.0,  1.0), 1.0) : 0.0;
	d.y = SMAASearchDiag1(edgesTex, texcoord, float2(1.0, -1.0), 0.0);

	SMAA_BRANCH
		if (d.r + d.g > 2.0) // d.r + d.g + 1 > 3
		{
			float4 coords = SMAAMad(float4(-d.r, d.r, d.g, -d.g), SMAA_RT_METRICS.xyxy, texcoord.xyxy);

			float4 c;
			c.x = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(-1,  0)).g;
			c.y = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2( 0,  0)).r;
			c.z = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2( 1,  0)).g;
			c.w = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2( 1, -1)).r;
			float2 e = 2.0 * c.xz + c.yw;
			float t = float(SMAA_MAX_SEARCH_STEPS_DIAG) - 1.0;
			e *= step(d.rg, float2(t, t));

			weights += SMAAAreaDiag(areaTex, d, e, float(subsampleIndices.z));
		}

	d.x = SMAASearchDiag2(edgesTex, texcoord, float2(-1.0, -1.0), 0.0);
	float right = SMAASampleLevelZeroOffset(edgesTex, texcoord, int2(1, 0)).r;
	d.y = right > 0.0? SMAASearchDiag2(edgesTex, texcoord, float2(1.0, 1.0), 1.0) : 0.0;

	SMAA_BRANCH
		if (d.r + d.g > 2.0) // d.r + d.g + 1 > 3
		{
			float4 coords = SMAAMad(float4(-d.r, -d.r, d.g, d.g), SMAA_RT_METRICS.xyxy, texcoord.xyxy);

			float4 c;
			c.x  = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(-1,  0)).g;
			c.y  = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2( 0, -1)).r;
			c.zw = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2( 1,  0)).gr;
			float2 e = 2.0 * c.xz + c.yw;
			float t = float(SMAA_MAX_SEARCH_STEPS_DIAG) - 1.0;
			e *= step(d.rg, float2(t, t));

			weights += SMAAAreaDiag(areaTex, d, e, float(subsampleIndices.w)).gr;
		}

	return weights;
}



#endif

//-----------------------------------------------------------------------------
// Horizontal/Vertical Search Functions

/**
 * This allows to determine how much length should we add in the last step
 * of the searches. It takes the bilinearly interpolated edge (see 
 * @PSEUDO_GATHER4), and adds 0, 1 or 2, depending on which edges and
 * crossing edges are active.
 */

float SMAASearchLength(SMAATexture2D searchTex, float2 e, float bias, float scale)
{
	// Not required if searchTex accesses are set to point:
	// float2 SEARCH_TEX_PIXEL_SIZE = 1.0 / float2(66.0, 33.0);
	// e = float2(bias, 0.0) + 0.5 * SEARCH_TEX_PIXEL_SIZE + 
	//     e * float2(scale, 1.0) * float2(64.0, 32.0) * SEARCH_TEX_PIXEL_SIZE;
	e.r = bias + e.r * scale;
	e.g = -e.g;
	return 255.0 * SMAASampleLevelZeroPoint(searchTex, e).r;
}



/**
 * Horizontal/vertical search functions for the 2nd pass.
 */

float SMAASearchXLeft(SMAATexture2D edgesTex, SMAATexture2D searchTex, float2 texcoord, float end)
{
    /**
     * @PSEUDO_GATHER4
     * This texcoord has been offset by (-0.25, -0.125) in the vertex shader to
     * sample between edge, thus fetching four edges in a row.
     * Sampling with different offsets in each direction allows to disambiguate
     * which edges are active from the four fetched ones.
     */
    float2 e = float2(0.0, 1.0);
    while (texcoord.x > end && 
           e.g > 0.8281 && // Is there some edge not activated?
           e.r == 0.0) { // Or is there a crossing edge that breaks the line?
        e = SMAASampleLevelZero(edgesTex, texcoord).rg;
        texcoord -= float2(2.0, 0.0) * SMAA_RT_METRICS.xy;
    }

    // We correct the previous (-0.25, -0.125) offset we applied:
    texcoord.x += 0.25 * SMAA_RT_METRICS.x;

    // The searches are bias by 1, so adjust the coords accordingly:
    texcoord.x += SMAA_RT_METRICS.x;

    // Disambiguate the length added by the last step:
    texcoord.x += 2.0 * SMAA_RT_METRICS.x; // Undo last step
    texcoord.x -= SMAA_RT_METRICS.x * SMAASearchLength(searchTex, e, 0.0, 0.5);

    return texcoord.x;
}

float SMAASearchXRight(SMAATexture2D edgesTex, SMAATexture2D searchTex, float2 texcoord, float end)
{
    float2 e = float2(0.0, 1.0);
    while (texcoord.x < end && 
           e.g > 0.8281 && // Is there some edge not activated?
           e.r == 0.0) { // Or is there a crossing edge that breaks the line?
        e = SMAASampleLevelZero(edgesTex, texcoord).rg;
        texcoord += float2(2.0, 0.0) * SMAA_RT_METRICS.xy;
    }

    texcoord.x -= 0.25 * SMAA_RT_METRICS.x;
    texcoord.x -= SMAA_RT_METRICS.x;
    texcoord.x -= 2.0 * SMAA_RT_METRICS.x;
    texcoord.x += SMAA_RT_METRICS.x * SMAASearchLength(searchTex, e, 0.5, 0.5);
    return texcoord.x;
}

float SMAASearchYUp(SMAATexture2D edgesTex, SMAATexture2D searchTex, float2 texcoord, float end)
{
    float2 e = float2(1.0, 0.0);
    while (texcoord.y > end && 
           e.r > 0.8281 && // Is there some edge not activated?
           e.g == 0.0) { // Or is there a crossing edge that breaks the line?
        e = SMAASampleLevelZero(edgesTex, texcoord).rg;
        texcoord -= float2(0.0, 2.0) * SMAA_RT_METRICS.xy;
    }

    texcoord.y += 0.25 * SMAA_RT_METRICS.y;
    texcoord.y += SMAA_RT_METRICS.y;
    texcoord.y += 2.0 * SMAA_RT_METRICS.y;
    texcoord.y -= SMAA_RT_METRICS.y * SMAASearchLength(searchTex, e.gr, 0.0, 0.5);
    return texcoord.y;
}

float SMAASearchYDown(SMAATexture2D edgesTex, SMAATexture2D searchTex, float2 texcoord, float end)
{
    float2 e = float2(1.0, 0.0);
    while (texcoord.y < end && 
           e.r > 0.8281 && // Is there some edge not activated?
           e.g == 0.0) { // Or is there a crossing edge that breaks the line?
        e = SMAASampleLevelZero(edgesTex, texcoord).rg;
        texcoord += float2(0.0, 2.0) * SMAA_RT_METRICS.xy;
    }
    
    texcoord.y -= 0.25 * SMAA_RT_METRICS.y;
    texcoord.y -= SMAA_RT_METRICS.y;
    texcoord.y -= 2.0 * SMAA_RT_METRICS.y;
    texcoord.y += SMAA_RT_METRICS.y * SMAASearchLength(searchTex, e.gr, 0.5, 0.5);
    return texcoord.y;
}










/** 
 * Ok, we have the distance and both crossing edges. So, what are the areas
 * at each side of current edge?
 */
float2 SMAAArea( SMAATexture2D areaTex, float2 dist, float e1, float e2, float offset )
{
    // Rounding prevents precision errors of bilinear filtering:
    float2 texcoord = dist + SMAA_AREATEX_MAX_DISTANCE * round(4.0 * float2(e1, e2));	//mad(float2(SMAA_AREATEX_MAX_DISTANCE, SMAA_AREATEX_MAX_DISTANCE), round(4.0 * float2(e1, e2)), dist);
    
    // We do a scale and bias for mapping to texel space:
    texcoord = 0.5 * SMAA_AREATEX_PIXEL_SIZE + SMAA_AREATEX_PIXEL_SIZE * texcoord;	//mad(SMAA_AREATEX_PIXEL_SIZE, texcoord, 0.5 * SMAA_AREATEX_PIXEL_SIZE);

    // Move to proper place, according to the subpixel offset:
    texcoord.y = texcoord.y + SMAA_AREATEX_SUBTEX_SIZE * offset;	//mad(SMAA_AREATEX_SUBTEX_SIZE, offset, texcoord.y);

    // Do it!
    return SMAA_AREATEX_SELECT(SMAASampleLevelZero(areaTex, texcoord));
}

//-----------------------------------------------------------------------------
// Corner Detection Functions

void SMAADetectHorizontalCornerPattern(SMAATexture2D edgesTex, inout float2 weights, float2 texcoord, float2 d)
{
#if SMAA_CORNER_ROUNDING < 100 || SMAA_FORCE_CORNER_DETECTION == 1
	float4 coords = SMAAMad(float4(d.x, 0.0, d.y, 0.0),
		SMAA_RT_METRICS.xyxy, texcoord.xyxy);
	float2 e;
	e.r = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(0.0,  1.0)).r;
	bool left = abs(d.x) < abs(d.y);
	e.g = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(0.0, -2.0)).r;
	if (left) weights *= saturate(float(SMAA_CORNER_ROUNDING) / 100.0 + 1.0 - e);

	e.r = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2(1.0,  1.0)).r;
	e.g = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2(1.0, -2.0)).r;
	if (!left) weights *= saturate(float(SMAA_CORNER_ROUNDING) / 100.0 + 1.0 - e);
#endif
}


void SMAADetectVerticalCornerPattern(SMAATexture2D edgesTex, inout float2 weights, float2 texcoord, float2 d)
{
#if SMAA_CORNER_ROUNDING < 100 || SMAA_FORCE_CORNER_DETECTION == 1
	float4 coords = SMAAMad(float4(0.0, d.x, 0.0, d.y),
		SMAA_RT_METRICS.xyxy, texcoord.xyxy);
	float2 e;
	e.r = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2( 1.0, 0.0)).g;
	bool left = abs(d.x) < abs(d.y);
	e.g = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(-2.0, 0.0)).g;
	if (left) weights *= saturate(float(SMAA_CORNER_ROUNDING) / 100.0 + 1.0 - e);

	e.r = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2( 1.0, 1.0)).g;
	e.g = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2(-2.0, 1.0)).g;
	if (!left) weights *= saturate(float(SMAA_CORNER_ROUNDING) / 100.0 + 1.0 - e);
#endif
}



//-----------------------------------------------------------------------------
// Blending Weight Calculation Pixel Shader (Second Pass)

float4 SMAABlendingWeightCalculationPS(float2 texcoord,
                                       float2 pixcoord,
                                       float4 offset[3],
                                       SMAATexture2D edgesTex, 
                                       SMAATexture2D areaTex, 
                                       SMAATexture2D searchTex,
                                       int4 subsampleIndices) { // Just pass zero for SMAA 1x, see @SUBSAMPLE_INDICES.
    float4 weights = float4(0.0, 0.0, 0.0, 0.0);
	
    float2 e = SMAASample(edgesTex, texcoord).rg;

    SMAA_BRANCH
    if (e.g > 0.0) { // Edge at north
        #if !defined(SMAA_DISABLE_DIAG_DETECTION)//#if SMAA_MAX_SEARCH_STEPS_DIAG > 0 || SMAA_FORCE_DIAGONAL_DETECTION == 1
        // Diagonals have both north and west edges, so searching for them in
        // one of the boundaries is enough.
        weights.rg = SMAACalculateDiagWeights(edgesTex, areaTex, texcoord, e, subsampleIndices);

        // We give priority to diagonals, so if we find a diagonal we skip 
        // horizontal/vertical processing.
        SMAA_BRANCH
        if (dot(weights.rg, float2(1.0, 1.0)) == 0.0) {
        #endif

        float2 d;

        // Find the distance to the left:
        float2 coords;
        coords.x = SMAASearchXLeft(edgesTex, searchTex, offset[0].xy, offset[2].x);
        coords.y = offset[1].y; // offset[1].y = texcoord.y - 0.25 * SMAA_RT_METRICS.y (@CROSSING_OFFSET)
        d.x = coords.x;

        // Now fetch the left crossing edges, two at a time using bilinear
        // filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
        // discern what value each edge has:
        float e1 = SMAASampleLevelZero(edgesTex, coords).r;

        // Find the distance to the right:
        coords.x = SMAASearchXRight(edgesTex, searchTex, offset[0].zw, offset[2].y);
        d.y = coords.x;

        // We want the distances to be in pixel units (doing this here allow to
        // better interleave arithmetic and memory accesses):
        d = d / SMAA_RT_METRICS.x - pixcoord.x;

        // SMAAArea below needs a sqrt, as the areas texture is compressed 
        // quadratically:
        float2 sqrt_d = sqrt(abs(d));

        // Fetch the right crossing edges:
        float e2 = SMAASampleLevelZeroOffset(edgesTex, coords, int2(1, 0)).r;

        // Ok, we know how this pattern looks like, now it is time for getting
        // the actual area:
        weights.rg = SMAAArea(areaTex, sqrt_d, e1, e2, float(subsampleIndices.y));

        // Fix corners:
        SMAADetectHorizontalCornerPattern(edgesTex, weights.rg, texcoord, d);

        #if !defined(SMAA_DISABLE_DIAG_DETECTION)//#if SMAA_MAX_SEARCH_STEPS_DIAG > 0 || SMAA_FORCE_DIAGONAL_DETECTION == 1
        } else
            e.r = 0.0; // Skip vertical processing.
        #endif
    }

    SMAA_BRANCH
    if (e.r > 0.0) { // Edge at west
        float2 d;

        // Find the distance to the top:
        float2 coords;
        coords.y = SMAASearchYUp(edgesTex, searchTex, offset[1].xy, offset[2].z);
        coords.x = offset[0].x; // offset[1].x = texcoord.x - 0.25 * SMAA_RT_METRICS.x;
        d.x = coords.y;

        // Fetch the top crossing edges:
        float e1 = SMAASampleLevelZero(edgesTex, coords).g;

        // Find the distance to the bottom:
        coords.y = SMAASearchYDown(edgesTex, searchTex, offset[1].zw, offset[2].w);
        d.y = coords.y;

        // We want the distances to be in pixel units:
        d = d / SMAA_RT_METRICS.y - pixcoord.y;

        // SMAAArea below needs a sqrt, as the areas texture is compressed 
        // quadratically:
        float2 sqrt_d = sqrt(abs(d));

        // Fetch the bottom crossing edges:
        float e2 = SMAASampleLevelZeroOffset(edgesTex, coords, int2(0, 1)).g;

        // Get the area for this direction:
        weights.ba = SMAAArea(areaTex, sqrt_d, e1, e2, float(subsampleIndices.x));

        // Fix corners:
        SMAADetectVerticalCornerPattern(edgesTex, weights.ba, texcoord, d);
    }
	
    return weights;
}

//-----------------------------------------------------------------------------
// Neighborhood Blending Pixel Shader (Third Pass)

float4 SMAANeighborhoodBlendingPS(float2 texcoord,
                                  float4 offset[3],
                                  SMAATexture2D colorTex,
                                  SMAATexture2D blendTex)
{
    // Fetch the blending weights for current pixel:
    float4 a;
    a.xz = SMAASample(blendTex, texcoord).xz;
    a.y = SMAASample(blendTex, offset[1].zw).g;
    a.w = SMAASample(blendTex, offset[1].xy).a;

    // Is there any blending weight with a value greater than 0.0?
    SMAA_BRANCH
    if (dot(a, float4(1.0, 1.0, 1.0, 1.0)) < 1e-5)
        return SMAASampleLevelZero(colorTex, texcoord);
    else {
        float4 color = float4(0.0, 0.0, 0.0, 0.0);

        // Up to 4 lines can be crossing a pixel (one through each edge). We
        // favor blending by choosing the line with the maximum weight for each
        // direction:
        float2 offset;
        offset.x = a.a > a.b? a.a : -a.b; // left vs. right 
        offset.y = a.g > a.r? a.g : -a.r; // top vs. bottom

        // Then we go in the direction that has the maximum weight:
        if (abs(offset.x) > abs(offset.y)) // horizontal vs. vertical
            offset.y = 0.0;
        else
            offset.x = 0.0;

        #if SMAA_REPROJECTION == 1
        // Fetch the opposite color and lerp by hand:
        float4 C = SMAASampleLevelZero(colorTex, texcoord);
        texcoord += sign(offset) * SMAA_RT_METRICS.xy;
        float4 Cop = SMAASampleLevelZero(colorTex, texcoord);
        float s = abs(offset.x) > abs(offset.y)? abs(offset.x) : abs(offset.y);

        // Unpack the velocity values:
        C.a *= C.a;
        Cop.a *= Cop.a;

        // Lerp the colors:
        float4 Caa = lerp(C, Cop, s);

        // Unpack velocity and return the resulting value:
        Caa.a = sqrt(Caa.a);
        return Caa;
        #elif SMAA_HLSL_4 == 1 || SMAA_DIRECTX9_LINEAR_BLEND == 0
        // We exploit bilinear filtering to mix current pixel with the chosen
        // neighbor:
        texcoord += offset * SMAA_RT_METRICS.xy;
        return SMAASampleLevelZero(colorTex, texcoord);
        #else
        // Fetch the opposite color and lerp by hand:
        float4 C = SMAASampleLevelZero(colorTex, texcoord);
        texcoord += sign(offset) * SMAA_RT_METRICS.xy;
        float4 Cop = SMAASampleLevelZero(colorTex, texcoord);
        float s = abs(offset.x) > abs(offset.y)? abs(offset.x) : abs(offset.y);
        return lerp(C, Cop, s);
        #endif
    }
}








//-----------------------------------------------------------------------------
// Temporal Resolve Pixel Shader (Optional Pass)

float4 SMAAResolvePS(float2 texcoord,
	SMAATexture2D colorTexCurr,
	SMAATexture2D colorTexPrev
#if SMAA_REPROJECTION == 1
	, SMAATexture2D velocityTex
#endif
	) {
#if SMAA_REPROJECTION == 1
	// Velocity is calculated from previous to current position, so we need to
	// inverse it:
	float2 velocity = -SMAASample(velocityTex, texcoord).rg;

	// Fetch current pixel:
	float4 current = SMAASample(colorTexCurr, texcoord);

	// Reproject current coordinates and fetch previous pixel:
	float4 previous = SMAASample(colorTexPrev, texcoord + velocity);

	// Attenuate the previous pixel if the velocity is different:
	float delta = abs(current.a * current.a - previous.a * previous.a) / 5.0;
	float weight = 0.5 * saturate(1.0 - (sqrt(delta) * SMAA_REPROJECTION_WEIGHT_SCALE));

	// Blend the pixels according to the calculated weight:
	return lerp(current, previous, weight);
#else
	// Just blend the pixels:
	float4 current = SMAASample(colorTexCurr, texcoord);
	float4 previous = SMAASample(colorTexPrev, texcoord);
	return lerp(current, previous, 0.5);
#endif
}


//-----------------------------------------------------------------------------
// Separate Multisamples Pixel Shader (Optional Pass)

#ifdef SMAALoad
void SMAASeparatePS(float4 position,
                    float2 texcoord,
                    inout float4 target0,
                    inout float4 target1,
                    SMAATexture2DMS2(colorTexMS)) {
    int2 pos = int2(position.xy);
    target0 = SMAALoad(colorTexMS, pos, 0);
    target1 = SMAALoad(colorTexMS, pos, 1);
}
#endif

//-----------------------------------------------------------------------------
#endif // SMAA_INCLUDE_PS









































#ifdef _VERTEX_SHADER_


// Static Functions



// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Unfiorm Locations


// Vertex Shader Output
noperspective out vec2	vTexCoord;
noperspective out vec2	vPixcoord;
noperspective out vec4	offset[3];


// Vertex Shader
void main(void)
{
	gl_Position = vec4( POSITION, 1.0 );//
	vTexCoord	= TEXCOORD;

	if( g_Pass==0 )			SMAAEdgeDetectionVS( vTexCoord, offset );
	else if( g_Pass==1 )	SMAABlendingWeightCalculationVS( vTexCoord, vPixcoord, offset );
	else					SMAANeighborhoodBlendingVS( vTexCoord, offset );
}


#endif




#ifdef _FRAGMENT_SHADER_



// Unfiorm Locations



// Static Functions




// Fragment Shader Input
noperspective in vec2	vTexCoord;
noperspective in vec2	vPixcoord;
noperspective in vec4	offset[3];


// Fragment Shader Output
out vec4	outColor;


// Fragment Shader
void main (void)
{	
	if( g_Pass==0 )			outColor	= vec4( SMAALumaEdgeDetectionPS( vTexCoord, offset, uInpTex ), 0.0, 0.0 );
	else if( g_Pass==1 )	outColor	= SMAABlendingWeightCalculationPS( vTexCoord, vPixcoord, offset, uInpTex, uInpTex2, uInpTex3, ivec4(0) );
	else					outColor	= SMAANeighborhoodBlendingPS( vTexCoord, offset, uInpTex, uInpTex2 );

	//else					outColor	= SMAABlendingWeightCalculationPS( vTexCoord, vPixcoord, offset, uInpTex, uInpTex2, uInpTex3, ivec4(0) );
	//outColor	= vec4( SMAALumaEdgeDetectionPS( vTexCoord, offset, uInpTex ), 0.0, 0.0 );
}

#endif


