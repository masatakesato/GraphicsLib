#include "TerrainMaterial.h"


#include	<iostream>
using namespace std;


TerrainMaterial::TerrainMaterial()
{
	m_numMats		= 0;
	m_SurfaceProperties	= NULL;
}


TerrainMaterial::TerrainMaterial(int nummats)
{
tcout << _T("TerrainMaterial::TerrainMaterial()...") << tendl;

	m_numMats		= nummats;
	m_SurfaceProperties = new SurfaceProperty[m_numMats];
}



TerrainMaterial::~TerrainMaterial()
{
	if(m_SurfaceProperties)	delete [] m_SurfaceProperties;
}