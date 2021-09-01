#include	<GL/glew.h>
#include	<GL/glut.h>
#pragma comment(lib, "glew32.lib")

#include	<oreore/mathlib/GraphicsMath.h>



namespace MyGlPrimitive
{

	static void DRAW_XYZ()
	{
		glBegin(GL_LINES);
		glColor3f(1,0,0);// x
		glVertex3f(0,0,0);
		glVertex3f(10,0,0);

		glColor3f(0,1,0);// y
		glVertex3f(0,0,0);
		glVertex3f(0,10,0);

		glColor3f(0,0,1);// z
		glVertex3f(0,0,0);
		glVertex3f(0,0,10);
		glEnd();
	}

	// 文字列を表示する
	static void DrawString(	float x, float y, const Vec4f& color, const char *str, void *font )
	{
		glColor4fv( color.rgba );
		glRasterPos2f(x, y);
		for(int i=0; str[i]!='\0'; i++)
			glutBitmapCharacter(font, (int)str[i]);//	OpenGL最新版では破棄されている。文字列描画のプログラムが別途必要
	}

	static void DrawString(	float x, float y, const Vec4f& color, const char *str,void *font, unsigned int fontbase )
	{
		glColor4fv( color.rgba );
		glRasterPos2f(x, y);

		glPushAttrib(GL_LIST_BIT);
		glListBase(fontbase -32);
		glCallLists( strlen(str), GL_UNSIGNED_BYTE, str );
		glPopAttrib();
	}

	// 点を描く
	static void DrawPoint(float x, float y, float pointsize, const Vec4f& color )
	{
		float	VertexArray[2] = { x+0.5f*pointsize, y+0.5f*pointsize };
		GLuint	index[1] = {0};

		glPointSize(pointsize);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		
		glColorPointer( 4, GL_FLOAT, 0, color.rgba );
		glVertexPointer(2, GL_FLOAT, 0, VertexArray);
		glDrawElements(GL_POINTS,1, GL_UNSIGNED_INT, index);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		glPointSize(1.0f);
	}


	// 直線を描く
	static void DrawLine()
	{
		float	TexCoordArray[] = { 0.0f, 0.0f, 1.0f ,0.0f };
		float	VertexArray[] = { -1.0f, 0.0f, 1.0f, 0.0f };
		GLuint	index[] = { 0,1 };

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);
		glVertexPointer(2, GL_FLOAT, 0, VertexArray);
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, index);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}


	// 正方形をを描く
	static void DrawQuad()
	{
		float	TexCoordArray[8] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
		float	VertexArray[8] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f };
		GLuint	index[6] = { 0,1,2,	3,0,2 };

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);
		glVertexPointer(2, GL_FLOAT, 0, VertexArray);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, index);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}


	// テクスチャマッピングした板を描く
	static void DrawQuad(float x, float y, float width, float height, const Vec4f& color )
	{
		float vertexColors[16] =
		{
			color.r, color.g, color.b, color.a,	// 頂点0
			color.r, color.g, color.b, color.a,// 頂点1
			color.r, color.g, color.b, color.a,// 頂点2
			color.r, color.g, color.b, color.a,// 頂点3
		};
		
		float	VertexArray[8] = 
		{
			x,			y,				// 頂点0
			x,			y+height,		// 頂点1
			x+width,	y+height,	// 頂点2
			x+width,	y,		// 頂点3
		};

		GLuint	index[6] = { 0,1,2,	3,0,2 };
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		
		glColorPointer(4, GL_FLOAT, 0, vertexColors);
		glVertexPointer(2, GL_FLOAT, 0, VertexArray);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, index);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}



	// 立方体を描画する
	static void DrawCube(float dim)
	{
		float	ColorArray[24][3] =
		{
			{0.3f,0.3f,1.0f},{0.3f,0.3f,1.0f},{0.3f,0.3f,1.0f},{0.3f,0.3f,1.0f},// 面1
			{1.0f,0.3f,0.3f},{1.0f,0.3f,0.3f},{1.0f,0.3f,0.3f},{1.0f,0.3f,0.3f},// 面2		
			{0.3f,1.0f,1.0f},{0.3f,1.0f,1.0f},{0.3f,1.0f,1.0f},{0.3f,1.0f,1.0f},// 面3		
			{1.0f,1.0f,0.3f},{1.0f,1.0f,0.3f},{1.0f,1.0f,0.3f},{1.0f,1.0f,0.3f},// 面4		
			{0.3f,1.0f,0.3f},{0.3f,1.0f,0.3f},{0.3f,1.0f,0.3f},{0.3f,1.0f,0.3f},// 面5
			{1.0f,0.3f,1.0f},{1.0f,0.3f,1.0f},{1.0f,0.3f,1.0f},{1.0f,0.3f,1.0f},// 面6
		};

		float	TexCoordArray[24][2] =
		{
			{0,0},{1,0},{1,1},{0,1},// 面1
			{0,0},{1,0},{1,1},{0,1},// 面2
			{0,0},{1,0},{1,1},{0,1},// 面3
			{0,0},{1,0},{1,1},{0,1},// 面4
			{0,0},{1,0},{1,1},{0,1},// 面5
			{0,0},{1,0},{1,1},{0,1},// 面6
		};

		float	VertexArray[24][3] =
		{
			// 面1
			{-dim,-dim,-dim},//v0
			{+dim,-dim,-dim},//v1
			{+dim,+dim,-dim},//v2
			{-dim,+dim,-dim},//v3
			// 面2
			{+dim,-dim,-dim},//v1
			{+dim,-dim,+dim},//v5
			{+dim,+dim,+dim},//v6
			{+dim,+dim,-dim},//v2
			// 面3
			{+dim,-dim,+dim},//v5
			{-dim,-dim,+dim},//v4
			{-dim,+dim,+dim},//v7
			{+dim,+dim,+dim},//v6
			// 面4
			{-dim,-dim,+dim},//v4
			{-dim,-dim,-dim},//v0
			{-dim,+dim,-dim},//v3
			{-dim,+dim,+dim},//v7

			// 面5
			{-dim,+dim,-dim},//v3
			{+dim,+dim,-dim},//v2
			{+dim,+dim,+dim},//v6
			{-dim,+dim,+dim},//v7
			// 面6
			{-dim,-dim,-dim},//v0
			{-dim,-dim,+dim},//v4
			{+dim,-dim,+dim},//v5
			{+dim,-dim,-dim},//v1
		};

		GLuint	index[36] =
		{
			0,1,2, 3,0,2,
			4,5,6, 7,4,6,
			8,9,10, 11,8,10,
			12,13,14, 15,12,14,
			16,17,18, 19,16,18,
			20,21,22, 23,20,22,
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);
		glVertexPointer(3, GL_FLOAT, 0, VertexArray);
		glColorPointer(3, GL_FLOAT, 0, ColorArray);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, index);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}


};