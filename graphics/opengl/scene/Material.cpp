#include	"Material.h"


#include	<string.h>
#include	<iostream>
using namespace std;

Material::Material()
{
	m_Name[0] = '\0';
	InitVec(m_Luminance,	0.0f, 0.0f, 0.0f);
	InitVec(m_Ambient,	0.0f, 0.0f, 0.0f, 0.0f);
	InitVec(m_Diffuse,	0.0f, 0.0f, 0.0f, 0.0f);
	InitVec(m_Specular,	0.0f, 0.0f, 0.0f, 0.0f);
	m_SpecularPower = 0.0f;
	m_SpecularIntensity = 0.0f;
}


void Material::SetName(const char *name)
{
	strcpy_s(m_Name, sizeof(m_Name), name);
}

void Material::SetAmbient(float r, float g, float b, float a)
{
	InitVec(m_Ambient, r,g,b,a);
}

void Material::SetAmbient(const Vec4f &ambient)
{
	m_Ambient = ambient;
}

void Material::SetDiffuse(float r, float g, float b, float a)
{
	InitVec(m_Diffuse, r,g,b,a);
}

void Material::SetDiffuse(const Vec4f &diffuse)
{
	m_Diffuse = diffuse;
}

void Material::SetSpecular(float r, float g, float b, float a)
{
	InitVec(m_Specular, r,g,b,a);
}

void Material::SetSpecular(const Vec4f &specular)
{
	m_Specular = specular;
}

void Material::SetSpecularIntensity(float intensity)
{
	m_SpecularIntensity	= intensity;
}

void Material::SetSpecularPower(float power)
{
	m_SpecularPower	= power;
}

void Material::SetAlpha(float alpha)
{
	m_Ambient.a = alpha;
	m_Diffuse.a = alpha;
	m_Specular.a = alpha;
}

void Material::SetLuminance(float r, float g, float b)
{
	InitVec(m_Luminance, r,g,b);
}

void Material::SetLuminance(const Vec3f &luminance)
{
	m_Luminance = luminance;
}



void Material::Info()
{
	cout << "//============= Material Info ================//" << endl;

	cout << "Diffuse: (" << m_Diffuse.x << ", "  << m_Diffuse.y << ", " <<  m_Diffuse.z << ", " <<  m_Diffuse.w << ")" << endl;
	cout << "Specular: (" << m_Specular.x << ", "  << m_Specular.y << ", " <<  m_Specular.z << ", " <<  m_Specular.w << ")" << endl;
	

}