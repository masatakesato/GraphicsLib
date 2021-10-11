#ifndef CAMERA_H
#define CAMERA_H

#include	<oreore/mathlib/GraphicsMath.h>



class Camera
{
private:

	Vec3f	m_Position;// 位置
	Vec3f	m_Forward;	// 正面方向ベクトル
	Vec3f	m_Vertical;	// 垂直方向ベクトル
	Vec3f	m_Horizontal;// 水平方向ベクトル

	Mat4f	m_matProj;// 射影変換行列
	Mat4f	m_matView;// ビュー変換行列
	Mat4f	m_ViewProjection;	// ViewProjectionMatrix

	float	m_AspectRatio;	// アスペクト比
	float	m_Fovy;			// 視野角(縦軸方向)
	float	m_Near;			// クリッピング距離（手前）
	float	m_Far;			// クリッピング距離（奥）


public:
	
	Camera();
	Camera(	float pos_x, float pos_y, float pos_z,
			float dir_x, float dir_y, float dir_z,
			float up_x, float up_y, float up_z);
	~Camera(){};

	
	void Rotate(float Angle, float x, float y, float z);	// 任意軸周りにカメラを回転する
	void Rotate(float dh, float dv);						// 視点移動でカメラを回転する

//	void Yaw(float Angle);	// 未実装.必要なら作る
//	void Pitch(float Angle);// 未実装.必要なら作る
	void Roll(float Angle);// 進行方向軸まわりの回転
	void Transrate(float df, float dh, float dv);		// カメラを移動する


	inline const Vec3f*	GetPosition() const		{ return (Vec3f *)&m_Position; };// カメラの位置を取得する
	inline const Vec3f*	GetForward() const		{ return (Vec3f *)&m_Forward; };// カメラの位置を取得する
	inline const Vec3f*	GetVertical() const		{ return (Vec3f *)&m_Vertical; };// カメラの位置を取得する
	inline const Vec3f*	GetHorizontal() const	{ return (Vec3f *)&m_Horizontal; };// カメラの位置を取得する

	inline float GetNearClip() const	{ return m_Near; };
	inline float GetFarClip() const		{ return m_Far; };
	inline float GetFovy() const		{ return m_Fovy; };
	inline float GetAspect() const		{ return m_AspectRatio; };

	inline const Mat4f*	GetViewTransformationMatrix() const	{ return (Mat4f *)&m_matView; };// ビュー変換行列を取得する
	inline const Mat4f*	GetProjectionMatrix() const			{ return (Mat4f *)&m_matProj; };// 透視変換行列を取得する
	inline const Mat4f*	GetViewProjectionMatrix() const		{ return (Mat4f *)&m_ViewProjection; };

	inline void	Update(){	Multiply(m_ViewProjection, m_matProj, m_matView); };// 行列を更新する

	// 姿勢パラメータを個々に設定する
	void SetPosition(const Vec3f &pos)			{ m_Position			= pos; };
	void SetDirection(const Vec3f &dir)			{ m_Forward		= dir; };
	void SetVerticalVector(const Vec3f &up)		{ m_Vertical	= up; };
	void SetHorizontalVector(const Vec3f &hol)	{ m_Horizontal	= hol; };
	
	// 射影変換パラメータを個々に設定する
	void SetAspect(float aspect_ratio)	{ m_AspectRatio = aspect_ratio; };
	void SetNearClip(float znear)		{ m_Near		= znear; };
	void SetFarClip(float zfar)			{ m_Far			= zfar; };
	void SetFovy(float fovy)			{ m_Fovy		= fovy; };


	// カメラ姿勢パラメータをまとめて設定する
	inline void SetViewParameter(const Vec3f &pos, const Vec3f &dir, const Vec3f &up)
	{
		//============== 初期位置の設定 ===============//
		m_Position = pos;
	
		//============== 初期姿勢の設定 ===============//
		m_Forward = dir;// 方向
		Normalize(m_Forward);
	
		m_Vertical = up;// 水平軸計算用に仮に値を入れる
		Normalize(m_Vertical);
		CrossProduct(m_Horizontal, m_Vertical, m_Forward);//　水平
		Normalize(m_Horizontal);
		
		CrossProduct(m_Vertical, m_Forward, m_Horizontal);// 垂直

		//============ ビュー変換行列を作成する ==========//
		MatViewGL( m_matView, m_Horizontal, m_Vertical, m_Forward, m_Position );//MatView( m_matView, m_Horizontal, m_Vertical, m_Forward, m_Position );
	};


	// 射影変換のパラメータをまとめて設定する
	inline void SetProjectionParameter(float aspect_ratio, float fovy, float znear, float zfar)// アスペクト比と視野角を設定する
	{
		//============ パラメータを設定する ==========//
		m_AspectRatio	= aspect_ratio;
		m_Fovy			= fovy;
		m_Near			= znear;
		m_Far			= zfar;

		//=========== 射影変換行列を作成する =========//
		MatPerspectiveFov( m_matProj, m_Fovy, m_AspectRatio, m_Near, m_Far );
	};

	// 正射影変換のパラメータをまとめて設定する
	inline void SetProjectionParameter(float left, float right, float bottom, float top, float znear, float zfar)
	{
		//============ パラメータを設定する ==========//
		m_AspectRatio	= (right - left) / (top - bottom);
		m_Near			= znear;
		m_Far			= zfar;

		//=========== 射影変換行列を作成する =========//
		MatOrtho(m_matProj, left, right, bottom, top, m_Near, m_Far);
	};

	
	void Information();
};



#endif /* CAMERA_H */