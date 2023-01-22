#ifndef IMPORTS_H
#define IMPORTS_H

#ifndef NULL
#define NULL (const void*)0
#define offsetof(t, d) __builtin_offsetof(t, d)
#define sqrtf(x) __builtin_sqrtf(x)

#define GL_DEPTH_TEST 0x0B71
#define GL_FLOAT 0x1406
#define GL_COLOR_BUFFER_BIT 0x4000
#define GL_DEPTH_BUFFER_BIT 0x0100
#define GL_TRIANGLES 0x0004
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_ARRAY_BUFFER 0x8892
#define GL_DYNAMIC_DRAW 0x88E8
#endif

#ifndef __cplusplus
typedef enum { false, true } bool;
#endif

extern float frand(void);
extern float fsin(float);
extern float fcos(float);
extern int puts(const char*);

extern int glCreateBuffer();
extern int glCreateShader(unsigned int);
extern int glCreateProgram(void);
extern int glGetUniformLocation(unsigned int, const char*);
extern int glGetShaderParameter(unsigned int, unsigned int);
extern void glViewport(int, int, int, int);
extern void glSetShaderSource(unsigned int, const char*);
extern void glCompileShader(unsigned int);
extern void glDeleteShader(unsigned int);
extern void glAttachShader(unsigned int, unsigned int);
extern void glLinkProgram(unsigned int);
extern void glValidateProgram(unsigned int);
extern void glEnable(unsigned int);
extern void glUseProgram(unsigned int);
extern void glBindBuffer(unsigned int, unsigned int);
extern void glEnableVertexAttribArray(unsigned int);
extern void glBufferData(unsigned int, int, const void*, unsigned int);
extern void
glVertexAttribPointer(unsigned int, int, unsigned int, bool, int, const void*);
extern void glClear(unsigned int);
extern void glBufferSubData(unsigned int, int, int, void*);
extern void glUniform2f(int, float, float);
extern void glUniform1f(int, float);
extern void glUniform1i(int, int);
extern void glDrawArrays(unsigned int, int, int);

#endif
