#include <iostream>
#include <GL/freeglut.h>

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutCreateWindow("Check OpenGL");

	std::cout << "Vendor :"<<glGetString(GL_VENDOR)<<'\n';
	std::cout << "GPU : "<<glGetString(GL_RENDERER) <<'\n';
	std::cout << "OpenGL ver. " <<glGetString(GL_VERSION)<<'\n';

	return 0;
}