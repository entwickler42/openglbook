#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

/* preprocessor defines */
#define WINDOW_TITLE_PREFIX "Chapter 3.2"
#define CC_GLSL_VERSION 0x0103
#define CC_GL_VERSION 0x0300

/* custom data types */
typedef struct
{
	float XYZW[4];
	float RGBA[4];
} Vertex;

/* global variables */
int g_width = 600;
int g_height = 600;
int g_hwnd = 0;
unsigned int g_cfps = 0;

GLuint g_id_vertex_shader;
GLuint g_id_fragment_shader;
GLuint g_id_program;
GLuint g_id_vao;
GLuint g_id_vbo;
GLuint g_id_idx[2];
GLuint g_cur_idx;

const GLchar* VertexShader =
{
    "#version 130\n"\

    "in vec4 in_Position;\n"\
    "in vec4 in_Color;\n"\
    "out vec4 ex_Color;\n"\

    "void main(void)\n"\
    "{\n"\
    "   gl_Position = in_Position;\n"\
    "   ex_Color = in_Color;\n"\
    "}\n"
};

const GLchar* FragmentShader =
{
    "#version 130\n"\

    "in vec4 ex_Color;\n"\
    "out vec4 out_Color;\n"\

    "void main(void)\n"\
    "{\n"\
    "   out_Color = ex_Color;\n"\
    "}\n"
};

void create_shaders()
{
	GLenum eval = glGetError();

	g_id_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(g_id_vertex_shader, 1, &VertexShader, NULL);
	glCompileShader(g_id_vertex_shader);

	g_id_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(g_id_fragment_shader, 1, &FragmentShader, NULL);
	glCompileShader(g_id_fragment_shader);

	g_id_program = glCreateProgram();
	glAttachShader(g_id_program, g_id_vertex_shader);
	glAttachShader(g_id_program, g_id_fragment_shader);
#if CC_GLSL_VERSION <= 0x0103
	glBindAttribLocation(g_id_program, 0, "in_Position");
	glBindAttribLocation(g_id_program, 1, "in_Color");
#endif
	glLinkProgram(g_id_program);
	glUseProgram(g_id_program);

    if ((eval = glGetError())!= GL_NO_ERROR)
    	error_at_line(-1, 0, __FILE__, __LINE__, "failed to create shaders: %s\n",
    			gluErrorString(eval));
}

void create_vbo()
{
	Vertex Vertices[] =
	{
	    { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
	    // Top
	    { { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	    { { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	    { { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
	    { { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	    // Bottom
	    { { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	    { { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	    { { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
	    { { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	    // Left
	    { { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	    { { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	    { { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
	    { { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	    // Right
	    { { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	    { { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	    { { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
	    { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
	};

	GLubyte Indices[] = {
	    // Top
	    0, 1, 3,
	    0, 3, 2,
	    3, 1, 4,
	    3, 4, 2,
	    // Bottom
	    0, 5, 7,
	    0, 7, 6,
	    7, 5, 8,
	    7, 8, 6,
	    // Left
	    0, 9, 11,
	    0, 11, 10,
	    11, 9, 12,
	    11, 12, 10,
	    // Right
	    0, 13, 15,
	    0, 15, 14,
	    15, 13, 16,
	    15, 16, 14
	};

	GLubyte AlternateIndices[] = {
	    // Outer square border:
	    3, 4, 16,
	    3, 15, 16,
	    15, 16, 8,
	    15, 7, 8,
	    7, 8, 12,
	    7, 11, 12,
	    11, 12, 4,
	    11, 3, 4,

	    // Inner square
	    0, 11, 3,
	    0, 3, 15,
	    0, 15, 7,
	    0, 7, 11
	};

    GLenum eval = glGetError();
    const size_t bsize = sizeof(Vertices);
    const size_t vsize = sizeof(Vertices[0]);
    const size_t offrgb = sizeof(Vertices[0].XYZW);

	glGenBuffers(1, &g_id_vbo);
    glGenVertexArrays(1, &g_id_vao);
    glBindVertexArray(g_id_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_id_vbo);
    glBufferData(GL_ARRAY_BUFFER, bsize, Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vsize, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vsize, (GLvoid*)offrgb);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenBuffers(2, g_id_idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_id_idx[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices),
    		Indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_id_idx[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(AlternateIndices),
    		AlternateIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_id_idx[0]);

    if ((eval = glGetError()) != GL_NO_ERROR)
    	error_at_line(-1, 0, __FILE__, __LINE__, "failed to create VBO: %s\n",
    			gluErrorString(eval));
}

void destroy_shaders()
{
	GLenum eval = glGetError();

	glUseProgram(0);
	glDetachShader(g_id_program, g_id_vertex_shader);
	glDetachShader(g_id_program, g_id_fragment_shader);

	glDeleteShader(g_id_fragment_shader);
	glDeleteShader(g_id_vertex_shader);

	glDeleteProgram(g_id_program);

    if ((eval = glGetError())!= GL_NO_ERROR)
    	error_at_line(-1, 0, __FILE__, __LINE__, "failed to destroy shaders: %s\n",
    			gluErrorString(eval));
}

void destroy_vbo()
{
	GLenum eval = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &g_id_vbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, g_id_idx);

    if ((eval = glGetError())!= GL_NO_ERROR)
    	error_at_line(-1, 0, __FILE__, __LINE__, "failed to destroy VBO: %s\n",
    			gluErrorString(eval));
}

/* update display  */
void render_callback()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	if( g_cur_idx == 0 ){
		glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*)0);
	}else{
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (GLvoid*)0);
	}
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

/* handle keyboard events */
void keyboard_callback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'T':
	case 't':
	{
		g_cur_idx = g_cur_idx == 1 ? 0 : 1;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_id_idx[g_cur_idx]);
		break;
	}

	default:
		break;
	}
}

/* do garbage collection when GLUT exits */
void close_callback()
{
	destroy_shaders();
	destroy_vbo();
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
		error_at_line(-1, 0, __FILE__, __LINE__-2, "failed to create glut window");
	/* setup event callbacks */
	glutDisplayFunc(render_callback);
	glutReshapeFunc(resize_callback);
	glutIdleFunc(idle_callback);
	glutCloseFunc(close_callback);
	glutKeyboardFunc(keyboard_callback);
	glutTimerFunc(0, timer_callback, 0);
	/* initialize glew library */
	GLenum rs = glewInit();
	if (rs != GLEW_OK)
		error_at_line(-1, 0, __FILE__, __LINE__-2, "%s", glewGetErrorString(rs));
	/* additional OpenGL setup */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	create_shaders();
	create_vbo();
	printf("INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
	/* enter event loop */
	glutMainLoop();
	return 0;
}
