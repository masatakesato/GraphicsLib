#ifndef	LIGHT_SOURCE_BUFFER_H
#define	LIGHT_SOURCE_BUFFER_H


//#include	"TileCacheTexture.h"
#include	<oreore/mathlib/GraphicsMath.h>

#include	"../resource/GLUniformBufferObject.h"
#include	"../resource/IBufferObject.h"
#include	"../resource/CacheDataManager.h"

#include	"Frustum.h"
#include	"MovableObject.h"



namespace OreOreLib
{


	class LightAttributeBuffer: public IBufferObject
	{
	public:

		LightAttributeBuffer();
		~LightAttributeBuffer();

		void Init( int num_max_lights );
		void Release();
		
		MovableObject* GetSceneObject( int slot_id ) const	{ return m_ReferredObjects[ slot_id ]; }
		Vec3f* GetPosition( int slot_id ) const				{ return &m_PackedPosAttrib[slot_id].pos; }
		void GetShadowCasterIndexList( int slot_id, int &numshadows, int indices[] )
		{
			numshadows = int( ( m_PackedPosAttrib[slot_id].attrib & BITMASK_NUMSHADOWS ) >> BITSHIFT_NUMSHADOWS );
			m_CacheManager.GetSerialSlots( slot_id, numshadows, indices );
		}
		LIGHT_TYPE GetLightType( int slot_id ) const		{ return (LIGHT_TYPE)UnpackLightType( m_PackedPosAttrib[slot_id].attrib ); }
		Vec3f* GetDirection( int slot_id ) const			{ return &m_PackedDirConeAngle[slot_id].dir; }


		bool SetLightAttribute( int slot_id, const FrustumAttribute& attribFrustum, const LightAttribute& attribLight, uint8 numshadows=0, short shadowcaster_idx=-1, short lightmat_idx=-1 );
		bool SetShadowCasterIndex( int slot_id, uint8 numshadows, short shadowmap_idx );
		bool SetLightMatrixIndex( int slot_id, short lightmat_idx );

		int NumMaxSlots() const					{ return m_CacheManager.numMaxSlots(); }
		int	NumActiveSlots() const				{ return m_CacheManager.numReservedSlots(); }
		bool HasActiveShadowMap( int slot_id )	{ return ( m_ReferredObjects[slot_id ] != NULL ) && ( UnpackNumShadows( m_PackedPosAttrib[ slot_id ].attrib ) > 0 ); }

		bool IsRererved( int slot_id ) const	{ return m_CacheManager.IsRererved(slot_id); }	// 指定スロットが使用中かどうか調べる
		bool IsDirty( int slot_id ) const		{ return m_CacheManager.IsDirty(slot_id); }		// 最近指定スロットへのアクセスがあったかどうか調べる
		bool IsClean( int slot_id ) const		{ return m_CacheManager.IsClean(slot_id); }		// 指定スロットがアクセスなしの放置状態になっているかどうか調べる
		bool IsFull() const						{ return m_CacheManager.IsFull(); }				// キャッシュが満杯かどうかチェックする
	
		bool SetClean( int slot_id )			{ return m_CacheManager.SetClean(slot_id); }
		bool SetDirty( int slot_id )			{ return m_CacheManager.SetDirty(slot_id); }
		bool ReserveSlot( MovableObject *referredObj )	// 空きスロットを予約する．
		{
			if( !m_ReferredObjects )
				return false;
		
			int reserved_slot = m_CacheManager.ReserveSlot();
			if( reserved_slot==-1 )
				return false;

			referredObj->SetBufferID( reserved_slot );
			m_ReferredObjects[reserved_slot ]	= referredObj;

			return true;//m_CacheManager.ReserveSlot();
		}	
		bool FreeSlot( int slot_id )			// 指定したスロットを解放する
		{
			if( !m_ReferredObjects )
				return false;

			if( m_CacheManager.FreeSlot(slot_id)==false )
				return false;

			m_ReferredObjects[ slot_id ]->SetBufferID(-1);
			m_ReferredObjects[ slot_id ]	= NULL;

			return true;//m_CacheManager.FreeSlot(slot_id);
		}
		void Clear()			// 全スロットを解放する
		{
			// リンク先の～を解放する
			for( int i=0; i<m_CacheManager.numMaxSlots(); ++i )
				if( m_ReferredObjects[i] )	m_ReferredObjects[i]->SetBufferID(-1);
			memset( m_ReferredObjects, 0, sizeof(MovableObject*) * m_CacheManager.numMaxSlots() );

			m_CacheManager.Clear();
		}

		// TODO: PageOut機能が必要


		inline virtual void Bind()		{ for( int i=0; i<UB_PACKED_LIGHT_NUM; ++i ) m_UniformBlocks[i].Bind(); }
		inline virtual void Unbind()	{ for( int i=0; i<UB_PACKED_LIGHT_NUM; ++i ) m_UniformBlocks[i].Unbind(); }
		void BindToShader( GLuint program_id );
		void UpdateSubresource();


	private:
		
		struct PosAttrib
		{
			Vec3f	pos;
			unsigned int attrib;// 24-31bit: type, 16-23bit: numshadowmaps, 0-15bit: shadowmap_index
			// 新しい圧縮.  LightType:29-31[bit], EmitDiffuse:28[bit], EmitSpecular:27[bit], numShadows:22-26[bits], shadowcaster_idx:11-21[bit](21ビット目は符号), lightmatrix_idx:0-10[bit](10ビット目は符号)
		};
		
		struct ColorDropoff
		{
			Vec3f	colorintensity;
			float	dropoff;
		};

		struct DirConeAngle
		{
			Vec3f	dir;
			float	coneangle;
		};

		struct AttenRadius
		{
			float	constatten;
			float	linearAtten;
			float	quadratten;
			float	radius;
		};


		// Cache Data Manager
		CacheDataManager	m_CacheManager;


		// Light Source Attributes
		PosAttrib			*m_PackedPosAttrib;		// xyz:Position, w[24-31]: lighttype, w[16-23]: numshadows, w[0-15bit]:shadowmapid
		ColorDropoff		*m_PackedColorDropoff;	// xyz:ColorIndensity, w:Dropoff for spotlight
		DirConeAngle		*m_PackedDirConeAngle;	// xyz:Direction, w:ConeAngle for spotlight
		AttenRadius			*m_PackedAttenRadius;	// x:ConstantAttenuation, y:LinearAttenuation, z:QuadraticAttenuation, w:LightRadius


		// 
		MovableObject		**m_ReferredObjects;

		// Referred ShadowCaster Attachments
		//ShadowAttachments	*m_refShadowAttachments;


		// Unifom Buffer Objects
		enum UB_PACKED_LIGHT
		{
			UB_PACKED_LIGHT_POS_ATTRIB,
			UB_PACKED_LIGHT_COLOR_DROPOFF,
			UB_PACKED_LIGHT_DIR_CONEANGLE,
			UB_PACKED_LIGHT_ATTEN_RADIUS,
			UB_PACKED_LIGHT_NUM
		};

		GLUniformBufferObject	m_UniformBlocks[ UB_PACKED_LIGHT_NUM ];

		static const char	*m_LightSourceUniformBlockNames[ UB_PACKED_LIGHT_NUM ];
		//static const tstring	m_LightSourceUniformBlockNames[UB_PACKED_LIGHT_NUM];
		static GLuint			m_BindPoints[ UB_PACKED_LIGHT_NUM ];


		// Bitmask Operations for m_PackedPosAttrib.attrib
		enum ATTRIB_BITMASK
		{
			BITMASK_LIGHTTYPE			= 0xE0000000,	// LightType:29-31[bit]				//11100000000000000000000000000000
			BITMASK_EMITDIFFUSE			= 0x10000000,	// EmitDiffuse:28[bit]				//00010000000000000000000000000000
			BITMASK_EMITSPECULAR		= 0x08000000,	// EmitSpecular:27[bit]				//00001000000000000000000000000000
			BITMASK_NUMSHADOWS			= 0x07C00000,	// numShadows:22-26[bit]			//00000111110000000000000000000000
			BITMASK_SHADOWCASTER_VALID	= 0x00200000,	// shadowcaster_idx validity:21[bit]//00000000001000000000000000000000
			BITMASK_SHADOWCASTER_IDX	= 0x001FF800,	// shadowcaster_idx:11-20[bit]		//00000000000111111111100000000000
			BITMASK_LIGHTMAT_VALID		= 0x00000400,	// lightmatrix_idx vaidity:10[bit]	//00000000000000000000010000000000
			BITMASK_LIGHTMAT_IDX		= 0x000003FF,	// lightmatrix_idx:0-9[bit]			//00000000000000000000001111111111
			BITMASK_NULL				= 0x00000000,
		};

		enum ATTRIB_BITSHIFT
		{
			BITSHIFT_LIGHTTYPE			= 29,	// LightType:29-31[bit]
			BITSHIFT_EMITDIFFUSE		= 28,	// EmitDiffuse:28[bit]
			BITSHIFT_EMITSPECULAR		= 27,	// EmitSpecular:27[bit]
			BITSHIFT_NUMSHADOWS			= 22,	// numShadows:22-26[bits]
			BITSHIFT_SHADOWCASTER_VALID	= 21,	// shadowcaster_idx validity:21[bit]
			BITSHIFT_SHADOWCASTER_IDX	= 11,	// shadowcaster_idx:11-20[bit]
			BITSHIFT_LIGHTMAT_IDX_VALID	= 10,	// lightmatrix_idx vaidity:10[bit]
			BITSHIFT_LIGHTMAT_IDX		= 0,	// lightmatrix_idx:0-9[bit]
		};


		// Packed Light Attribute data
		inline uint32 PackLightTypeBits( uint8 val )
		{
			return BITMASK_LIGHTTYPE	& ( val << BITSHIFT_LIGHTTYPE );
		}

		inline uint32 PackEmitDiffuseBits( bool val )
		{
			return val ? BITMASK_EMITDIFFUSE : BITMASK_NULL;
		}

		inline uint32 PackEmitSpecularBits( bool val )
		{
			return val ? BITMASK_EMITSPECULAR : BITMASK_NULL;
		}

		inline uint32 PackNumShadowsBits( uint8 val )
		{
			return BITMASK_NUMSHADOWS	& ( val << BITSHIFT_NUMSHADOWS );
		}

		inline uint32 PackShadowCasterIndexBits( short val )
		{
			return	( BITMASK_SHADOWCASTER_VALID	& ( val>=0 ? BITMASK_SHADOWCASTER_VALID : 0 ) )	|
					( BITMASK_SHADOWCASTER_IDX		& ( val << BITSHIFT_SHADOWCASTER_IDX ) );
		}

		inline uint32 PackLightMatIndexBits( short val )
		{
			return	( BITMASK_LIGHTMAT_VALID	& ( val>=0 ? BITMASK_LIGHTMAT_VALID : 0 ) ) |
					( BITMASK_LIGHTMAT_IDX		& val );
		}


		// Unpacked Light Attribute Data
		inline uint32 UnpackLightType( uint32 val ) const
		{
			return uint32( ( val & BITMASK_LIGHTTYPE ) >> BITSHIFT_LIGHTTYPE );
		}

		inline bool UnpackEmitDiffuse( uint32 val ) const
		{
			return	(val & BITMASK_EMITDIFFUSE) != 0;
		}

		inline bool UnpackEmitSpecular( uint32 val ) const
		{
			return	(val & BITMASK_EMITSPECULAR) != 0;
		}

		inline uint32 UnpackNumShadows( uint32 val ) const
		{
			return	uint32( ( val & BITMASK_NUMSHADOWS ) >> BITSHIFT_NUMSHADOWS );
		}

		inline bool UnpackShadowCasterValid( uint32 val ) const
		{
			return	(val & BITMASK_SHADOWCASTER_VALID) != 0;
		}

		inline int	UnpackShadowCasterIndex( uint32 val ) const
		{
			return	int( ( val & BITMASK_SHADOWCASTER_IDX ) >> BITSHIFT_SHADOWCASTER_IDX );
		}

		inline bool UnpackLightMatValid( uint32 val ) const
		{
			return	(val & BITMASK_LIGHTMAT_VALID) != 0;
		}

		inline uint32 UnpackLightMatIndex( uint32 val ) const
		{
			return uint32( val & BITMASK_LIGHTMAT_IDX );
		}








		LightAttributeBuffer( const LightAttributeBuffer& obj ){}// copy constructor
	};


	// メモリ上に配列でバッファを格納しておく
	// シーン情報のアップロードはタイル毎に実行。タイルに所属する複数光源をいっぺんにロード

	// Updateはどうする？いちいちlightFrustumから値をコピー？ -> DirectUpdateの仕組みを考える. 


}// end of namespace



#endif	// LIGHT_SOURCE_BUFFER_H //