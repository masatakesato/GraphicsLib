#include	"Frustum.h"


namespace OreOreLib
{

	enum FrustumSide
	{
		RIGHT	= 0,		// The RIGHT side of the frustum
		LEFT	= 1,		// The LEFT	 side of the frustum
		BOTTOM	= 2,		// The BOTTOM side of the frustum
		TOP		= 3,		// The TOP side of the frustum
		BACK	= 4,		// The BACK	side of the frustum
		FRONT	= 5			// The FRONT side of the frustum
	}; 




	Frustum::Frustum()
	{
		SetViewParams( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f ); 
		SetProjectionParams( 0.0f, 1.0f, 1.0e-3f, 1.0e+3f );
		//SetProjectionParams( M_PI_4, 1.0f, 1.0e-3f, 1.0e+3f );

		InitQuat( m_Quaternion, 0.0f, 0.0f, 0.0f, 0.0f );

		for( int i=0; i<6; ++i )	InitZero( m_FrustumPlane[i] );
		for( int i=0; i<8; ++i )	InitZero( m_FrustumCornerPoints[i] );

	}



	Frustum::~Frustum()
	{

	}



	void Frustum::SetAttribute( const FrustumAttribute &cam_attr )
	{
		SetViewParams( cam_attr.m_Position, cam_attr.m_Forward, cam_attr.m_Vertical );
		SetProjectionParams( cam_attr.m_Fov, cam_attr.m_AspectRatio, cam_attr.m_zNear, cam_attr.m_zFar );
	}



	void Frustum::SetViewParams( float posx, float posy, float posz, float dirx, float diry, float dirz, float upx, float upy, float upz )
	{
		InitVec( m_FrustumAttrib.m_Position, posx, posy, posz );// set position
		SetDirection( dirx, diry, dirz, upx, upy, upz );// set direction
	}



	void Frustum::SetViewParams( const Vec3f &pos, const Vec3f &dir, const Vec3f &up )
	{
		m_FrustumAttrib.m_Position = pos;// set position
		SetDirection( dir, up );// set direction
	}



	void Frustum::SetViewParams( float posx, float posy, float posz, float dirx, float diry, float dirz, float upx, float upy, float upz, float horx, float hory, float horz )		// Set View Paramete
	{
		InitVec( m_FrustumAttrib.m_Position, posx, posy, posz );// set position
		SetDirection( dirx, diry, dirz, upx, upy, upz, horx, hory, horz );// set direction
	}



	void Frustum::SetViewParams( const Vec3f &pos, const Vec3f &dir, const Vec3f &up, const Vec3f &hor )
	{
		m_FrustumAttrib.m_Position = pos;// set position
		SetDirection( dir, up, hor );// set direction
	}



	// 位置を設定する
	void Frustum::SetPosition( const Vec3f &pos )
	{
		m_FrustumAttrib.m_Position	= pos;
	}



	// 位置を設定する
	void Frustum::SetPosition( float posx, float posy, float posz )
	{
		InitVec( m_FrustumAttrib.m_Position, posx, posy, posz );	
	}



	// 向きを設定する
	void Frustum::SetDirection( float dirx, float diry, float dirz, float upx, float upy, float upz )
	{
		// set forward vector
		InitVec( m_FrustumAttrib.m_Forward, dirx, diry, dirz );

		// set horizontal vector
		Vec3f tmp_vertical	= { upx, upy, upz };
		CrossProduct( m_FrustumAttrib.m_Horizontal, tmp_vertical, m_FrustumAttrib.m_Forward );

		// set vertical vector
		CrossProduct( m_FrustumAttrib.m_Vertical, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Horizontal );

		// normalize
		Normalize( m_FrustumAttrib.m_Forward );
		Normalize( m_FrustumAttrib.m_Horizontal );
		Normalize( m_FrustumAttrib.m_Vertical );

		// calculate quaternion from forward and up vectors
		QuaternionLookAt( m_Quaternion, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Vertical, Z_AXIS, Y_AXIS );
	}



	void Frustum::SetDirection( const Vec3f &dir, const Vec3f &up )
	{
		// set forward vector
		m_FrustumAttrib.m_Forward	= dir;

		// set horizontal vector
		Vec3f tmp_vertical	= up;
		CrossProduct( m_FrustumAttrib.m_Horizontal, tmp_vertical, m_FrustumAttrib.m_Forward );

		// set vertical vector
		CrossProduct( m_FrustumAttrib.m_Vertical, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Horizontal );

		// normalize
		Normalize( m_FrustumAttrib.m_Forward );
		Normalize( m_FrustumAttrib.m_Horizontal );
		Normalize( m_FrustumAttrib.m_Vertical );

		// calculate quaternion from forward and up vectors
		QuaternionLookAt( m_Quaternion, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Vertical, Z_AXIS, Y_AXIS );
	}



	void Frustum::SetDirection( float dirx, float diry, float dirz, float upx, float upy, float upz, float horx, float hory, float horz )
	{
		InitVec( m_FrustumAttrib.m_Forward, dirx, diry, dirz );
		InitVec( m_FrustumAttrib.m_Horizontal, horx, hory, horz );
		InitVec( m_FrustumAttrib.m_Vertical, upx, upy, upz );

		// calculate quaternion from forward and up vectors
		QuaternionLookAt( m_Quaternion, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Vertical, Z_AXIS, Y_AXIS );
	}



	void Frustum::SetDirection( const Vec3f &dir, const Vec3f &up, const Vec3f &hor )
	{
		m_FrustumAttrib.m_Forward		= dir;
		m_FrustumAttrib.m_Horizontal	= hor;
		m_FrustumAttrib.m_Vertical		= up;

		// calculate quaternion from forward and up vectors
		QuaternionLookAt( m_Quaternion, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Vertical, Z_AXIS, Y_AXIS );
	}



	// 射影変換のパラメータを設定する
	void Frustum::SetProjectionParams( float fov, float aspect, float znear, float zfar )
	{
		m_FrustumAttrib.m_Fov			= fov;
		m_FrustumAttrib.m_AspectRatio	= aspect;
		m_FrustumAttrib.m_zNear			= znear;
		m_FrustumAttrib.m_zFar			= zfar;

		MatPerspectiveFov( m_Transform.matProjection, m_FrustumAttrib.m_Fov, m_FrustumAttrib.m_AspectRatio, m_FrustumAttrib.m_zNear, m_FrustumAttrib.m_zFar );
	}



	// Fovを設定する
	void Frustum::SetFov( float fov )
	{
		m_FrustumAttrib.m_Fov = fov;
		MatPerspectiveFov( m_Transform.matProjection, m_FrustumAttrib.m_Fov, m_FrustumAttrib.m_AspectRatio, m_FrustumAttrib.m_zNear, m_FrustumAttrib.m_zFar );
	}



	// アスペクト比を設定する
	void Frustum::SetAspectRatio( float aspect )
	{
		m_FrustumAttrib.m_AspectRatio = aspect;
		MatPerspectiveFov( m_Transform.matProjection, m_FrustumAttrib.m_Fov, m_FrustumAttrib.m_AspectRatio, m_FrustumAttrib.m_zNear, m_FrustumAttrib.m_zFar );
	}



	// 奥行き最小値を設定する
	void Frustum::SetZNear( float znear )
	{
		m_FrustumAttrib.m_zNear	= znear;
		MatPerspectiveFov( m_Transform.matProjection, m_FrustumAttrib.m_Fov, m_FrustumAttrib.m_AspectRatio, m_FrustumAttrib.m_zNear, m_FrustumAttrib.m_zFar );
	}



	// 奥行き最大値を設定する
	void Frustum::SetZFar( float zfar )
	{
		m_FrustumAttrib.m_zFar	= zfar;
		MatPerspectiveFov( m_Transform.matProjection, m_FrustumAttrib.m_Fov, m_FrustumAttrib.m_AspectRatio, m_FrustumAttrib.m_zNear, m_FrustumAttrib.m_zFar );
	}



	// 任意軸まわりの回転
	void Frustum::Rotate( float Angle, float x, float y, float z )
	{
		//=============== クォータニオン設定 ==============//
		//Vec4f	newQuat, prevQuat;
		Quatf	newQuat, prevQuat;

		prevQuat = m_Quaternion;
		InitQuat( newQuat, Angle, x, y, z );	// クォータニオンは半時計周りなので、回転角度を正負反転させる

		Multiply( m_Quaternion, newQuat, prevQuat );//m_CameraAttrib.m_Quaternion = mult( newQuat, prevQuat ); //
		UpdateBasis();
	}



	// ビュー平面上の移動ベクトルに応じた回転
	void Frustum::Rotate( float dh, float dv )
	{
		Vec3f	vert_scaled, hol_scaled;
		Vec3f	rot_axis;
		float		angle;

		if(dh == 0.0f && dv == 0.0f) return;

		//=============== 回転軸と角度を決める ===============//
		Scale( hol_scaled, m_FrustumAttrib.m_Horizontal, dv );// 画面横方向の動きでカメラ垂直軸の回転を制御する
		Scale( vert_scaled, m_FrustumAttrib.m_Vertical, dh );// 画面たて方向の動きでカメラ水平軸の回転を制御する

		Subtract( rot_axis, vert_scaled, hol_scaled  );
		//SET(rot_axis, vert_scaled);
		angle = Length(rot_axis);	
		Scale(rot_axis, 1.0f/angle);

		//================== 軸周りに回転 ====================//
		Rotate( angle, rot_axis.x, rot_axis.y, rot_axis.z );
	}



	void Frustum::Yaw( float Angle )
	{
		Quatf	newQuat, prevQuat;//Vec4f	newQuat, prevQuat;//

		prevQuat	= m_Quaternion;
		InitQuat( newQuat, Angle, m_FrustumAttrib.m_Vertical );

		Multiply( m_Quaternion, newQuat, prevQuat  );// 注意!. 最後にかけたクォータニオンが最初に有効になる
		UpdateBasis();
	}



	void Frustum::Pitch( float Angle )
	{
		Quatf	newQuat, prevQuat;// Vec4f	newQuat, prevQuat;

		prevQuat	= m_Quaternion;
		InitQuat( newQuat, Angle, m_FrustumAttrib.m_Horizontal );

		Multiply( m_Quaternion, newQuat, prevQuat  );// 注意!. 最後にかけたクォータニオンが最初に有効になる
		UpdateBasis();
	}



	void Frustum::Roll( float Angle )
	{
		Quatf	newQuat, prevQuat;

		prevQuat	= m_Quaternion;
		InitQuat( newQuat, Angle, m_FrustumAttrib.m_Forward );

		Multiply( m_Quaternion, newQuat, prevQuat );// 注意!. 最後にかけたクォータニオンが最初に有効になる
		UpdateBasis();
	}



	void Frustum::Translate( float df, float dh, float dv )
	{
		FrustumAttribute *pattrib = &m_FrustumAttrib;
		//============ カメラの位置を移動する ============//
		pattrib->m_Position.x += df * pattrib->m_Forward.x + dh * pattrib->m_Horizontal.x + dv * pattrib->m_Vertical.x;
		pattrib->m_Position.y += df * pattrib->m_Forward.y + dh * pattrib->m_Horizontal.y + dv * pattrib->m_Vertical.y;
		pattrib->m_Position.z += df * pattrib->m_Forward.z + dh * pattrib->m_Horizontal.z + dv * pattrib->m_Vertical.z;
	}



	void Frustum::GetSlicePoints( Vec3f* slice_points, float slicedepth ) const
	{
		
		//const Vec3f *pos	= &m_FrustumAttrib.m_Position;
		//const Vec3f *dir	= &m_FrustumAttrib.m_Forward;
		//const Vec3f *up		= &m_FrustumAttrib.m_Vertical;
		//const Vec3f *hor	= &m_FrustumAttrib.m_Horizontal;


		////================ slicedepthの4点を計算 =================//
		//float v		= tan( m_FrustumAttrib.m_Fov * 0.5f ) * slicedepth;
		//float h		= v * m_FrustumAttrib.m_AspectRatio;

		//Vec3f Cnear		= { pos->x + dir->x * slicedepth, pos->y + dir->y * slicedepth, pos->z + dir->z * slicedepth };
		//Vec3f vVecNear	= { up->x * v, up->y * v, up->z * v };
		//Vec3f hVecNear	= { hor->x * h, hor->y * h, hor->z * h };

		//// top left
		//InitVec( slice_points[0], Cnear.x + vVecNear.x - hVecNear.x, Cnear.y + vVecNear.y - hVecNear.y, Cnear.z + vVecNear.z - hVecNear.z );
		//// top right
		//InitVec( slice_points[1], Cnear.x + vVecNear.x + hVecNear.x, Cnear.y + vVecNear.y + hVecNear.y, Cnear.z + vVecNear.z + hVecNear.z );
		//// bottom left
		//InitVec( slice_points[2], Cnear.x - vVecNear.x - hVecNear.x, Cnear.y - vVecNear.y - hVecNear.y, Cnear.z - vVecNear.z - hVecNear.z );
		//// bottom right
		//InitVec( slice_points[3], Cnear.x - vVecNear.x + hVecNear.x, Cnear.y - vVecNear.y + hVecNear.y, Cnear.z - vVecNear.z + hVecNear.z );
		
		float coeff	= ( slicedepth - m_FrustumAttrib.m_zNear ) / ( m_FrustumAttrib.m_zFar - m_FrustumAttrib.m_zNear );
		Lerp( slice_points[0], m_FrustumCornerPoints[0], m_FrustumCornerPoints[4], coeff );
		Lerp( slice_points[1], m_FrustumCornerPoints[1], m_FrustumCornerPoints[5], coeff );
		Lerp( slice_points[2], m_FrustumCornerPoints[2], m_FrustumCornerPoints[6], coeff );
		Lerp( slice_points[3], m_FrustumCornerPoints[3], m_FrustumCornerPoints[7], coeff );
	}



	bool Frustum::PointInFrustum( const Vec3f& p ) const
	{
		// Go through all the sides of the frustum
		for(int i = 0; i < 6; i++ )
		{
			// Calculate the plane equation and check if the point is behind a side of the frustum
			if( m_FrustumPlane[i].x * p.x + m_FrustumPlane[i].y * p.y + m_FrustumPlane[i].z * p.z + m_FrustumPlane[i].w <= 0 )
			{
				// The point was behind a side, so it ISN'T in the frustum
				return false;
			}
		}

		// The point was inside of the frustum (In front of ALL the sides of the frustum)
		return true;
	}


	bool Frustum::SphereInFrustum( const Vec3f& center, float radius ) const
	{
		// Now this function is almost identical to the PointInFrustum(), except we
		// now have to deal with a radius around the point.  The point is the center of
		// the radius.  So, the point might be outside of the frustum, but it doesn't
		// mean that the rest of the sphere is.  It could be half and half.  So instead of
		// checking if it's less than 0, we need to add on the radius to that.  Say the
		// equation produced -2, which means the center of the sphere is the distance of
		// 2 behind the plane.  Well, what if the radius was 5?  The sphere is still inside,
		// so we would say, if(-2 < -5) then we are outside.  In that case it's false,
		// so we are inside of the frustum, but a distance of 3.  This is reflected below.

		// Go through all the sides of the frustum
		for(int i = 0; i < 6; i++ )	
		{
			// If the center of the sphere is farther away from the plane than the radius
			if( m_FrustumPlane[i].x * center.x + m_FrustumPlane[i].y * center.y + m_FrustumPlane[i].z * center.z + m_FrustumPlane[i].w <= -radius )
			{
				// The distance was greater than the radius so the sphere is outside of the frustum
				return false;
			}
		}

		// The sphere was inside of the frustum!
		return true;
	}



	bool Frustum::AABBInFrustum( const Vec3f& boxMin, const Vec3f& boxMax ) const
	{
		// check box outside/inside of frustum
		for( int i=0; i<6; i++ )
		{
			if( m_FrustumPlane[i].x * boxMin.x + m_FrustumPlane[i].y * boxMin.y + m_FrustumPlane[i].z * boxMin.z + m_FrustumPlane[i].w > 0.0f )	continue;
			if( m_FrustumPlane[i].x * boxMax.x + m_FrustumPlane[i].y * boxMin.y + m_FrustumPlane[i].z * boxMin.z + m_FrustumPlane[i].w > 0.0f )	continue;
			if( m_FrustumPlane[i].x * boxMin.x + m_FrustumPlane[i].y * boxMax.y + m_FrustumPlane[i].z * boxMin.z + m_FrustumPlane[i].w > 0.0f )	continue;
			if( m_FrustumPlane[i].x * boxMax.x + m_FrustumPlane[i].y * boxMax.y + m_FrustumPlane[i].z * boxMin.z + m_FrustumPlane[i].w > 0.0f )	continue;
			if( m_FrustumPlane[i].x * boxMin.x + m_FrustumPlane[i].y * boxMin.y + m_FrustumPlane[i].z * boxMax.z + m_FrustumPlane[i].w > 0.0f )	continue;
			if( m_FrustumPlane[i].x * boxMax.x + m_FrustumPlane[i].y * boxMin.y + m_FrustumPlane[i].z * boxMax.z + m_FrustumPlane[i].w > 0.0f )	continue;
			if( m_FrustumPlane[i].x * boxMin.x + m_FrustumPlane[i].y * boxMax.y + m_FrustumPlane[i].z * boxMax.z + m_FrustumPlane[i].w > 0.0f )	continue;
			if( m_FrustumPlane[i].x * boxMax.x + m_FrustumPlane[i].y * boxMax.y + m_FrustumPlane[i].z * boxMax.z + m_FrustumPlane[i].w > 0.0f )	continue;

			// If we get here, it isn't in the frustum
			return false;
		}

		return true;
	}



	void Frustum::UpdateMatrix( uint32 mode )
	{
		FrustumAttribute *pattrib = &m_FrustumAttrib;

		// Update View Matrix
		if( mode & FRUSTUM_UPDATE_VIEW )
			MatViewGL( m_Transform.matView, pattrib->m_Horizontal, pattrib->m_Vertical, pattrib->m_Forward, pattrib->m_Position );

		// Update Projection Matrix
		if( mode & FRUSTUM_UPDATE_PROJECTION )
			MatPerspectiveFov( m_Transform.matProjection, pattrib->m_Fov, pattrib->m_AspectRatio, pattrib->m_zNear, pattrib->m_zFar );

		// Update ViewProjection Matrix /  Inverse View Projection Matrix
		if( mode & ( FRUSTUM_UPDATE_VIEW | FRUSTUM_UPDATE_PROJECTION) )
		{
			float det	= 0.0f;
			Multiply( m_Transform.matViewProjection, m_Transform.matProjection, m_Transform.matView );
			MatInverse( m_Transform.matInvViewProjection, det, m_Transform.matViewProjection );
		}

		// Update ViewFrustum
		if( mode &  FRUSTUM_UPDATE_FRUSTUMPLANE )
		{
			CalculateFrustumPlane( m_Transform.matViewProjection );
			CalculateFrustumCornerPoints( m_FrustumCornerPoints );
		}

	}



	void Frustum::UpdateBasis()
	{
		FrustumAttribute *pattrib = &m_FrustumAttrib;

		rm_cal_err_qt( m_Quaternion );

		Mat4f matQuat;
		Quat2Mat( matQuat, m_Quaternion );

		Multiply( pattrib->m_Forward, matQuat, OreOreLib::Z_AXIS );
		Multiply( pattrib->m_Vertical, matQuat, OreOreLib::Y_AXIS );
		Multiply( pattrib->m_Horizontal, matQuat, OreOreLib::X_AXIS );
	}




	// This normalizes a plane (A side) from a given frustum.
	void Frustum::NormalizePlane( Vec4f* frustumPlane )
	{
		// Here we calculate the magnitude of the normal to the plane (point A B C)
		// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
		// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
		float magnitude = (float)sqrt( frustumPlane->x * frustumPlane->x + 
			frustumPlane->y * frustumPlane->y + 
			frustumPlane->z * frustumPlane->z );

		// Then we divide the plane's values by it's magnitude.
		// This makes it easier to work with.
		frustumPlane->x /= magnitude;
		frustumPlane->y /= magnitude;
		frustumPlane->z /= magnitude;
		frustumPlane->w /= magnitude; 
	}



	void Frustum::CalculateFrustumPlane( const Mat4f &matViewProj )
	{
		// Now we actually want to get the sides of the frustum.  To do this we take
		// the clipping planes we received above and extract the sides from them.

		// This will extract the RIGHT side of the frustum
		m_FrustumPlane[RIGHT].x		= matViewProj.m30 - matViewProj.m00;
		m_FrustumPlane[RIGHT].y		= matViewProj.m31 - matViewProj.m01;
		m_FrustumPlane[RIGHT].z		= matViewProj.m32 - matViewProj.m02;
		m_FrustumPlane[RIGHT].w		= matViewProj.m33 - matViewProj.m03;

		// Now that we have a normal (A,B,C) and a distance (D) to the plane,
		// we want to normalize that normal and distance.

		// Normalize the RIGHT side
		NormalizePlane( &m_FrustumPlane[RIGHT]);

		// This will extract the LEFT side of the frustum
		m_FrustumPlane[LEFT].x		= matViewProj.m30 + matViewProj.m00;
		m_FrustumPlane[LEFT].y		= matViewProj.m31 + matViewProj.m01;
		m_FrustumPlane[LEFT].z		= matViewProj.m32 + matViewProj.m02;
		m_FrustumPlane[LEFT].w		= matViewProj.m33 + matViewProj.m03;

		// Normalize the LEFT side
		NormalizePlane( &m_FrustumPlane[LEFT] );

		// This will extract the BOTTOM side of the frustum
		m_FrustumPlane[BOTTOM].x	= matViewProj.m30 + matViewProj.m10;
		m_FrustumPlane[BOTTOM].y	= matViewProj.m31 + matViewProj.m11;
		m_FrustumPlane[BOTTOM].z	= matViewProj.m32 + matViewProj.m12;
		m_FrustumPlane[BOTTOM].w	= matViewProj.m33 + matViewProj.m13;

		// Normalize the BOTTOM side
		NormalizePlane( &m_FrustumPlane[BOTTOM] );

		// This will extract the TOP side of the frustum
		m_FrustumPlane[TOP].x		= matViewProj.m30 - matViewProj.m10;
		m_FrustumPlane[TOP].y		= matViewProj.m31 - matViewProj.m11;
		m_FrustumPlane[TOP].z		= matViewProj.m32 - matViewProj.m12;
		m_FrustumPlane[TOP].w		= matViewProj.m33 - matViewProj.m13;

		// Normalize the TOP side
		NormalizePlane( &m_FrustumPlane[TOP] );

		// This will extract the BACK side of the frustum
		m_FrustumPlane[BACK].x		= matViewProj.m30 - matViewProj.m20;
		m_FrustumPlane[BACK].y		= matViewProj.m31 - matViewProj.m21;
		m_FrustumPlane[BACK].z		= matViewProj.m32 - matViewProj.m22;
		m_FrustumPlane[BACK].w		= matViewProj.m33 - matViewProj.m23;

		// Normalize the BACK side
		NormalizePlane( &m_FrustumPlane[BACK] );

		// This will extract the FRONT side of the frustum
		m_FrustumPlane[FRONT].x		= matViewProj.m30 + matViewProj.m20;
		m_FrustumPlane[FRONT].y		= matViewProj.m31 + matViewProj.m21;
		m_FrustumPlane[FRONT].z		= matViewProj.m32 + matViewProj.m22;
		m_FrustumPlane[FRONT].w		= matViewProj.m33 + matViewProj.m23;

		// Normalize the FRONT side
		NormalizePlane( &m_FrustumPlane[FRONT] );
	}



	// ビューフラスタム(視錐台)の8頂点を取得する
	// http://gamedev.stackexchange.com/questions/19774/determine-corners-of-a-specific-plane-in-the-frustum
	void Frustum::CalculateFrustumCornerPoints( Vec3f* corner_points ) const
	{
		const Vec3f *pos	= &m_FrustumAttrib.m_Position;
		const Vec3f *dir	= &m_FrustumAttrib.m_Forward;
		const Vec3f *up		= &m_FrustumAttrib.m_Vertical;
		const Vec3f *hor	= &m_FrustumAttrib.m_Horizontal;


		//================ zNearの4点を計算 =================//
		float znear		= m_FrustumAttrib.m_zNear;
		float vNear		= tan( m_FrustumAttrib.m_Fov * 0.5f ) * znear;
		float hNear		= vNear * m_FrustumAttrib.m_AspectRatio;

		Vec3f Cnear		= { pos->x + dir->x * znear, pos->y + dir->y * znear, pos->z + dir->z * znear };
		Vec3f vVecNear	= { up->x * vNear, up->y * vNear, up->z * vNear };
		Vec3f hVecNear	= { hor->x * hNear, hor->y * hNear, hor->z * hNear };

		// near top left
		InitVec( corner_points[0], Cnear.x + vVecNear.x - hVecNear.x, Cnear.y + vVecNear.y - hVecNear.y, Cnear.z + vVecNear.z - hVecNear.z );
		// near top right
		InitVec( corner_points[1], Cnear.x + vVecNear.x + hVecNear.x, Cnear.y + vVecNear.y + hVecNear.y, Cnear.z + vVecNear.z + hVecNear.z );
		// near bottom left
		InitVec( corner_points[2], Cnear.x - vVecNear.x - hVecNear.x, Cnear.y - vVecNear.y - hVecNear.y, Cnear.z - vVecNear.z - hVecNear.z );
		// near bottom right
		InitVec( corner_points[3], Cnear.x - vVecNear.x + hVecNear.x, Cnear.y - vVecNear.y + hVecNear.y, Cnear.z - vVecNear.z + hVecNear.z );

		//================= zFarの4点を計算 =================//	
		float zfar		= m_FrustumAttrib.m_zFar;
		float vFar		= tan( m_FrustumAttrib.m_Fov * 0.5f ) * zfar;
		float hFar		= vFar * m_FrustumAttrib.m_AspectRatio;

		Vec3f Cfar		= { pos->x + dir->x * zfar, pos->y + dir->y * zfar, pos->z + dir->z * zfar };
		Vec3f vVecFar	= { up->x * vFar, up->y * vFar, up->z * vFar };
		Vec3f hVecFar	= { hor->x * hFar, hor->y * hFar, hor->z * hFar };

		// far top left
		InitVec( corner_points[4], Cfar.x + vVecFar.x - hVecFar.x, Cfar.y + vVecFar.y - hVecFar.y, Cfar.z + vVecFar.z - hVecFar.z );
		// far top right
		InitVec( corner_points[5], Cfar.x + vVecFar.x + hVecFar.x, Cfar.y + vVecFar.y + hVecFar.y, Cfar.z + vVecFar.z + hVecFar.z );
		// far bottom left
		InitVec( corner_points[6], Cfar.x - vVecFar.x - hVecFar.x, Cfar.y - vVecFar.y - hVecFar.y, Cfar.z - vVecFar.z - hVecFar.z );
		// far bottom right
		InitVec( corner_points[7], Cfar.x - vVecFar.x + hVecFar.x, Cfar.y - vVecFar.y + hVecFar.y, Cfar.z - vVecFar.z + hVecFar.z );
	}




}// end of namespace