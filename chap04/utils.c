#include "utils.h"
#include <math.h>
#include <time.h>
#include <errno.h>
#include <string.h>

const double g_deg2rad = M_PI / 180.0;
const double g_rad2deg = 180.0 / M_PI;

const ccmatrix CC_IDENTITY_MATRIX =
{
		{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
		}
};

float cc_cotangent(float a)
{
	return (float)(1.0/tan(a));
}

float cc_deg2rad(float deg)
{
	return deg * g_deg2rad;
}

float cc_rad2deg(float rad)
{
	return rad * g_rad2deg;
}

ccmatrix cc_mmul(const ccmatrix* m1, const ccmatrix* m2)
{
	ccmatrix m = CC_IDENTITY_MATRIX;
	unsigned int row, column, row_offset;

	for (row=0, row_offset=row*4; row<4; ++row, row_offset = row*4){
		for (column=0; column<4; ++column)
			m.m[row_offset+column] =
					(m1->m[row_offset+0] * m2->m[column+0])+
					(m1->m[row_offset+1] * m2->m[column+4]) +
					(m1->m[row_offset+2] * m2->m[column+8]) +
					(m1->m[row_offset+3] * m2->m[column+12]) ;
	}

	return m;
}

void cc_mrotx(ccmatrix* m, float a)
{
	ccmatrix r = CC_IDENTITY_MATRIX;
	float sine = (float)sin(a);
	float cosine = (float)cos(a);

	r.m[5] = cosine;
	r.m[6] = -sine;
	r.m[9] = sine;
	r.m[10] = cosine;

	ccmatrix mul = cc_mmul(m, &r);
	memcpy(m->m, mul.m, sizeof(m->m));
}

void cc_mroty(ccmatrix* m, float a)
{
	ccmatrix r = CC_IDENTITY_MATRIX;
	float sine = (float)sin(a);
	float cosine = (float)cos(a);

	r.m[0] = cosine;
	r.m[8] = -sine;
	r.m[2] = sine;
	r.m[10] = cosine;

	ccmatrix mul = cc_mmul(m, &r);
	memcpy(m->m, mul.m, sizeof(m->m));
}

void cc_mrotz(ccmatrix* m, float a)
{
	ccmatrix r = CC_IDENTITY_MATRIX;
	float sine = (float)sin(a);
	float cosine = (float)cos(a);

	r.m[0] = cosine;
	r.m[1] = -sine;
	r.m[4] = sine;
	r.m[5] = cosine;

	ccmatrix mul = cc_mmul(m, &r);
	memcpy(m->m, mul.m, sizeof(m->m));
}

void cc_mscale(ccmatrix* m, float x, float y, float z)
{
	ccmatrix s = CC_IDENTITY_MATRIX;

	s.m[0] = x;
	s.m[5] = y;
	s.m[10] = z;

	ccmatrix mul = cc_mmul(m, &s);
	memcpy(m->m, mul.m, sizeof(m->m));
}

void cc_mtrans(ccmatrix* m, float x, float y, float z)
{
	ccmatrix t = CC_IDENTITY_MATRIX;

	t.m[12] = x;
	t.m[13] = y;
	t.m[14] = z;

	ccmatrix mul = cc_mmul(m, &t);
	memcpy(m->m, mul.m, sizeof(m->m));
}

ccmatrix cc_projectionmatrix(
		float fovy,
		float aspect_ratio,
		float near_plane,
		float far_plane
		)
{
	ccmatrix m = { { 0 } };

	const float y_scale = cc_cotangent(cc_deg2rad(fovy/2));
	const float x_scale = y_scale / aspect_ratio;
	const float len_frustum = far_plane - near_plane;

	m.m[0] = x_scale;
	m.m[5] = y_scale;
	m.m[10] = -((1*far_plane+near_plane) / len_frustum);
	m.m[11] = -1;
	m.m[14] = -((2*near_plane-far_plane) / len_frustum);

	return m;
}

void cc_gluassert(const char* msg)
{
	const GLenum eval = glGetError();
	if (eval != GL_NO_ERROR){
		error_at_line(-1, 0, __FILE__, __LINE__,
				"%s: %s", msg, gluErrorString(eval));
	}
}

void cc_glewassert(GLenum status, const char* msg)
{
	if (status != GLEW_OK){
		error_at_line(-1, 0, __FILE__, __LINE__,
				"%s: %s", msg, glewGetErrorString(status));
	}
}

GLuint cc_createshaderfromfile(const char* rootdir,
		const char* filename,
		GLenum shader_type)
{
	GLuint id_shader;
	FILE* f;
	size_t fsize, rs;
	char* glsl_source;
	char* filepath;

	/* prepend root directory to filename */
	if (rootdir != 0){
		filepath = malloc(strlen(rootdir)+strlen(filename)+2);
		strcpy(filepath, rootdir);
		char ch = *(rootdir + strlen(rootdir) - 1);
		if (ch != '/' && ch != '\\') strcat(filepath, "/");
		strcat(filepath, filename);
	}else{
		filepath = malloc(strlen(filename)+1);
		strcpy(filepath, filename);
	}
	char* pch = strchr(filepath, '\\');
	while (pch){
		*pch = '/';
		pch = strchr(filepath, '\\');
	}
	f = fopen(filepath, "rb");
	free(filepath);
	if (NULL == f){
		error(0, errno, __func__);
		return -1;
	}
	if (fseek(f, 0, SEEK_END) != 0){
		error(0, errno, __func__);
		return -2;
	}
	if (-1 == (fsize = ftell(f))){
		error(0, errno, __func__);
		return -3;
	}
	if (NULL == (glsl_source = (char*)malloc(fsize+1))){
		error(0, errno, __func__);
		return -4;
	}

	rewind(f);
	rs = fread(glsl_source, sizeof(char), fsize, f);
	fclose(f);

	if (fsize == rs){
		glsl_source[fsize] = '\0';
		if (0 != (id_shader = glCreateShader(shader_type))){
			glShaderSource(id_shader, 1, (const GLchar**)&glsl_source, NULL);
			glCompileShader(id_shader);
			cc_gluassert("failed to compile shader");
		}else{
			error(0, errno, __func__);
			id_shader = -6;
		}
	}else{
		error(0, errno, __func__);
		id_shader = -5;
	}

	free(glsl_source);
	return id_shader;
}
