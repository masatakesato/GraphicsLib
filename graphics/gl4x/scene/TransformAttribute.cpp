#include	"TransformAttribute.h"

#include	"../resource/BufferLayout.h"



namespace OreOreLib
{

	TransformAttribute::TransformAttribute()
	{
		m_InitialBasis[X_SPACE]		= X_AXIS;
		m_InitialBasis[Y_SPACE]		= Y_AXIS;
		m_InitialBasis[Z_SPACE]		= Z_AXIS;

		m_TransformedBasis[X_SPACE]	= m_InitialBasis[X_SPACE];
		m_TransformedBasis[Y_SPACE]	= m_InitialBasis[Y_SPACE];
		m_TransformedBasis[Z_SPACE]	= m_InitialBasis[Z_SPACE];

		InitZero( m_Origin );

		InitVec( m_Scale, 1.0f, 1.0f, 1.0f );

		QuatIdentity( m_Quaternion );
	}



	//TransformAttribute::TransformAttribute( BASIS_TYPE basis )
	//{
	//	//int basis_x, basis_y, basis_z;

	//	switch( basis )
	//	{
	//	case BASIS_XYZ:
	//		m_DirU = 0;	m_DirV = 1;	m_DirN = 2;
	//		break;
	//	case BASIS_XZY:
	//		m_DirU = 0;	m_DirV = 2;	m_DirN = 1;
	//		break;
	//	case BASIS_YXZ:
	//		m_DirU = 1;	m_DirV = 0;	m_DirN = 2;
	//		break;
	//	case BASIS_YZX:
	//		m_DirU = 2;	m_DirV = 0;	m_DirN = 1;
	//		break;
	//	case BASIS_ZXY:
	//		m_DirU = 1;	m_DirV = 2;	m_DirN = 0;
	//		break;
	//	case BASIS_ZYX:
	//		m_DirU = 2;	m_DirV = 1;	m_DirN = 0;
	//		break;
	//	default:
	//		m_DirU = 2;	m_DirV = 1;	m_DirN = 0;
	//		break;
	//	}

	//	m_InitialBasis[m_DirU]	= X_AXIS;
	//	m_InitialBasis[m_DirV]	= Y_AXIS;
	//	m_InitialBasis[m_DirN]	= Z_AXIS;

	//	m_TransformedBasis[0]		= m_InitialBasis[0];
	//	m_TransformedBasis[1]		= m_InitialBasis[1];
	//	m_TransformedBasis[2]		= m_InitialBasis[2];

	//	InitZero( m_Position );

	//	InitVec( m_Scale, 1.0f, 1.0f, 1.0f );

	//	QuatIdentity( m_Quaternion );
	//}




	// 向きを設定する. これはカメラ向き
	void TransformAttribute::SetDirection( float dirx, float diry, float dirz, float upx, float upy, float upz )
	{
		// set forward vector
		InitVec( m_TransformedBasis[Z_SPACE], dirx, diry, dirz );

		// set horizontal vector
		Vec3f tmp_vertical		= { upx, upy, upz };
		Vec3f temp_hortizontal;
		CrossProduct( /*m_TransformedBasis[X_SPACE]*/temp_hortizontal, tmp_vertical, m_TransformedBasis[Z_SPACE] );

		// set vertical vector
		CrossProduct( m_TransformedBasis[Y_SPACE], m_TransformedBasis[Z_SPACE], /*m_TransformedBasis[X_SPACE]*/temp_hortizontal );

		// normalize
		Normalize( m_TransformedBasis[Z_SPACE] );// m_FrustumAttrib.m_Forward );
		//Normalize( m_TransformedBasis[X_SPACE] );// m_FrustumAttrib.m_Horizontal );
		Normalize( m_TransformedBasis[Y_SPACE] );// m_FrustumAttrib.m_Vertical );

		// calculate quaternion from forward and up vectors
		//QuaternionLookAt( m_Quaternion, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Vertical );
		QuaternionLookAt( m_Quaternion, m_TransformedBasis[Z_SPACE], m_TransformedBasis[Y_SPACE], m_InitialBasis[Z_SPACE], m_InitialBasis[Y_SPACE] );

		rm_cal_err_qt( m_Quaternion );

		Mat4f matQuat;
		Quat2Mat( matQuat, m_Quaternion );

		// rotate x-axis
		Multiply( m_TransformedBasis[X_SPACE], matQuat, m_InitialBasis[X_SPACE] );

	}



	void TransformAttribute::SetDirection( const Vec3f &dir, const Vec3f &up )
	{
		// set forward vector
		m_TransformedBasis[Z_SPACE]	= dir;//m_FrustumAttrib.m_Forward = dir;

		// set horizontal vector
		Vec3f tmp_vertical = up;
		Vec3f temp_hortizontal;
		CrossProduct( /*m_TransformedBasis[X_SPACE]*/temp_hortizontal, tmp_vertical, m_TransformedBasis[Z_SPACE] );//CrossProduct( m_FrustumAttrib.m_Horizontal, tmp_vertical, m_FrustumAttrib.m_Forward );

		// set vertical vector
		CrossProduct( m_TransformedBasis[Y_SPACE], m_TransformedBasis[Z_SPACE], /*m_TransformedBasis[X_SPACE]*/temp_hortizontal );// CrossProduct( m_FrustumAttrib.m_Vertical, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Horizontal );

		// normalize
		Normalize( m_TransformedBasis[Z_SPACE] );// m_FrustumAttrib.m_Forward );
		//Normalize( m_TransformedBasis[X_SPACE] );// m_FrustumAttrib.m_Horizontal );
		Normalize( m_TransformedBasis[Y_SPACE] );// m_FrustumAttrib.m_Vertical );

		// calculate quaternion from forward and up vectors
		//QuaternionLookAt( m_Quaternion, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Vertical );
		QuaternionLookAt( m_Quaternion, m_TransformedBasis[Z_SPACE], m_TransformedBasis[Y_SPACE], m_InitialBasis[Z_SPACE], m_InitialBasis[Y_SPACE] );


		rm_cal_err_qt( m_Quaternion );

		Mat4f matQuat;
		Quat2Mat( matQuat, m_Quaternion );

		// rotate x-axis
		Multiply( m_TransformedBasis[X_SPACE], matQuat, m_InitialBasis[X_SPACE] );
	}



	// 任意軸まわりの回転
	void TransformAttribute::Rotate( float Angle, float x, float y, float z )
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
	void TransformAttribute::Rotate( float dh, float dv )
	{
		Vec3f	vert_scaled, hol_scaled;
		Vec3f	rot_axis;
		float		angle;

		if(dh == 0.0f && dv == 0.0f) return;

		//=============== 回転軸と角度を決める ===============//
		Scale( hol_scaled, m_TransformedBasis[X_SPACE]/*m_FrustumAttrib.m_Horizontal*/, dv );// 画面横方向の動きでカメラ垂直軸の回転を制御する
		Scale( vert_scaled, m_TransformedBasis[Y_SPACE]/*m_FrustumAttrib.m_Vertical*/, dh );// 画面たて方向の動きでカメラ水平軸の回転を制御する

		Subtract( rot_axis, vert_scaled, hol_scaled );
		//SET(rot_axis, vert_scaled);
		angle = Length( rot_axis );
		Scale( rot_axis, 1.0f/angle );

		//================== 軸周りに回転 ====================//
		Rotate( angle, rot_axis.x, rot_axis.y, rot_axis.z );
	}



	void TransformAttribute::Yaw( float Angle )
	{
		Quatf	newQuat, prevQuat;//Vec4f	newQuat, prevQuat;//

		prevQuat	= m_Quaternion;
		InitQuat( newQuat, Angle, m_TransformedBasis[Y_SPACE]/*m_FrustumAttrib.m_Vertical*/ );

		Multiply( m_Quaternion, newQuat, prevQuat );// 注意!. 最後にかけたクォータニオンが最初に有効になる
		UpdateBasis();
	}



	void TransformAttribute::Pitch( float Angle )
	{
		Quatf	newQuat, prevQuat;// Vec4f	newQuat, prevQuat;

		prevQuat	= m_Quaternion;
		InitQuat( newQuat, Angle, m_TransformedBasis[X_SPACE]/*m_FrustumAttrib.m_Horizontal*/ );

		Multiply( m_Quaternion, newQuat, prevQuat );// 注意!. 最後にかけたクォータニオンが最初に有効になる
		UpdateBasis();
	}



	void TransformAttribute::Roll( float Angle )
	{
		Quatf	newQuat, prevQuat;

		prevQuat	= m_Quaternion;
		InitQuat( newQuat, Angle, m_TransformedBasis[Z_SPACE]/*m_FrustumAttrib.m_Forward*/ );

		Multiply( m_Quaternion, newQuat, prevQuat );// 注意!. 最後にかけたクォータニオンが最初に有効になる
		UpdateBasis();
	}



	void TransformAttribute::Translate( float df, float dh, float dv )
	{
		//============ カメラの姿勢ベクトルを使って位置を平行移動する ============//
		m_Origin.x	+= df * m_TransformedBasis[Z_SPACE].x + dh * m_TransformedBasis[X_SPACE].x + dv * m_TransformedBasis[Y_SPACE].x;
		m_Origin.y	+= df * m_TransformedBasis[Z_SPACE].y + dh * m_TransformedBasis[X_SPACE].y + dv * m_TransformedBasis[Y_SPACE].y;
		m_Origin.z	+= df * m_TransformedBasis[Z_SPACE].z + dh * m_TransformedBasis[X_SPACE].z + dv * m_TransformedBasis[Y_SPACE].z;
	}


	// 注意!: 所期姿勢から行列を使って変換する
	void TransformAttribute::Transform( const Mat4f& matTransform )
	{
		Vec4f _in, _out;

		InitVec( _in, 0.0f, 0.0f, 0.0f, 1.0f );
		Multiply( _out, matTransform, _in );
		m_Origin	= _out.xyz;


		Mat4f _matin, _matout;
		
		MatInit( _matin,
			m_InitialBasis[0].x,	m_InitialBasis[1].x,	m_InitialBasis[2].x,	0.0f,
			m_InitialBasis[0].y,	m_InitialBasis[1].y,	m_InitialBasis[2].y,	0.0f,
			m_InitialBasis[0].z,	m_InitialBasis[1].z,	m_InitialBasis[2].z,	0.0f,
			0.0f,					0.0f,					0.0f,					0.0f
		);

		Multiply( _matout, matTransform, _matin );
		InitVec( m_TransformedBasis[0], _matout.m00, _matout.m10, _matout.m20 );
		InitVec( m_TransformedBasis[1], _matout.m01, _matout.m11, _matout.m21 );
		InitVec( m_TransformedBasis[2], _matout.m02, _matout.m12, _matout.m22 );

		QuaternionLookAt( m_Quaternion, m_TransformedBasis[Z_SPACE], m_TransformedBasis[Y_SPACE], m_InitialBasis[Z_SPACE], m_InitialBasis[Y_SPACE] );


		//Vec4f _in, _out;

		//InitVec( _in, m_Origin.x, m_Origin.y, m_Origin.z, 1.0f );
		//Multiply( _out, matTransform, _in );
		//m_Origin	= _out.xyz;


		//Mat4f _matin, _matout;

		//MatInit( _matin,
		//	m_TransformedBasis[0].x, m_TransformedBasis[1].x, m_TransformedBasis[2].x, 0.0f,
		//	m_TransformedBasis[0].y, m_TransformedBasis[1].y, m_TransformedBasis[2].y, 0.0f,
		//	m_TransformedBasis[0].z, m_TransformedBasis[1].z, m_TransformedBasis[2].z, 0.0f,
		//	0.0f, 0.0f, 0.0f, 0.0f
		//	);

		//Multiply( _matout, matTransform, _matin );
		//InitVec( m_TransformedBasis[0], _matout.m00, _matout.m10, _matout.m20 );
		//InitVec( m_TransformedBasis[1], _matout.m01, _matout.m11, _matout.m21 );
		//InitVec( m_TransformedBasis[2], _matout.m02, _matout.m12, _matout.m22 );

	}



	void TransformAttribute::SetDirection( float dirx, float diry, float dirz, float upx, float upy, float upz, float horx, float hory, float horz )
	{
		InitVec( m_TransformedBasis[Z_SPACE]/*m_FrustumAttrib.m_Forward*/, dirx, diry, dirz );
		InitVec( m_TransformedBasis[X_SPACE]/*m_FrustumAttrib.m_Horizontal*/, horx, hory, horz );
		InitVec( m_TransformedBasis[Y_SPACE]/*m_FrustumAttrib.m_Vertical*/, upx, upy, upz );

		// calculate quaternion from forward and up vectors
		//QuaternionLookAt( m_Quaternion, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Vertical );
		QuaternionLookAt( m_Quaternion, m_TransformedBasis[Z_SPACE], m_TransformedBasis[Y_SPACE], m_InitialBasis[Z_SPACE], m_InitialBasis[Y_SPACE] );
	}



	void TransformAttribute::SetDirection( const Vec3f &dir, const Vec3f &up, const Vec3f &hor )
	{
		m_TransformedBasis[Z_SPACE]	= dir;// m_FrustumAttrib.m_Forward = dir;
		m_TransformedBasis[X_SPACE]	= hor;// m_FrustumAttrib.m_Horizontal = hor;
		m_TransformedBasis[Y_SPACE]	= up;// m_FrustumAttrib.m_Vertical = up;

		// calculate quaternion from forward and up vectors
		//QuaternionLookAt( m_Quaternion, m_FrustumAttrib.m_Forward, m_FrustumAttrib.m_Vertical );
		QuaternionLookAt( m_Quaternion, m_TransformedBasis[Z_SPACE], m_TransformedBasis[Y_SPACE], m_InitialBasis[Z_SPACE], m_InitialBasis[Y_SPACE] );
	}



	void TransformAttribute::SetBasis( const Vec3f& vecX, const Vec3f& vecY, const Vec3f& vecZ )
	{
		m_InitialBasis[X_SPACE]	= vecX;// x axis
		m_InitialBasis[Y_SPACE]	= vecY;// y axis
		m_InitialBasis[Z_SPACE]	= vecZ;// z axis

		QuaternionLookAt( m_Quaternion, m_TransformedBasis[Z_SPACE], m_TransformedBasis[Y_SPACE], m_InitialBasis[Z_SPACE], m_InitialBasis[Y_SPACE] );
	}



	void TransformAttribute::SetBasis( float vecX_x, float vecX_y, float vecX_z, float vecY_x, float vecY_y, float vecY_z, float vecZ_x, float vecZ_y, float vecZ_z )
	{
		InitVec( m_InitialBasis[X_SPACE], vecX_x, vecX_y, vecX_z );// x axis
		InitVec( m_InitialBasis[Y_SPACE], vecY_x, vecY_y, vecY_z );// y axis
		InitVec( m_InitialBasis[Z_SPACE], vecZ_x, vecZ_y, vecZ_z );// z axis	

		QuaternionLookAt( m_Quaternion, m_TransformedBasis[Z_SPACE], m_TransformedBasis[Y_SPACE], m_InitialBasis[Z_SPACE], m_InitialBasis[Y_SPACE] );
	}



	void TransformAttribute::SetOrientation( const Quatf& quat )
	{
		m_Quaternion	= quat;
		UpdateBasis();
		//rm_cal_err_qt( m_Quaternion );

		//Mat4f matQuat;
		//Quat2Mat( matQuat, quat );

		//Multiply( m_TransformedBasis[0], matQuat, m_InitialBasis[0] );
		//Multiply( m_TransformedBasis[1], matQuat, m_InitialBasis[1] );
		//Multiply( m_TransformedBasis[2], matQuat, m_InitialBasis[2] );
	}



	void TransformAttribute::UpdateBasis()
	{
		rm_cal_err_qt( m_Quaternion );

		Mat4f matQuat;
		Quat2Mat( matQuat, m_Quaternion );

		Multiply( m_TransformedBasis[0], matQuat, m_InitialBasis[0] );
		Multiply( m_TransformedBasis[1], matQuat, m_InitialBasis[1] );
		Multiply( m_TransformedBasis[2], matQuat, m_InitialBasis[2] );
	}




}// end of namespace