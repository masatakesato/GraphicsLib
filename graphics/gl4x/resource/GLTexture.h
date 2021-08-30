#include	<iostream>
using namespace std;
#include	<GL/glew.h>
//#pragma comment(lib, "glew32.lib")


// OpenGLのテクスチャオブジェクトを生成する関数

namespace MyGlTexture
{

#ifndef GL_TEXTURE_INIT_H
#define GL_TEXTURE_INIT_H


	// 1次元テクスチャを生成し，tex_idにテクスチャIDを返す
	static GLuint AllocateTexture1D(GLint mag_filter, GLint min_filter,
									GLsizei width,
									GLenum iformat, GLenum format, GLenum type, const GLvoid* data = NULL)
	{	
		GLenum errCode;
		GLuint tex_id = NULL;

		glGenTextures(1, &tex_id);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << "Error at AllocateTexture1D().glGenTextures(): "<< gluErrorString(errCode) << endl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		glBindTexture(GL_TEXTURE_1D, tex_id);
		{
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexImage1D(GL_TEXTURE_1D, 0, iformat, width, 0, format, type, data);
		}
		glBindTexture(GL_TEXTURE_1D, 0);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << "Error at ReallocateTexture1D().glTexImage1D(): " << gluErrorString(errCode) << endl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		return tex_id;
	}

	
	// 2次元テクスチャの生成
	static GLuint AllocateTexture2D(GLint mag_filter, GLint min_filter,
									GLint wrap_s, GLint wrap_t,
									GLsizei width, GLsizei height,
									GLenum iformat, GLenum format, GLenum type, const GLvoid* data = NULL)
	{	
		GLenum errCode;
		GLuint tex_id = NULL;

		glGenTextures(1, &tex_id);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << "Error at AllocateTexture2D().glGenTextures(): " << gluErrorString(errCode) << endl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		glBindTexture(GL_TEXTURE_2D, tex_id);
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
			glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, type, data);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << "Error at ReallocateTexture2D().glTexImage2D(): " << gluErrorString(errCode) << endl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		return tex_id;
	}


	// 既存の2次元テクスチャオブジェクトを再利用する
	static void ReallocateTexture2D(GLuint &tex_id, 
									GLint mag_filter, GLint min_filter,
									GLint wrap_s, GLint wrap_t,
									GLsizei width, GLsizei height,
									GLenum iformat, GLenum format, GLenum type, const GLvoid* data = NULL)
	{
		GLenum errCode;

		if(tex_id)	glDeleteTextures(1,	&tex_id);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << gluErrorString(errCode) << endl;
			tex_id = NULL;
			return;
		}
		
		glBindTexture(GL_TEXTURE_2D, tex_id);
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
			glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, type, data);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		
		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << "Error at ReallocateTexture2D().glTexImage2D(): " << gluErrorString(errCode) << endl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return;
		}
	}




	// 3次元テクスチャを生成する
	static GLuint AllocateTexture3D(GLint mag_filter, GLint min_filter,
									GLint wrap_s, GLint wrap_t, GLint wrap_r,
									GLsizei width, GLsizei height, GLsizei depth,
									GLenum iformat, GLenum format, GLenum type, const GLvoid* data = NULL)
	{
		GLenum errCode;
		GLuint tex_id;
	
		glGenTextures(1, &tex_id);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << "Error at AllocateTexture3D().glGenTextures(): " << gluErrorString(errCode) << endl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		glBindTexture(GL_TEXTURE_3D, tex_id);
		{
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap_s);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap_t);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap_r);
			glTexImage3D(GL_TEXTURE_3D, 0, iformat, width, height, depth, 0, format, type, data);
		}
		glBindTexture(GL_TEXTURE_3D, 0);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << "Error at AllocateTexture3D().glTexImage3D(): " << gluErrorString(errCode) << endl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		return tex_id;
	}


	// テクスチャを削除する
	static void ReleaseTexture( GLuint &texid )
	{
		if( texid != 0)
		glDeleteTextures( 1, &texid );
	}


	// 2Dテクスチャを描画する処理(GL2.0まで対応)
	static void DrawTexture2D(int width, int height, unsigned int texture)
	{
		glViewport(0, 0, width, height);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);// テクスチャユニット0を指定
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(-1, -1);
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(+1, -1);
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(+1, +1);
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(-1, +1);
		glEnd();

		glDisable(GL_TEXTURE_2D);

	}


	// 3Dテクスチャを描画する処理(GL2.0まで対応)
	static void DrawTexture3D(int width, int height, int depth, unsigned int texture, int layer=0)
	{
		float z = float(layer) / float(depth);
	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);// テクスチャユニット0を指定
		glEnable(GL_TEXTURE_3D);

		glBindTexture(GL_TEXTURE_3D, texture);
		glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord3f(0.0f, 0.0f, z); /*glColor3f(0,0,0);*/	glVertex2f(-1, -1);
		glTexCoord3f(1.0f, 0.0f, z); /*glColor3f(1,0,0);*/	glVertex2f(+1, -1);
		glTexCoord3f(1.0f, 1.0f, z); /*glColor3f(1,1,0);*/	glVertex2f(+1, +1);
		glTexCoord3f(0.0f, 1.0f, z); /*glColor3f(0,1,0);*/	glVertex2f(-1, +1);
		glEnd();

		glDisable(GL_TEXTURE_3D);
	}


#endif // GL_TEXTURE_INIT_H //

};