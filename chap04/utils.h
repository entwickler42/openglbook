#ifndef UTILES_H
#define UTILES_H

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#define CC_GLSL_VERSION 0x0103
#define CC_GL_VERSION 0x0300

#ifndef WIN32
#include <error.h>
#else
#define error(status, errnum, format, ...) \
	fprintf(stderr, format, ## __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	if (status != 0) { exit(status); }
#define error_at_line(status, errnum, filename, linenum, format, ...) \
		fprintf(stderr, " %s at line %i: " format "\n", filename, linenum, ## __VA_ARGS__); \
		if (status != 0) { exit(status); }
#endif

typedef struct _ccvertex
{
	float p[4];
	float c[4];
} ccvertex;

typedef struct _ccmatrix
{
	float m[16];
} ccmatrix;

extern const ccmatrix CC_IDENTITY_MATRIX;

float cc_cotangent(float a);
float cc_deg2rad(float deg);
float cc_rad2deg(float rad);

ccmatrix cc_mmul(const ccmatrix* m1, const ccmatrix* m2);
void cc_mrotx(ccmatrix* m, float a);
void cc_mroty(ccmatrix* m, float a);
void cc_mrotz(ccmatrix* m, float a);
void cc_mscale(ccmatrix* m, float x, float y, float z);
void cc_mtrans(ccmatrix* m, float x, float y, float z);

ccmatrix cc_projectionmatrix(
		float fovy,
		float aspect_ratio,
		float near_plane,
		float far_plane
		);

void cc_gluassert(const char* msg);
void cc_glewassert(GLenum status, const char* msg);

GLuint cc_createshaderfromfile(const char* rootdir,
		const char* filename,
		GLenum shader_type);

#endif
