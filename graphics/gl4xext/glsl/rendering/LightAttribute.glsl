#ifndef	LIGHT_ATTRIBUTE_GLSL
#define	LIGHT_ATTRIBUTE_GLSL


#define	MAX_LIGHTS	1024


// Light Type Definition
#define LIGHT_TYPE_POINT		0
#define	LIGHT_TYPE_DIRECTIONAL	1
#define	LIGHT_TYPE_SPOT			2
#define	LIGHT_TYPE_AREA			3



// LightAttribute's bitmask (g_PackedPosAttrib.w)
#define	BITMASK_LIGHTTYPE			0xE0000000	// LightType:29-31[bit]				//11100000000000000000000000000000
#define	BITMASK_EMITDIFFUSE			0x10000000	// EmitDiffuse:28[bit]				//00010000000000000000000000000000
#define	BITMASK_EMITSPECULAR		0x08000000	// EmitSpecular:27[bit]				//00001000000000000000000000000000
#define	BITMASK_NUMSHADOWS			0x07C00000	// numShadows:22-26[bit]			//00000111110000000000000000000000
#define	BITMASK_SHADOWCASTER_VALID	0x00200000	// shadowcaster_idx validity:21[bit]//00000000001000000000000000000000
#define	BITMASK_SHADOWCASTER_IDX	0x001FF800	// shadowcaster_idx:11-20[bit]		//00000000000111111111100000000000
#define	BITMASK_LIGHTMAT_VALID		0x00000400	// lightmatrix_idx vaidity:10[bit]	//00000000000000000000010000000000
#define	BITMASK_LIGHTMAT_IDX		0x000003FF	// lightmatrix_idx:0-9[bit]			//00000000000000000000001111111111

// Bit-shift amount for attribute param extraction
#define	BITSHIFT_LIGHTTYPE			29	// LightType:29-31[bit]
#define	BITSHIFT_EMITDIFFUSE		28	// EmitDiffuse:28[bit]
#define	BITSHIFT_EMITSPECULAR		27	// EmitSpecular:27[bit]
#define	BITSHIFT_NUMSHADOWS			22	// numShadows:22-26[bits]
#define	BITSHIFT_SHADOWCASTER_VALID	21	// shadowcaster_idx validity:21[bit]
#define	BITSHIFT_SHADOWCASTER_IDX	11	// shadowcaster_idx:11-20[bit]
#define	BITSHIFT_LIGHTMAT_IDX_VALID	10	// lightmatrix_idx vaidity:10[bit]
#define	BITSHIFT_LIGHTMAT_IDX		0	// lightmatrix_idx:0-9[bit]




// xyz:Position, w:LightAttribute
layout( std140 ) uniform ubPackedPosAttrib
{
	vec4	g_PackedPosAttrib[ MAX_LIGHTS ];
};

// xyz:ColorIndensity, w:Dropoff for spotlight
layout( std140 ) uniform ubPackedColorDropoff
{
	vec4	g_PackedColorDropoff[ MAX_LIGHTS ];
};

// xyz:Direction, w:ConeAngle for spotlight
layout( std140 ) uniform ubPackedDirConeAngle
{
	vec4	g_PackedDirConeAngle[ MAX_LIGHTS ];
};

// x:ConstantAttenuation, y:LinearAttenuation, z:QuadraticAttenuation, w:LightRadius
layout( std140 ) uniform ubPackedAttenRadius
{
	vec4	g_PackedAttenRadius[ MAX_LIGHTS ];
};

// Light View Projection Matrix
//layout( std140 ) uniform ubMatLightViewProj
//{
//	vec4	g_MatLightViewProj[ MAX_LIGHTS ];
//};


// Macros for light attribute extraction

#define	g_LightPosition( light_id )			g_PackedPosAttrib[light_id].xyz
#define	g_LightType( light_id )				int( ( floatBitsToUint(g_PackedPosAttrib[light_id].w) & BITMASK_LIGHTTYPE ) >> BITSHIFT_LIGHTTYPE )
#define	g_EmitDiffuse( light_id )			bool( ( floatBitsToUint(g_PackedPosAttrib[light_id].w) & BITMASK_EMITDIFFUSE ) )
#define	g_EmitSpecular( light_id )			bool( ( floatBitsToUint(g_PackedPosAttrib[light_id].w) & BITMASK_EMITSPECULAR ) )
#define	g_NumShadows( light_id )			int( ( floatBitsToUint(g_PackedPosAttrib[light_id].w) & BITMASK_NUMSHADOWS ) >> BITSHIFT_NUMSHADOWS )
#define	g_ShadowCasterValid( light_id )		bool( ( floatBitsToUint(g_PackedPosAttrib[light_id].w) & BITMASK_SHADOWCASTER_VALID ) )
#define	g_ShadowCasterIndex( light_id )		int( ( floatBitsToUint(g_PackedPosAttrib[light_id].w) & BITMASK_SHADOWCASTER_IDX ) >> BITSHIFT_SHADOWCASTER_IDX )
#define	g_LightMatValid( light_id )			bool( ( floatBitsToUint(g_PackedPosAttrib[light_id].w) & BITMASK_LIGHTMAT_VALID ) )
#define	g_LightMatIndex( light_id )			int( ( floatBitsToUint(g_PackedPosAttrib[light_id].w) & BITMASK_LIGHTMAT_IDX ) )

#define g_LightIntensity( light_id )		g_PackedColorDropoff[light_id].xyz
#define g_LightDropoff( light_id )			g_PackedColorDropoff[light_id].w

#define	g_LightDir( light_id )				-g_PackedDirConeAngle[light_id].xyz
#define	g_LightConeAngle( light_id )		g_PackedDirConeAngle[light_id].w

#define	g_LightAttenConstant( light_id )	g_PackedAttenRadius[light_id].x
#define	g_LightAttenLinear( light_id )		g_PackedAttenRadius[light_id].y
#define	g_LightAttenQuad( light_id )		g_PackedAttenRadius[light_id].z
#define	g_LightRadius( light_id )			g_PackedAttenRadius[light_id].w




#endif	// LIGHT_ATTRIBUTE_GLSL //
