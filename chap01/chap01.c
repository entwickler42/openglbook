#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

/* preprocessor defines */
#define WINDOW_TITLE_PREFIX "Chapter 1"

/* global variables */
int g_width = 800;
int g_height = 600;
int g_hwnd = 0;
unsigned int g_cfps = 0;

/* update display  */
void render_callback()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glutPostRedisplay();

	++g_cfps;
}

/* FPS counting */
void timer_callback(int tid)
{
	if (tid == 1){
		char buf[512];
		snprintf(buf, 512, "%s: %i FPS @ %i x %i",
				WINDOW_TITLE_PREFIX, g_cfps * 4, g_width, g_height);
		glutSetWindowTitle(buf);
	}
	g_cfps = 0;
	glutTimerFunc(250, timer_callback, 1);
}

/* continuously update the display */
void idle_callback()
{
	glutPostRedisplay();
}

/* adjust opengl viewport to window dimensions */
void resize_callback(int w, int h)
{
	g_width = w;
	g_height = h;
	glViewport(0, 0, w, h);
}

/* main program */
int main(int argc, char* argv[])
{
	/* initialize glut library */
	glutInit(&argc, argv);
	/* create glut opengl context*/
#ifdef CC_GLUT_SWITCH_CONTEXT
	/* causes crashes on linux/x11 regardless any argument */
	glutInitContextVersion(3, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
#endif
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	/* initialize and show window*/
	glutInitWindowSize(g_width, g_height);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	g_hwnd = glutCreateWindow(WINDOW_TITLE_PREFIX);
	if (g_hwnd < 1)
		error_at_line(-1, 0, __FILE__, __LINE__-2, "failed to create glut window");
	/* setup event callbacks */
	glutDisplayFunc(render_callback);
	glutReshapeFunc(resize_callback);
	glutIdleFunc(idle_callback);
	glutTimerFunc(0, timer_callback, 0);
	/* initialize glew library */
	GLenum rs = glewInit();
	if (rs != GLEW_OK)
		error_at_line(-1, 0, __FILE__, __LINE__-2, "%s", glewGetErrorString(rs));
	/* additional OpenGL setup */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	printf("INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
	/* enter event loop */
	glutMainLoop();
	return 0;
}
