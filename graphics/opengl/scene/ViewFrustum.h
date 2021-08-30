#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H


#include	<oreore/MathLib.h>


class ViewFrustum
{
private:

	// This holds the A B C and D values for each side of our frustum.
	float m_Frustum[6][4];

public:

	ViewFrustum(){};// デフォルトコンストラクタ
	~ViewFrustum(){};	// デストラクタ

	// フラスタムを計算する
	//void CalculateFrustum();
	void CalculateFrustum(const Mat4f &matViewProj);

	
	// フラスタム内に頂点が存在するかどうか調べる．
	bool PointInFrustum(float x, float y, float z);

	// フラスタム内に球が存在するかどうか調べる．
	bool SphereInFrustum(float x, float y, float z, float radius);

	// フラスタム内に立方体が存在するかどうか調べる．
	bool CubeInFrustum( float x, float y, float z, float size );

	void Info();

};


#endif // VIEWFRUSTUM_H //