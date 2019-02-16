#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include <time.h>

/* preprocessor defines */
#define WINDOW_TITLE_PREFIX "Chapter 4"
#define SHADER_SOURCE "e:/src/openglbook/chap04/"

/* global variables */
int g_width = 800;
int g_height = 600;
int g_hwnd = 0;
unsigned int g_cfps = 0;
float g_cube_rot = 0;
clock_t g_last_time = 0;

GLuint g_uloc_project;
GLuint g_uloc_view;
GLuint g_uloc_model;
GLuint g_id_buf[3] = {0};
GLuint g_id_shader[3] = {0};

ccmatrix g_a_project;
ccmatrix g_a_view;
ccmatrix g_a_model;

void createcube()
{
	const ccvertex vertices[8] =
	{
	    { { -.5f, -.5f,  .5f, 1 }, { 0, 0, 1, 1 } },
	    { { -.5f,  .5f,  .5f, 1 }, { 1, 0, 0, 1 } },
	    { {  .5f,  .5f,  .5f, 1 }, { 0, 1, 0, 1 } },
	    { {  .5f, -.5f,  .5f, 1 }, { 1, 1, 0, 1 } },
	    { { -.5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } },
	    { { -.5f,  .5f, -.5f, 1 }, { 1, 0, 0, 1 } },
	    { {  .5f,  .5f, -.5f, 1 }, { 1, 0, 1, 1 } },
	    { {  .5f, -.5f, -.5f, 1 }, { 0, 0, 1, 1 } }
	};
	const GLuint vidx[36] =
	{
	    0,2,1,  0,3,2,
	    4,3,0,  4,7,3,
	    4,1,5,  4,0,1,
	    3,6,2,  3,7,6,
	    1,6,5,  1,2,6,
	    7,5,6,  7,4,5
	};

	g_id_shader[0] = glCreateProgram();
	cc_gluassert("could not create shader program");
	g_id_shader[1] = cc_createshaderfromfile("e:\\src\\openglbook\\chap04\\",
			"SimpleShader.fragment.glsl",
			GL_FRAGMENT_SHADER);
	g_id_shader[2] = cc_createshaderfromfile("e:\\src\\openglbook\\chap04\\",
			"SimpleShader.vertex.glsl",
			GL_VERTEX_SHADER);
	glAttachShader(g_id_shader[0], g_id_shader[1]);
	glAttachShader(g_id_shader[0], g_id_shader[2]);
#if CC_GLSL_VERSION <= 0x0103
	glBindAttribLocation(g_id_shader[0], 0, "in_Position");
	glBindAttribLocation(g_id_shader[0], 1, "in_Color");
#endif
	glLinkProgram(g_id_shader[0]);
	cc_gluassert("could not link shader program");
	g_uloc_model = glGetUniformLocation(g_id_shader[0], "g_a_model");
	g_uloc_view = glGetUniformLocation(g_id_shader[0], "g_a_view");
	g_uloc_project = glGetUniformLocation(g_id_shader[0], "g_a_project");
	cc_gluassert("could not get the shader uniform locations");
	glGenVertexArrays(1, &g_id_buf[0]);
	cc_gluassert("could not generate VAO");
	glBindVertexArray(g_id_buf[0]);
	cc_gluassert("could not bind VAO");
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	cc_gluassert("could not enable vertex attributes");
	glGenBuffers(2, &g_id_buf[1]);
	cc_gluassert("could not create buffer objects");
	glBindBuffer(GL_ARRAY_BUFFER, g_id_buf[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	cc_gluassert("could not bind the VBO to the VAO");
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
			sizeof(vertices[0]), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
			sizeof(vertices[0]), (GLvoid*)sizeof(vertices[0].p));
	cc_gluassert("could not set VAO attributes");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_id_buf[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vidx), vidx, GL_STATIC_DRAW);
	cc_gluassert("could not bind IBO to the VAO");
	glBindVertexArray(0);
}

void destroycube()
{
	glDetachShader(g_id_shader[0], g_id_shader[1]);
	glDetachShader(g_id_shader[0], g_id_shader[2]);
	glDeleteShader(g_id_shader[1]);
	glDeleteShader(g_id_shader[2]);
	glDeleteProgram(g_id_shader[0]);
	cc_gluassert("could not destroy shaders");
	glDeleteBuffers(2, &g_id_buf[1]);
	glDeleteVertexArrays(1, &g_id_buf[0]);
	cc_gluassert("could not destroy buffer objects");
}

void drawcube()
{
	float angle;
	clock_t now = clock();
	if (g_last_time == 0)
		g_last_time = now;

	g_cube_rot += 45.0f * ((float)(now - g_last_time) / CLOCKS_PER_SEC);
	angle = cc_deg2rad(g_cube_rot);
	g_last_time = now;

	g_a_model = CC_IDENTITY_MATRIX;
	cc_mroty(&g_a_model, angle);
	cc_mrotx(&g_a_model, angle);

	glUseProgram(g_id_shader[0]);
	cc_gluassert("could not set shader program");

	glUniformMatrix4fv(g_uloc_model, 1, GL_FALSE, g_a_model.m);
	glUniformMatrix4fv(g_uloc_view, 1, GL_FALSE, g_a_view.m);
	cc_gluassert("could not set shader uniforms");

	glBindVertexArray(g_id_buf[0]);
	cc_gluassert("could not bind the VAO for drawing purposes");

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
	cc_gluassert("could not draw the cube");
	glBindVertexArray(0);
	glUseProgram(0);
}

/* update display  */
void displayfunc()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawcube();
	glutSwapBuffers();
	glutPostRedisplay();

	++g_cfps;
}

/* FPS counting */
void timerfunc(int tid)
{
	if (tid == 1){
		char buf[512];
		snprintf(buf, 512, "%s: %i FPS @ %i x %i",
				WINDOW_TITLE_PREFIX, g_cfps * 4, g_width, g_height);
		glutSetWindowTitle(buf);
	}
	g_cfps = 0;
	glutTimerFunc(250, timerfunc, 1);
}

/* continuously update the display */
void idlefunc()
{
	glutPostRedisplay();
}

/* adjust opengl viewport to window dimensions */
void reshapefunc(int w, int h)
{
	g_width = w;
	g_height = h;
	glViewport(0, 0, w, h);

	g_a_project =
			cc_projectionmatrix(60, (float)w / h, 1.0f, 100.0f);
	glUseProgram(g_id_shader[0]);
	glUniformMatrix4fv(g_uloc_project, 1, GL_FALSE, g_a_project.m);
	glUseProgram(0);
}

/* do garbage collection when GLUT exits */
void closefunc()
{
	destroycube();
}

/* main program */
int main(int argc, char* argv[])
{
	/* initialize glut library */
	glutInit(&argc, argv);
	/* create glut opengl context*/
#if CC_GL_VERSION >= 0x0320
	/* causes crashes on linux/x11 regardless any argument */
	glutInitContextVersion(3, 2); //!\todo choose highest available context version
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
#endif
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	/* initialize and show window*/
	glutInitWindowSize(g_width, g_height);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	g_hwnd = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if (g_hwnd < 1)
		cc_gluassert("failed to create glut window");

	/* setup event callbacks */
	glutIdleFunc(idlefunc);
	glutCloseFunc(closefunc);
	glutDisplayFunc(displayfunc);
	glutReshapeFunc(reshapefunc);
	glutTimerFunc(0, timerfunc, 0);
	/* initialize glew library */
	cc_glewassert(glewInit(), "could not init GLEW");
	/* additional OpenGL setup */
	glGetError();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	cc_gluassert("could not set OpenGL depth testing");
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	cc_gluassert("could not set OpenGL culling");
	g_a_view = g_a_model = g_a_project = CC_IDENTITY_MATRIX;
	cc_mtrans(&g_a_view, 0, 0, -2);
	createcube();
	printf("INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
	/* enter event loop */
	glutMainLoop();
	return 0;
}
