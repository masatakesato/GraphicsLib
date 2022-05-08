#ifndef	BUFFER_LAYOUT_H
#define	BUFFER_LAYOUT_H

#include	<oreore/mathlib/GraphicsMath.h>

#include	"Buffer.h"




namespace OreOreLib
{

	const Vec3f X_AXIS ={ 1.0f, 0.0f, 0.0f };
	const Vec3f Y_AXIS ={ 0.0f, 1.0f, 0.0f };
	const Vec3f Z_AXIS ={ 0.0f, 0.0f, 1.0f };



	//enum BASIS_TYPE
	//{
	//	BASIS_XYZ,
	//	BASIS_XZY,
	//	BASIS_YXZ,
	//	BASIS_YZX,
	//	BASIS_ZXY,
	//	BASIS_ZYX,
	//	NUM_BASIS_TYPE,
	//};



	enum COLOR_CHANNEL
	{
		COLOR_R,
		COLOR_G,
		COLOR_B,
		COLOR_A,
	};





	// data format
	enum DATA_FORMAT
	{
		FORMAT_UNKNOWN                     = 0,
		FORMAT_R32G32B32A32_TYPELESS       = 1,
		FORMAT_R32G32B32A32_FLOAT          = 2,
		FORMAT_R32G32B32A32_UINT           = 3,
		FORMAT_R32G32B32A32_SINT           = 4,
		FORMAT_R32G32B32_TYPELESS          = 5,
		FORMAT_R32G32B32_FLOAT             = 6,
		FORMAT_R32G32B32_UINT              = 7,
		FORMAT_R32G32B32_SINT              = 8,
		FORMAT_R16G16B16A16_TYPELESS       = 9,
		FORMAT_R16G16B16A16_FLOAT          = 10,
		FORMAT_R16G16B16A16_UNORM          = 11,
		FORMAT_R16G16B16A16_UINT           = 12,
		FORMAT_R16G16B16A16_SNORM          = 13,
		FORMAT_R16G16B16A16_SINT           = 14,
		FORMAT_R32G32_TYPELESS             = 15,
		FORMAT_R32G32_FLOAT                = 16,
		FORMAT_R32G32_UINT                 = 17,
		FORMAT_R32G32_SINT                 = 18,
		FORMAT_R32G8X24_TYPELESS           = 19,
		FORMAT_D32_FLOAT_S8X24_UINT        = 20,
		FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
		FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
		FORMAT_R10G10B10A2_TYPELESS        = 23,
		FORMAT_R10G10B10A2_UNORM           = 24,
		FORMAT_R10G10B10A2_UINT            = 25,
		FORMAT_R11G11B10_FLOAT             = 26,
		FORMAT_R8G8B8A8_TYPELESS           = 27,
		FORMAT_R8G8B8A8_UNORM              = 28,
		FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
		FORMAT_R8G8B8A8_UINT               = 30,
		FORMAT_R8G8B8A8_SNORM              = 31,
		FORMAT_R8G8B8A8_SINT               = 32,
		FORMAT_R16G16_TYPELESS             = 33,
		FORMAT_R16G16_FLOAT                = 34,
		FORMAT_R16G16_UNORM                = 35,
		FORMAT_R16G16_UINT                 = 36,
		FORMAT_R16G16_SNORM                = 37,
		FORMAT_R16G16_SINT                 = 38,
		FORMAT_R32_TYPELESS                = 39,
		FORMAT_D32_FLOAT                   = 40,
		FORMAT_R32_FLOAT                   = 41,
		FORMAT_R32_UINT                    = 42,
		FORMAT_R32_SINT                    = 43,
		FORMAT_R24G8_TYPELESS              = 44,
		FORMAT_D24_UNORM_S8_UINT           = 45,
		FORMAT_R24_UNORM_X8_TYPELESS       = 46,
		FORMAT_X24_TYPELESS_G8_UINT        = 47,
		FORMAT_R8G8_TYPELESS               = 48,
		FORMAT_R8G8_UNORM                  = 49,
		FORMAT_R8G8_UINT                   = 50,
		FORMAT_R8G8_SNORM                  = 51,
		FORMAT_R8G8_SINT                   = 52,
		FORMAT_R16_TYPELESS                = 53,
		FORMAT_R16_FLOAT                   = 54,
		FORMAT_D16_UNORM                   = 55,
		FORMAT_R16_UNORM                   = 56,
		FORMAT_R16_UINT                    = 57,
		FORMAT_R16_SNORM                   = 58,
		FORMAT_R16_SINT                    = 59,
		FORMAT_R8_TYPELESS                 = 60,
		FORMAT_R8_UNORM                    = 61,
		FORMAT_R8_UINT                     = 62,
		FORMAT_R8_SNORM                    = 63,
		FORMAT_R8_SINT                     = 64,
		FORMAT_A8_UNORM                    = 65,
		FORMAT_R1_UNORM                    = 66,
		FORMAT_R9G9B9E5_SHAREDEXP          = 67,
		FORMAT_R8G8_B8G8_UNORM             = 68,
		FORMAT_G8R8_G8B8_UNORM             = 69,
		FORMAT_BC1_TYPELESS                = 70,
		FORMAT_BC1_UNORM                   = 71,
		FORMAT_BC1_UNORM_SRGB              = 72,
		FORMAT_BC2_TYPELESS                = 73,
		FORMAT_BC2_UNORM                   = 74,
		FORMAT_BC2_UNORM_SRGB              = 75,
		FORMAT_BC3_TYPELESS                = 76,
		FORMAT_BC3_UNORM                   = 77,
		FORMAT_BC3_UNORM_SRGB              = 78,
		FORMAT_BC4_TYPELESS                = 79,
		FORMAT_BC4_UNORM                   = 80,
		FORMAT_BC4_SNORM                   = 81,
		FORMAT_BC5_TYPELESS                = 82,
		FORMAT_BC5_UNORM                   = 83,
		FORMAT_BC5_SNORM                   = 84,
		FORMAT_B5G6R5_UNORM                = 85,
		FORMAT_B5G5R5A1_UNORM              = 86,
		FORMAT_B8G8R8A8_UNORM              = 87,
		FORMAT_B8G8R8X8_UNORM              = 88,
		FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
		FORMAT_B8G8R8A8_TYPELESS           = 90,
		FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
		FORMAT_B8G8R8X8_TYPELESS           = 92,
		FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
		FORMAT_BC6H_TYPELESS               = 94,
		FORMAT_BC6H_UF16                   = 95,
		FORMAT_BC6H_SF16                   = 96,
		FORMAT_BC7_TYPELESS                = 97,
		FORMAT_BC7_UNORM                   = 98,
		FORMAT_BC7_UNORM_SRGB              = 99,
		FORMAT_AYUV                        = 100,
		FORMAT_Y410                        = 101,
		FORMAT_Y416                        = 102,
		FORMAT_NV12                        = 103,
		FORMAT_P010                        = 104,
		FORMAT_P016                        = 105,
		FORMAT_420_OPAQUE                  = 106,
		FORMAT_YUY2                        = 107,
		FORMAT_Y210                        = 108,
		FORMAT_Y216                        = 109,
		FORMAT_NV11                        = 110,
		FORMAT_AI44                        = 111,
		FORMAT_IA44                        = 112,
		FORMAT_P8                          = 113,
		FORMAT_A8P8                        = 114,
		FORMAT_B4G4R4A4_UNORM              = 115,

		FORMAT_R16G16B16_FLOAT				= 116,
		FORMAT_R16G16B16_UNORM				= 117,
		FORMAT_R16G16B16_UINT				= 118,
		FORMAT_R16G16B16_SNORM				= 119,
		FORMAT_R16G16B16_SINT				= 120,

		FORMAT_R8G8B8_UNORM				= 121,
		FORMAT_R8G8B8_UINT					= 122,
		FORMAT_R8G8B8_SNORM				= 123,
		FORMAT_R8G8B8_SINT					= 124,

		FORMAT_B8G8R8_UNORM					= 125,

		FORMAT_FORCE_UINT                  = 0xffffffffUL,
	};



	// Texture Address Mode
	enum TEXTURE_ADDRESS_MODE
	{
		TEX_ADDRESS_WRAP         = 1,
		TEX_ADDRESS_MIRROR       = 2,
		TEX_ADDRESS_CLAMP        = 3,
		TEX_ADDRESS_BORDER       = 4,
		//D3D11_TEXTURE_ADDRESS_MIRROR_ONCE  = 5
	};



	// Filter Mode
	enum FILTER_MODE
	{
		FILTER_MAG_NEAREST_MIN_NEAREST_MIPMAP_NEAREST,
		FILTER_MAG_NEAREST_MIN_LINEAR_MIPMAP_NEAREST,
		FILTER_MAG_NEAREST_MIN_NEAREST_MIPMAP_LINEAR,
		FILTER_MAG_NEAREST_MIN_LINEAR_MIPMAP_LINEAR,
		FILTER_MAG_MIN_NEAREST,
		FILTER_MAG_NEAREST_MIN_LINEAR,
		FILTER_MAG_LINEAR_MIN_NEAREST_MIPMAP_NEAREST,
		FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_NEAREST,
		FILTER_MAG_LINEAR_MIN_NEAREST_MIPMAP_LINEAR,
		FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_LINEAR,
		FILTER_MAG_LINEAR_MIN_NEAREST,
		FILTER_MAG_MIN_LINEAR,
	};



	// buffer type
	//enum BUFFER_TYPE
	//{
	//	BUF_UNKNOWN					= -1,
	//	BUF_VERTEX					,
	//	BUF_INDEX					,
	//	BUF_TEXTURE					,
	//	BUF_COMMON_MATERIAL			,
	//	NUM_BUF_TYPE,
	//};



	enum ATTRIB_OPTION
	{
		VISIBLE				= 1,
		INVISIBLE			= 0,
		ENABLE				= 1,
		DISABLE				= 0,
		CAST_SHADOW_ON		= 1,
		CAST_SHADOW_OFF		= 0,
		RECEIVE_SHADOW_ON	= 1,
		RECEIVE_SHADOW_OFF	= 0,
		INSTANCE_ON			= 1,
		INSTANCE_OFF		= 0,
		TRANSPARENT_ON		= 1,
		TRANSPARENT_OFF		= 0,
	};



	// vertex layout
	enum VERTEX_LAYOUT
	{
		VERTEX_LAYOUT_UNKNOWN	= -1,
		VERTEX_LAYOUT_3D		= 0,
		VERTEX_LAYOUT_3D_EXT	= 1,
		VERTEX_LAYOUT_2D		= 2,
		VERTEX_LAYOUT_2D_EXT	= 3,
	};



	// primitive type
	enum PRIMITIVE_TYPE
	{
		PRIM_UNKNOWN		= -1,
		PRIM_POINT,
		PRIM_LINE,
		PRIM_TRIANGLE,
		PRIM_TRIANGLE_PATCH,
		PRIM_QUAD_PATCH,
		NUM_PRIM_TYPE,
	};



	// material type
	enum MATERIAL_TYPE
	{
		MATERIAL_TYPE_UNKNOWN		= -1,
		MATERIAL_TYPE_OPAQUE,
		MATERIAL_TYPE_TRANSPARENT,
		MATERIAL_TYPE_TRANSLUCENT,
		MATERIAL_TYPE_VOLUMETRIC,

		NUM_MATERIAL_TYPE,
	};



	// lightsource type
	enum LIGHT_TYPE
	{
		LIGHT_TYPE_UNKNOWN		= -1,
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_DIRECTIONAL,
		LIGHT_TYPE_SPOT,
		LIGHT_TYPE_AREA,
		NUM_LIGHT_TYPE,
	};


	// Shader type
	enum SHADER_TYPE
	{
		SHADER_UNKNOWN		= -1,
		SHADER_CONSTANT,
		SHADER_SIMPLEMESH,
		SHADER_SIMPLEDEFERRED,
		SHADER_TRANSPARENT,
		SHADER_SHADOW,
		SHADER_GBUFFER,
		SHADER_Z_PREPASS,
		SHADER_QUAD,
		SHADER_QUAD_TEXARRAY,
		SHADER_VOXEL,
		SHADER_FXAA,
		SHADER_SMAA,
		SHADER_SSDO,
		SHADER_SEPARABLE_SSS,
		SHADER_LIGHT_SHAFT,
		SHADER_LOCAL_REFLECTION,
		SHADER_DOF,
		SHADER_HDR,
		SHADER_REFLECTIVE_SHADOWMAPS,
		SHADER_LPV,
		SHADER_VOXEL_CONE_TRACING,
		SHADER_VOLUME_RAY_CASTING,

		//SHADER_HDR,

		NUM_SHADER_TYPE,
	};


	// Render Pass Type
	enum RENDER_PASS_TYPE
	{
		PASS_TYPE_DEPTHPREPASS,
		PASS_TYPE_SHADOWMAP,
		PASS_TYPE_FORWARD,
		PASS_TYPE_DEFERRED,

		PASS_TYPE_TRANSLUCENCY,
		PASS_TYPE_POSTPROCESS,
		PASS_TYPE_DECAL,

		PASS_TYPE_LIGHTING,

		PASS_TYPE_UI,

		NUM_PASS_TYPE,
	};


	// Frustum Update Mode
	enum FRUSTUM_UPDATE
	{
		FRUSTUM_UPDATE_VIEW				= 0x01,
		FRUSTUM_UPDATE_PROJECTION		= 0x02,
		//FRUSTUM_UPDATE_VIEW_PROJECTION	= 0x11,

		FRUSTUM_UPDATE_FRUSTUMPLANE		= 0x04,
	};



	// Projection Type
	enum PROJECTION_TYPE
	{
		PROJECION_TYPE_PERSPECTIVE,
		PROJECION_TYPE_ORTHOGRAPHIC,
		NUM_PROJECTION_TYPE,
	};



	enum ATTRIB_LOCATION
	{
		ATTRIB_LOCATION_POSITION,
		ATTRIB_LOCATION_NORMAL,
		ATTRIB_LOCATION_TEXCOORD,
		ATTRIB_LOCATION_COLOR
	};



	// TODO: TexCoordはVec2fに戻す 2013.11.07
	// standard vertex layout
	struct VertexLayout
	{
		Vec3f	Position;	// 位置
		Vec3f	Normal;		// 法線ベクトル
		Vec2f	TexCoord;	// テクスチャ座標
		Vec3f	Color;
	};


	struct VertexLayoutSolid
	{
		Vec3f	Position;
		Vec3f	Normal;
		Vec3f	TexCoord;
	};


	// extended vertex layout
	struct VertexLayout_Ext
	{
		Vec3f	Position;	// 位置
		Vec3f	TexCoord;	// テクスチャ座標
		Vec3f	Normal;		// 法線ベクトル
		Vec3f	Tangent;	// 接線ベクトル
		Vec3f	Binormal;	// 従接線ベクトル
	};



	// 2d vertex layout
	struct VertexLayout_2d
	{
		Vec2f	Position;
		Vec2f	TexCoord;
	};



	// mesh attrribute
	struct MeshAttribute
	{
		PRIMITIVE_TYPE	PrimitiveType;
		uint8			ShaderType;		// シェーダーの種類. TODO: 別のクラス/構造体に移動できないか検討する。この場所はよくない. マテリアルクラスを用意する
		bool			Instance;		// インスタンスかどうか
	};



	struct RenderStats
	{
		bool	Visible;		// 可視オブジェクト
		bool	CastShadow;		// 他のオブジェクトに影を投影する
		bool	ReceiveShadow;	// 他のオブジェクトから影を受像する
	};



	// Terrain Attribute
	struct TerrainAttribute
	{
		Vec3f	Origin;			// ワールド空間上の中心位置
		Vec3f	TangentSpace[3];// 3次元空間上でのテレインの向き
		float	Width;			// 横幅
		float	Depth;			// 縦幅
		float	Height;			// 高さ
	};


	// vertex buffer
	struct VertexBuffer : Buffer
	{
		inline Vec3f* Position( int i=0 ) const	{ return  &( (VertexLayout *)( pData + elmSize*i ) )->Position; }
		inline Vec2f* TexCoord( int i=0 ) const	{ return  &( (VertexLayout *)( pData + elmSize*i ) )->TexCoord; }
		inline Vec3f* Normal( int i=0 ) const		{ return  &( (VertexLayout *)( pData + elmSize*i ) )->Normal; }
		inline Vec3f* Color( int i=0 ) const		{ return  &( (VertexLayout *)( pData + elmSize*i ) )->Color; }
	};



	// index buffer
	struct IndexBuffer : Buffer
	{
		inline int* Index( int i=0 ) const	{ return (int *)( pData + elmSize*i ); }
	};



	// face group buffer
	struct FaceGroupBuffer : Buffer
	{
		inline Vec2ui* FaceGroup( int i = 0 ) const	{ return (Vec2ui*)( pData + elmSize*i ); }
	};



	// texture parameter
	struct TexParam
	{
		uint32	Type;		// テクスチャの種類(GL_TEXTURE_3Dとかみたいな)
		uint32	texUnit;	// テクスチャユニット
		uint32	texID;		// テクスチャID
	};



	// texture buffer
	struct TextureBuffer : Buffer
	{
		inline TexParam* Layer( int i=0 ) const	{ return (TexParam *)( pData + elmSize*i ); }
		inline uint32* Type( int i=0 ) const	{ return &( (TexParam *)( pData + elmSize*i ) )->Type; }
		inline uint32* texUnit( int i=0 ) const	{ return &( (TexParam *)( pData + elmSize*i ) )->texUnit; }
		inline uint32* texID( int i=0 ) const	{ return &( (TexParam *)( pData + elmSize*i ) )->texID; }
	};



	// material attribute
	struct MaterialAttribute
	{
		// uint8	ShaderType;	// シェーダーの種類. TODO: MeshAttributeからこの構造体に移動できないか検討する。

		Vec3f	Ambient;		// 環境光反射成分
		Vec3f	Diffuse;		// 拡散反射成分
		Vec3f	Specular;		// 鏡面反射成分
		float	Shininess;		// スペキュラーの強さ
		float	RefractiveIndex;// 屈折率
		float	Opacity;			// 不透明度
		Vec3f	TransparentColor;	// 透過色
	};



	// multilayer material parameter
	struct MaterialBuffer : Buffer
	{
		//inline MaterialAttribute* Layer(int i=0) const	{ return (MaterialAttribute *)(pData + elmSize*i); }
		//inline Vec3f* Ambient(int i=0) const			{ return &((MaterialAttribute *)(pData + elmSize*i))->Ambient; }
		//inline Vec3f* Diffuse(int i=0) const			{ return &((MaterialAttribute *)(pData + elmSize*i))->Diffuse; }
		//inline Vec3f* Specular(int i=0) const			{ return &((MaterialAttribute *)(pData + elmSize*i))->Specular; }
		//inline float* Shininess(int i=0) const			{ return &((MaterialAttribute *)(pData + elmSize*i))->Shininess; }
		//inline float* RefractiveIndex( int i=0 ) const	{ return &( (MaterialAttribute *)( pData + elmSize*i ) )->RefractiveIndex; }
		//inline float* Opacity(int i=0) const			{ return &((MaterialAttribute *)(pData + elmSize*i))->Opacity; }
		//inline Vec3f* TransparentColor( int i=0 ) const	{ return &( (MaterialAttribute *)( pData + elmSize*i ) )->TransparentColor; }

		inline Vec3f* Ambient( int i=0 ) const			{ return (Vec3f *)( pData + sizeof( Vec3f )*i ); };
		inline Vec3f* Diffuse( int i=0 ) const			{ return (Vec3f *)( pData + sizeof( Vec3f )*( numElms + i ) ); };
		inline Vec3f* Specular( int i=0 ) const			{ return (Vec3f *)( pData + sizeof( Vec3f )*( numElms*2 + i ) ); };
		inline float* Shininess( int i=0 ) const			{ return (float *)( pData + sizeof( Vec3f )*numElms*3 + sizeof( float )*i ); };
		inline float* RefractiveIndex( int i=0 ) const	{ return (float *)( pData + sizeof( Vec3f )*numElms*3 + sizeof( float )*( numElms + i ) ); };
		inline float* Opacity( int i=0 ) const			{ return (float *)( pData + sizeof( Vec3f )*numElms*3 + sizeof( float )*( numElms*2 + i ) ); };
		inline Vec3f* TransparentColor( int i=0 ) const	{ return (Vec3f *)( pData + sizeof( Vec3f )*( numElms*3 + i ) + sizeof( float )*( numElms*3 ) ); };
	};



	// Layerの中に、色々種類がある
	// MaterialBuffer -> ShaderParam
	// AddMeshの可変引数で、何個かいっぺんに渡される


	// transform attribute -> クラス化.別ファイルへ移動 2014.12.07
	//struct TransformAttribute
	//{
	//	Vec3f	Scale;
	//	//Mat4f	Rotation;	// TODO: 廃止予定.代わりにクォータニオンを使用
	//	Quatf	Quaternion;	
	//	Vec3f	Translation;
	//};


	// transform matrix
	struct TransformMatrices
	{
		// coordinate transformation
		Mat4f	matLocal;	// local transfotmation matrix
		Mat4f	matWorld;	// local-to-world transfromation matrix
	};


	// frustum attribute
	struct FrustumAttribute
	{
		Vec3f		m_Position;		// TODO: 整理. シェーダーで使ってる. 2015.12.05
		Vec3f		m_Forward;		// TODO: 整理. シェーダーで使ってない. 2015.12.05
		Vec3f		m_Vertical;		// TODO: 整理. シェーダーで使ってない. 2015.12.05
		Vec3f		m_Horizontal;	// TODO: 整理. シェーダーで使ってない. 2015.12.05

		//====================== Vec4f1つにまとめられる？ =====================//
		float		m_Fov;
		float		m_AspectRatio;
		float		m_zNear;
		float		m_zFar;
	};


	// camera matrix
	struct ViewTransformMatrix//Buffer : Buffer
	{
		// transformation matrix
		Mat4f	matView;	// view transformation matrix
		Mat4f	matProjection;		// projection matrix
		Mat4f	matViewProjection;	// view projection matrix
		Mat4f	matInvViewProjection;
	};

	/*
	// Common Light attribute
	struct LightAttribute
	{
	uint8	LightType;	// Light type
	bool	Enable;		// Lighting OnOff
	bool	CastShadow;	// cast shadow OnOff
	};
	*/


	struct LightAttribute
	{
		LIGHT_TYPE	LightType;
		bool		Enable;			// Lighting OnOff
		bool		EmitDiffuse;
		bool		EmitSpecular;
		bool		CastShadow;

		Vec3f		Color;
		float		Intensity;
		float		Dropoff;
		float		ConeAngle;	// xyz:Direction, w:ConeAngle for spotlight
		float		ConstantAttenuation;
		float		LinearAttenuation;
		float		QuadraticAttenuation;
		float		Radius;
	};





	//#################################################################################//
	//	Helper functions
	//#################################################################################//


	static int SizeOfFormatElement( DATA_FORMAT format )
	{
		switch( format )
		{
		case FORMAT_R32G32B32A32_TYPELESS:
		case FORMAT_R32G32B32A32_FLOAT:
		case FORMAT_R32G32B32A32_UINT:
		case FORMAT_R32G32B32A32_SINT:
			return 16;//128;

		case FORMAT_R32G32B32_TYPELESS:
		case FORMAT_R32G32B32_FLOAT:
		case FORMAT_R32G32B32_UINT:
		case FORMAT_R32G32B32_SINT:
			return 12;//96;

		case FORMAT_R16G16B16A16_TYPELESS:
		case FORMAT_R16G16B16A16_FLOAT:
		case FORMAT_R16G16B16A16_UNORM:
		case FORMAT_R16G16B16A16_UINT:
		case FORMAT_R16G16B16A16_SNORM:
		case FORMAT_R16G16B16A16_SINT:
		case FORMAT_R32G32_TYPELESS:
		case FORMAT_R32G32_FLOAT:
		case FORMAT_R32G32_UINT:
		case FORMAT_R32G32_SINT:
		case FORMAT_R32G8X24_TYPELESS:
		case FORMAT_D32_FLOAT_S8X24_UINT:
		case FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case FORMAT_X32_TYPELESS_G8X24_UINT:
			return 8;//64;

		case FORMAT_R16G16B16_FLOAT:
		case FORMAT_R16G16B16_UNORM:
		case FORMAT_R16G16B16_UINT:
		case FORMAT_R16G16B16_SNORM:
		case FORMAT_R16G16B16_SINT:
			return 6;//48

		case FORMAT_R10G10B10A2_TYPELESS:
		case FORMAT_R10G10B10A2_UNORM:
		case FORMAT_R10G10B10A2_UINT:
		case FORMAT_R11G11B10_FLOAT:
		case FORMAT_R8G8B8A8_TYPELESS:
		case FORMAT_R8G8B8A8_UNORM:
		case FORMAT_R8G8B8A8_UNORM_SRGB:
		case FORMAT_R8G8B8A8_UINT:
		case FORMAT_R8G8B8A8_SNORM:
		case FORMAT_R8G8B8A8_SINT:
		case FORMAT_R16G16_TYPELESS:
		case FORMAT_R16G16_FLOAT:
		case FORMAT_R16G16_UNORM:
		case FORMAT_R16G16_UINT:
		case FORMAT_R16G16_SNORM:
		case FORMAT_R16G16_SINT:
		case FORMAT_R32_TYPELESS:
		case FORMAT_D32_FLOAT:
		case FORMAT_R32_FLOAT:
		case FORMAT_R32_UINT:
		case FORMAT_R32_SINT:
		case FORMAT_R24G8_TYPELESS:
		case FORMAT_D24_UNORM_S8_UINT:
		case FORMAT_R24_UNORM_X8_TYPELESS:
		case FORMAT_X24_TYPELESS_G8_UINT:
		case FORMAT_B8G8R8A8_UNORM:
		case FORMAT_B8G8R8X8_UNORM:
			return 4;//32;

		case FORMAT_R8G8B8_UNORM:
		case FORMAT_R8G8B8_UINT:
		case FORMAT_R8G8B8_SNORM:
		case FORMAT_R8G8B8_SINT:
		case FORMAT_B8G8R8_UNORM:
			return 3;//24

		case FORMAT_R8G8_TYPELESS:
		case FORMAT_R8G8_UNORM:
		case FORMAT_R8G8_UINT:
		case FORMAT_R8G8_SNORM:
		case FORMAT_R8G8_SINT:
		case FORMAT_R16_TYPELESS:
		case FORMAT_R16_FLOAT:
		case FORMAT_D16_UNORM:
		case FORMAT_R16_UNORM:
		case FORMAT_R16_UINT:
		case FORMAT_R16_SNORM:
		case FORMAT_R16_SINT:
		case FORMAT_B5G6R5_UNORM:
		case FORMAT_B5G5R5A1_UNORM:
			return 2;//16;

		case FORMAT_R8_TYPELESS:
		case FORMAT_R8_UNORM:
		case FORMAT_R8_UINT:
		case FORMAT_R8_SNORM:
		case FORMAT_R8_SINT:
		case FORMAT_A8_UNORM:
			return 1;//8;               

			// Compressed format; http://msdn2.microso.../bb694531(VS.85).aspx
		case FORMAT_BC2_TYPELESS:
		case FORMAT_BC2_UNORM:
		case FORMAT_BC2_UNORM_SRGB:
		case FORMAT_BC3_TYPELESS:
		case FORMAT_BC3_UNORM:
		case FORMAT_BC3_UNORM_SRGB:
		case FORMAT_BC5_TYPELESS:
		case FORMAT_BC5_UNORM:
		case FORMAT_BC5_SNORM:
			return 16;//128;

			// Compressed format; http://msdn2.microso.../bb694531(VS.85).aspx
		case FORMAT_R1_UNORM:
		case FORMAT_BC1_TYPELESS:
		case FORMAT_BC1_UNORM:
		case FORMAT_BC1_UNORM_SRGB:
		case FORMAT_BC4_TYPELESS:
		case FORMAT_BC4_UNORM:
		case FORMAT_BC4_SNORM:
			return 8;//64;

			// Compressed format; http://msdn2.microso.../bb694531(VS.85).aspx
		case FORMAT_R9G9B9E5_SHAREDEXP:
			return 4;//32;

			// These are compressed, but bit-size information is unclear.
		case FORMAT_R8G8_B8G8_UNORM:
		case FORMAT_G8R8_G8B8_UNORM:
			return 4;//32;

		case FORMAT_UNKNOWN:
		default:
			return 0;//throw gcnew InvalidOperationException( "Cannot determine format element size; invalid format specified." );
		}

	}



	//######################################	Vertex Buffer	#########################################//

	// Allocate Vertex Buffer
	static void AllocateVertexBuffer( VertexBuffer *vb, int numelms, size_t size=sizeof( VertexLayout ), void *pdata=0 )
	{
		AllocateBuffer( vb, numelms, size/*, BUF_VERTEX*/ );
		if( pdata )	memcpy( vb->pData, pdata, vb->elmSize*vb->numElms );
	}

	// Copy Vertex Buffer
#define CopyVertexBuffer(dest, src) CopyBuffer(dest, src)

	// Release Vertex Buffer
#define ReleaseVertexBuffer(vb) ReleaseBuffer(vb)



	//######################################	Index Buffer	#########################################//

	// Allocate Index Buffer
	static void AllocateIndexBuffer( IndexBuffer *ib, int numelms, uint8 primitive_type, void *pdata=0 )
	{
		AllocateBuffer( ib, numelms, sizeof( int )/*, primitive_type*/ );
		if( pdata )	memcpy( ib->pData, pdata, ib->elmSize*ib->numElms );
	}

	// Copy Index Buffer
	static void CopyIndexBuffer( IndexBuffer *dest, const IndexBuffer *src )
	{
		CopyBuffer( dest, src );
		//	dest->BufferType	= src->BufferType;
	}

	// Release Index Buffer
	static void ReleaseIndexBuffer( IndexBuffer *ib )
	{
		ReleaseBuffer( ib );
		//	ib->BufferType	= PRIM_UNKNOWN;
	}



	//###################################	FaceGroup Buffer	########################################//

	// Init FaceGroup buffer
	static void AllocateFaceGroupBuffer( FaceGroupBuffer *fgb, int numgroups )
	{
		AllocateBuffer( fgb, numgroups, sizeof( Vec2ui ) );
	}


	// Copy FaceGroup buffer
	static void CopyFaceGroupBuffer( FaceGroupBuffer *dest, FaceGroupBuffer *src )
	{
		CopyBuffer( dest, src );
	}

	// Release FaceGroup buffer
	static void ReleaseFaceGroupBuffer( FaceGroupBuffer *fgb )
	{
		ReleaseBuffer( fgb );
	}



	//###################################	Texture Buffer	########################################//

	// Allocate Texture Buffer
	static void AllocateTextureBuffer( TextureBuffer *tb, int numelms )
	{
		AllocateBuffer( tb, numelms, sizeof( TexParam )/*, BUF_TEXTURE*/ );
	}

	// Copy Texture Buffer
#define CopyTextureBuffer( dest, src ) CopyBuffer( dest, src )

	// Release Texture Buffer
#define ReleaseTextureBuffer(tb) ReleaseBuffer(tb)



	//###################################	Material Buffer	########################################//

	// TODO: BUFFER_TYPEにマテリアルタイプを追記する or マテリアルタイプはMTL_TYPEで独立させる？
	// Allocate Material Buffer
	static void AllocateMaterialBuffer( MaterialBuffer *mb, int numelms, uint8 mat_type, int size=sizeof( MaterialAttribute ) )
	{
		AllocateBuffer( mb, numelms, size/*, mat_type*/ );
	}

	// Copy Material Buffer
	static void CopyMaterialBuffer( MaterialBuffer *dest, const MaterialBuffer *src )
	{
		CopyBuffer( dest, src );
	}

	// Release Material Buffer
	static void ReleaseMaterialBuffer( MaterialBuffer *mb )
	{
		ReleaseBuffer( mb );
		//	mb->BufferType	= MTL_UNKNOWN;
	}


}// end of namespace


#endif	// BUFFER_LAYOUT_H //
