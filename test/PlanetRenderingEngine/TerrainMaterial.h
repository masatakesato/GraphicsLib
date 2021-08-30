#ifndef TERRAIN_MATERIAL_H
#define TERRAIN_MATERIAL_H


#include	<iostream>
#include	<oreore/mathlib/GraphicsMath.h>

#include	<graphics/gl4x/scene/Material.h>



class SurfaceProperty : public Material
{
private:

	Vec3f	m_Distribution;	// 分布高度(x:下限値, y：上限値, z:フェードアウトする距離[0,1])
	float		m_SlopeParam;	// 勾配パラメータ


public:
	SurfaceProperty(){ InitVec(m_Distribution, 0.0f, 0.0f, 0.0f); m_SlopeParam = 0.0f; };
	SurfaceProperty(const Vec3f &distribution, float param);
	~SurfaceProperty(){};

	void SetDistribution(const Vec3f &distribution){	m_Distribution = distribution; };
	void SetSlopeParam(float slopeparam){ m_SlopeParam = slopeparam; };

	Vec3f GetDistribution() const { return m_Distribution; };
	float	GetSlopeParameter() const { return m_SlopeParam; };
};






class TerrainMaterial
{
private:
	
	int m_numMats;// マテリアル数
	SurfaceProperty *m_SurfaceProperties;
	
public:
	
	TerrainMaterial();
	TerrainMaterial(int nummats);
	~TerrainMaterial();

	int numMaterials(){ return m_numMats; };

	void SetSurfaceProperties(int i, const Vec3f &amb, const Vec3f &diff, const Vec3f &spec, const float intensity, const float specpow, const Vec3f dist, const float slope)
	{
		m_SurfaceProperties[i].SetAmbient(amb.x, amb.y, amb.z, 0.0f);
		m_SurfaceProperties[i].SetDiffuse(diff.x, diff.y, diff.z, 0.0f);
		m_SurfaceProperties[i].SetSpecular(spec.x, spec.y, spec.z, 0.0f);
		m_SurfaceProperties[i].SetSpecularIntensity(intensity);
		m_SurfaceProperties[i].SetSpecularPower(specpow);
		m_SurfaceProperties[i].SetDistribution(dist);
		m_SurfaceProperties[i].SetSlopeParam(slope);
	}

	SurfaceProperty &GetSurfaceProperty(int i) const { return m_SurfaceProperties[i]; };

	void SetDistribution(int i, const Vec3f &distribution)	{ m_SurfaceProperties[i].SetDistribution(distribution); };
	void SetSlopeParam(int i, float slope)						{ m_SurfaceProperties[i].SetSlopeParam(slope); };
};




#endif /* TERRAIN_MATERIAL_H */