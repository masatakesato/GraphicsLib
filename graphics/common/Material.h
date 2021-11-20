#ifndef MATERIAL_H
#define MATERIAL_H

#include	<oreore/mathlib/GraphicsMath.h>



// x,y,z,wのキー優先順序でVec4fの大小を比較する
// 小さいときは-1, 大きいときは+1, 同一値のときは0
inline short compareVec4f(const Vec4f& a, const Vec4f& b)
{
	if(a.x<b.x)			return -1;
	else if(a.x>b.x)	return +1;

	if(a.y<b.y)			return -1;
	else if(a.y>b.y)	return +1;

	if(a.z<b.z)			return -1;
	else if(a.z>b.z)	return +1;

	if(a.w<b.w)			return -1;
	else if(a.w>b.w)	return +1;
	
	return 0;
}







class Material
{
private:

	tstring	m_Name;	// マテリアル名称
	
	Vec4f	m_Ambient;	// 環境光
	Vec4f	m_Diffuse;	// 拡散反射
	Vec4f	m_Specular;	// 鏡面反射
	float	m_SpecularPower;	// スペキュラーの大きさ
	float	m_SpecularIntensity;// スペキュラーの強さ

	Vec3f	m_Luminance;	// 放射輝度
	//Vec4f	Kt;//Transparency
	//double NU, NV;


public:

	Material();
	~Material(){};
	
	// マテリアル名称をセットする
	void SetName( const tstring& matname );

	// アンビエントカラーをセットする
	void SetAmbient( float r, float g, float b, float a );
	void SetAmbient( const Vec4f &ambient);

	// 拡散反射率をセットする
	void SetDiffuse( float r, float g, float b, float a );
	void SetDiffuse( const Vec4f &diffuse );

	// 鏡面反射率をセットする
	void SetSpecular( float r, float g, float b, float a );
	void SetSpecular( const Vec4f &specular );

	void SetSpecularIntensity( float intensity );
	void SetSpecularPower( float power );

	// アルファ値をセットする
	void SetAlpha( float alpha );

	// 放射輝度をセットする
	void SetLuminance( float r, float g, float b );
	void SetLuminance( const Vec3f &luminance );


	inline const tstring& GetName() const			{ return m_Name; };
	inline const Vec4f* const GetAmbient() const	{ return (const Vec4f *)&m_Ambient; };
	inline const Vec4f* const GetDiffuse() const	{ return (const Vec4f *)&m_Diffuse; };
	inline const Vec4f* const GetSpecular() const	{ return (const Vec4f *)&m_Specular; };
	inline const float GetSpecularPower() const		{ return m_SpecularPower; };
	inline const float GetSpecularIntensity() const	{ return m_SpecularIntensity; };
	inline const Vec3f* const GetLuminance() const	{ return (const Vec3f *)&m_Luminance; };

	void Info();

};




// マテリアルをソート
struct stl_sort_functor_cmp_mat
{
	inline bool operator()( const Material& Left, const Material& Right )
	{
		int i;

		// シェーダーの種類


		// テクスチャIDでソート


		// Diffuse
		i = compareVec4f( *Left.GetDiffuse(), *Right.GetDiffuse() );
		if(i==-1)		return true;
		else if(i==+1)	return false;

		// Specular
		i = compareVec4f( *Left.GetSpecular(), *Right.GetSpecular() );
		if(i==-1)		return true;
		else if(i==1)	return false;
		
		return true;
	}
};






#endif // MATERIAL_H //