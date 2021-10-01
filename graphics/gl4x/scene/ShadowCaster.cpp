#include	"ShadowCaster.h"

#include	<oreore/common/TString.h>

#include	"../common/Defines.h"



namespace  OreOreLib
{

#define	CASCADES_LIMIT	64

	const DATA_FORMAT	SHADOW_MAP_FORMAT	= FORMAT_R32_FLOAT;


	ShadowCaster::ShadowCaster()
	{
		m_MaxDistance			= 1000.0f;
		m_SplitLogFactor		= 0.97f;
		InitVec( m_HalfShadowMapSize, 512.0f, 512.0f );
	}



	ShadowCaster::ShadowCaster( int width, int height, float distlimit, float logfactor )
	{
		Init( width, height, distlimit, logfactor );	
	}



	ShadowCaster::~ShadowCaster()
	{
		Release();
	}



	//ShadowCaster( const ShadowCaster& obj )
	//{
	//
	//}



	void ShadowCaster::Init( int width, int height, float distlimit, float logfactor )
	{
		m_MaxDistance		= distlimit;
		m_SplitLogFactor	= logfactor;
		InitVec( m_HalfShadowMapSize, 0.5f * float(width), 0.5f * float(height) ); 
	}



	void ShadowCaster::Release()
	{
		
	}



	bool ShadowCaster::CalcMatrix_Directional( int num_cascades, Mat4f matLightViewProjCrops[], float splitDepths[], const Frustum& cameraFrustum, const Frustum& lightFrustum )
	{
		if( num_cascades > CASCADES_LIMIT )	return false;
		
		Quad splitPlanes[ MAX_SHADOW_CASCADES + 1 ];
		
		const Vec3f& lightPos = *cameraFrustum.GetPosition();//*lightFrustum.GetPosition();//
		const Vec3f& lightDir		= *lightFrustum.GetDirection();
		const float& lightZNear		= *lightFrustum.GetZNear();
		//const Mat4f& matLightView	= *lightFrustum.GetViewMatrix();
		const Vec3f cropRangeMin	= { -1.0f, -1.0f, 0.0f },
					cropRangeMax	= { 1.0f, 1.0f, 1.0f };

		Mat4f matLightView;
		MatViewGL( matLightView, *lightFrustum.GetHorizontal(), *lightFrustum.GetVertical(), lightDir, lightPos );


		//================== Update SplitDepths ===================//
		float camNear		= *cameraFrustum.GetZNear();
		float camFar		= Min( *cameraFrustum.GetZFar(), m_MaxDistance );
		float camDepthRange	= camFar - camNear;

		UpdateSplitDepths( num_cascades, splitDepths, camNear, camFar );


		//================== Update Split Planes ==================//

		// Get view frustum corner points
		Vec3f	ws_view_frustum_corners[8];
		cameraFrustum.GetSlicePoints( &ws_view_frustum_corners[0], camNear );
		cameraFrustum.GetSlicePoints( &ws_view_frustum_corners[4], camFar );

		// Calculate split points
		for( int i=0; i<=num_cascades; ++i )
		{
			if( i==0 )
			{
				splitPlanes[i].verts[0]	= ws_view_frustum_corners[0];
				splitPlanes[i].verts[1]	= ws_view_frustum_corners[1];
				splitPlanes[i].verts[2]	= ws_view_frustum_corners[2];
				splitPlanes[i].verts[3]	= ws_view_frustum_corners[3];
			}
			else
			{
				float weight = (splitDepths[i-1] + 1.0e-1f) / camDepthRange;
				Lerp( splitPlanes[i].verts[0], ws_view_frustum_corners[0], ws_view_frustum_corners[4], weight );
				Lerp( splitPlanes[i].verts[1], ws_view_frustum_corners[1], ws_view_frustum_corners[5], weight );
				Lerp( splitPlanes[i].verts[2], ws_view_frustum_corners[2], ws_view_frustum_corners[6], weight );
				Lerp( splitPlanes[i].verts[3], ws_view_frustum_corners[3], ws_view_frustum_corners[7], weight );
			}
		}// end of i loop


		//=============== Update m_matLightViewProjCrop ===========// 
		for( int i=0; i<num_cascades; ++i )
		{
			Vec3f	ws_cascade_frustum_corners[8],
					ls_cascade_frustum_corners[8];
			Mat4f	newMatLightView, newMatLightProj,
					newMatLightViewProj;

			//=============== カスケード頂点を光源座標系に変換する ==============//
			for( int k=0; k<4; ++k )
			{
				ws_cascade_frustum_corners[k]	= splitPlanes[i].verts[k];
				ws_cascade_frustum_corners[k+4]	= splitPlanes[i+1].verts[k];
				Multiply( ls_cascade_frustum_corners[k], matLightView, ws_cascade_frustum_corners[k] );
				Multiply( ls_cascade_frustum_corners[k+4], matLightView, ws_cascade_frustum_corners[k+4] );
			}

			//============= Calculate Max Light-Depth of the Cascede ============//
			float newLightZfar = Min( Min( Min( Min( Min( Min( Min( Min(
				ls_cascade_frustum_corners[0].z,	ls_cascade_frustum_corners[1].z ),
				ls_cascade_frustum_corners[2].z ),	ls_cascade_frustum_corners[3].z ),
				ls_cascade_frustum_corners[4].z ),	ls_cascade_frustum_corners[5].z ),
				ls_cascade_frustum_corners[6].z ),	ls_cascade_frustum_corners[7].z ),
				-lightZNear );
			newLightZfar = abs( newLightZfar );//Min( *lightFrustum.GetZFar(), abs(newLightZfar) );

			float backslide_amount = Max( Max( Max( Max( Max( Max( Max( Max(
				ls_cascade_frustum_corners[0].z,	ls_cascade_frustum_corners[1].z ),
				ls_cascade_frustum_corners[2].z ),	ls_cascade_frustum_corners[3].z ),
				ls_cascade_frustum_corners[4].z ),	ls_cascade_frustum_corners[5].z ),
				ls_cascade_frustum_corners[6].z ),	ls_cascade_frustum_corners[7].z ),
				0.0f );

			if ( backslide_amount > 0.0f )	backslide_amount += ( lightZNear + 1.0e-5f );

			// TODO: シーンないオブジェクトのバウンディングボックスから決定する方がいい.上記backslide_amountだと、フラスタム外側のオブジェクトが影を落とす場合に対応できない
			backslide_amount	= Max( backslide_amount, 1000.0f );


			// 光源位置を後ろにずらす。ただし、クリップクリップ面の位置は変わらないようにする
			Vec3f newLightPos;
			AddScaled( newLightPos, lightPos, -backslide_amount, lightDir );
			newLightZfar	+= backslide_amount;

			// 後ろにずらした光源位置を使って、再度ビュー変換行列を計算する
			MatViewGL( newMatLightView, *lightFrustum.GetHorizontal(), *lightFrustum.GetVertical(), lightDir, newLightPos );


			//========== Calculate new view projection matrix for this cascade ======//
			MatOrtho( newMatLightProj, -1.0f, 1.0f, -1.0f, 1.0f, lightZNear, newLightZfar );
			Multiply( newMatLightViewProj, newMatLightProj, newMatLightView );


			//======== 仮想光源の座標系にカスケードフラスタム頂点を変換する =========//
			Vec3f lsproj_view_frustum_corners[8];

			for( int k=0; k<8; ++k )
			{
				Vec4f lsproj_view_frustum_point;
				Multiply( lsproj_view_frustum_point, newMatLightViewProj, ws_cascade_frustum_corners[k] );
				lsproj_view_frustum_point.x /= lsproj_view_frustum_point.w;
				lsproj_view_frustum_point.y /= lsproj_view_frustum_point.w;
				lsproj_view_frustum_point.z /= lsproj_view_frustum_point.w;
				InitVec( lsproj_view_frustum_corners[k], lsproj_view_frustum_point.x, lsproj_view_frustum_point.y, lsproj_view_frustum_point.z );
			}

			//============================ Calculate Crop Matrix =====================//
			Vec3f cropBBMin = lsproj_view_frustum_corners[0], cropBBMax = lsproj_view_frustum_corners[0];
		//cropBBMin	= cropRangeMax;	cropBBMax	= cropRangeMin;// for cropmatrix debug 

			for( int k=1; k<8; ++k )
			{
				Min( cropBBMin, cropBBMin, lsproj_view_frustum_corners[k] );
				Max( cropBBMax, cropBBMax, lsproj_view_frustum_corners[k] );
			}

			if( cropBBMin.x >= cropBBMax.x )	cropBBMin.x	= cropRangeMin.x;
			if( cropBBMin.y >= cropBBMax.y )	cropBBMin.y	= cropRangeMin.y;
			//if( cropBBMin.z >= cropBBMax.z )	cropBBMin.z	= cropRangeMin.z;

			if( cropBBMax.x <= cropBBMin.x )	cropBBMax.x	= cropRangeMax.x;
			if( cropBBMax.y <= cropBBMin.y )	cropBBMax.y	= cropRangeMax.y;
			//if( cropBBMax.z <= cropBBMin.z )	cropBBMax.z	= cropRangeMax.z;

			Mat4f matCrop;
			CalculateCropMatrix_flicker( &matCrop, cropBBMin, cropBBMax );

			Multiply( matLightViewProjCrops[i], matCrop, newMatLightViewProj );

		}// end of i loop

		return true;
	}



	bool ShadowCaster::CalcMatrix_Omni( int num_cascades, Mat4f matLightViewProjCrops[], float splitDepths[], const Frustum& cameraFrustum, const Frustum& lightFrustum )
	{
		if( num_cascades > CASCADES_LIMIT )	return false;

		Quad splitPlanes[ MAX_SHADOW_CASCADES + 1 ];

		const Vec3f& lightPos		= *lightFrustum.GetPosition();
		const Vec3f& lightDir		= *lightFrustum.GetDirection();
		const float& lightZNear		= *lightFrustum.GetZNear( );
		const Mat4f& matLightView	= *lightFrustum.GetViewMatrix();//lightmat->matView;
		const Mat4f& matLightProj	= *lightFrustum.GetProjectionMatrix();//lightmat->matProjection;

		const Vec3f cropRangeMin	= { -1.0f, -1.0f, 0.0f }, cropRangeMax	= { 1.0f, 1.0f, 1.0f };
		Vec3f ws_light_cascade_frustum_corners[8];
		lightFrustum.GetCornerPoints( ws_light_cascade_frustum_corners );


		//================== Update SplitDepths ===================//
		float camNear		= *cameraFrustum.GetZNear();
		float camFar		= Min( *cameraFrustum.GetZFar(), m_MaxDistance );
		float camDepthRange	= camFar - camNear;

		UpdateSplitDepths( num_cascades, splitDepths, camNear, camFar );


		//================== Update Split Planes ==================//

		// Get view frustum corner points
		Vec3f	ws_view_frustum_corners[8];
		cameraFrustum.GetSlicePoints( &ws_view_frustum_corners[0], camNear );
		cameraFrustum.GetSlicePoints( &ws_view_frustum_corners[4], camFar );

		// Calculate split points
		for( int i=0; i<=num_cascades; ++i )
		{
			if( i==0 )
			{
				splitPlanes[i].verts[0]	= ws_view_frustum_corners[0];
				splitPlanes[i].verts[1]	= ws_view_frustum_corners[1];
				splitPlanes[i].verts[2]	= ws_view_frustum_corners[2];
				splitPlanes[i].verts[3]	= ws_view_frustum_corners[3];
			}
			else
			{
				float weight = splitDepths[i-1] / camDepthRange;
				Lerp( splitPlanes[i].verts[0], ws_view_frustum_corners[0], ws_view_frustum_corners[4], weight );
				Lerp( splitPlanes[i].verts[1], ws_view_frustum_corners[1], ws_view_frustum_corners[5], weight );
				Lerp( splitPlanes[i].verts[2], ws_view_frustum_corners[2], ws_view_frustum_corners[6], weight );
				Lerp( splitPlanes[i].verts[3], ws_view_frustum_corners[3], ws_view_frustum_corners[7], weight );
			}
		}// end of i loop

		
		//=============== Update m_matLightViewProjCrop ===========// 
		for( int i=0; i<num_cascades; ++i )
		{
			Vec3f	ws_cascade_frustum_corners[8],
					ls_cascade_frustum_corners[8];
			Mat4f	newMatLightProj, newMatLightViewProj;

			//=============== カスケード頂点を光源座標系に変換する ==============//
			for( int k=0; k<4; ++k )
			{
				ws_cascade_frustum_corners[k]	= splitPlanes[i].verts[k];
				ws_cascade_frustum_corners[k+4]	= splitPlanes[i+1].verts[k];
				Multiply( ls_cascade_frustum_corners[k], matLightView, ws_cascade_frustum_corners[k] );
				Multiply( ls_cascade_frustum_corners[k+4], matLightView, ws_cascade_frustum_corners[k+4] );
			}

			//============= Calculate Max Light-Depth of the Cascede ============//
			float newLightZfar = Min( Min( Min( Min( Min( Min( Min( Min(
				ls_cascade_frustum_corners[0].z,	ls_cascade_frustum_corners[1].z ),
				ls_cascade_frustum_corners[2].z ),	ls_cascade_frustum_corners[3].z ),
				ls_cascade_frustum_corners[4].z ),	ls_cascade_frustum_corners[5].z ),
				ls_cascade_frustum_corners[6].z ),	ls_cascade_frustum_corners[7].z ),
				-lightZNear );
			newLightZfar = abs( newLightZfar );// Min( *lightFrustum.GetZFar( ), abs( newLightZfar ) );

			//========== Calculate new view projection matrix for this cascade ======//
			MatPerspectiveFov( newMatLightProj, *lightFrustum.GetFov(), *lightFrustum.GetAspectRatio(), lightZNear, newLightZfar );
			Multiply( newMatLightViewProj, newMatLightProj, matLightView );

			lightFrustum.GetSlicePoints( &ws_light_cascade_frustum_corners[4], newLightZfar );


			//=================== 仮想光源のフラスタムを作成する ==================//
			// 後ろにスライドさせる量を計算する
			float vl_backslide_amount	= Max(Max(Max(Max(Max(Max(Max(Max(
				ls_cascade_frustum_corners[0].z,	ls_cascade_frustum_corners[1].z),
				ls_cascade_frustum_corners[2].z),	ls_cascade_frustum_corners[3].z),
				ls_cascade_frustum_corners[4].z),	ls_cascade_frustum_corners[5].z),
				ls_cascade_frustum_corners[6].z),	ls_cascade_frustum_corners[7].z),
				0.0f );

			if ( vl_backslide_amount > 0.0f )	vl_backslide_amount += lightZNear + 1.0e-5f;

			// 光源位置を後ろにずらす。ただし、クリップ面の位置は変わらないようにする
			Vec3f virtualLightPos;
			AddScaled( virtualLightPos, lightPos, -vl_backslide_amount, lightDir );
			float virtualLightNear	= lightZNear + vl_backslide_amount;
			float virtualLightFar	= newLightZfar + vl_backslide_amount;

			//ViewTransformMatrix	*pmatVirtualLight;
			Frustum virtualLightFrustum;
			virtualLightFrustum.SetViewParams( virtualLightPos, lightDir, *lightFrustum.GetVertical() );
			virtualLightFrustum.SetProjectionParams( *lightFrustum.GetFov(), *lightFrustum.GetAspectRatio(), virtualLightNear, virtualLightFar );
			virtualLightFrustum.UpdateMatrix( FRUSTUM_UPDATE_VIEW | /*FRUSTUM_UPDATE_PROJECTION |*/ FRUSTUM_UPDATE_FRUSTUMPLANE );
			
			Mat4f& matVirtualLightViewProj	= *virtualLightFrustum.GetViewProjectionMatrix();


			//================== 仮想光源フラスタムの縦横の長さを計算する =================//
			Vec3f ws_vl_corner_points[4];
			virtualLightFrustum.GetSlicePoints( ws_vl_corner_points, virtualLightNear );
			Vec2f vl_frustum_scale	= { Distance( ws_vl_corner_points[0], ws_vl_corner_points[1] ), Distance( ws_vl_corner_points[0], ws_vl_corner_points[2] ) };

			//================== lightFrustumの縦横の長さを計算する =================//
			Vec2f l_frustum_scale	= { Distance( ws_light_cascade_frustum_corners[0], ws_light_cascade_frustum_corners[1] ), Distance( ws_light_cascade_frustum_corners[0], ws_light_cascade_frustum_corners[2] ) };

			//================= 仮想光源からlightFrustum空間に戻す際の拡大縮小率を計算する =====//
			Vec2f scale_vl2l	= { vl_frustum_scale.x / l_frustum_scale.x, vl_frustum_scale.y / l_frustum_scale.y };


			//======== 仮想光源の座標系にカスケードフラスタム頂点を変換する =========//
			Vec3f vlsproj_view_frustum_corners[8];

			for( int k=0; k<8; ++k )
			{
				Vec4f vlsproj_view_frustum_point;
				Multiply( vlsproj_view_frustum_point, matVirtualLightViewProj, ws_cascade_frustum_corners[k] );
				vlsproj_view_frustum_point.x /= vlsproj_view_frustum_point.w;
				vlsproj_view_frustum_point.y /= vlsproj_view_frustum_point.w;
				vlsproj_view_frustum_point.z /= vlsproj_view_frustum_point.w;

				InitVec( vlsproj_view_frustum_corners[k], vlsproj_view_frustum_point.x, vlsproj_view_frustum_point.y, vlsproj_view_frustum_point.z );
			}

			//============================ Calculate Crop Matrix =====================//
			Vec3f cropBBMin = vlsproj_view_frustum_corners[0], cropBBMax = vlsproj_view_frustum_corners[0];
			//cropBBMin	= cropRangeMax;	cropBBMax	= cropRangeMin;// for cropmatrix debug 

			for( int k=1; k<8; ++k )
			{
				Min( cropBBMin, cropBBMin, vlsproj_view_frustum_corners[k] );
				Max( cropBBMax, cropBBMax, vlsproj_view_frustum_corners[k] );
			}

			// 仮想光源系のビュー空間に射影したcropBBを、lightFrustum空間の大きさにスケーリングする
			cropBBMin.x *= scale_vl2l.x;
			cropBBMin.y *= scale_vl2l.y;
			cropBBMax.x *= scale_vl2l.x;
			cropBBMax.y *= scale_vl2l.y;
			
			cropBBMin.x	= Max( cropBBMin.x, cropRangeMin.x );
			cropBBMax.x	= Min( cropBBMax.x, cropRangeMax.x );
			cropBBMin.y	= Max( cropBBMin.y, cropRangeMin.y );
			cropBBMax.y	= Min( cropBBMax.y, cropRangeMax.y );
			cropBBMin.z	= Min( cropBBMin.z, cropRangeMin.z );
			cropBBMax.z	= Min( cropBBMax.z, cropRangeMax.z );

			if( cropBBMin.x >= cropBBMax.x )	cropBBMin.x	= cropRangeMin.x;
			if( cropBBMin.y >= cropBBMax.y )	cropBBMin.y	= cropRangeMin.y;
			//if( cropBBMin.z >= cropBBMax.z )	cropBBMin.z	= cropRangeMin.z;

			if( cropBBMax.x <= cropBBMin.x )	cropBBMax.x	= cropRangeMax.x;
			if( cropBBMax.y <= cropBBMin.y )	cropBBMax.y	= cropRangeMax.y;
			//if( cropBBMax.z <= cropBBMin.z )	cropBBMax.z	= cropRangeMax.z;
			
			Mat4f matCrop;
			CalculateCropMatrix_flicker( &matCrop, cropBBMin, cropBBMax );

			Multiply( matLightViewProjCrops[i], matCrop, newMatLightViewProj );

		}// end of i loop
		


		//for( int i=0; i<num_cascades; ++i )
		//	matLightViewProjCrops[i]	= *lightFrustum.GetViewProjectionMatrix();

		//return true;




		return true;
	}









	void ShadowCaster::UpdateSplitDepths( int num_cascades, float splitDepths[], float znear, float zfar )
	{
		float i_f = 1.0f, cascadeCount	= (float)num_cascades;

		for( int i=0; i<num_cascades-1; ++i, i_f+=1.0f )
		{
			Lerp( splitDepths[i],
				znear + (i_f / cascadeCount) * (zfar - znear),	// Cuni
				znear * powf( zfar / znear, i_f / cascadeCount ),// Clog
				m_SplitLogFactor );	
		}

		splitDepths[ num_cascades - 1 ]	= zfar;
	}



	void ShadowCaster::CalculateCropMatrix( Mat4f* matCrop, const Vec3f& cropBBmin, const Vec3f& cropBBmax )
	{
		float scaleX, scaleY, scaleZ;
		float offsetX, offsetY, offsetZ;

		scaleX = 2.0f / ( cropBBmax.x - cropBBmin.x );
		scaleY = 2.0f / ( cropBBmax.y - cropBBmin.y );
		offsetX	= -0.5f * ( cropBBmax.x + cropBBmin.x ) * scaleX;
		offsetY	= -0.5f * ( cropBBmax.y + cropBBmin.y ) * scaleY;
		scaleZ	= 1.0f / ( cropBBmax.z - cropBBmin.z );
		offsetZ	= -cropBBmin.z * scaleZ;

		MatInit( *matCrop,
			scaleX,	0.0f,	0.0f,	offsetX,
			0.0f,	scaleY,	0.0f,	offsetY,
			0.0f,	0.0f,	1.0f,	0.0f,//scaleZ,	offsetZ,
			0.0f,	0.0f,	0.0f,	1.0f );
	}



	void ShadowCaster::CalculateCropMatrix_flicker( Mat4f* matCrop, const Vec3f& cropBBmin, const Vec3f& cropBBmax )
	{
		float scaleX, scaleY, scaleZ;
		float offsetX, offsetY, offsetZ;

		scaleX = 2.0f / ( cropBBmax.x - cropBBmin.x );
		scaleY = 2.0f / ( cropBBmax.y - cropBBmin.y );

		float scaleQuantizer	= 64.0f;
		scaleX	= 1.0f / ceilf( 1.0f / scaleX * scaleQuantizer ) * scaleQuantizer;
		scaleY	= 1.0f / ceilf( 1.0f / scaleY * scaleQuantizer ) * scaleQuantizer;


		offsetX	= -0.5f * ( cropBBmax.x + cropBBmin.x ) * scaleX;
		offsetY	= -0.5f * ( cropBBmax.y + cropBBmin.y ) * scaleY;

		//float halfTextureSizex	= 0.5f * float( m_ShadowMaps[0].Width() );
		//float halfTextureSizey	= 0.5f * float( m_ShadowMaps[0].Height() );
		//offsetX	= ceilf( offsetX * halfTextureSizex ) / halfTextureSizex;
		//offsetY	= ceilf( offsetY * halfTextureSizey ) / halfTextureSizey;
		offsetX	= ceilf( offsetX * m_HalfShadowMapSize.x ) / m_HalfShadowMapSize.x;
		offsetY	= ceilf( offsetY * m_HalfShadowMapSize.y ) / m_HalfShadowMapSize.y;

		scaleZ	= 1.0f / ( cropBBmax.z - cropBBmin.z );
		offsetZ	= -cropBBmin.z * scaleZ;

		MatInit( *matCrop,
			scaleX,	0.0f,	0.0f,	offsetX,
			0.0f,	scaleY,	0.0f,	offsetY,
			0.0f,	0.0f,	1.0f,	0.0f,//scaleZ,	offsetZ,
			0.0f,	0.0f,	0.0f,	1.0f );
	}






	ShadowAttachments::ShadowAttachments()
	{
		m_numShadowCasters	= 0;
		m_Attachments		= NULL;
	}



	ShadowAttachments::~ShadowAttachments()
	{
		Release();
	}



	void ShadowAttachments::Init( int numshadowcasters )
	{
		Release();

		m_numShadowCasters	= numshadowcasters;
		m_Attachments		= new FrameBufferAttachment[ m_numShadowCasters ];
	}


	
	void ShadowAttachments::Release()
	{
		SafeDeleteArray( m_Attachments );
		m_numShadowCasters	= 0;
	}


	
	void ShadowAttachments::BindShadowMap( uint32 texid )
	{
		for( int i=0; i<m_numShadowCasters; ++i )
			for( int j=0; j<m_Attachments[i].NumAttachPoints(); ++j )
				m_Attachments[i].SetTexID( j, texid );
	}
	


	void ShadowAttachments::UnbindShadowMap()
	{
		BindShadowMap( 0 );
	}



	bool ShadowAttachments::SetupAttachment( int idx, int numattachments, const uint32 texids[], const uint32 attachments[],  const int levels[], const int layers[] )
	{
		if( idx >= m_numShadowCasters )	return false;
		(m_Attachments + idx)->Init( numattachments, attachments, texids, levels, layers );
		return true;
	}


}// endo of namespace