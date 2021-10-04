//=============================================================================
///	球面調和関数の描画
//=============================================================================

#include <stdio.h>
#include <GL/freeglut.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#ifndef M_PI
#define	M_PI		(3.1415926535f)
#endif

int clientWidth, clientHeight;
int bWire;
int mode;
float dt;
static int s_l, s_m;

/// 階乗
double factorial(int x)
{
	if (x <= 1)	return 1;
	return x * factorial(x-1);
}

/// evaluate an Associated Legendre Polynomial P(l,m,x) at x
double P(int l, int m, double x)
{
	double pmm = 1.0;
	if (m>0) {
		double somx2 = sqrt((1.0-x)*(1.0+x));
		double fact = 1.0;
		for (int i=1; i<=m; i++) {
			pmm *= (-fact) * somx2;
			fact += 2.0;
		}
	}
	if (l==m) return pmm;
	double pmmp1 = x * (2.0*m+1.0) * pmm;
	if (l==m+1) return pmmp1;
	double pll = 0.0;
	for (int ll=m+2; ll<=l; ++ll) {
		pll = ( (2.0*ll-1.0)*x*pmmp1-(ll+m-1.0)*pmm ) / (ll-m);
		pmm = pmmp1;
		pmmp1 = pll;
	}
	return pll;
}

/// renormalisation constant for SH function
double K(int l, int m)
{
	assert(l >= 0);
	assert(0 <= m && m <= l);

	double temp = ((2.0*l+1.0) * factorial(l-m)) / (4.0 * M_PI * factorial(l+m));
	return sqrt(temp);
}

/// return a point sample of a Spherical Harmonic basis function
/**
	@param	l		l is the band, range [0..N]
	@param	m		m in the range [-l..l]
	@param	theta	theta in the range [0..Pi]
	@param	phi		phi in the range [0..2*Pi]
*/
double SH(int l, int m, double theta, double phi)
{
	assert( l >= 0 );
	assert( -l <= m && m <= +l );

	const double sqrt2 = sqrt(2.0);
	if (m==0)		return K(l, 0) * P(l, m, cos(theta));
	else if (m>0)	return sqrt2 * K(l, m) * cos(m*phi) * P(l, m, cos(theta));
	else			return sqrt2 * K(l, -m) * sin(-m*phi) * P(l, -m, cos(theta));
}

void renderSH(int l, int m, int mode)
{
	const int DIV = 128;

	for (int iphi = 0; iphi < DIV; ++iphi) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int itheta=0; itheta <= DIV/2; ++itheta) {
			double theta = (double)itheta * (2 * M_PI / DIV);
			for (int i=0; i<2; ++i) {
				double phi = (double)(iphi+i) * (2 * M_PI / DIV);
				double Y = SH(l, m, theta, phi);

				double x = sin(theta) * cos(phi);
				double y = sin(theta) * sin(phi);
				double z = cos(theta);

				if (Y >= 0)		glColor4f(0, Y, 0, 1);
				else			glColor4f(-Y, 0, 0, 1);

				if (mode != 0) {
					double s = fabs(Y);
					x *= s;
					y *= s;
					z *= s;
				}

				glVertex3d(x, y, z);
			}
		}
		glEnd();
	}
}

/// ウィンドウサイズ変更
void resize_window(int w, int h) {
	clientWidth = w;
	clientHeight = h;
	glViewport(0, 0, w, h);
}

/// キー入力
void press_key(unsigned char key , int x , int y) {
	static const char* onofftbl[] = { "off", "on" };
	int l = s_l, m = s_m;

	switch (key) {
	default:	break;
	case 0x1b:	exit(0);
	case 'w':	bWire = !bWire;	printf("wireframe %s\n", onofftbl[bWire]); break;
	case 'l':	++l;	break;
	case 'L':	if (--l < 0)	l = 0;	break;
	case 'm':	if (++m > +l)	m = +l;	break;
	case 'M':	if (--m < -l)	m = -l;	break;
	case ' ':	if (++mode >= 2)	mode = 0;	break;
	}

	if (l != s_l || m != s_m) {
		if (l != s_l)	m = 0;
		printf("l=%d, m=%d\n", l, m);
		s_l = l;
		s_m = m;
	}
}

/// アイドル
void on_idle() {
	static int prev;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (prev != 0) {
		dt = (time - prev) * 0.001f;
	}
	prev = time;

	glutPostRedisplay();
}

/// 描画
void render(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 投影マトリクス
	{
		float fov = 40.0f;													///< 画角 [degree]
		float aspectRatio = (float)clientWidth / (float)clientHeight;		///< アスペクト比
		float nearPlane = 0.1f;
		float farPlane = 1000.0f;
	
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( fov, aspectRatio, nearPlane, farPlane );
	}

		// 視点マトリクス
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	1.0, 2.0, 3.0,				/* eye xyz*/
				0.0, 0.0, 0.0,				/* center xyz */
				0.0, 1.0, 0.);				/* up xyz */

		// 各種設定
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glPolygonMode(GL_FRONT_AND_BACK, (bWire ? GL_LINE : GL_FILL));

		// 物体の描画
	{
		glPushMatrix(); {
			static float ry, rx;
			ry += dt * (360 * (M_PI/180));
			rx += dt * (270 * (M_PI/180));

			glRotatef(ry, 0, 1, 0);
			glRotatef(rx, 1, 0, 0);

			renderSH(s_l, s_m, mode);

				// 軸描画
			glBegin(GL_LINES);
				glColor4f(1, 0, 0, 1);	glVertex3f(0, 0, 0);	glVertex3f(2, 0, 0);
				glColor4f(0, 1, 0, 1);	glVertex3f(0, 0, 0);	glVertex3f(0, 2, 0);
				glColor4f(0, 0, 1, 1);	glVertex3f(0, 0, 0);	glVertex3f(0, 0, 2);
			glEnd();

		} glPopMatrix();
	}

		// 可視化
	glFlush();
	glutSwapBuffers();
}

/// 初期化
void init() {
		// 使い方説明
	printf(
		"usage:\n"
		"	w    toggle wireframe\n"
		"	spc  change render mode\n"
		"	l,L  increase/decrease l\n"
		"	m,M  increase/decrease m\n"
		"\n"
	);
}

/// エントリ
int main(int argc , char* argv[]) {
	glutInit(&argc , argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutCreateWindow("GLUT App");
	glutDisplayFunc(render);
	glutKeyboardFunc(press_key);
	glutIdleFunc(on_idle);
	glutReshapeFunc(resize_window);

	init();

	glutMainLoop();
	return 0;
}

