#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "gl_core_4_5.h"

#if defined(__APPLE__)
#include <dlfcn.h>

static void* AppleGLGetProcAddress (const char *name)
{
	static void* image = NULL;
	
	if (NULL == image)
		image = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);

	return (image ? dlsym(image, name) : NULL);
}
#endif /* __APPLE__ */

#if defined(__sgi) || defined (__sun)
#include <dlfcn.h>
#include <stdio.h>

static void* SunGetProcAddress (const GLubyte* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}
#endif /* __sgi || __sun */

#if defined(_WIN32)

#ifdef _MSC_VER
#pragma warning(disable: 4055)
#pragma warning(disable: 4054)
#pragma warning(disable: 4996)
#endif

static int TestPointer(const PROC pTest)
{
	ptrdiff_t iTest;
	if(!pTest) return 0;
	iTest = (ptrdiff_t)pTest;
	
	if(iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1) return 0;
	
	return 1;
}

static PROC WinGetProcAddress(const char *name)
{
	HMODULE glMod = NULL;
	PROC pFunc = wglGetProcAddress((LPCSTR)name);
	if(TestPointer(pFunc))
	{
		return pFunc;
	}
	glMod = GetModuleHandleA("OpenGL32.dll");
	return (PROC)GetProcAddress(glMod, (LPCSTR)name);
}
	
#define IntGetProcAddress(name) WinGetProcAddress(name)
#else
	#if defined(__APPLE__)
		#define IntGetProcAddress(name) AppleGLGetProcAddress(name)
	#else
		#if defined(__sgi) || defined(__sun)
			#define IntGetProcAddress(name) SunGetProcAddress(name)
		#else /* GLX */
		    #include <GL/glx.h>

			#define IntGetProcAddress(name) (*glXGetProcAddressARB)((const GLubyte*)name)
		#endif
	#endif
#endif

int ogl_ext_EXT_texture_compression_s3tc = ogl_LOAD_FAILED;
int ogl_ext_EXT_texture_sRGB = ogl_LOAD_FAILED;
int ogl_ext_EXT_texture_filter_anisotropic = ogl_LOAD_FAILED;
int ogl_ext_ARB_compressed_texture_pixel_storage = ogl_LOAD_FAILED;
int ogl_ext_ARB_conservative_depth = ogl_LOAD_FAILED;
int ogl_ext_ARB_ES2_compatibility = ogl_LOAD_FAILED;
int ogl_ext_ARB_get_program_binary = ogl_LOAD_FAILED;
int ogl_ext_ARB_explicit_uniform_location = ogl_LOAD_FAILED;
int ogl_ext_ARB_internalformat_query = ogl_LOAD_FAILED;
int ogl_ext_ARB_internalformat_query2 = ogl_LOAD_FAILED;
int ogl_ext_ARB_map_buffer_alignment = ogl_LOAD_FAILED;
int ogl_ext_ARB_program_interface_query = ogl_LOAD_FAILED;
int ogl_ext_ARB_separate_shader_objects = ogl_LOAD_FAILED;
int ogl_ext_ARB_shading_language_420pack = ogl_LOAD_FAILED;
int ogl_ext_ARB_shading_language_packing = ogl_LOAD_FAILED;
int ogl_ext_ARB_texture_buffer_range = ogl_LOAD_FAILED;
int ogl_ext_ARB_texture_storage = ogl_LOAD_FAILED;
int ogl_ext_ARB_texture_view = ogl_LOAD_FAILED;
int ogl_ext_ARB_vertex_attrib_binding = ogl_LOAD_FAILED;
int ogl_ext_ARB_viewport_array = ogl_LOAD_FAILED;
int ogl_ext_ARB_arrays_of_arrays = ogl_LOAD_FAILED;
int ogl_ext_ARB_clear_buffer_object = ogl_LOAD_FAILED;
int ogl_ext_ARB_copy_image = ogl_LOAD_FAILED;
int ogl_ext_ARB_ES3_compatibility = ogl_LOAD_FAILED;
int ogl_ext_ARB_fragment_layer_viewport = ogl_LOAD_FAILED;
int ogl_ext_ARB_framebuffer_no_attachments = ogl_LOAD_FAILED;
int ogl_ext_ARB_invalidate_subdata = ogl_LOAD_FAILED;
int ogl_ext_ARB_robust_buffer_access_behavior = ogl_LOAD_FAILED;
int ogl_ext_ARB_stencil_texturing = ogl_LOAD_FAILED;
int ogl_ext_ARB_texture_query_levels = ogl_LOAD_FAILED;
int ogl_ext_ARB_texture_storage_multisample = ogl_LOAD_FAILED;
int ogl_ext_KHR_debug = ogl_LOAD_FAILED;
int ogl_ext_ARB_buffer_storage = ogl_LOAD_FAILED;
int ogl_ext_ARB_clear_texture = ogl_LOAD_FAILED;
int ogl_ext_ARB_enhanced_layouts = ogl_LOAD_FAILED;
int ogl_ext_ARB_multi_bind = ogl_LOAD_FAILED;
int ogl_ext_ARB_query_buffer_object = ogl_LOAD_FAILED;
int ogl_ext_ARB_texture_mirror_clamp_to_edge = ogl_LOAD_FAILED;
int ogl_ext_ARB_texture_stencil8 = ogl_LOAD_FAILED;
int ogl_ext_ARB_vertex_type_10f_11f_11f_rev = ogl_LOAD_FAILED;
int ogl_ext_ARB_seamless_cubemap_per_texture = ogl_LOAD_FAILED;
int ogl_ext_ARB_clip_control = ogl_LOAD_FAILED;
int ogl_ext_ARB_conditional_render_inverted = ogl_LOAD_FAILED;
int ogl_ext_ARB_cull_distance = ogl_LOAD_FAILED;
int ogl_ext_ARB_derivative_control = ogl_LOAD_FAILED;
int ogl_ext_ARB_direct_state_access = ogl_LOAD_FAILED;
int ogl_ext_ARB_get_texture_sub_image = ogl_LOAD_FAILED;
int ogl_ext_ARB_shader_texture_image_samples = ogl_LOAD_FAILED;
int ogl_ext_ARB_texture_barrier = ogl_LOAD_FAILED;
int ogl_ext_KHR_context_flush_control = ogl_LOAD_FAILED;
int ogl_ext_KHR_robust_buffer_access_behavior = ogl_LOAD_FAILED;
int ogl_ext_KHR_robustness = ogl_LOAD_FAILED;

void (CODEGEN_FUNCPTR *_ptrc_glClearDepthf)(GLfloat d) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthRangef)(GLfloat n, GLfloat f) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint * range, GLint * precision) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReleaseShaderCompiler)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glShaderBinary)(GLsizei count, const GLuint * shaders, GLenum binaryformat, const void * binary, GLsizei length) = NULL;

static int Load_ARB_ES2_compatibility(void)
{
	int numFailed = 0;
	_ptrc_glClearDepthf = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glClearDepthf");
	if(!_ptrc_glClearDepthf) numFailed++;
	_ptrc_glDepthRangef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glDepthRangef");
	if(!_ptrc_glDepthRangef) numFailed++;
	_ptrc_glGetShaderPrecisionFormat = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *, GLint *))IntGetProcAddress("glGetShaderPrecisionFormat");
	if(!_ptrc_glGetShaderPrecisionFormat) numFailed++;
	_ptrc_glReleaseShaderCompiler = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glReleaseShaderCompiler");
	if(!_ptrc_glReleaseShaderCompiler) numFailed++;
	_ptrc_glShaderBinary = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *, GLenum, const void *, GLsizei))IntGetProcAddress("glShaderBinary");
	if(!_ptrc_glShaderBinary) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glGetProgramBinary)(GLuint program, GLsizei bufSize, GLsizei * length, GLenum * binaryFormat, void * binary) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramBinary)(GLuint program, GLenum binaryFormat, const void * binary, GLsizei length) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramParameteri)(GLuint program, GLenum pname, GLint value) = NULL;

static int Load_ARB_get_program_binary(void)
{
	int numFailed = 0;
	_ptrc_glGetProgramBinary = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLenum *, void *))IntGetProcAddress("glGetProgramBinary");
	if(!_ptrc_glGetProgramBinary) numFailed++;
	_ptrc_glProgramBinary = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const void *, GLsizei))IntGetProcAddress("glProgramBinary");
	if(!_ptrc_glProgramBinary) numFailed++;
	_ptrc_glProgramParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glProgramParameteri");
	if(!_ptrc_glProgramParameteri) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glGetInternalformativ)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint * params) = NULL;

static int Load_ARB_internalformat_query(void)
{
	int numFailed = 0;
	_ptrc_glGetInternalformativ = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, GLint *))IntGetProcAddress("glGetInternalformativ");
	if(!_ptrc_glGetInternalformativ) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glGetInternalformati64v)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64 * params) = NULL;

static int Load_ARB_internalformat_query2(void)
{
	int numFailed = 0;
	_ptrc_glGetInternalformati64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, GLint64 *))IntGetProcAddress("glGetInternalformati64v");
	if(!_ptrc_glGetInternalformati64v) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glGetProgramInterfaceiv)(GLuint program, GLenum programInterface, GLenum pname, GLint * params) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceIndex)(GLuint program, GLenum programInterface, const GLchar * name) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceLocation)(GLuint program, GLenum programInterface, const GLchar * name) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceLocationIndex)(GLuint program, GLenum programInterface, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceName)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei * length, GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceiv)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum * props, GLsizei bufSize, GLsizei * length, GLint * params) = NULL;

static int Load_ARB_program_interface_query(void)
{
	int numFailed = 0;
	_ptrc_glGetProgramInterfaceiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLint *))IntGetProcAddress("glGetProgramInterfaceiv");
	if(!_ptrc_glGetProgramInterfaceiv) numFailed++;
	_ptrc_glGetProgramResourceIndex = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceIndex");
	if(!_ptrc_glGetProgramResourceIndex) numFailed++;
	_ptrc_glGetProgramResourceLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceLocation");
	if(!_ptrc_glGetProgramResourceLocation) numFailed++;
	_ptrc_glGetProgramResourceLocationIndex = (GLint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceLocationIndex");
	if(!_ptrc_glGetProgramResourceLocationIndex) numFailed++;
	_ptrc_glGetProgramResourceName = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramResourceName");
	if(!_ptrc_glGetProgramResourceName) numFailed++;
	_ptrc_glGetProgramResourceiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *))IntGetProcAddress("glGetProgramResourceiv");
	if(!_ptrc_glGetProgramResourceiv) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glActiveShaderProgram)(GLuint pipeline, GLuint program) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindProgramPipeline)(GLuint pipeline) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateShaderProgramv)(GLenum type, GLsizei count, const GLchar *const* strings) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteProgramPipelines)(GLsizei n, const GLuint * pipelines) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenProgramPipelines)(GLsizei n, GLuint * pipelines) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramPipelineInfoLog)(GLuint pipeline, GLsizei bufSize, GLsizei * length, GLchar * infoLog) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramPipelineiv)(GLuint pipeline, GLenum pname, GLint * params) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsProgramPipeline)(GLuint pipeline) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1d)(GLuint program, GLint location, GLdouble v0) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1dv)(GLuint program, GLint location, GLsizei count, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1f)(GLuint program, GLint location, GLfloat v0) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1fv)(GLuint program, GLint location, GLsizei count, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1i)(GLuint program, GLint location, GLint v0) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1iv)(GLuint program, GLint location, GLsizei count, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1ui)(GLuint program, GLint location, GLuint v0) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1uiv)(GLuint program, GLint location, GLsizei count, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2d)(GLuint program, GLint location, GLdouble v0, GLdouble v1) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2dv)(GLuint program, GLint location, GLsizei count, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2f)(GLuint program, GLint location, GLfloat v0, GLfloat v1) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2fv)(GLuint program, GLint location, GLsizei count, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2i)(GLuint program, GLint location, GLint v0, GLint v1) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2iv)(GLuint program, GLint location, GLsizei count, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2ui)(GLuint program, GLint location, GLuint v0, GLuint v1) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2uiv)(GLuint program, GLint location, GLsizei count, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3dv)(GLuint program, GLint location, GLsizei count, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3fv)(GLuint program, GLint location, GLsizei count, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3iv)(GLuint program, GLint location, GLsizei count, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3uiv)(GLuint program, GLint location, GLsizei count, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4dv)(GLuint program, GLint location, GLsizei count, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4fv)(GLuint program, GLint location, GLsizei count, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4iv)(GLuint program, GLint location, GLsizei count, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4uiv)(GLuint program, GLint location, GLsizei count, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUseProgramStages)(GLuint pipeline, GLbitfield stages, GLuint program) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glValidateProgramPipeline)(GLuint pipeline) = NULL;

static int Load_ARB_separate_shader_objects(void)
{
	int numFailed = 0;
	_ptrc_glActiveShaderProgram = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glActiveShaderProgram");
	if(!_ptrc_glActiveShaderProgram) numFailed++;
	_ptrc_glBindProgramPipeline = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glBindProgramPipeline");
	if(!_ptrc_glBindProgramPipeline) numFailed++;
	_ptrc_glCreateShaderProgramv = (GLuint (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLchar *const*))IntGetProcAddress("glCreateShaderProgramv");
	if(!_ptrc_glCreateShaderProgramv) numFailed++;
	_ptrc_glDeleteProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteProgramPipelines");
	if(!_ptrc_glDeleteProgramPipelines) numFailed++;
	_ptrc_glGenProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenProgramPipelines");
	if(!_ptrc_glGenProgramPipelines) numFailed++;
	_ptrc_glGetProgramPipelineInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramPipelineInfoLog");
	if(!_ptrc_glGetProgramPipelineInfoLog) numFailed++;
	_ptrc_glGetProgramPipelineiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetProgramPipelineiv");
	if(!_ptrc_glGetProgramPipelineiv) numFailed++;
	_ptrc_glIsProgramPipeline = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsProgramPipeline");
	if(!_ptrc_glIsProgramPipeline) numFailed++;
	_ptrc_glProgramUniform1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble))IntGetProcAddress("glProgramUniform1d");
	if(!_ptrc_glProgramUniform1d) numFailed++;
	_ptrc_glProgramUniform1dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform1dv");
	if(!_ptrc_glProgramUniform1dv) numFailed++;
	_ptrc_glProgramUniform1f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat))IntGetProcAddress("glProgramUniform1f");
	if(!_ptrc_glProgramUniform1f) numFailed++;
	_ptrc_glProgramUniform1fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform1fv");
	if(!_ptrc_glProgramUniform1fv) numFailed++;
	_ptrc_glProgramUniform1i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint))IntGetProcAddress("glProgramUniform1i");
	if(!_ptrc_glProgramUniform1i) numFailed++;
	_ptrc_glProgramUniform1iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform1iv");
	if(!_ptrc_glProgramUniform1iv) numFailed++;
	_ptrc_glProgramUniform1ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint))IntGetProcAddress("glProgramUniform1ui");
	if(!_ptrc_glProgramUniform1ui) numFailed++;
	_ptrc_glProgramUniform1uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform1uiv");
	if(!_ptrc_glProgramUniform1uiv) numFailed++;
	_ptrc_glProgramUniform2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform2d");
	if(!_ptrc_glProgramUniform2d) numFailed++;
	_ptrc_glProgramUniform2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform2dv");
	if(!_ptrc_glProgramUniform2dv) numFailed++;
	_ptrc_glProgramUniform2f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform2f");
	if(!_ptrc_glProgramUniform2f) numFailed++;
	_ptrc_glProgramUniform2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform2fv");
	if(!_ptrc_glProgramUniform2fv) numFailed++;
	_ptrc_glProgramUniform2i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform2i");
	if(!_ptrc_glProgramUniform2i) numFailed++;
	_ptrc_glProgramUniform2iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform2iv");
	if(!_ptrc_glProgramUniform2iv) numFailed++;
	_ptrc_glProgramUniform2ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint))IntGetProcAddress("glProgramUniform2ui");
	if(!_ptrc_glProgramUniform2ui) numFailed++;
	_ptrc_glProgramUniform2uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform2uiv");
	if(!_ptrc_glProgramUniform2uiv) numFailed++;
	_ptrc_glProgramUniform3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform3d");
	if(!_ptrc_glProgramUniform3d) numFailed++;
	_ptrc_glProgramUniform3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform3dv");
	if(!_ptrc_glProgramUniform3dv) numFailed++;
	_ptrc_glProgramUniform3f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform3f");
	if(!_ptrc_glProgramUniform3f) numFailed++;
	_ptrc_glProgramUniform3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform3fv");
	if(!_ptrc_glProgramUniform3fv) numFailed++;
	_ptrc_glProgramUniform3i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform3i");
	if(!_ptrc_glProgramUniform3i) numFailed++;
	_ptrc_glProgramUniform3iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform3iv");
	if(!_ptrc_glProgramUniform3iv) numFailed++;
	_ptrc_glProgramUniform3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint, GLuint))IntGetProcAddress("glProgramUniform3ui");
	if(!_ptrc_glProgramUniform3ui) numFailed++;
	_ptrc_glProgramUniform3uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform3uiv");
	if(!_ptrc_glProgramUniform3uiv) numFailed++;
	_ptrc_glProgramUniform4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform4d");
	if(!_ptrc_glProgramUniform4d) numFailed++;
	_ptrc_glProgramUniform4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform4dv");
	if(!_ptrc_glProgramUniform4dv) numFailed++;
	_ptrc_glProgramUniform4f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform4f");
	if(!_ptrc_glProgramUniform4f) numFailed++;
	_ptrc_glProgramUniform4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform4fv");
	if(!_ptrc_glProgramUniform4fv) numFailed++;
	_ptrc_glProgramUniform4i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform4i");
	if(!_ptrc_glProgramUniform4i) numFailed++;
	_ptrc_glProgramUniform4iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform4iv");
	if(!_ptrc_glProgramUniform4iv) numFailed++;
	_ptrc_glProgramUniform4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glProgramUniform4ui");
	if(!_ptrc_glProgramUniform4ui) numFailed++;
	_ptrc_glProgramUniform4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform4uiv");
	if(!_ptrc_glProgramUniform4uiv) numFailed++;
	_ptrc_glProgramUniformMatrix2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2dv");
	if(!_ptrc_glProgramUniformMatrix2dv) numFailed++;
	_ptrc_glProgramUniformMatrix2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2fv");
	if(!_ptrc_glProgramUniformMatrix2fv) numFailed++;
	_ptrc_glProgramUniformMatrix2x3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2x3dv");
	if(!_ptrc_glProgramUniformMatrix2x3dv) numFailed++;
	_ptrc_glProgramUniformMatrix2x3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2x3fv");
	if(!_ptrc_glProgramUniformMatrix2x3fv) numFailed++;
	_ptrc_glProgramUniformMatrix2x4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2x4dv");
	if(!_ptrc_glProgramUniformMatrix2x4dv) numFailed++;
	_ptrc_glProgramUniformMatrix2x4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2x4fv");
	if(!_ptrc_glProgramUniformMatrix2x4fv) numFailed++;
	_ptrc_glProgramUniformMatrix3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3dv");
	if(!_ptrc_glProgramUniformMatrix3dv) numFailed++;
	_ptrc_glProgramUniformMatrix3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3fv");
	if(!_ptrc_glProgramUniformMatrix3fv) numFailed++;
	_ptrc_glProgramUniformMatrix3x2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3x2dv");
	if(!_ptrc_glProgramUniformMatrix3x2dv) numFailed++;
	_ptrc_glProgramUniformMatrix3x2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3x2fv");
	if(!_ptrc_glProgramUniformMatrix3x2fv) numFailed++;
	_ptrc_glProgramUniformMatrix3x4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3x4dv");
	if(!_ptrc_glProgramUniformMatrix3x4dv) numFailed++;
	_ptrc_glProgramUniformMatrix3x4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3x4fv");
	if(!_ptrc_glProgramUniformMatrix3x4fv) numFailed++;
	_ptrc_glProgramUniformMatrix4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4dv");
	if(!_ptrc_glProgramUniformMatrix4dv) numFailed++;
	_ptrc_glProgramUniformMatrix4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4fv");
	if(!_ptrc_glProgramUniformMatrix4fv) numFailed++;
	_ptrc_glProgramUniformMatrix4x2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4x2dv");
	if(!_ptrc_glProgramUniformMatrix4x2dv) numFailed++;
	_ptrc_glProgramUniformMatrix4x2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4x2fv");
	if(!_ptrc_glProgramUniformMatrix4x2fv) numFailed++;
	_ptrc_glProgramUniformMatrix4x3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4x3dv");
	if(!_ptrc_glProgramUniformMatrix4x3dv) numFailed++;
	_ptrc_glProgramUniformMatrix4x3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4x3fv");
	if(!_ptrc_glProgramUniformMatrix4x3fv) numFailed++;
	_ptrc_glUseProgramStages = (void (CODEGEN_FUNCPTR *)(GLuint, GLbitfield, GLuint))IntGetProcAddress("glUseProgramStages");
	if(!_ptrc_glUseProgramStages) numFailed++;
	_ptrc_glValidateProgramPipeline = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glValidateProgramPipeline");
	if(!_ptrc_glValidateProgramPipeline) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glTexBufferRange)(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) = NULL;

static int Load_ARB_texture_buffer_range(void)
{
	int numFailed = 0;
	_ptrc_glTexBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glTexBufferRange");
	if(!_ptrc_glTexBufferRange) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glTexStorage1D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexStorage2D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexStorage3D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) = NULL;

static int Load_ARB_texture_storage(void)
{
	int numFailed = 0;
	_ptrc_glTexStorage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei))IntGetProcAddress("glTexStorage1D");
	if(!_ptrc_glTexStorage1D) numFailed++;
	_ptrc_glTexStorage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glTexStorage2D");
	if(!_ptrc_glTexStorage2D) numFailed++;
	_ptrc_glTexStorage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glTexStorage3D");
	if(!_ptrc_glTexStorage3D) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glTextureView)(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers) = NULL;

static int Load_ARB_texture_view(void)
{
	int numFailed = 0;
	_ptrc_glTextureView = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glTextureView");
	if(!_ptrc_glTextureView) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glBindVertexBuffer)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribBinding)(GLuint attribindex, GLuint bindingindex) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribFormat)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribIFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribLFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexBindingDivisor)(GLuint bindingindex, GLuint divisor) = NULL;

static int Load_ARB_vertex_attrib_binding(void)
{
	int numFailed = 0;
	_ptrc_glBindVertexBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLintptr, GLsizei))IntGetProcAddress("glBindVertexBuffer");
	if(!_ptrc_glBindVertexBuffer) numFailed++;
	_ptrc_glVertexAttribBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexAttribBinding");
	if(!_ptrc_glVertexAttribBinding) numFailed++;
	_ptrc_glVertexAttribFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribFormat");
	if(!_ptrc_glVertexAttribFormat) numFailed++;
	_ptrc_glVertexAttribIFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexAttribIFormat");
	if(!_ptrc_glVertexAttribIFormat) numFailed++;
	_ptrc_glVertexAttribLFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexAttribLFormat");
	if(!_ptrc_glVertexAttribLFormat) numFailed++;
	_ptrc_glVertexBindingDivisor = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexBindingDivisor");
	if(!_ptrc_glVertexBindingDivisor) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glDepthRangeArrayv)(GLuint first, GLsizei count, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthRangeIndexed)(GLuint index, GLdouble n, GLdouble f) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetDoublei_v)(GLenum target, GLuint index, GLdouble * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFloati_v)(GLenum target, GLuint index, GLfloat * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissorArrayv)(GLuint first, GLsizei count, const GLint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissorIndexed)(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissorIndexedv)(GLuint index, const GLint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewportArrayv)(GLuint first, GLsizei count, const GLfloat * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewportIndexedf)(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewportIndexedfv)(GLuint index, const GLfloat * v) = NULL;

static int Load_ARB_viewport_array(void)
{
	int numFailed = 0;
	_ptrc_glDepthRangeArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLdouble *))IntGetProcAddress("glDepthRangeArrayv");
	if(!_ptrc_glDepthRangeArrayv) numFailed++;
	_ptrc_glDepthRangeIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glDepthRangeIndexed");
	if(!_ptrc_glDepthRangeIndexed) numFailed++;
	_ptrc_glGetDoublei_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLdouble *))IntGetProcAddress("glGetDoublei_v");
	if(!_ptrc_glGetDoublei_v) numFailed++;
	_ptrc_glGetFloati_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLfloat *))IntGetProcAddress("glGetFloati_v");
	if(!_ptrc_glGetFloati_v) numFailed++;
	_ptrc_glScissorArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLint *))IntGetProcAddress("glScissorArrayv");
	if(!_ptrc_glScissorArrayv) numFailed++;
	_ptrc_glScissorIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissorIndexed");
	if(!_ptrc_glScissorIndexed) numFailed++;
	_ptrc_glScissorIndexedv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glScissorIndexedv");
	if(!_ptrc_glScissorIndexedv) numFailed++;
	_ptrc_glViewportArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLfloat *))IntGetProcAddress("glViewportArrayv");
	if(!_ptrc_glViewportArrayv) numFailed++;
	_ptrc_glViewportIndexedf = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glViewportIndexedf");
	if(!_ptrc_glViewportIndexedf) numFailed++;
	_ptrc_glViewportIndexedfv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glViewportIndexedfv");
	if(!_ptrc_glViewportIndexedfv) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glClearBufferData)(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferSubData)(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void * data) = NULL;

static int Load_ARB_clear_buffer_object(void)
{
	int numFailed = 0;
	_ptrc_glClearBufferData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum, const void *))IntGetProcAddress("glClearBufferData");
	if(!_ptrc_glClearBufferData) numFailed++;
	_ptrc_glClearBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *))IntGetProcAddress("glClearBufferSubData");
	if(!_ptrc_glClearBufferSubData) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glCopyImageSubData)(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth) = NULL;

static int Load_ARB_copy_image(void)
{
	int numFailed = 0;
	_ptrc_glCopyImageSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glCopyImageSubData");
	if(!_ptrc_glCopyImageSubData) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glFramebufferParameteri)(GLenum target, GLenum pname, GLint param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFramebufferParameteriv)(GLenum target, GLenum pname, GLint * params) = NULL;

static int Load_ARB_framebuffer_no_attachments(void)
{
	int numFailed = 0;
	_ptrc_glFramebufferParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glFramebufferParameteri");
	if(!_ptrc_glFramebufferParameteri) numFailed++;
	_ptrc_glGetFramebufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetFramebufferParameteriv");
	if(!_ptrc_glGetFramebufferParameteriv) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glInvalidateBufferData)(GLuint buffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr length) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum * attachments) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateSubFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum * attachments, GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateTexImage)(GLuint texture, GLint level) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth) = NULL;

static int Load_ARB_invalidate_subdata(void)
{
	int numFailed = 0;
	_ptrc_glInvalidateBufferData = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glInvalidateBufferData");
	if(!_ptrc_glInvalidateBufferData) numFailed++;
	_ptrc_glInvalidateBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glInvalidateBufferSubData");
	if(!_ptrc_glInvalidateBufferSubData) numFailed++;
	_ptrc_glInvalidateFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLenum *))IntGetProcAddress("glInvalidateFramebuffer");
	if(!_ptrc_glInvalidateFramebuffer) numFailed++;
	_ptrc_glInvalidateSubFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glInvalidateSubFramebuffer");
	if(!_ptrc_glInvalidateSubFramebuffer) numFailed++;
	_ptrc_glInvalidateTexImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint))IntGetProcAddress("glInvalidateTexImage");
	if(!_ptrc_glInvalidateTexImage) numFailed++;
	_ptrc_glInvalidateTexSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glInvalidateTexSubImage");
	if(!_ptrc_glInvalidateTexSubImage) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glTexStorage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexStorage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) = NULL;

static int Load_ARB_texture_storage_multisample(void)
{
	int numFailed = 0;
	_ptrc_glTexStorage2DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexStorage2DMultisample");
	if(!_ptrc_glTexStorage2DMultisample) numFailed++;
	_ptrc_glTexStorage3DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexStorage3DMultisample");
	if(!_ptrc_glTexStorage3DMultisample) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageCallback)(GLDEBUGPROC callback, const void * userParam) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageControl)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint * ids, GLboolean enabled) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageInsert)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetDebugMessageLog)(GLuint count, GLsizei bufSize, GLenum * sources, GLenum * types, GLuint * ids, GLenum * severities, GLsizei * lengths, GLchar * messageLog) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetObjectLabel)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei * length, GLchar * label) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetObjectPtrLabel)(const void * ptr, GLsizei bufSize, GLsizei * length, GLchar * label) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPointerv)(GLenum pname, void ** params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glObjectLabel)(GLenum identifier, GLuint name, GLsizei length, const GLchar * label) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glObjectPtrLabel)(const void * ptr, GLsizei length, const GLchar * label) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopDebugGroup)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushDebugGroup)(GLenum source, GLuint id, GLsizei length, const GLchar * message) = NULL;

static int Load_KHR_debug(void)
{
	int numFailed = 0;
	_ptrc_glDebugMessageCallback = (void (CODEGEN_FUNCPTR *)(GLDEBUGPROC, const void *))IntGetProcAddress("glDebugMessageCallback");
	if(!_ptrc_glDebugMessageCallback) numFailed++;
	_ptrc_glDebugMessageControl = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean))IntGetProcAddress("glDebugMessageControl");
	if(!_ptrc_glDebugMessageControl) numFailed++;
	_ptrc_glDebugMessageInsert = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *))IntGetProcAddress("glDebugMessageInsert");
	if(!_ptrc_glDebugMessageInsert) numFailed++;
	_ptrc_glGetDebugMessageLog = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *))IntGetProcAddress("glGetDebugMessageLog");
	if(!_ptrc_glGetDebugMessageLog) numFailed++;
	_ptrc_glGetObjectLabel = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetObjectLabel");
	if(!_ptrc_glGetObjectLabel) numFailed++;
	_ptrc_glGetObjectPtrLabel = (void (CODEGEN_FUNCPTR *)(const void *, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetObjectPtrLabel");
	if(!_ptrc_glGetObjectPtrLabel) numFailed++;
	_ptrc_glGetPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, void **))IntGetProcAddress("glGetPointerv");
	if(!_ptrc_glGetPointerv) numFailed++;
	_ptrc_glObjectLabel = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLchar *))IntGetProcAddress("glObjectLabel");
	if(!_ptrc_glObjectLabel) numFailed++;
	_ptrc_glObjectPtrLabel = (void (CODEGEN_FUNCPTR *)(const void *, GLsizei, const GLchar *))IntGetProcAddress("glObjectPtrLabel");
	if(!_ptrc_glObjectPtrLabel) numFailed++;
	_ptrc_glPopDebugGroup = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glPopDebugGroup");
	if(!_ptrc_glPopDebugGroup) numFailed++;
	_ptrc_glPushDebugGroup = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLchar *))IntGetProcAddress("glPushDebugGroup");
	if(!_ptrc_glPushDebugGroup) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glBufferStorage)(GLenum target, GLsizeiptr size, const void * data, GLbitfield flags) = NULL;

static int Load_ARB_buffer_storage(void)
{
	int numFailed = 0;
	_ptrc_glBufferStorage = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizeiptr, const void *, GLbitfield))IntGetProcAddress("glBufferStorage");
	if(!_ptrc_glBufferStorage) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glClearTexImage)(GLuint texture, GLint level, GLenum format, GLenum type, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * data) = NULL;

static int Load_ARB_clear_texture(void)
{
	int numFailed = 0;
	_ptrc_glClearTexImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLenum, const void *))IntGetProcAddress("glClearTexImage");
	if(!_ptrc_glClearTexImage) numFailed++;
	_ptrc_glClearTexSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glClearTexSubImage");
	if(!_ptrc_glClearTexSubImage) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glBindBuffersBase)(GLenum target, GLuint first, GLsizei count, const GLuint * buffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindBuffersRange)(GLenum target, GLuint first, GLsizei count, const GLuint * buffers, const GLintptr * offsets, const GLsizeiptr * sizes) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindImageTextures)(GLuint first, GLsizei count, const GLuint * textures) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindSamplers)(GLuint first, GLsizei count, const GLuint * samplers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindTextures)(GLuint first, GLsizei count, const GLuint * textures) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindVertexBuffers)(GLuint first, GLsizei count, const GLuint * buffers, const GLintptr * offsets, const GLsizei * strides) = NULL;

static int Load_ARB_multi_bind(void)
{
	int numFailed = 0;
	_ptrc_glBindBuffersBase = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLuint *))IntGetProcAddress("glBindBuffersBase");
	if(!_ptrc_glBindBuffersBase) numFailed++;
	_ptrc_glBindBuffersRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizeiptr *))IntGetProcAddress("glBindBuffersRange");
	if(!_ptrc_glBindBuffersRange) numFailed++;
	_ptrc_glBindImageTextures = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *))IntGetProcAddress("glBindImageTextures");
	if(!_ptrc_glBindImageTextures) numFailed++;
	_ptrc_glBindSamplers = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *))IntGetProcAddress("glBindSamplers");
	if(!_ptrc_glBindSamplers) numFailed++;
	_ptrc_glBindTextures = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *))IntGetProcAddress("glBindTextures");
	if(!_ptrc_glBindTextures) numFailed++;
	_ptrc_glBindVertexBuffers = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizei *))IntGetProcAddress("glBindVertexBuffers");
	if(!_ptrc_glBindVertexBuffers) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glClipControl)(GLenum origin, GLenum depth) = NULL;

static int Load_ARB_clip_control(void)
{
	int numFailed = 0;
	_ptrc_glClipControl = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glClipControl");
	if(!_ptrc_glClipControl) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glBindTextureUnit)(GLuint unit, GLuint texture) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlitNamedFramebuffer)(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glCheckNamedFramebufferStatus)(GLuint framebuffer, GLenum target) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearNamedBufferData)(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearNamedBufferSubData)(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearNamedFramebufferfi)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat depth, GLint stencil) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearNamedFramebufferfv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearNamedFramebufferiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearNamedFramebufferuiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyNamedBufferSubData)(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateBuffers)(GLsizei n, GLuint * buffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateFramebuffers)(GLsizei n, GLuint * framebuffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateProgramPipelines)(GLsizei n, GLuint * pipelines) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateQueries)(GLenum target, GLsizei n, GLuint * ids) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateRenderbuffers)(GLsizei n, GLuint * renderbuffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateSamplers)(GLsizei n, GLuint * samplers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateTextures)(GLenum target, GLsizei n, GLuint * textures) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateTransformFeedbacks)(GLsizei n, GLuint * ids) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCreateVertexArrays)(GLsizei n, GLuint * arrays) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisableVertexArrayAttrib)(GLuint vaobj, GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnableVertexArrayAttrib)(GLuint vaobj, GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFlushMappedNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenerateTextureMipmap)(GLuint texture) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetCompressedTextureImage)(GLuint texture, GLint level, GLsizei bufSize, void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetNamedBufferParameteri64v)(GLuint buffer, GLenum pname, GLint64 * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetNamedBufferParameteriv)(GLuint buffer, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetNamedBufferPointerv)(GLuint buffer, GLenum pname, void ** params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetNamedFramebufferAttachmentParameteriv)(GLuint framebuffer, GLenum attachment, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetNamedFramebufferParameteriv)(GLuint framebuffer, GLenum pname, GLint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetNamedRenderbufferParameteriv)(GLuint renderbuffer, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryBufferObjecti64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryBufferObjectiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryBufferObjectui64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryBufferObjectuiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTextureImage)(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTextureLevelParameterfv)(GLuint texture, GLint level, GLenum pname, GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTextureLevelParameteriv)(GLuint texture, GLint level, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTextureParameterIiv)(GLuint texture, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTextureParameterIuiv)(GLuint texture, GLenum pname, GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTextureParameterfv)(GLuint texture, GLenum pname, GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTextureParameteriv)(GLuint texture, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTransformFeedbacki64_v)(GLuint xfb, GLenum pname, GLuint index, GLint64 * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTransformFeedbacki_v)(GLuint xfb, GLenum pname, GLuint index, GLint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTransformFeedbackiv)(GLuint xfb, GLenum pname, GLint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexArrayIndexed64iv)(GLuint vaobj, GLuint index, GLenum pname, GLint64 * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexArrayIndexediv)(GLuint vaobj, GLuint index, GLenum pname, GLint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexArrayiv)(GLuint vaobj, GLenum pname, GLint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateNamedFramebufferData)(GLuint framebuffer, GLsizei numAttachments, const GLenum * attachments) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateNamedFramebufferSubData)(GLuint framebuffer, GLsizei numAttachments, const GLenum * attachments, GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
void * (CODEGEN_FUNCPTR *_ptrc_glMapNamedBuffer)(GLuint buffer, GLenum access) = NULL;
void * (CODEGEN_FUNCPTR *_ptrc_glMapNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedBufferData)(GLuint buffer, GLsizeiptr size, const void * data, GLenum usage) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedBufferStorage)(GLuint buffer, GLsizeiptr size, const void * data, GLbitfield flags) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedFramebufferDrawBuffer)(GLuint framebuffer, GLenum buf) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedFramebufferDrawBuffers)(GLuint framebuffer, GLsizei n, const GLenum * bufs) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedFramebufferParameteri)(GLuint framebuffer, GLenum pname, GLint param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedFramebufferReadBuffer)(GLuint framebuffer, GLenum src) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedFramebufferRenderbuffer)(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedFramebufferTexture)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedFramebufferTextureLayer)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedRenderbufferStorage)(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNamedRenderbufferStorageMultisample)(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureBuffer)(GLuint texture, GLenum internalformat, GLuint buffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureBufferRange)(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureParameterIiv)(GLuint texture, GLenum pname, const GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureParameterIuiv)(GLuint texture, GLenum pname, const GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureParameterf)(GLuint texture, GLenum pname, GLfloat param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureParameterfv)(GLuint texture, GLenum pname, const GLfloat * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureParameteri)(GLuint texture, GLenum pname, GLint param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureParameteriv)(GLuint texture, GLenum pname, const GLint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureStorage1D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureStorage2D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureStorage2DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureStorage3D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureStorage3DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTransformFeedbackBufferBase)(GLuint xfb, GLuint index, GLuint buffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTransformFeedbackBufferRange)(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glUnmapNamedBuffer)(GLuint buffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexArrayAttribBinding)(GLuint vaobj, GLuint attribindex, GLuint bindingindex) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexArrayAttribFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexArrayAttribIFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexArrayAttribLFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexArrayBindingDivisor)(GLuint vaobj, GLuint bindingindex, GLuint divisor) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexArrayElementBuffer)(GLuint vaobj, GLuint buffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexArrayVertexBuffer)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexArrayVertexBuffers)(GLuint vaobj, GLuint first, GLsizei count, const GLuint * buffers, const GLintptr * offsets, const GLsizei * strides) = NULL;

static int Load_ARB_direct_state_access(void)
{
	int numFailed = 0;
	_ptrc_glBindTextureUnit = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glBindTextureUnit");
	if(!_ptrc_glBindTextureUnit) numFailed++;
	_ptrc_glBlitNamedFramebuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum))IntGetProcAddress("glBlitNamedFramebuffer");
	if(!_ptrc_glBlitNamedFramebuffer) numFailed++;
	_ptrc_glCheckNamedFramebufferStatus = (GLenum (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glCheckNamedFramebufferStatus");
	if(!_ptrc_glCheckNamedFramebufferStatus) numFailed++;
	_ptrc_glClearNamedBufferData = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLenum, const void *))IntGetProcAddress("glClearNamedBufferData");
	if(!_ptrc_glClearNamedBufferData) numFailed++;
	_ptrc_glClearNamedBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *))IntGetProcAddress("glClearNamedBufferSubData");
	if(!_ptrc_glClearNamedBufferSubData) numFailed++;
	_ptrc_glClearNamedFramebufferfi = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, const GLfloat, GLint))IntGetProcAddress("glClearNamedFramebufferfi");
	if(!_ptrc_glClearNamedFramebufferfi) numFailed++;
	_ptrc_glClearNamedFramebufferfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, const GLfloat *))IntGetProcAddress("glClearNamedFramebufferfv");
	if(!_ptrc_glClearNamedFramebufferfv) numFailed++;
	_ptrc_glClearNamedFramebufferiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, const GLint *))IntGetProcAddress("glClearNamedFramebufferiv");
	if(!_ptrc_glClearNamedFramebufferiv) numFailed++;
	_ptrc_glClearNamedFramebufferuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, const GLuint *))IntGetProcAddress("glClearNamedFramebufferuiv");
	if(!_ptrc_glClearNamedFramebufferuiv) numFailed++;
	_ptrc_glCompressedTextureSubImage1D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTextureSubImage1D");
	if(!_ptrc_glCompressedTextureSubImage1D) numFailed++;
	_ptrc_glCompressedTextureSubImage2D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTextureSubImage2D");
	if(!_ptrc_glCompressedTextureSubImage2D) numFailed++;
	_ptrc_glCompressedTextureSubImage3D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTextureSubImage3D");
	if(!_ptrc_glCompressedTextureSubImage3D) numFailed++;
	_ptrc_glCopyNamedBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLintptr, GLintptr, GLsizeiptr))IntGetProcAddress("glCopyNamedBufferSubData");
	if(!_ptrc_glCopyNamedBufferSubData) numFailed++;
	_ptrc_glCopyTextureSubImage1D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei))IntGetProcAddress("glCopyTextureSubImage1D");
	if(!_ptrc_glCopyTextureSubImage1D) numFailed++;
	_ptrc_glCopyTextureSubImage2D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTextureSubImage2D");
	if(!_ptrc_glCopyTextureSubImage2D) numFailed++;
	_ptrc_glCopyTextureSubImage3D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTextureSubImage3D");
	if(!_ptrc_glCopyTextureSubImage3D) numFailed++;
	_ptrc_glCreateBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateBuffers");
	if(!_ptrc_glCreateBuffers) numFailed++;
	_ptrc_glCreateFramebuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateFramebuffers");
	if(!_ptrc_glCreateFramebuffers) numFailed++;
	_ptrc_glCreateProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateProgramPipelines");
	if(!_ptrc_glCreateProgramPipelines) numFailed++;
	_ptrc_glCreateQueries = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLuint *))IntGetProcAddress("glCreateQueries");
	if(!_ptrc_glCreateQueries) numFailed++;
	_ptrc_glCreateRenderbuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateRenderbuffers");
	if(!_ptrc_glCreateRenderbuffers) numFailed++;
	_ptrc_glCreateSamplers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateSamplers");
	if(!_ptrc_glCreateSamplers) numFailed++;
	_ptrc_glCreateTextures = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLuint *))IntGetProcAddress("glCreateTextures");
	if(!_ptrc_glCreateTextures) numFailed++;
	_ptrc_glCreateTransformFeedbacks = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateTransformFeedbacks");
	if(!_ptrc_glCreateTransformFeedbacks) numFailed++;
	_ptrc_glCreateVertexArrays = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateVertexArrays");
	if(!_ptrc_glCreateVertexArrays) numFailed++;
	_ptrc_glDisableVertexArrayAttrib = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glDisableVertexArrayAttrib");
	if(!_ptrc_glDisableVertexArrayAttrib) numFailed++;
	_ptrc_glEnableVertexArrayAttrib = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glEnableVertexArrayAttrib");
	if(!_ptrc_glEnableVertexArrayAttrib) numFailed++;
	_ptrc_glFlushMappedNamedBufferRange = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glFlushMappedNamedBufferRange");
	if(!_ptrc_glFlushMappedNamedBufferRange) numFailed++;
	_ptrc_glGenerateTextureMipmap = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glGenerateTextureMipmap");
	if(!_ptrc_glGenerateTextureMipmap) numFailed++;
	_ptrc_glGetCompressedTextureImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, void *))IntGetProcAddress("glGetCompressedTextureImage");
	if(!_ptrc_glGetCompressedTextureImage) numFailed++;
	_ptrc_glGetNamedBufferParameteri64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint64 *))IntGetProcAddress("glGetNamedBufferParameteri64v");
	if(!_ptrc_glGetNamedBufferParameteri64v) numFailed++;
	_ptrc_glGetNamedBufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetNamedBufferParameteriv");
	if(!_ptrc_glGetNamedBufferParameteriv) numFailed++;
	_ptrc_glGetNamedBufferPointerv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, void **))IntGetProcAddress("glGetNamedBufferPointerv");
	if(!_ptrc_glGetNamedBufferPointerv) numFailed++;
	_ptrc_glGetNamedBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr, void *))IntGetProcAddress("glGetNamedBufferSubData");
	if(!_ptrc_glGetNamedBufferSubData) numFailed++;
	_ptrc_glGetNamedFramebufferAttachmentParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLint *))IntGetProcAddress("glGetNamedFramebufferAttachmentParameteriv");
	if(!_ptrc_glGetNamedFramebufferAttachmentParameteriv) numFailed++;
	_ptrc_glGetNamedFramebufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetNamedFramebufferParameteriv");
	if(!_ptrc_glGetNamedFramebufferParameteriv) numFailed++;
	_ptrc_glGetNamedRenderbufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetNamedRenderbufferParameteriv");
	if(!_ptrc_glGetNamedRenderbufferParameteriv) numFailed++;
	_ptrc_glGetQueryBufferObjecti64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLintptr))IntGetProcAddress("glGetQueryBufferObjecti64v");
	if(!_ptrc_glGetQueryBufferObjecti64v) numFailed++;
	_ptrc_glGetQueryBufferObjectiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLintptr))IntGetProcAddress("glGetQueryBufferObjectiv");
	if(!_ptrc_glGetQueryBufferObjectiv) numFailed++;
	_ptrc_glGetQueryBufferObjectui64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLintptr))IntGetProcAddress("glGetQueryBufferObjectui64v");
	if(!_ptrc_glGetQueryBufferObjectui64v) numFailed++;
	_ptrc_glGetQueryBufferObjectuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLintptr))IntGetProcAddress("glGetQueryBufferObjectuiv");
	if(!_ptrc_glGetQueryBufferObjectuiv) numFailed++;
	_ptrc_glGetTextureImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLenum, GLsizei, void *))IntGetProcAddress("glGetTextureImage");
	if(!_ptrc_glGetTextureImage) numFailed++;
	_ptrc_glGetTextureLevelParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLfloat *))IntGetProcAddress("glGetTextureLevelParameterfv");
	if(!_ptrc_glGetTextureLevelParameterfv) numFailed++;
	_ptrc_glGetTextureLevelParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLint *))IntGetProcAddress("glGetTextureLevelParameteriv");
	if(!_ptrc_glGetTextureLevelParameteriv) numFailed++;
	_ptrc_glGetTextureParameterIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetTextureParameterIiv");
	if(!_ptrc_glGetTextureParameterIiv) numFailed++;
	_ptrc_glGetTextureParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetTextureParameterIuiv");
	if(!_ptrc_glGetTextureParameterIuiv) numFailed++;
	_ptrc_glGetTextureParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat *))IntGetProcAddress("glGetTextureParameterfv");
	if(!_ptrc_glGetTextureParameterfv) numFailed++;
	_ptrc_glGetTextureParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetTextureParameteriv");
	if(!_ptrc_glGetTextureParameteriv) numFailed++;
	_ptrc_glGetTransformFeedbacki64_v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLint64 *))IntGetProcAddress("glGetTransformFeedbacki64_v");
	if(!_ptrc_glGetTransformFeedbacki64_v) numFailed++;
	_ptrc_glGetTransformFeedbacki_v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLint *))IntGetProcAddress("glGetTransformFeedbacki_v");
	if(!_ptrc_glGetTransformFeedbacki_v) numFailed++;
	_ptrc_glGetTransformFeedbackiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetTransformFeedbackiv");
	if(!_ptrc_glGetTransformFeedbackiv) numFailed++;
	_ptrc_glGetVertexArrayIndexed64iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLint64 *))IntGetProcAddress("glGetVertexArrayIndexed64iv");
	if(!_ptrc_glGetVertexArrayIndexed64iv) numFailed++;
	_ptrc_glGetVertexArrayIndexediv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexArrayIndexediv");
	if(!_ptrc_glGetVertexArrayIndexediv) numFailed++;
	_ptrc_glGetVertexArrayiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexArrayiv");
	if(!_ptrc_glGetVertexArrayiv) numFailed++;
	_ptrc_glInvalidateNamedFramebufferData = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLenum *))IntGetProcAddress("glInvalidateNamedFramebufferData");
	if(!_ptrc_glInvalidateNamedFramebufferData) numFailed++;
	_ptrc_glInvalidateNamedFramebufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glInvalidateNamedFramebufferSubData");
	if(!_ptrc_glInvalidateNamedFramebufferSubData) numFailed++;
	_ptrc_glMapNamedBuffer = (void * (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glMapNamedBuffer");
	if(!_ptrc_glMapNamedBuffer) numFailed++;
	_ptrc_glMapNamedBufferRange = (void * (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr, GLbitfield))IntGetProcAddress("glMapNamedBufferRange");
	if(!_ptrc_glMapNamedBufferRange) numFailed++;
	_ptrc_glNamedBufferData = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizeiptr, const void *, GLenum))IntGetProcAddress("glNamedBufferData");
	if(!_ptrc_glNamedBufferData) numFailed++;
	_ptrc_glNamedBufferStorage = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizeiptr, const void *, GLbitfield))IntGetProcAddress("glNamedBufferStorage");
	if(!_ptrc_glNamedBufferStorage) numFailed++;
	_ptrc_glNamedBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr, const void *))IntGetProcAddress("glNamedBufferSubData");
	if(!_ptrc_glNamedBufferSubData) numFailed++;
	_ptrc_glNamedFramebufferDrawBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glNamedFramebufferDrawBuffer");
	if(!_ptrc_glNamedFramebufferDrawBuffer) numFailed++;
	_ptrc_glNamedFramebufferDrawBuffers = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLenum *))IntGetProcAddress("glNamedFramebufferDrawBuffers");
	if(!_ptrc_glNamedFramebufferDrawBuffers) numFailed++;
	_ptrc_glNamedFramebufferParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glNamedFramebufferParameteri");
	if(!_ptrc_glNamedFramebufferParameteri) numFailed++;
	_ptrc_glNamedFramebufferReadBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glNamedFramebufferReadBuffer");
	if(!_ptrc_glNamedFramebufferReadBuffer) numFailed++;
	_ptrc_glNamedFramebufferRenderbuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLuint))IntGetProcAddress("glNamedFramebufferRenderbuffer");
	if(!_ptrc_glNamedFramebufferRenderbuffer) numFailed++;
	_ptrc_glNamedFramebufferTexture = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLint))IntGetProcAddress("glNamedFramebufferTexture");
	if(!_ptrc_glNamedFramebufferTexture) numFailed++;
	_ptrc_glNamedFramebufferTextureLayer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glNamedFramebufferTextureLayer");
	if(!_ptrc_glNamedFramebufferTextureLayer) numFailed++;
	_ptrc_glNamedRenderbufferStorage = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLsizei, GLsizei))IntGetProcAddress("glNamedRenderbufferStorage");
	if(!_ptrc_glNamedRenderbufferStorage) numFailed++;
	_ptrc_glNamedRenderbufferStorageMultisample = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glNamedRenderbufferStorageMultisample");
	if(!_ptrc_glNamedRenderbufferStorageMultisample) numFailed++;
	_ptrc_glTextureBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint))IntGetProcAddress("glTextureBuffer");
	if(!_ptrc_glTextureBuffer) numFailed++;
	_ptrc_glTextureBufferRange = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glTextureBufferRange");
	if(!_ptrc_glTextureBufferRange) numFailed++;
	_ptrc_glTextureParameterIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLint *))IntGetProcAddress("glTextureParameterIiv");
	if(!_ptrc_glTextureParameterIiv) numFailed++;
	_ptrc_glTextureParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLuint *))IntGetProcAddress("glTextureParameterIuiv");
	if(!_ptrc_glTextureParameterIuiv) numFailed++;
	_ptrc_glTextureParameterf = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat))IntGetProcAddress("glTextureParameterf");
	if(!_ptrc_glTextureParameterf) numFailed++;
	_ptrc_glTextureParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLfloat *))IntGetProcAddress("glTextureParameterfv");
	if(!_ptrc_glTextureParameterfv) numFailed++;
	_ptrc_glTextureParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glTextureParameteri");
	if(!_ptrc_glTextureParameteri) numFailed++;
	_ptrc_glTextureParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLint *))IntGetProcAddress("glTextureParameteriv");
	if(!_ptrc_glTextureParameteriv) numFailed++;
	_ptrc_glTextureStorage1D = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei))IntGetProcAddress("glTextureStorage1D");
	if(!_ptrc_glTextureStorage1D) numFailed++;
	_ptrc_glTextureStorage2D = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glTextureStorage2D");
	if(!_ptrc_glTextureStorage2D) numFailed++;
	_ptrc_glTextureStorage2DMultisample = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTextureStorage2DMultisample");
	if(!_ptrc_glTextureStorage2DMultisample) numFailed++;
	_ptrc_glTextureStorage3D = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glTextureStorage3D");
	if(!_ptrc_glTextureStorage3D) numFailed++;
	_ptrc_glTextureStorage3DMultisample = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTextureStorage3DMultisample");
	if(!_ptrc_glTextureStorage3DMultisample) numFailed++;
	_ptrc_glTextureSubImage1D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTextureSubImage1D");
	if(!_ptrc_glTextureSubImage1D) numFailed++;
	_ptrc_glTextureSubImage2D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTextureSubImage2D");
	if(!_ptrc_glTextureSubImage2D) numFailed++;
	_ptrc_glTextureSubImage3D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTextureSubImage3D");
	if(!_ptrc_glTextureSubImage3D) numFailed++;
	_ptrc_glTransformFeedbackBufferBase = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glTransformFeedbackBufferBase");
	if(!_ptrc_glTransformFeedbackBufferBase) numFailed++;
	_ptrc_glTransformFeedbackBufferRange = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glTransformFeedbackBufferRange");
	if(!_ptrc_glTransformFeedbackBufferRange) numFailed++;
	_ptrc_glUnmapNamedBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glUnmapNamedBuffer");
	if(!_ptrc_glUnmapNamedBuffer) numFailed++;
	_ptrc_glVertexArrayAttribBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glVertexArrayAttribBinding");
	if(!_ptrc_glVertexArrayAttribBinding) numFailed++;
	_ptrc_glVertexArrayAttribFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexArrayAttribFormat");
	if(!_ptrc_glVertexArrayAttribFormat) numFailed++;
	_ptrc_glVertexArrayAttribIFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexArrayAttribIFormat");
	if(!_ptrc_glVertexArrayAttribIFormat) numFailed++;
	_ptrc_glVertexArrayAttribLFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexArrayAttribLFormat");
	if(!_ptrc_glVertexArrayAttribLFormat) numFailed++;
	_ptrc_glVertexArrayBindingDivisor = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glVertexArrayBindingDivisor");
	if(!_ptrc_glVertexArrayBindingDivisor) numFailed++;
	_ptrc_glVertexArrayElementBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexArrayElementBuffer");
	if(!_ptrc_glVertexArrayElementBuffer) numFailed++;
	_ptrc_glVertexArrayVertexBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLintptr, GLsizei))IntGetProcAddress("glVertexArrayVertexBuffer");
	if(!_ptrc_glVertexArrayVertexBuffer) numFailed++;
	_ptrc_glVertexArrayVertexBuffers = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizei *))IntGetProcAddress("glVertexArrayVertexBuffers");
	if(!_ptrc_glVertexArrayVertexBuffers) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glGetCompressedTextureSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTextureSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void * pixels) = NULL;

static int Load_ARB_get_texture_sub_image(void)
{
	int numFailed = 0;
	_ptrc_glGetCompressedTextureSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLsizei, void *))IntGetProcAddress("glGetCompressedTextureSubImage");
	if(!_ptrc_glGetCompressedTextureSubImage) numFailed++;
	_ptrc_glGetTextureSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *))IntGetProcAddress("glGetTextureSubImage");
	if(!_ptrc_glGetTextureSubImage) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glTextureBarrier)(void) = NULL;

static int Load_ARB_texture_barrier(void)
{
	int numFailed = 0;
	_ptrc_glTextureBarrier = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glTextureBarrier");
	if(!_ptrc_glTextureBarrier) numFailed++;
	return numFailed;
}

GLenum (CODEGEN_FUNCPTR *_ptrc_glGetGraphicsResetStatus)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetnUniformfv)(GLuint program, GLint location, GLsizei bufSize, GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetnUniformiv)(GLuint program, GLint location, GLsizei bufSize, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetnUniformuiv)(GLuint program, GLint location, GLsizei bufSize, GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadnPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void * data) = NULL;

static int Load_KHR_robustness(void)
{
	int numFailed = 0;
	_ptrc_glGetGraphicsResetStatus = (GLenum (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glGetGraphicsResetStatus");
	if(!_ptrc_glGetGraphicsResetStatus) numFailed++;
	_ptrc_glGetnUniformfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLfloat *))IntGetProcAddress("glGetnUniformfv");
	if(!_ptrc_glGetnUniformfv) numFailed++;
	_ptrc_glGetnUniformiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLint *))IntGetProcAddress("glGetnUniformiv");
	if(!_ptrc_glGetnUniformiv) numFailed++;
	_ptrc_glGetnUniformuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLuint *))IntGetProcAddress("glGetnUniformuiv");
	if(!_ptrc_glGetnUniformuiv) numFailed++;
	_ptrc_glReadnPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *))IntGetProcAddress("glReadnPixels");
	if(!_ptrc_glReadnPixels) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glBlendFunc)(GLenum sfactor, GLenum dfactor) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClear)(GLbitfield mask) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearDepth)(GLdouble depth) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearStencil)(GLint s) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCullFace)(GLenum mode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthFunc)(GLenum func) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthMask)(GLboolean flag) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthRange)(GLdouble ren_near, GLdouble ren_far) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisable)(GLenum cap) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawBuffer)(GLenum buf) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnable)(GLenum cap) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFinish)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFlush)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFrontFace)(GLenum mode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBooleanv)(GLenum pname, GLboolean * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetDoublev)(GLenum pname, GLdouble * data) = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glGetError)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFloatv)(GLenum pname, GLfloat * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetIntegerv)(GLenum pname, GLint * data) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetString)(GLenum name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexImage)(GLenum target, GLint level, GLenum format, GLenum type, void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameteriv)(GLenum target, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glHint)(GLenum target, GLenum mode) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabled)(GLenum cap) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLineWidth)(GLfloat width) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLogicOp)(GLenum opcode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelStoref)(GLenum pname, GLfloat param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelStorei)(GLenum pname, GLint param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointSize)(GLfloat size) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonMode)(GLenum face, GLenum mode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadBuffer)(GLenum src) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissor)(GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilFunc)(GLenum func, GLint ref, GLuint mask) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilMask)(GLuint mask) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterf)(GLenum target, GLenum pname, GLfloat param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterfv)(GLenum target, GLenum pname, const GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameteri)(GLenum target, GLenum pname, GLint param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameteriv)(GLenum target, GLenum pname, const GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBindTexture)(GLenum target, GLuint texture) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteTextures)(GLsizei n, const GLuint * textures) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArrays)(GLenum mode, GLint first, GLsizei count) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void * indices) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenTextures)(GLsizei n, GLuint * textures) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTexture)(GLuint texture) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonOffset)(GLfloat factor, GLfloat units) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawRangeElements)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void * indices) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage3D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glActiveTexture)(GLenum texture) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetCompressedTexImage)(GLenum target, GLint level, void * img) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSampleCoverage)(GLfloat value, GLboolean invert) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquation)(GLenum mode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawArrays)(GLenum mode, const GLint * first, const GLsizei * count, GLsizei drawcount) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawElements)(GLenum mode, const GLsizei * count, GLenum type, const void *const* indices, GLsizei drawcount) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameterf)(GLenum pname, GLfloat param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameterfv)(GLenum pname, const GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameteri)(GLenum pname, GLint param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameteriv)(GLenum pname, const GLint * params) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBeginQuery)(GLenum target, GLuint id) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindBuffer)(GLenum target, GLuint buffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBufferData)(GLenum target, GLsizeiptr size, const void * data, GLenum usage) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteBuffers)(GLsizei n, const GLuint * buffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteQueries)(GLsizei n, const GLuint * ids) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndQuery)(GLenum target) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenBuffers)(GLsizei n, GLuint * buffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenQueries)(GLsizei n, GLuint * ids) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferParameteriv)(GLenum target, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferPointerv)(GLenum target, GLenum pname, void ** params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, void * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectiv)(GLuint id, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryiv)(GLenum target, GLenum pname, GLint * params) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsBuffer)(GLuint buffer) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsQuery)(GLuint id) = NULL;
void * (CODEGEN_FUNCPTR *_ptrc_glMapBuffer)(GLenum target, GLenum access) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glUnmapBuffer)(GLenum target) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glAttachShader)(GLuint program, GLuint shader) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindAttribLocation)(GLuint program, GLuint index, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompileShader)(GLuint shader) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateProgram)(void) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateShader)(GLenum type) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteProgram)(GLuint program) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteShader)(GLuint shader) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDetachShader)(GLuint program, GLuint shader) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisableVertexAttribArray)(GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawBuffers)(GLsizei n, const GLenum * bufs) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnableVertexAttribArray)(GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei * count, GLuint * shaders) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetAttribLocation)(GLuint program, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramiv)(GLuint program, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderiv)(GLuint shader, GLenum pname, GLint * params) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetUniformLocation)(GLuint program, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformfv)(GLuint program, GLint location, GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformiv)(GLuint program, GLint location, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribPointerv)(GLuint index, GLenum pname, void ** pointer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribdv)(GLuint index, GLenum pname, GLdouble * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribiv)(GLuint index, GLenum pname, GLint * params) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsProgram)(GLuint program) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsShader)(GLuint shader) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLinkProgram)(GLuint program) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glShaderSource)(GLuint shader, GLsizei count, const GLchar *const* string, const GLint * length) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilMaskSeparate)(GLenum face, GLuint mask) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1f)(GLint location, GLfloat v0) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1fv)(GLint location, GLsizei count, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1i)(GLint location, GLint v0) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1iv)(GLint location, GLsizei count, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2f)(GLint location, GLfloat v0, GLfloat v1) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2fv)(GLint location, GLsizei count, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2i)(GLint location, GLint v0, GLint v1) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2iv)(GLint location, GLsizei count, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3fv)(GLint location, GLsizei count, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3iv)(GLint location, GLsizei count, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4fv)(GLint location, GLsizei count, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4iv)(GLint location, GLsizei count, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUseProgram)(GLuint program) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glValidateProgram)(GLuint program) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1d)(GLuint index, GLdouble x) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1dv)(GLuint index, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1f)(GLuint index, GLfloat x) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1fv)(GLuint index, const GLfloat * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1s)(GLuint index, GLshort x) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1sv)(GLuint index, const GLshort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2dv)(GLuint index, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2fv)(GLuint index, const GLfloat * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2s)(GLuint index, GLshort x, GLshort y) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2sv)(GLuint index, const GLshort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3dv)(GLuint index, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3fv)(GLuint index, const GLfloat * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3s)(GLuint index, GLshort x, GLshort y, GLshort z) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3sv)(GLuint index, const GLshort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nbv)(GLuint index, const GLbyte * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Niv)(GLuint index, const GLint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nsv)(GLuint index, const GLshort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nubv)(GLuint index, const GLubyte * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nuiv)(GLuint index, const GLuint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nusv)(GLuint index, const GLushort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4bv)(GLuint index, const GLbyte * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4dv)(GLuint index, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4fv)(GLuint index, const GLfloat * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4iv)(GLuint index, const GLint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4s)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4sv)(GLuint index, const GLshort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4ubv)(GLuint index, const GLubyte * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4uiv)(GLuint index, const GLuint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4usv)(GLuint index, const GLushort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBeginConditionalRender)(GLuint id, GLenum mode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBeginTransformFeedback)(GLenum primitiveMode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindBufferBase)(GLenum target, GLuint index, GLuint buffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindBufferRange)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindFragDataLocation)(GLuint program, GLuint color, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindFramebuffer)(GLenum target, GLuint framebuffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindRenderbuffer)(GLenum target, GLuint renderbuffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindVertexArray)(GLuint ren_array) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glCheckFramebufferStatus)(GLenum target) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClampColor)(GLenum target, GLenum clamp) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferfi)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferfv)(GLenum buffer, GLint drawbuffer, const GLfloat * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferiv)(GLenum buffer, GLint drawbuffer, const GLint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferuiv)(GLenum buffer, GLint drawbuffer, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorMaski)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteFramebuffers)(GLsizei n, const GLuint * framebuffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteRenderbuffers)(GLsizei n, const GLuint * renderbuffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteVertexArrays)(GLsizei n, const GLuint * arrays) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisablei)(GLenum target, GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnablei)(GLenum target, GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndConditionalRender)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndTransformFeedback)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFlushMappedBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture1D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture3D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureLayer)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenFramebuffers)(GLsizei n, GLuint * framebuffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenRenderbuffers)(GLsizei n, GLuint * renderbuffers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenVertexArrays)(GLsizei n, GLuint * arrays) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenerateMipmap)(GLenum target) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBooleani_v)(GLenum target, GLuint index, GLboolean * data) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetFragDataLocation)(GLuint program, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetIntegeri_v)(GLenum target, GLuint index, GLint * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint * params) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetStringi)(GLenum name, GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterIiv)(GLenum target, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterIuiv)(GLenum target, GLenum pname, GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTransformFeedbackVarying)(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformuiv)(GLuint program, GLint location, GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribIiv)(GLuint index, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribIuiv)(GLuint index, GLenum pname, GLuint * params) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabledi)(GLenum target, GLuint index) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsFramebuffer)(GLuint framebuffer) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsRenderbuffer)(GLuint renderbuffer) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsVertexArray)(GLuint ren_array) = NULL;
void * (CODEGEN_FUNCPTR *_ptrc_glMapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterIiv)(GLenum target, GLenum pname, const GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterIuiv)(GLenum target, GLenum pname, const GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTransformFeedbackVaryings)(GLuint program, GLsizei count, const GLchar *const* varyings, GLenum bufferMode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1ui)(GLint location, GLuint v0) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1uiv)(GLint location, GLsizei count, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2ui)(GLint location, GLuint v0, GLuint v1) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2uiv)(GLint location, GLsizei count, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3ui)(GLint location, GLuint v0, GLuint v1, GLuint v2) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3uiv)(GLint location, GLsizei count, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4ui)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4uiv)(GLint location, GLsizei count, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1i)(GLuint index, GLint x) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1iv)(GLuint index, const GLint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1ui)(GLuint index, GLuint x) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1uiv)(GLuint index, const GLuint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2i)(GLuint index, GLint x, GLint y) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2iv)(GLuint index, const GLint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2ui)(GLuint index, GLuint x, GLuint y) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2uiv)(GLuint index, const GLuint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3i)(GLuint index, GLint x, GLint y, GLint z) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3iv)(GLuint index, const GLint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3ui)(GLuint index, GLuint x, GLuint y, GLuint z) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3uiv)(GLuint index, const GLuint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4bv)(GLuint index, const GLbyte * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z, GLint w) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4iv)(GLuint index, const GLint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4sv)(GLuint index, const GLshort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4ubv)(GLuint index, const GLubyte * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4ui)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4uiv)(GLuint index, const GLuint * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4usv)(GLuint index, const GLushort * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribIPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void * pointer) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glCopyBufferSubData)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformBlockName)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformBlockName) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformName)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformName) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformsiv)(GLuint program, GLsizei uniformCount, const GLuint * uniformIndices, GLenum pname, GLint * params) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetUniformBlockIndex)(GLuint program, const GLchar * uniformBlockName) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformIndices)(GLuint program, GLsizei uniformCount, const GLchar *const* uniformNames, GLuint * uniformIndices) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPrimitiveRestartIndex)(GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexBuffer)(GLenum target, GLenum internalformat, GLuint buffer) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) = NULL;

GLenum (CODEGEN_FUNCPTR *_ptrc_glClientWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteSync)(GLsync sync) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLint basevertex) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsInstancedBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount, GLint basevertex) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawRangeElementsBaseVertex)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void * indices, GLint basevertex) = NULL;
GLsync (CODEGEN_FUNCPTR *_ptrc_glFenceSync)(GLenum condition, GLbitfield flags) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferParameteri64v)(GLenum target, GLenum pname, GLint64 * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetInteger64i_v)(GLenum target, GLuint index, GLint64 * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetInteger64v)(GLenum pname, GLint64 * data) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat * val) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSynciv)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsSync)(GLsync sync) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawElementsBaseVertex)(GLenum mode, const GLsizei * count, GLenum type, const void *const* indices, GLsizei drawcount, const GLint * basevertex) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProvokingVertex)(GLenum mode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSampleMaski)(GLuint maskNumber, GLbitfield mask) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBindFragDataLocationIndexed)(GLuint program, GLuint colorNumber, GLuint index, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindSampler)(GLuint unit, GLuint sampler) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteSamplers)(GLsizei count, const GLuint * samplers) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenSamplers)(GLsizei count, GLuint * samplers) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetFragDataIndex)(GLuint program, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjecti64v)(GLuint id, GLenum pname, GLint64 * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectui64v)(GLuint id, GLenum pname, GLuint64 * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterIiv)(GLuint sampler, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterIuiv)(GLuint sampler, GLenum pname, GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterfv)(GLuint sampler, GLenum pname, GLfloat * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameteriv)(GLuint sampler, GLenum pname, GLint * params) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsSampler)(GLuint sampler) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glQueryCounter)(GLuint id, GLenum target) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterIiv)(GLuint sampler, GLenum pname, const GLint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterIuiv)(GLuint sampler, GLenum pname, const GLuint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterf)(GLuint sampler, GLenum pname, GLfloat param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterfv)(GLuint sampler, GLenum pname, const GLfloat * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameteri)(GLuint sampler, GLenum pname, GLint param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameteriv)(GLuint sampler, GLenum pname, const GLint * param) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribDivisor)(GLuint index, GLuint divisor) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP1ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP1uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP2ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP2uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP3ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP3uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP4ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP4uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBeginQueryIndexed)(GLenum target, GLuint index, GLuint id) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindTransformFeedback)(GLenum target, GLuint id) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationSeparatei)(GLuint buf, GLenum modeRGB, GLenum modeAlpha) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationi)(GLuint buf, GLenum mode) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFuncSeparatei)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFunci)(GLuint buf, GLenum src, GLenum dst) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteTransformFeedbacks)(GLsizei n, const GLuint * ids) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArraysIndirect)(GLenum mode, const void * indirect) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsIndirect)(GLenum mode, GLenum type, const void * indirect) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawTransformFeedback)(GLenum mode, GLuint id) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawTransformFeedbackStream)(GLenum mode, GLuint id, GLuint stream) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndQueryIndexed)(GLenum target, GLuint index) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenTransformFeedbacks)(GLsizei n, GLuint * ids) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineUniformName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineUniformiv)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint * values) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramStageiv)(GLuint program, GLenum shadertype, GLenum pname, GLint * values) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryIndexediv)(GLenum target, GLuint index, GLenum pname, GLint * params) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetSubroutineIndex)(GLuint program, GLenum shadertype, const GLchar * name) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetSubroutineUniformLocation)(GLuint program, GLenum shadertype, const GLchar * name) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformSubroutineuiv)(GLenum shadertype, GLint location, GLuint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformdv)(GLuint program, GLint location, GLdouble * params) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTransformFeedback)(GLuint id) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMinSampleShading)(GLfloat value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPatchParameterfv)(GLenum pname, const GLfloat * values) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPatchParameteri)(GLenum pname, GLint value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPauseTransformFeedback)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glResumeTransformFeedback)(void) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1d)(GLint location, GLdouble x) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1dv)(GLint location, GLsizei count, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2d)(GLint location, GLdouble x, GLdouble y) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2dv)(GLint location, GLsizei count, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3d)(GLint location, GLdouble x, GLdouble y, GLdouble z) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3dv)(GLint location, GLsizei count, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4d)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4dv)(GLint location, GLsizei count, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformSubroutinesuiv)(GLenum shadertype, GLsizei count, const GLuint * indices) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribLdv)(GLuint index, GLenum pname, GLdouble * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL1d)(GLuint index, GLdouble x) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL1dv)(GLuint index, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL2d)(GLuint index, GLdouble x, GLdouble y) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL2dv)(GLuint index, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL3dv)(GLuint index, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL4dv)(GLuint index, const GLdouble * v) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribLPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void * pointer) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBindImageTexture)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArraysInstancedBaseInstance)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsInstancedBaseInstance)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount, GLuint baseinstance) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsInstancedBaseVertexBaseInstance)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawTransformFeedbackInstanced)(GLenum mode, GLuint id, GLsizei instancecount) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawTransformFeedbackStreamInstanced)(GLenum mode, GLuint id, GLuint stream, GLsizei instancecount) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveAtomicCounterBufferiv)(GLuint program, GLuint bufferIndex, GLenum pname, GLint * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMemoryBarrier)(GLbitfield barriers) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glDispatchCompute)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDispatchComputeIndirect)(GLintptr indirect) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawArraysIndirect)(GLenum mode, const void * indirect, GLsizei drawcount, GLsizei stride) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawElementsIndirect)(GLenum mode, GLenum type, const void * indirect, GLsizei drawcount, GLsizei stride) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glShaderStorageBlockBinding)(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding) = NULL;


void (CODEGEN_FUNCPTR *_ptrc_glGetnCompressedTexImage)(GLenum target, GLint lod, GLsizei bufSize, void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetnTexImage)(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void * pixels) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetnUniformdv)(GLuint program, GLint location, GLsizei bufSize, GLdouble * params) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMemoryBarrierByRegion)(GLbitfield barriers) = NULL;

static int Load_Version_4_5(void)
{
	int numFailed = 0;
	_ptrc_glBlendFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendFunc");
	if(!_ptrc_glBlendFunc) numFailed++;
	_ptrc_glClear = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glClear");
	if(!_ptrc_glClear) numFailed++;
	_ptrc_glClearColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glClearColor");
	if(!_ptrc_glClearColor) numFailed++;
	_ptrc_glClearDepth = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glClearDepth");
	if(!_ptrc_glClearDepth) numFailed++;
	_ptrc_glClearStencil = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glClearStencil");
	if(!_ptrc_glClearStencil) numFailed++;
	_ptrc_glColorMask = (void (CODEGEN_FUNCPTR *)(GLboolean, GLboolean, GLboolean, GLboolean))IntGetProcAddress("glColorMask");
	if(!_ptrc_glColorMask) numFailed++;
	_ptrc_glCullFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCullFace");
	if(!_ptrc_glCullFace) numFailed++;
	_ptrc_glDepthFunc = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDepthFunc");
	if(!_ptrc_glDepthFunc) numFailed++;
	_ptrc_glDepthMask = (void (CODEGEN_FUNCPTR *)(GLboolean))IntGetProcAddress("glDepthMask");
	if(!_ptrc_glDepthMask) numFailed++;
	_ptrc_glDepthRange = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glDepthRange");
	if(!_ptrc_glDepthRange) numFailed++;
	_ptrc_glDisable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDisable");
	if(!_ptrc_glDisable) numFailed++;
	_ptrc_glDrawBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDrawBuffer");
	if(!_ptrc_glDrawBuffer) numFailed++;
	_ptrc_glEnable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEnable");
	if(!_ptrc_glEnable) numFailed++;
	_ptrc_glFinish = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glFinish");
	if(!_ptrc_glFinish) numFailed++;
	_ptrc_glFlush = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glFlush");
	if(!_ptrc_glFlush) numFailed++;
	_ptrc_glFrontFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glFrontFace");
	if(!_ptrc_glFrontFace) numFailed++;
	_ptrc_glGetBooleanv = (void (CODEGEN_FUNCPTR *)(GLenum, GLboolean *))IntGetProcAddress("glGetBooleanv");
	if(!_ptrc_glGetBooleanv) numFailed++;
	_ptrc_glGetDoublev = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble *))IntGetProcAddress("glGetDoublev");
	if(!_ptrc_glGetDoublev) numFailed++;
	_ptrc_glGetError = (GLenum (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glGetError");
	if(!_ptrc_glGetError) numFailed++;
	_ptrc_glGetFloatv = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat *))IntGetProcAddress("glGetFloatv");
	if(!_ptrc_glGetFloatv) numFailed++;
	_ptrc_glGetIntegerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint *))IntGetProcAddress("glGetIntegerv");
	if(!_ptrc_glGetIntegerv) numFailed++;
	_ptrc_glGetString = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGetString");
	if(!_ptrc_glGetString) numFailed++;
	_ptrc_glGetTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLenum, void *))IntGetProcAddress("glGetTexImage");
	if(!_ptrc_glGetTexImage) numFailed++;
	_ptrc_glGetTexLevelParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLfloat *))IntGetProcAddress("glGetTexLevelParameterfv");
	if(!_ptrc_glGetTexLevelParameterfv) numFailed++;
	_ptrc_glGetTexLevelParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint *))IntGetProcAddress("glGetTexLevelParameteriv");
	if(!_ptrc_glGetTexLevelParameteriv) numFailed++;
	_ptrc_glGetTexParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetTexParameterfv");
	if(!_ptrc_glGetTexParameterfv) numFailed++;
	_ptrc_glGetTexParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexParameteriv");
	if(!_ptrc_glGetTexParameteriv) numFailed++;
	_ptrc_glHint = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glHint");
	if(!_ptrc_glHint) numFailed++;
	_ptrc_glIsEnabled = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glIsEnabled");
	if(!_ptrc_glIsEnabled) numFailed++;
	_ptrc_glLineWidth = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glLineWidth");
	if(!_ptrc_glLineWidth) numFailed++;
	_ptrc_glLogicOp = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glLogicOp");
	if(!_ptrc_glLogicOp) numFailed++;
	_ptrc_glPixelStoref = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPixelStoref");
	if(!_ptrc_glPixelStoref) numFailed++;
	_ptrc_glPixelStorei = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPixelStorei");
	if(!_ptrc_glPixelStorei) numFailed++;
	_ptrc_glPointSize = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glPointSize");
	if(!_ptrc_glPointSize) numFailed++;
	_ptrc_glPolygonMode = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glPolygonMode");
	if(!_ptrc_glPolygonMode) numFailed++;
	_ptrc_glReadBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glReadBuffer");
	if(!_ptrc_glReadBuffer) numFailed++;
	_ptrc_glReadPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *))IntGetProcAddress("glReadPixels");
	if(!_ptrc_glReadPixels) numFailed++;
	_ptrc_glScissor = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissor");
	if(!_ptrc_glScissor) numFailed++;
	_ptrc_glStencilFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLuint))IntGetProcAddress("glStencilFunc");
	if(!_ptrc_glStencilFunc) numFailed++;
	_ptrc_glStencilMask = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glStencilMask");
	if(!_ptrc_glStencilMask) numFailed++;
	_ptrc_glStencilOp = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOp");
	if(!_ptrc_glStencilOp) numFailed++;
	_ptrc_glTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const void *))IntGetProcAddress("glTexImage1D");
	if(!_ptrc_glTexImage1D) numFailed++;
	_ptrc_glTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *))IntGetProcAddress("glTexImage2D");
	if(!_ptrc_glTexImage2D) numFailed++;
	_ptrc_glTexParameterf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glTexParameterf");
	if(!_ptrc_glTexParameterf) numFailed++;
	_ptrc_glTexParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glTexParameterfv");
	if(!_ptrc_glTexParameterfv) numFailed++;
	_ptrc_glTexParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glTexParameteri");
	if(!_ptrc_glTexParameteri) numFailed++;
	_ptrc_glTexParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexParameteriv");
	if(!_ptrc_glTexParameteriv) numFailed++;
	_ptrc_glViewport = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glViewport");
	if(!_ptrc_glViewport) numFailed++;
	_ptrc_glBindTexture = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindTexture");
	if(!_ptrc_glBindTexture) numFailed++;
	_ptrc_glCopyTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))IntGetProcAddress("glCopyTexImage1D");
	if(!_ptrc_glCopyTexImage1D) numFailed++;
	_ptrc_glCopyTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))IntGetProcAddress("glCopyTexImage2D");
	if(!_ptrc_glCopyTexImage2D) numFailed++;
	_ptrc_glCopyTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei))IntGetProcAddress("glCopyTexSubImage1D");
	if(!_ptrc_glCopyTexSubImage1D) numFailed++;
	_ptrc_glCopyTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTexSubImage2D");
	if(!_ptrc_glCopyTexSubImage2D) numFailed++;
	_ptrc_glDeleteTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteTextures");
	if(!_ptrc_glDeleteTextures) numFailed++;
	_ptrc_glDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei))IntGetProcAddress("glDrawArrays");
	if(!_ptrc_glDrawArrays) numFailed++;
	_ptrc_glDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const void *))IntGetProcAddress("glDrawElements");
	if(!_ptrc_glDrawElements) numFailed++;
	_ptrc_glGenTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenTextures");
	if(!_ptrc_glGenTextures) numFailed++;
	_ptrc_glGetPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, void **))IntGetProcAddress("glGetPointerv");
	if(!_ptrc_glGetPointerv) numFailed++;
	_ptrc_glIsTexture = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsTexture");
	if(!_ptrc_glIsTexture) numFailed++;
	_ptrc_glPolygonOffset = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glPolygonOffset");
	if(!_ptrc_glPolygonOffset) numFailed++;
	_ptrc_glTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTexSubImage1D");
	if(!_ptrc_glTexSubImage1D) numFailed++;
	_ptrc_glTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTexSubImage2D");
	if(!_ptrc_glTexSubImage2D) numFailed++;
	_ptrc_glCopyTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTexSubImage3D");
	if(!_ptrc_glCopyTexSubImage3D) numFailed++;
	_ptrc_glDrawRangeElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *))IntGetProcAddress("glDrawRangeElements");
	if(!_ptrc_glDrawRangeElements) numFailed++;
	_ptrc_glTexImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *))IntGetProcAddress("glTexImage3D");
	if(!_ptrc_glTexImage3D) numFailed++;
	_ptrc_glTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTexSubImage3D");
	if(!_ptrc_glTexSubImage3D) numFailed++;
	_ptrc_glActiveTexture = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glActiveTexture");
	if(!_ptrc_glActiveTexture) numFailed++;
	_ptrc_glCompressedTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const void *))IntGetProcAddress("glCompressedTexImage1D");
	if(!_ptrc_glCompressedTexImage1D) numFailed++;
	_ptrc_glCompressedTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *))IntGetProcAddress("glCompressedTexImage2D");
	if(!_ptrc_glCompressedTexImage2D) numFailed++;
	_ptrc_glCompressedTexImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *))IntGetProcAddress("glCompressedTexImage3D");
	if(!_ptrc_glCompressedTexImage3D) numFailed++;
	_ptrc_glCompressedTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTexSubImage1D");
	if(!_ptrc_glCompressedTexSubImage1D) numFailed++;
	_ptrc_glCompressedTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTexSubImage2D");
	if(!_ptrc_glCompressedTexSubImage2D) numFailed++;
	_ptrc_glCompressedTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTexSubImage3D");
	if(!_ptrc_glCompressedTexSubImage3D) numFailed++;
	_ptrc_glGetCompressedTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, void *))IntGetProcAddress("glGetCompressedTexImage");
	if(!_ptrc_glGetCompressedTexImage) numFailed++;
	_ptrc_glSampleCoverage = (void (CODEGEN_FUNCPTR *)(GLfloat, GLboolean))IntGetProcAddress("glSampleCoverage");
	if(!_ptrc_glSampleCoverage) numFailed++;
	_ptrc_glBlendColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glBlendColor");
	if(!_ptrc_glBlendColor) numFailed++;
	_ptrc_glBlendEquation = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBlendEquation");
	if(!_ptrc_glBlendEquation) numFailed++;
	_ptrc_glBlendFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glBlendFuncSeparate");
	if(!_ptrc_glBlendFuncSeparate) numFailed++;
	_ptrc_glMultiDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *, const GLsizei *, GLsizei))IntGetProcAddress("glMultiDrawArrays");
	if(!_ptrc_glMultiDrawArrays) numFailed++;
	_ptrc_glMultiDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei))IntGetProcAddress("glMultiDrawElements");
	if(!_ptrc_glMultiDrawElements) numFailed++;
	_ptrc_glPointParameterf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPointParameterf");
	if(!_ptrc_glPointParameterf) numFailed++;
	_ptrc_glPointParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glPointParameterfv");
	if(!_ptrc_glPointParameterfv) numFailed++;
	_ptrc_glPointParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPointParameteri");
	if(!_ptrc_glPointParameteri) numFailed++;
	_ptrc_glPointParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glPointParameteriv");
	if(!_ptrc_glPointParameteriv) numFailed++;
	_ptrc_glBeginQuery = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBeginQuery");
	if(!_ptrc_glBeginQuery) numFailed++;
	_ptrc_glBindBuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindBuffer");
	if(!_ptrc_glBindBuffer) numFailed++;
	_ptrc_glBufferData = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizeiptr, const void *, GLenum))IntGetProcAddress("glBufferData");
	if(!_ptrc_glBufferData) numFailed++;
	_ptrc_glBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, const void *))IntGetProcAddress("glBufferSubData");
	if(!_ptrc_glBufferSubData) numFailed++;
	_ptrc_glDeleteBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteBuffers");
	if(!_ptrc_glDeleteBuffers) numFailed++;
	_ptrc_glDeleteQueries = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteQueries");
	if(!_ptrc_glDeleteQueries) numFailed++;
	_ptrc_glEndQuery = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEndQuery");
	if(!_ptrc_glEndQuery) numFailed++;
	_ptrc_glGenBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenBuffers");
	if(!_ptrc_glGenBuffers) numFailed++;
	_ptrc_glGenQueries = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenQueries");
	if(!_ptrc_glGenQueries) numFailed++;
	_ptrc_glGetBufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetBufferParameteriv");
	if(!_ptrc_glGetBufferParameteriv) numFailed++;
	_ptrc_glGetBufferPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, void **))IntGetProcAddress("glGetBufferPointerv");
	if(!_ptrc_glGetBufferPointerv) numFailed++;
	_ptrc_glGetBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, void *))IntGetProcAddress("glGetBufferSubData");
	if(!_ptrc_glGetBufferSubData) numFailed++;
	_ptrc_glGetQueryObjectiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetQueryObjectiv");
	if(!_ptrc_glGetQueryObjectiv) numFailed++;
	_ptrc_glGetQueryObjectuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetQueryObjectuiv");
	if(!_ptrc_glGetQueryObjectuiv) numFailed++;
	_ptrc_glGetQueryiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetQueryiv");
	if(!_ptrc_glGetQueryiv) numFailed++;
	_ptrc_glIsBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsBuffer");
	if(!_ptrc_glIsBuffer) numFailed++;
	_ptrc_glIsQuery = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsQuery");
	if(!_ptrc_glIsQuery) numFailed++;
	_ptrc_glMapBuffer = (void * (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glMapBuffer");
	if(!_ptrc_glMapBuffer) numFailed++;
	_ptrc_glUnmapBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glUnmapBuffer");
	if(!_ptrc_glUnmapBuffer) numFailed++;
	_ptrc_glAttachShader = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glAttachShader");
	if(!_ptrc_glAttachShader) numFailed++;
	_ptrc_glBindAttribLocation = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindAttribLocation");
	if(!_ptrc_glBindAttribLocation) numFailed++;
	_ptrc_glBlendEquationSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendEquationSeparate");
	if(!_ptrc_glBlendEquationSeparate) numFailed++;
	_ptrc_glCompileShader = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glCompileShader");
	if(!_ptrc_glCompileShader) numFailed++;
	_ptrc_glCreateProgram = (GLuint (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glCreateProgram");
	if(!_ptrc_glCreateProgram) numFailed++;
	_ptrc_glCreateShader = (GLuint (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCreateShader");
	if(!_ptrc_glCreateShader) numFailed++;
	_ptrc_glDeleteProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDeleteProgram");
	if(!_ptrc_glDeleteProgram) numFailed++;
	_ptrc_glDeleteShader = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDeleteShader");
	if(!_ptrc_glDeleteShader) numFailed++;
	_ptrc_glDetachShader = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glDetachShader");
	if(!_ptrc_glDetachShader) numFailed++;
	_ptrc_glDisableVertexAttribArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDisableVertexAttribArray");
	if(!_ptrc_glDisableVertexAttribArray) numFailed++;
	_ptrc_glDrawBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLenum *))IntGetProcAddress("glDrawBuffers");
	if(!_ptrc_glDrawBuffers) numFailed++;
	_ptrc_glEnableVertexAttribArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glEnableVertexAttribArray");
	if(!_ptrc_glEnableVertexAttribArray) numFailed++;
	_ptrc_glGetActiveAttrib = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))IntGetProcAddress("glGetActiveAttrib");
	if(!_ptrc_glGetActiveAttrib) numFailed++;
	_ptrc_glGetActiveUniform = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))IntGetProcAddress("glGetActiveUniform");
	if(!_ptrc_glGetActiveUniform) numFailed++;
	_ptrc_glGetAttachedShaders = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLuint *))IntGetProcAddress("glGetAttachedShaders");
	if(!_ptrc_glGetAttachedShaders) numFailed++;
	_ptrc_glGetAttribLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetAttribLocation");
	if(!_ptrc_glGetAttribLocation) numFailed++;
	_ptrc_glGetProgramInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramInfoLog");
	if(!_ptrc_glGetProgramInfoLog) numFailed++;
	_ptrc_glGetProgramiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetProgramiv");
	if(!_ptrc_glGetProgramiv) numFailed++;
	_ptrc_glGetShaderInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetShaderInfoLog");
	if(!_ptrc_glGetShaderInfoLog) numFailed++;
	_ptrc_glGetShaderSource = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetShaderSource");
	if(!_ptrc_glGetShaderSource) numFailed++;
	_ptrc_glGetShaderiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetShaderiv");
	if(!_ptrc_glGetShaderiv) numFailed++;
	_ptrc_glGetUniformLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetUniformLocation");
	if(!_ptrc_glGetUniformLocation) numFailed++;
	_ptrc_glGetUniformfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat *))IntGetProcAddress("glGetUniformfv");
	if(!_ptrc_glGetUniformfv) numFailed++;
	_ptrc_glGetUniformiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint *))IntGetProcAddress("glGetUniformiv");
	if(!_ptrc_glGetUniformiv) numFailed++;
	_ptrc_glGetVertexAttribPointerv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, void **))IntGetProcAddress("glGetVertexAttribPointerv");
	if(!_ptrc_glGetVertexAttribPointerv) numFailed++;
	_ptrc_glGetVertexAttribdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLdouble *))IntGetProcAddress("glGetVertexAttribdv");
	if(!_ptrc_glGetVertexAttribdv) numFailed++;
	_ptrc_glGetVertexAttribfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat *))IntGetProcAddress("glGetVertexAttribfv");
	if(!_ptrc_glGetVertexAttribfv) numFailed++;
	_ptrc_glGetVertexAttribiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexAttribiv");
	if(!_ptrc_glGetVertexAttribiv) numFailed++;
	_ptrc_glIsProgram = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsProgram");
	if(!_ptrc_glIsProgram) numFailed++;
	_ptrc_glIsShader = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsShader");
	if(!_ptrc_glIsShader) numFailed++;
	_ptrc_glLinkProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glLinkProgram");
	if(!_ptrc_glLinkProgram) numFailed++;
	_ptrc_glShaderSource = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, const GLint *))IntGetProcAddress("glShaderSource");
	if(!_ptrc_glShaderSource) numFailed++;
	_ptrc_glStencilFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint, GLuint))IntGetProcAddress("glStencilFuncSeparate");
	if(!_ptrc_glStencilFuncSeparate) numFailed++;
	_ptrc_glStencilMaskSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glStencilMaskSeparate");
	if(!_ptrc_glStencilMaskSeparate) numFailed++;
	_ptrc_glStencilOpSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOpSeparate");
	if(!_ptrc_glStencilOpSeparate) numFailed++;
	_ptrc_glUniform1f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat))IntGetProcAddress("glUniform1f");
	if(!_ptrc_glUniform1f) numFailed++;
	_ptrc_glUniform1fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform1fv");
	if(!_ptrc_glUniform1fv) numFailed++;
	_ptrc_glUniform1i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glUniform1i");
	if(!_ptrc_glUniform1i) numFailed++;
	_ptrc_glUniform1iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform1iv");
	if(!_ptrc_glUniform1iv) numFailed++;
	_ptrc_glUniform2f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat))IntGetProcAddress("glUniform2f");
	if(!_ptrc_glUniform2f) numFailed++;
	_ptrc_glUniform2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform2fv");
	if(!_ptrc_glUniform2fv) numFailed++;
	_ptrc_glUniform2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glUniform2i");
	if(!_ptrc_glUniform2i) numFailed++;
	_ptrc_glUniform2iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform2iv");
	if(!_ptrc_glUniform2iv) numFailed++;
	_ptrc_glUniform3f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform3f");
	if(!_ptrc_glUniform3f) numFailed++;
	_ptrc_glUniform3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform3fv");
	if(!_ptrc_glUniform3fv) numFailed++;
	_ptrc_glUniform3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform3i");
	if(!_ptrc_glUniform3i) numFailed++;
	_ptrc_glUniform3iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform3iv");
	if(!_ptrc_glUniform3iv) numFailed++;
	_ptrc_glUniform4f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform4f");
	if(!_ptrc_glUniform4f) numFailed++;
	_ptrc_glUniform4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform4fv");
	if(!_ptrc_glUniform4fv) numFailed++;
	_ptrc_glUniform4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform4i");
	if(!_ptrc_glUniform4i) numFailed++;
	_ptrc_glUniform4iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform4iv");
	if(!_ptrc_glUniform4iv) numFailed++;
	_ptrc_glUniformMatrix2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2fv");
	if(!_ptrc_glUniformMatrix2fv) numFailed++;
	_ptrc_glUniformMatrix3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3fv");
	if(!_ptrc_glUniformMatrix3fv) numFailed++;
	_ptrc_glUniformMatrix4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4fv");
	if(!_ptrc_glUniformMatrix4fv) numFailed++;
	_ptrc_glUseProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glUseProgram");
	if(!_ptrc_glUseProgram) numFailed++;
	_ptrc_glValidateProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glValidateProgram");
	if(!_ptrc_glValidateProgram) numFailed++;
	_ptrc_glVertexAttrib1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble))IntGetProcAddress("glVertexAttrib1d");
	if(!_ptrc_glVertexAttrib1d) numFailed++;
	_ptrc_glVertexAttrib1dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib1dv");
	if(!_ptrc_glVertexAttrib1dv) numFailed++;
	_ptrc_glVertexAttrib1f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat))IntGetProcAddress("glVertexAttrib1f");
	if(!_ptrc_glVertexAttrib1f) numFailed++;
	_ptrc_glVertexAttrib1fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib1fv");
	if(!_ptrc_glVertexAttrib1fv) numFailed++;
	_ptrc_glVertexAttrib1s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort))IntGetProcAddress("glVertexAttrib1s");
	if(!_ptrc_glVertexAttrib1s) numFailed++;
	_ptrc_glVertexAttrib1sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib1sv");
	if(!_ptrc_glVertexAttrib1sv) numFailed++;
	_ptrc_glVertexAttrib2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib2d");
	if(!_ptrc_glVertexAttrib2d) numFailed++;
	_ptrc_glVertexAttrib2dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib2dv");
	if(!_ptrc_glVertexAttrib2dv) numFailed++;
	_ptrc_glVertexAttrib2f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib2f");
	if(!_ptrc_glVertexAttrib2f) numFailed++;
	_ptrc_glVertexAttrib2fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib2fv");
	if(!_ptrc_glVertexAttrib2fv) numFailed++;
	_ptrc_glVertexAttrib2s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort))IntGetProcAddress("glVertexAttrib2s");
	if(!_ptrc_glVertexAttrib2s) numFailed++;
	_ptrc_glVertexAttrib2sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib2sv");
	if(!_ptrc_glVertexAttrib2sv) numFailed++;
	_ptrc_glVertexAttrib3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib3d");
	if(!_ptrc_glVertexAttrib3d) numFailed++;
	_ptrc_glVertexAttrib3dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib3dv");
	if(!_ptrc_glVertexAttrib3dv) numFailed++;
	_ptrc_glVertexAttrib3f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib3f");
	if(!_ptrc_glVertexAttrib3f) numFailed++;
	_ptrc_glVertexAttrib3fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib3fv");
	if(!_ptrc_glVertexAttrib3fv) numFailed++;
	_ptrc_glVertexAttrib3s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort, GLshort))IntGetProcAddress("glVertexAttrib3s");
	if(!_ptrc_glVertexAttrib3s) numFailed++;
	_ptrc_glVertexAttrib3sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib3sv");
	if(!_ptrc_glVertexAttrib3sv) numFailed++;
	_ptrc_glVertexAttrib4Nbv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttrib4Nbv");
	if(!_ptrc_glVertexAttrib4Nbv) numFailed++;
	_ptrc_glVertexAttrib4Niv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttrib4Niv");
	if(!_ptrc_glVertexAttrib4Niv) numFailed++;
	_ptrc_glVertexAttrib4Nsv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib4Nsv");
	if(!_ptrc_glVertexAttrib4Nsv) numFailed++;
	_ptrc_glVertexAttrib4Nub = (void (CODEGEN_FUNCPTR *)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte))IntGetProcAddress("glVertexAttrib4Nub");
	if(!_ptrc_glVertexAttrib4Nub) numFailed++;
	_ptrc_glVertexAttrib4Nubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttrib4Nubv");
	if(!_ptrc_glVertexAttrib4Nubv) numFailed++;
	_ptrc_glVertexAttrib4Nuiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttrib4Nuiv");
	if(!_ptrc_glVertexAttrib4Nuiv) numFailed++;
	_ptrc_glVertexAttrib4Nusv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttrib4Nusv");
	if(!_ptrc_glVertexAttrib4Nusv) numFailed++;
	_ptrc_glVertexAttrib4bv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttrib4bv");
	if(!_ptrc_glVertexAttrib4bv) numFailed++;
	_ptrc_glVertexAttrib4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib4d");
	if(!_ptrc_glVertexAttrib4d) numFailed++;
	_ptrc_glVertexAttrib4dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib4dv");
	if(!_ptrc_glVertexAttrib4dv) numFailed++;
	_ptrc_glVertexAttrib4f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib4f");
	if(!_ptrc_glVertexAttrib4f) numFailed++;
	_ptrc_glVertexAttrib4fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib4fv");
	if(!_ptrc_glVertexAttrib4fv) numFailed++;
	_ptrc_glVertexAttrib4iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttrib4iv");
	if(!_ptrc_glVertexAttrib4iv) numFailed++;
	_ptrc_glVertexAttrib4s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glVertexAttrib4s");
	if(!_ptrc_glVertexAttrib4s) numFailed++;
	_ptrc_glVertexAttrib4sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib4sv");
	if(!_ptrc_glVertexAttrib4sv) numFailed++;
	_ptrc_glVertexAttrib4ubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttrib4ubv");
	if(!_ptrc_glVertexAttrib4ubv) numFailed++;
	_ptrc_glVertexAttrib4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttrib4uiv");
	if(!_ptrc_glVertexAttrib4uiv) numFailed++;
	_ptrc_glVertexAttrib4usv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttrib4usv");
	if(!_ptrc_glVertexAttrib4usv) numFailed++;
	_ptrc_glVertexAttribPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *))IntGetProcAddress("glVertexAttribPointer");
	if(!_ptrc_glVertexAttribPointer) numFailed++;
	_ptrc_glUniformMatrix2x3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2x3fv");
	if(!_ptrc_glUniformMatrix2x3fv) numFailed++;
	_ptrc_glUniformMatrix2x4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2x4fv");
	if(!_ptrc_glUniformMatrix2x4fv) numFailed++;
	_ptrc_glUniformMatrix3x2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3x2fv");
	if(!_ptrc_glUniformMatrix3x2fv) numFailed++;
	_ptrc_glUniformMatrix3x4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3x4fv");
	if(!_ptrc_glUniformMatrix3x4fv) numFailed++;
	_ptrc_glUniformMatrix4x2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4x2fv");
	if(!_ptrc_glUniformMatrix4x2fv) numFailed++;
	_ptrc_glUniformMatrix4x3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4x3fv");
	if(!_ptrc_glUniformMatrix4x3fv) numFailed++;
	_ptrc_glBeginConditionalRender = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glBeginConditionalRender");
	if(!_ptrc_glBeginConditionalRender) numFailed++;
	_ptrc_glBeginTransformFeedback = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBeginTransformFeedback");
	if(!_ptrc_glBeginTransformFeedback) numFailed++;
	_ptrc_glBindBufferBase = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint))IntGetProcAddress("glBindBufferBase");
	if(!_ptrc_glBindBufferBase) numFailed++;
	_ptrc_glBindBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glBindBufferRange");
	if(!_ptrc_glBindBufferRange) numFailed++;
	_ptrc_glBindFragDataLocation = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindFragDataLocation");
	if(!_ptrc_glBindFragDataLocation) numFailed++;
	_ptrc_glBindFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindFramebuffer");
	if(!_ptrc_glBindFramebuffer) numFailed++;
	_ptrc_glBindRenderbuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindRenderbuffer");
	if(!_ptrc_glBindRenderbuffer) numFailed++;
	_ptrc_glBindVertexArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glBindVertexArray");
	if(!_ptrc_glBindVertexArray) numFailed++;
	_ptrc_glBlitFramebuffer = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum))IntGetProcAddress("glBlitFramebuffer");
	if(!_ptrc_glBlitFramebuffer) numFailed++;
	_ptrc_glCheckFramebufferStatus = (GLenum (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCheckFramebufferStatus");
	if(!_ptrc_glCheckFramebufferStatus) numFailed++;
	_ptrc_glClampColor = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glClampColor");
	if(!_ptrc_glClampColor) numFailed++;
	_ptrc_glClearBufferfi = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLfloat, GLint))IntGetProcAddress("glClearBufferfi");
	if(!_ptrc_glClearBufferfi) numFailed++;
	_ptrc_glClearBufferfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLfloat *))IntGetProcAddress("glClearBufferfv");
	if(!_ptrc_glClearBufferfv) numFailed++;
	_ptrc_glClearBufferiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLint *))IntGetProcAddress("glClearBufferiv");
	if(!_ptrc_glClearBufferiv) numFailed++;
	_ptrc_glClearBufferuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLuint *))IntGetProcAddress("glClearBufferuiv");
	if(!_ptrc_glClearBufferuiv) numFailed++;
	_ptrc_glColorMaski = (void (CODEGEN_FUNCPTR *)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean))IntGetProcAddress("glColorMaski");
	if(!_ptrc_glColorMaski) numFailed++;
	_ptrc_glDeleteFramebuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteFramebuffers");
	if(!_ptrc_glDeleteFramebuffers) numFailed++;
	_ptrc_glDeleteRenderbuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteRenderbuffers");
	if(!_ptrc_glDeleteRenderbuffers) numFailed++;
	_ptrc_glDeleteVertexArrays = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteVertexArrays");
	if(!_ptrc_glDeleteVertexArrays) numFailed++;
	_ptrc_glDisablei = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glDisablei");
	if(!_ptrc_glDisablei) numFailed++;
	_ptrc_glEnablei = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glEnablei");
	if(!_ptrc_glEnablei) numFailed++;
	_ptrc_glEndConditionalRender = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glEndConditionalRender");
	if(!_ptrc_glEndConditionalRender) numFailed++;
	_ptrc_glEndTransformFeedback = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glEndTransformFeedback");
	if(!_ptrc_glEndTransformFeedback) numFailed++;
	_ptrc_glFlushMappedBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr))IntGetProcAddress("glFlushMappedBufferRange");
	if(!_ptrc_glFlushMappedBufferRange) numFailed++;
	_ptrc_glFramebufferRenderbuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint))IntGetProcAddress("glFramebufferRenderbuffer");
	if(!_ptrc_glFramebufferRenderbuffer) numFailed++;
	_ptrc_glFramebufferTexture1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture1D");
	if(!_ptrc_glFramebufferTexture1D) numFailed++;
	_ptrc_glFramebufferTexture2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture2D");
	if(!_ptrc_glFramebufferTexture2D) numFailed++;
	_ptrc_glFramebufferTexture3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTexture3D");
	if(!_ptrc_glFramebufferTexture3D) numFailed++;
	_ptrc_glFramebufferTextureLayer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTextureLayer");
	if(!_ptrc_glFramebufferTextureLayer) numFailed++;
	_ptrc_glGenFramebuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenFramebuffers");
	if(!_ptrc_glGenFramebuffers) numFailed++;
	_ptrc_glGenRenderbuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenRenderbuffers");
	if(!_ptrc_glGenRenderbuffers) numFailed++;
	_ptrc_glGenVertexArrays = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenVertexArrays");
	if(!_ptrc_glGenVertexArrays) numFailed++;
	_ptrc_glGenerateMipmap = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGenerateMipmap");
	if(!_ptrc_glGenerateMipmap) numFailed++;
	_ptrc_glGetBooleani_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLboolean *))IntGetProcAddress("glGetBooleani_v");
	if(!_ptrc_glGetBooleani_v) numFailed++;
	_ptrc_glGetFragDataLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetFragDataLocation");
	if(!_ptrc_glGetFragDataLocation) numFailed++;
	_ptrc_glGetFramebufferAttachmentParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLint *))IntGetProcAddress("glGetFramebufferAttachmentParameteriv");
	if(!_ptrc_glGetFramebufferAttachmentParameteriv) numFailed++;
	_ptrc_glGetIntegeri_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLint *))IntGetProcAddress("glGetIntegeri_v");
	if(!_ptrc_glGetIntegeri_v) numFailed++;
	_ptrc_glGetRenderbufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetRenderbufferParameteriv");
	if(!_ptrc_glGetRenderbufferParameteriv) numFailed++;
	_ptrc_glGetStringi = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glGetStringi");
	if(!_ptrc_glGetStringi) numFailed++;
	_ptrc_glGetTexParameterIiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexParameterIiv");
	if(!_ptrc_glGetTexParameterIiv) numFailed++;
	_ptrc_glGetTexParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint *))IntGetProcAddress("glGetTexParameterIuiv");
	if(!_ptrc_glGetTexParameterIuiv) numFailed++;
	_ptrc_glGetTransformFeedbackVarying = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *))IntGetProcAddress("glGetTransformFeedbackVarying");
	if(!_ptrc_glGetTransformFeedbackVarying) numFailed++;
	_ptrc_glGetUniformuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint *))IntGetProcAddress("glGetUniformuiv");
	if(!_ptrc_glGetUniformuiv) numFailed++;
	_ptrc_glGetVertexAttribIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexAttribIiv");
	if(!_ptrc_glGetVertexAttribIiv) numFailed++;
	_ptrc_glGetVertexAttribIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetVertexAttribIuiv");
	if(!_ptrc_glGetVertexAttribIuiv) numFailed++;
	_ptrc_glIsEnabledi = (GLboolean (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glIsEnabledi");
	if(!_ptrc_glIsEnabledi) numFailed++;
	_ptrc_glIsFramebuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsFramebuffer");
	if(!_ptrc_glIsFramebuffer) numFailed++;
	_ptrc_glIsRenderbuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsRenderbuffer");
	if(!_ptrc_glIsRenderbuffer) numFailed++;
	_ptrc_glIsVertexArray = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsVertexArray");
	if(!_ptrc_glIsVertexArray) numFailed++;
	_ptrc_glMapBufferRange = (void * (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, GLbitfield))IntGetProcAddress("glMapBufferRange");
	if(!_ptrc_glMapBufferRange) numFailed++;
	_ptrc_glRenderbufferStorage = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLsizei, GLsizei))IntGetProcAddress("glRenderbufferStorage");
	if(!_ptrc_glRenderbufferStorage) numFailed++;
	_ptrc_glRenderbufferStorageMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glRenderbufferStorageMultisample");
	if(!_ptrc_glRenderbufferStorageMultisample) numFailed++;
	_ptrc_glTexParameterIiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexParameterIiv");
	if(!_ptrc_glTexParameterIiv) numFailed++;
	_ptrc_glTexParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLuint *))IntGetProcAddress("glTexParameterIuiv");
	if(!_ptrc_glTexParameterIuiv) numFailed++;
	_ptrc_glTransformFeedbackVaryings = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, GLenum))IntGetProcAddress("glTransformFeedbackVaryings");
	if(!_ptrc_glTransformFeedbackVaryings) numFailed++;
	_ptrc_glUniform1ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint))IntGetProcAddress("glUniform1ui");
	if(!_ptrc_glUniform1ui) numFailed++;
	_ptrc_glUniform1uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform1uiv");
	if(!_ptrc_glUniform1uiv) numFailed++;
	_ptrc_glUniform2ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint))IntGetProcAddress("glUniform2ui");
	if(!_ptrc_glUniform2ui) numFailed++;
	_ptrc_glUniform2uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform2uiv");
	if(!_ptrc_glUniform2uiv) numFailed++;
	_ptrc_glUniform3ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint, GLuint))IntGetProcAddress("glUniform3ui");
	if(!_ptrc_glUniform3ui) numFailed++;
	_ptrc_glUniform3uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform3uiv");
	if(!_ptrc_glUniform3uiv) numFailed++;
	_ptrc_glUniform4ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glUniform4ui");
	if(!_ptrc_glUniform4ui) numFailed++;
	_ptrc_glUniform4uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform4uiv");
	if(!_ptrc_glUniform4uiv) numFailed++;
	_ptrc_glVertexAttribI1i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint))IntGetProcAddress("glVertexAttribI1i");
	if(!_ptrc_glVertexAttribI1i) numFailed++;
	_ptrc_glVertexAttribI1iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI1iv");
	if(!_ptrc_glVertexAttribI1iv) numFailed++;
	_ptrc_glVertexAttribI1ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexAttribI1ui");
	if(!_ptrc_glVertexAttribI1ui) numFailed++;
	_ptrc_glVertexAttribI1uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI1uiv");
	if(!_ptrc_glVertexAttribI1uiv) numFailed++;
	_ptrc_glVertexAttribI2i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint))IntGetProcAddress("glVertexAttribI2i");
	if(!_ptrc_glVertexAttribI2i) numFailed++;
	_ptrc_glVertexAttribI2iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI2iv");
	if(!_ptrc_glVertexAttribI2iv) numFailed++;
	_ptrc_glVertexAttribI2ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI2ui");
	if(!_ptrc_glVertexAttribI2ui) numFailed++;
	_ptrc_glVertexAttribI2uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI2uiv");
	if(!_ptrc_glVertexAttribI2uiv) numFailed++;
	_ptrc_glVertexAttribI3i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint))IntGetProcAddress("glVertexAttribI3i");
	if(!_ptrc_glVertexAttribI3i) numFailed++;
	_ptrc_glVertexAttribI3iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI3iv");
	if(!_ptrc_glVertexAttribI3iv) numFailed++;
	_ptrc_glVertexAttribI3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI3ui");
	if(!_ptrc_glVertexAttribI3ui) numFailed++;
	_ptrc_glVertexAttribI3uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI3uiv");
	if(!_ptrc_glVertexAttribI3uiv) numFailed++;
	_ptrc_glVertexAttribI4bv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttribI4bv");
	if(!_ptrc_glVertexAttribI4bv) numFailed++;
	_ptrc_glVertexAttribI4i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint))IntGetProcAddress("glVertexAttribI4i");
	if(!_ptrc_glVertexAttribI4i) numFailed++;
	_ptrc_glVertexAttribI4iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI4iv");
	if(!_ptrc_glVertexAttribI4iv) numFailed++;
	_ptrc_glVertexAttribI4sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttribI4sv");
	if(!_ptrc_glVertexAttribI4sv) numFailed++;
	_ptrc_glVertexAttribI4ubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttribI4ubv");
	if(!_ptrc_glVertexAttribI4ubv) numFailed++;
	_ptrc_glVertexAttribI4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI4ui");
	if(!_ptrc_glVertexAttribI4ui) numFailed++;
	_ptrc_glVertexAttribI4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI4uiv");
	if(!_ptrc_glVertexAttribI4uiv) numFailed++;
	_ptrc_glVertexAttribI4usv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttribI4usv");
	if(!_ptrc_glVertexAttribI4usv) numFailed++;
	_ptrc_glVertexAttribIPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLsizei, const void *))IntGetProcAddress("glVertexAttribIPointer");
	if(!_ptrc_glVertexAttribIPointer) numFailed++;
	_ptrc_glCopyBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr))IntGetProcAddress("glCopyBufferSubData");
	if(!_ptrc_glCopyBufferSubData) numFailed++;
	_ptrc_glDrawArraysInstanced = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei, GLsizei))IntGetProcAddress("glDrawArraysInstanced");
	if(!_ptrc_glDrawArraysInstanced) numFailed++;
	_ptrc_glDrawElementsInstanced = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const void *, GLsizei))IntGetProcAddress("glDrawElementsInstanced");
	if(!_ptrc_glDrawElementsInstanced) numFailed++;
	_ptrc_glGetActiveUniformBlockName = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveUniformBlockName");
	if(!_ptrc_glGetActiveUniformBlockName) numFailed++;
	_ptrc_glGetActiveUniformBlockiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLint *))IntGetProcAddress("glGetActiveUniformBlockiv");
	if(!_ptrc_glGetActiveUniformBlockiv) numFailed++;
	_ptrc_glGetActiveUniformName = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveUniformName");
	if(!_ptrc_glGetActiveUniformName) numFailed++;
	_ptrc_glGetActiveUniformsiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *, GLenum, GLint *))IntGetProcAddress("glGetActiveUniformsiv");
	if(!_ptrc_glGetActiveUniformsiv) numFailed++;
	_ptrc_glGetUniformBlockIndex = (GLuint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetUniformBlockIndex");
	if(!_ptrc_glGetUniformBlockIndex) numFailed++;
	_ptrc_glGetUniformIndices = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, GLuint *))IntGetProcAddress("glGetUniformIndices");
	if(!_ptrc_glGetUniformIndices) numFailed++;
	_ptrc_glPrimitiveRestartIndex = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glPrimitiveRestartIndex");
	if(!_ptrc_glPrimitiveRestartIndex) numFailed++;
	_ptrc_glTexBuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint))IntGetProcAddress("glTexBuffer");
	if(!_ptrc_glTexBuffer) numFailed++;
	_ptrc_glUniformBlockBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glUniformBlockBinding");
	if(!_ptrc_glUniformBlockBinding) numFailed++;
	_ptrc_glClientWaitSync = (GLenum (CODEGEN_FUNCPTR *)(GLsync, GLbitfield, GLuint64))IntGetProcAddress("glClientWaitSync");
	if(!_ptrc_glClientWaitSync) numFailed++;
	_ptrc_glDeleteSync = (void (CODEGEN_FUNCPTR *)(GLsync))IntGetProcAddress("glDeleteSync");
	if(!_ptrc_glDeleteSync) numFailed++;
	_ptrc_glDrawElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const void *, GLint))IntGetProcAddress("glDrawElementsBaseVertex");
	if(!_ptrc_glDrawElementsBaseVertex) numFailed++;
	_ptrc_glDrawElementsInstancedBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint))IntGetProcAddress("glDrawElementsInstancedBaseVertex");
	if(!_ptrc_glDrawElementsInstancedBaseVertex) numFailed++;
	_ptrc_glDrawRangeElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *, GLint))IntGetProcAddress("glDrawRangeElementsBaseVertex");
	if(!_ptrc_glDrawRangeElementsBaseVertex) numFailed++;
	_ptrc_glFenceSync = (GLsync (CODEGEN_FUNCPTR *)(GLenum, GLbitfield))IntGetProcAddress("glFenceSync");
	if(!_ptrc_glFenceSync) numFailed++;
	_ptrc_glFramebufferTexture = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture");
	if(!_ptrc_glFramebufferTexture) numFailed++;
	_ptrc_glGetBufferParameteri64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint64 *))IntGetProcAddress("glGetBufferParameteri64v");
	if(!_ptrc_glGetBufferParameteri64v) numFailed++;
	_ptrc_glGetInteger64i_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLint64 *))IntGetProcAddress("glGetInteger64i_v");
	if(!_ptrc_glGetInteger64i_v) numFailed++;
	_ptrc_glGetInteger64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLint64 *))IntGetProcAddress("glGetInteger64v");
	if(!_ptrc_glGetInteger64v) numFailed++;
	_ptrc_glGetMultisamplefv = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLfloat *))IntGetProcAddress("glGetMultisamplefv");
	if(!_ptrc_glGetMultisamplefv) numFailed++;
	_ptrc_glGetSynciv = (void (CODEGEN_FUNCPTR *)(GLsync, GLenum, GLsizei, GLsizei *, GLint *))IntGetProcAddress("glGetSynciv");
	if(!_ptrc_glGetSynciv) numFailed++;
	_ptrc_glIsSync = (GLboolean (CODEGEN_FUNCPTR *)(GLsync))IntGetProcAddress("glIsSync");
	if(!_ptrc_glIsSync) numFailed++;
	_ptrc_glMultiDrawElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei, const GLint *))IntGetProcAddress("glMultiDrawElementsBaseVertex");
	if(!_ptrc_glMultiDrawElementsBaseVertex) numFailed++;
	_ptrc_glProvokingVertex = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glProvokingVertex");
	if(!_ptrc_glProvokingVertex) numFailed++;
	_ptrc_glSampleMaski = (void (CODEGEN_FUNCPTR *)(GLuint, GLbitfield))IntGetProcAddress("glSampleMaski");
	if(!_ptrc_glSampleMaski) numFailed++;
	_ptrc_glTexImage2DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexImage2DMultisample");
	if(!_ptrc_glTexImage2DMultisample) numFailed++;
	_ptrc_glTexImage3DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexImage3DMultisample");
	if(!_ptrc_glTexImage3DMultisample) numFailed++;
	_ptrc_glWaitSync = (void (CODEGEN_FUNCPTR *)(GLsync, GLbitfield, GLuint64))IntGetProcAddress("glWaitSync");
	if(!_ptrc_glWaitSync) numFailed++;
	_ptrc_glBindFragDataLocationIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindFragDataLocationIndexed");
	if(!_ptrc_glBindFragDataLocationIndexed) numFailed++;
	_ptrc_glBindSampler = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glBindSampler");
	if(!_ptrc_glBindSampler) numFailed++;
	_ptrc_glDeleteSamplers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteSamplers");
	if(!_ptrc_glDeleteSamplers) numFailed++;
	_ptrc_glGenSamplers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenSamplers");
	if(!_ptrc_glGenSamplers) numFailed++;
	_ptrc_glGetFragDataIndex = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetFragDataIndex");
	if(!_ptrc_glGetFragDataIndex) numFailed++;
	_ptrc_glGetQueryObjecti64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint64 *))IntGetProcAddress("glGetQueryObjecti64v");
	if(!_ptrc_glGetQueryObjecti64v) numFailed++;
	_ptrc_glGetQueryObjectui64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint64 *))IntGetProcAddress("glGetQueryObjectui64v");
	if(!_ptrc_glGetQueryObjectui64v) numFailed++;
	_ptrc_glGetSamplerParameterIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetSamplerParameterIiv");
	if(!_ptrc_glGetSamplerParameterIiv) numFailed++;
	_ptrc_glGetSamplerParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetSamplerParameterIuiv");
	if(!_ptrc_glGetSamplerParameterIuiv) numFailed++;
	_ptrc_glGetSamplerParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat *))IntGetProcAddress("glGetSamplerParameterfv");
	if(!_ptrc_glGetSamplerParameterfv) numFailed++;
	_ptrc_glGetSamplerParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetSamplerParameteriv");
	if(!_ptrc_glGetSamplerParameteriv) numFailed++;
	_ptrc_glIsSampler = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsSampler");
	if(!_ptrc_glIsSampler) numFailed++;
	_ptrc_glQueryCounter = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glQueryCounter");
	if(!_ptrc_glQueryCounter) numFailed++;
	_ptrc_glSamplerParameterIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLint *))IntGetProcAddress("glSamplerParameterIiv");
	if(!_ptrc_glSamplerParameterIiv) numFailed++;
	_ptrc_glSamplerParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLuint *))IntGetProcAddress("glSamplerParameterIuiv");
	if(!_ptrc_glSamplerParameterIuiv) numFailed++;
	_ptrc_glSamplerParameterf = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat))IntGetProcAddress("glSamplerParameterf");
	if(!_ptrc_glSamplerParameterf) numFailed++;
	_ptrc_glSamplerParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLfloat *))IntGetProcAddress("glSamplerParameterfv");
	if(!_ptrc_glSamplerParameterfv) numFailed++;
	_ptrc_glSamplerParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glSamplerParameteri");
	if(!_ptrc_glSamplerParameteri) numFailed++;
	_ptrc_glSamplerParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLint *))IntGetProcAddress("glSamplerParameteriv");
	if(!_ptrc_glSamplerParameteriv) numFailed++;
	_ptrc_glVertexAttribDivisor = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexAttribDivisor");
	if(!_ptrc_glVertexAttribDivisor) numFailed++;
	_ptrc_glVertexAttribP1ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribP1ui");
	if(!_ptrc_glVertexAttribP1ui) numFailed++;
	_ptrc_glVertexAttribP1uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, const GLuint *))IntGetProcAddress("glVertexAttribP1uiv");
	if(!_ptrc_glVertexAttribP1uiv) numFailed++;
	_ptrc_glVertexAttribP2ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribP2ui");
	if(!_ptrc_glVertexAttribP2ui) numFailed++;
	_ptrc_glVertexAttribP2uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, const GLuint *))IntGetProcAddress("glVertexAttribP2uiv");
	if(!_ptrc_glVertexAttribP2uiv) numFailed++;
	_ptrc_glVertexAttribP3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribP3ui");
	if(!_ptrc_glVertexAttribP3ui) numFailed++;
	_ptrc_glVertexAttribP3uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, const GLuint *))IntGetProcAddress("glVertexAttribP3uiv");
	if(!_ptrc_glVertexAttribP3uiv) numFailed++;
	_ptrc_glVertexAttribP4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribP4ui");
	if(!_ptrc_glVertexAttribP4ui) numFailed++;
	_ptrc_glVertexAttribP4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, const GLuint *))IntGetProcAddress("glVertexAttribP4uiv");
	if(!_ptrc_glVertexAttribP4uiv) numFailed++;
	_ptrc_glBeginQueryIndexed = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint))IntGetProcAddress("glBeginQueryIndexed");
	if(!_ptrc_glBeginQueryIndexed) numFailed++;
	_ptrc_glBindTransformFeedback = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindTransformFeedback");
	if(!_ptrc_glBindTransformFeedback) numFailed++;
	_ptrc_glBlendEquationSeparatei = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum))IntGetProcAddress("glBlendEquationSeparatei");
	if(!_ptrc_glBlendEquationSeparatei) numFailed++;
	_ptrc_glBlendEquationi = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glBlendEquationi");
	if(!_ptrc_glBlendEquationi) numFailed++;
	_ptrc_glBlendFuncSeparatei = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glBlendFuncSeparatei");
	if(!_ptrc_glBlendFuncSeparatei) numFailed++;
	_ptrc_glBlendFunci = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum))IntGetProcAddress("glBlendFunci");
	if(!_ptrc_glBlendFunci) numFailed++;
	_ptrc_glDeleteTransformFeedbacks = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteTransformFeedbacks");
	if(!_ptrc_glDeleteTransformFeedbacks) numFailed++;
	_ptrc_glDrawArraysIndirect = (void (CODEGEN_FUNCPTR *)(GLenum, const void *))IntGetProcAddress("glDrawArraysIndirect");
	if(!_ptrc_glDrawArraysIndirect) numFailed++;
	_ptrc_glDrawElementsIndirect = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const void *))IntGetProcAddress("glDrawElementsIndirect");
	if(!_ptrc_glDrawElementsIndirect) numFailed++;
	_ptrc_glDrawTransformFeedback = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glDrawTransformFeedback");
	if(!_ptrc_glDrawTransformFeedback) numFailed++;
	_ptrc_glDrawTransformFeedbackStream = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint))IntGetProcAddress("glDrawTransformFeedbackStream");
	if(!_ptrc_glDrawTransformFeedbackStream) numFailed++;
	_ptrc_glEndQueryIndexed = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glEndQueryIndexed");
	if(!_ptrc_glEndQueryIndexed) numFailed++;
	_ptrc_glGenTransformFeedbacks = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenTransformFeedbacks");
	if(!_ptrc_glGenTransformFeedbacks) numFailed++;
	_ptrc_glGetActiveSubroutineName = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveSubroutineName");
	if(!_ptrc_glGetActiveSubroutineName) numFailed++;
	_ptrc_glGetActiveSubroutineUniformName = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveSubroutineUniformName");
	if(!_ptrc_glGetActiveSubroutineUniformName) numFailed++;
	_ptrc_glGetActiveSubroutineUniformiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLenum, GLint *))IntGetProcAddress("glGetActiveSubroutineUniformiv");
	if(!_ptrc_glGetActiveSubroutineUniformiv) numFailed++;
	_ptrc_glGetProgramStageiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLint *))IntGetProcAddress("glGetProgramStageiv");
	if(!_ptrc_glGetProgramStageiv) numFailed++;
	_ptrc_glGetQueryIndexediv = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLenum, GLint *))IntGetProcAddress("glGetQueryIndexediv");
	if(!_ptrc_glGetQueryIndexediv) numFailed++;
	_ptrc_glGetSubroutineIndex = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetSubroutineIndex");
	if(!_ptrc_glGetSubroutineIndex) numFailed++;
	_ptrc_glGetSubroutineUniformLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetSubroutineUniformLocation");
	if(!_ptrc_glGetSubroutineUniformLocation) numFailed++;
	_ptrc_glGetUniformSubroutineuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLuint *))IntGetProcAddress("glGetUniformSubroutineuiv");
	if(!_ptrc_glGetUniformSubroutineuiv) numFailed++;
	_ptrc_glGetUniformdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble *))IntGetProcAddress("glGetUniformdv");
	if(!_ptrc_glGetUniformdv) numFailed++;
	_ptrc_glIsTransformFeedback = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsTransformFeedback");
	if(!_ptrc_glIsTransformFeedback) numFailed++;
	_ptrc_glMinSampleShading = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glMinSampleShading");
	if(!_ptrc_glMinSampleShading) numFailed++;
	_ptrc_glPatchParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glPatchParameterfv");
	if(!_ptrc_glPatchParameterfv) numFailed++;
	_ptrc_glPatchParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPatchParameteri");
	if(!_ptrc_glPatchParameteri) numFailed++;
	_ptrc_glPauseTransformFeedback = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glPauseTransformFeedback");
	if(!_ptrc_glPauseTransformFeedback) numFailed++;
	_ptrc_glResumeTransformFeedback = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glResumeTransformFeedback");
	if(!_ptrc_glResumeTransformFeedback) numFailed++;
	_ptrc_glUniform1d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble))IntGetProcAddress("glUniform1d");
	if(!_ptrc_glUniform1d) numFailed++;
	_ptrc_glUniform1dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLdouble *))IntGetProcAddress("glUniform1dv");
	if(!_ptrc_glUniform1dv) numFailed++;
	_ptrc_glUniform2d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble))IntGetProcAddress("glUniform2d");
	if(!_ptrc_glUniform2d) numFailed++;
	_ptrc_glUniform2dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLdouble *))IntGetProcAddress("glUniform2dv");
	if(!_ptrc_glUniform2dv) numFailed++;
	_ptrc_glUniform3d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glUniform3d");
	if(!_ptrc_glUniform3d) numFailed++;
	_ptrc_glUniform3dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLdouble *))IntGetProcAddress("glUniform3dv");
	if(!_ptrc_glUniform3dv) numFailed++;
	_ptrc_glUniform4d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glUniform4d");
	if(!_ptrc_glUniform4d) numFailed++;
	_ptrc_glUniform4dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLdouble *))IntGetProcAddress("glUniform4dv");
	if(!_ptrc_glUniform4dv) numFailed++;
	_ptrc_glUniformMatrix2dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix2dv");
	if(!_ptrc_glUniformMatrix2dv) numFailed++;
	_ptrc_glUniformMatrix2x3dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix2x3dv");
	if(!_ptrc_glUniformMatrix2x3dv) numFailed++;
	_ptrc_glUniformMatrix2x4dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix2x4dv");
	if(!_ptrc_glUniformMatrix2x4dv) numFailed++;
	_ptrc_glUniformMatrix3dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix3dv");
	if(!_ptrc_glUniformMatrix3dv) numFailed++;
	_ptrc_glUniformMatrix3x2dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix3x2dv");
	if(!_ptrc_glUniformMatrix3x2dv) numFailed++;
	_ptrc_glUniformMatrix3x4dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix3x4dv");
	if(!_ptrc_glUniformMatrix3x4dv) numFailed++;
	_ptrc_glUniformMatrix4dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix4dv");
	if(!_ptrc_glUniformMatrix4dv) numFailed++;
	_ptrc_glUniformMatrix4x2dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix4x2dv");
	if(!_ptrc_glUniformMatrix4x2dv) numFailed++;
	_ptrc_glUniformMatrix4x3dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix4x3dv");
	if(!_ptrc_glUniformMatrix4x3dv) numFailed++;
	_ptrc_glUniformSubroutinesuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLuint *))IntGetProcAddress("glUniformSubroutinesuiv");
	if(!_ptrc_glUniformSubroutinesuiv) numFailed++;
	_ptrc_glActiveShaderProgram = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glActiveShaderProgram");
	if(!_ptrc_glActiveShaderProgram) numFailed++;
	_ptrc_glBindProgramPipeline = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glBindProgramPipeline");
	if(!_ptrc_glBindProgramPipeline) numFailed++;
	_ptrc_glClearDepthf = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glClearDepthf");
	if(!_ptrc_glClearDepthf) numFailed++;
	_ptrc_glCreateShaderProgramv = (GLuint (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLchar *const*))IntGetProcAddress("glCreateShaderProgramv");
	if(!_ptrc_glCreateShaderProgramv) numFailed++;
	_ptrc_glDeleteProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteProgramPipelines");
	if(!_ptrc_glDeleteProgramPipelines) numFailed++;
	_ptrc_glDepthRangeArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLdouble *))IntGetProcAddress("glDepthRangeArrayv");
	if(!_ptrc_glDepthRangeArrayv) numFailed++;
	_ptrc_glDepthRangeIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glDepthRangeIndexed");
	if(!_ptrc_glDepthRangeIndexed) numFailed++;
	_ptrc_glDepthRangef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glDepthRangef");
	if(!_ptrc_glDepthRangef) numFailed++;
	_ptrc_glGenProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenProgramPipelines");
	if(!_ptrc_glGenProgramPipelines) numFailed++;
	_ptrc_glGetDoublei_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLdouble *))IntGetProcAddress("glGetDoublei_v");
	if(!_ptrc_glGetDoublei_v) numFailed++;
	_ptrc_glGetFloati_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLfloat *))IntGetProcAddress("glGetFloati_v");
	if(!_ptrc_glGetFloati_v) numFailed++;
	_ptrc_glGetProgramBinary = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLenum *, void *))IntGetProcAddress("glGetProgramBinary");
	if(!_ptrc_glGetProgramBinary) numFailed++;
	_ptrc_glGetProgramPipelineInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramPipelineInfoLog");
	if(!_ptrc_glGetProgramPipelineInfoLog) numFailed++;
	_ptrc_glGetProgramPipelineiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetProgramPipelineiv");
	if(!_ptrc_glGetProgramPipelineiv) numFailed++;
	_ptrc_glGetShaderPrecisionFormat = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *, GLint *))IntGetProcAddress("glGetShaderPrecisionFormat");
	if(!_ptrc_glGetShaderPrecisionFormat) numFailed++;
	_ptrc_glGetVertexAttribLdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLdouble *))IntGetProcAddress("glGetVertexAttribLdv");
	if(!_ptrc_glGetVertexAttribLdv) numFailed++;
	_ptrc_glIsProgramPipeline = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsProgramPipeline");
	if(!_ptrc_glIsProgramPipeline) numFailed++;
	_ptrc_glProgramBinary = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const void *, GLsizei))IntGetProcAddress("glProgramBinary");
	if(!_ptrc_glProgramBinary) numFailed++;
	_ptrc_glProgramParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glProgramParameteri");
	if(!_ptrc_glProgramParameteri) numFailed++;
	_ptrc_glProgramUniform1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble))IntGetProcAddress("glProgramUniform1d");
	if(!_ptrc_glProgramUniform1d) numFailed++;
	_ptrc_glProgramUniform1dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform1dv");
	if(!_ptrc_glProgramUniform1dv) numFailed++;
	_ptrc_glProgramUniform1f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat))IntGetProcAddress("glProgramUniform1f");
	if(!_ptrc_glProgramUniform1f) numFailed++;
	_ptrc_glProgramUniform1fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform1fv");
	if(!_ptrc_glProgramUniform1fv) numFailed++;
	_ptrc_glProgramUniform1i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint))IntGetProcAddress("glProgramUniform1i");
	if(!_ptrc_glProgramUniform1i) numFailed++;
	_ptrc_glProgramUniform1iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform1iv");
	if(!_ptrc_glProgramUniform1iv) numFailed++;
	_ptrc_glProgramUniform1ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint))IntGetProcAddress("glProgramUniform1ui");
	if(!_ptrc_glProgramUniform1ui) numFailed++;
	_ptrc_glProgramUniform1uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform1uiv");
	if(!_ptrc_glProgramUniform1uiv) numFailed++;
	_ptrc_glProgramUniform2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform2d");
	if(!_ptrc_glProgramUniform2d) numFailed++;
	_ptrc_glProgramUniform2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform2dv");
	if(!_ptrc_glProgramUniform2dv) numFailed++;
	_ptrc_glProgramUniform2f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform2f");
	if(!_ptrc_glProgramUniform2f) numFailed++;
	_ptrc_glProgramUniform2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform2fv");
	if(!_ptrc_glProgramUniform2fv) numFailed++;
	_ptrc_glProgramUniform2i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform2i");
	if(!_ptrc_glProgramUniform2i) numFailed++;
	_ptrc_glProgramUniform2iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform2iv");
	if(!_ptrc_glProgramUniform2iv) numFailed++;
	_ptrc_glProgramUniform2ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint))IntGetProcAddress("glProgramUniform2ui");
	if(!_ptrc_glProgramUniform2ui) numFailed++;
	_ptrc_glProgramUniform2uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform2uiv");
	if(!_ptrc_glProgramUniform2uiv) numFailed++;
	_ptrc_glProgramUniform3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform3d");
	if(!_ptrc_glProgramUniform3d) numFailed++;
	_ptrc_glProgramUniform3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform3dv");
	if(!_ptrc_glProgramUniform3dv) numFailed++;
	_ptrc_glProgramUniform3f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform3f");
	if(!_ptrc_glProgramUniform3f) numFailed++;
	_ptrc_glProgramUniform3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform3fv");
	if(!_ptrc_glProgramUniform3fv) numFailed++;
	_ptrc_glProgramUniform3i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform3i");
	if(!_ptrc_glProgramUniform3i) numFailed++;
	_ptrc_glProgramUniform3iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform3iv");
	if(!_ptrc_glProgramUniform3iv) numFailed++;
	_ptrc_glProgramUniform3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint, GLuint))IntGetProcAddress("glProgramUniform3ui");
	if(!_ptrc_glProgramUniform3ui) numFailed++;
	_ptrc_glProgramUniform3uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform3uiv");
	if(!_ptrc_glProgramUniform3uiv) numFailed++;
	_ptrc_glProgramUniform4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform4d");
	if(!_ptrc_glProgramUniform4d) numFailed++;
	_ptrc_glProgramUniform4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform4dv");
	if(!_ptrc_glProgramUniform4dv) numFailed++;
	_ptrc_glProgramUniform4f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform4f");
	if(!_ptrc_glProgramUniform4f) numFailed++;
	_ptrc_glProgramUniform4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform4fv");
	if(!_ptrc_glProgramUniform4fv) numFailed++;
	_ptrc_glProgramUniform4i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform4i");
	if(!_ptrc_glProgramUniform4i) numFailed++;
	_ptrc_glProgramUniform4iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform4iv");
	if(!_ptrc_glProgramUniform4iv) numFailed++;
	_ptrc_glProgramUniform4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glProgramUniform4ui");
	if(!_ptrc_glProgramUniform4ui) numFailed++;
	_ptrc_glProgramUniform4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform4uiv");
	if(!_ptrc_glProgramUniform4uiv) numFailed++;
	_ptrc_glProgramUniformMatrix2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2dv");
	if(!_ptrc_glProgramUniformMatrix2dv) numFailed++;
	_ptrc_glProgramUniformMatrix2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2fv");
	if(!_ptrc_glProgramUniformMatrix2fv) numFailed++;
	_ptrc_glProgramUniformMatrix2x3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2x3dv");
	if(!_ptrc_glProgramUniformMatrix2x3dv) numFailed++;
	_ptrc_glProgramUniformMatrix2x3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2x3fv");
	if(!_ptrc_glProgramUniformMatrix2x3fv) numFailed++;
	_ptrc_glProgramUniformMatrix2x4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2x4dv");
	if(!_ptrc_glProgramUniformMatrix2x4dv) numFailed++;
	_ptrc_glProgramUniformMatrix2x4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2x4fv");
	if(!_ptrc_glProgramUniformMatrix2x4fv) numFailed++;
	_ptrc_glProgramUniformMatrix3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3dv");
	if(!_ptrc_glProgramUniformMatrix3dv) numFailed++;
	_ptrc_glProgramUniformMatrix3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3fv");
	if(!_ptrc_glProgramUniformMatrix3fv) numFailed++;
	_ptrc_glProgramUniformMatrix3x2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3x2dv");
	if(!_ptrc_glProgramUniformMatrix3x2dv) numFailed++;
	_ptrc_glProgramUniformMatrix3x2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3x2fv");
	if(!_ptrc_glProgramUniformMatrix3x2fv) numFailed++;
	_ptrc_glProgramUniformMatrix3x4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3x4dv");
	if(!_ptrc_glProgramUniformMatrix3x4dv) numFailed++;
	_ptrc_glProgramUniformMatrix3x4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3x4fv");
	if(!_ptrc_glProgramUniformMatrix3x4fv) numFailed++;
	_ptrc_glProgramUniformMatrix4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4dv");
	if(!_ptrc_glProgramUniformMatrix4dv) numFailed++;
	_ptrc_glProgramUniformMatrix4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4fv");
	if(!_ptrc_glProgramUniformMatrix4fv) numFailed++;
	_ptrc_glProgramUniformMatrix4x2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4x2dv");
	if(!_ptrc_glProgramUniformMatrix4x2dv) numFailed++;
	_ptrc_glProgramUniformMatrix4x2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4x2fv");
	if(!_ptrc_glProgramUniformMatrix4x2fv) numFailed++;
	_ptrc_glProgramUniformMatrix4x3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4x3dv");
	if(!_ptrc_glProgramUniformMatrix4x3dv) numFailed++;
	_ptrc_glProgramUniformMatrix4x3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4x3fv");
	if(!_ptrc_glProgramUniformMatrix4x3fv) numFailed++;
	_ptrc_glReleaseShaderCompiler = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glReleaseShaderCompiler");
	if(!_ptrc_glReleaseShaderCompiler) numFailed++;
	_ptrc_glScissorArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLint *))IntGetProcAddress("glScissorArrayv");
	if(!_ptrc_glScissorArrayv) numFailed++;
	_ptrc_glScissorIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissorIndexed");
	if(!_ptrc_glScissorIndexed) numFailed++;
	_ptrc_glScissorIndexedv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glScissorIndexedv");
	if(!_ptrc_glScissorIndexedv) numFailed++;
	_ptrc_glShaderBinary = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *, GLenum, const void *, GLsizei))IntGetProcAddress("glShaderBinary");
	if(!_ptrc_glShaderBinary) numFailed++;
	_ptrc_glUseProgramStages = (void (CODEGEN_FUNCPTR *)(GLuint, GLbitfield, GLuint))IntGetProcAddress("glUseProgramStages");
	if(!_ptrc_glUseProgramStages) numFailed++;
	_ptrc_glValidateProgramPipeline = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glValidateProgramPipeline");
	if(!_ptrc_glValidateProgramPipeline) numFailed++;
	_ptrc_glVertexAttribL1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble))IntGetProcAddress("glVertexAttribL1d");
	if(!_ptrc_glVertexAttribL1d) numFailed++;
	_ptrc_glVertexAttribL1dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttribL1dv");
	if(!_ptrc_glVertexAttribL1dv) numFailed++;
	_ptrc_glVertexAttribL2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glVertexAttribL2d");
	if(!_ptrc_glVertexAttribL2d) numFailed++;
	_ptrc_glVertexAttribL2dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttribL2dv");
	if(!_ptrc_glVertexAttribL2dv) numFailed++;
	_ptrc_glVertexAttribL3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttribL3d");
	if(!_ptrc_glVertexAttribL3d) numFailed++;
	_ptrc_glVertexAttribL3dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttribL3dv");
	if(!_ptrc_glVertexAttribL3dv) numFailed++;
	_ptrc_glVertexAttribL4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttribL4d");
	if(!_ptrc_glVertexAttribL4d) numFailed++;
	_ptrc_glVertexAttribL4dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttribL4dv");
	if(!_ptrc_glVertexAttribL4dv) numFailed++;
	_ptrc_glVertexAttribLPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLsizei, const void *))IntGetProcAddress("glVertexAttribLPointer");
	if(!_ptrc_glVertexAttribLPointer) numFailed++;
	_ptrc_glViewportArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLfloat *))IntGetProcAddress("glViewportArrayv");
	if(!_ptrc_glViewportArrayv) numFailed++;
	_ptrc_glViewportIndexedf = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glViewportIndexedf");
	if(!_ptrc_glViewportIndexedf) numFailed++;
	_ptrc_glViewportIndexedfv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glViewportIndexedfv");
	if(!_ptrc_glViewportIndexedfv) numFailed++;
	_ptrc_glBindImageTexture = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum))IntGetProcAddress("glBindImageTexture");
	if(!_ptrc_glBindImageTexture) numFailed++;
	_ptrc_glDrawArraysInstancedBaseInstance = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei, GLsizei, GLuint))IntGetProcAddress("glDrawArraysInstancedBaseInstance");
	if(!_ptrc_glDrawArraysInstancedBaseInstance) numFailed++;
	_ptrc_glDrawElementsInstancedBaseInstance = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLuint))IntGetProcAddress("glDrawElementsInstancedBaseInstance");
	if(!_ptrc_glDrawElementsInstancedBaseInstance) numFailed++;
	_ptrc_glDrawElementsInstancedBaseVertexBaseInstance = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint, GLuint))IntGetProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
	if(!_ptrc_glDrawElementsInstancedBaseVertexBaseInstance) numFailed++;
	_ptrc_glDrawTransformFeedbackInstanced = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei))IntGetProcAddress("glDrawTransformFeedbackInstanced");
	if(!_ptrc_glDrawTransformFeedbackInstanced) numFailed++;
	_ptrc_glDrawTransformFeedbackStreamInstanced = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLsizei))IntGetProcAddress("glDrawTransformFeedbackStreamInstanced");
	if(!_ptrc_glDrawTransformFeedbackStreamInstanced) numFailed++;
	_ptrc_glGetActiveAtomicCounterBufferiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLint *))IntGetProcAddress("glGetActiveAtomicCounterBufferiv");
	if(!_ptrc_glGetActiveAtomicCounterBufferiv) numFailed++;
	_ptrc_glGetInternalformativ = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, GLint *))IntGetProcAddress("glGetInternalformativ");
	if(!_ptrc_glGetInternalformativ) numFailed++;
	_ptrc_glMemoryBarrier = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glMemoryBarrier");
	if(!_ptrc_glMemoryBarrier) numFailed++;
	_ptrc_glTexStorage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei))IntGetProcAddress("glTexStorage1D");
	if(!_ptrc_glTexStorage1D) numFailed++;
	_ptrc_glTexStorage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glTexStorage2D");
	if(!_ptrc_glTexStorage2D) numFailed++;
	_ptrc_glTexStorage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glTexStorage3D");
	if(!_ptrc_glTexStorage3D) numFailed++;
	_ptrc_glBindVertexBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLintptr, GLsizei))IntGetProcAddress("glBindVertexBuffer");
	if(!_ptrc_glBindVertexBuffer) numFailed++;
	_ptrc_glClearBufferData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum, const void *))IntGetProcAddress("glClearBufferData");
	if(!_ptrc_glClearBufferData) numFailed++;
	_ptrc_glClearBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *))IntGetProcAddress("glClearBufferSubData");
	if(!_ptrc_glClearBufferSubData) numFailed++;
	_ptrc_glCopyImageSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glCopyImageSubData");
	if(!_ptrc_glCopyImageSubData) numFailed++;
	_ptrc_glDebugMessageCallback = (void (CODEGEN_FUNCPTR *)(GLDEBUGPROC, const void *))IntGetProcAddress("glDebugMessageCallback");
	if(!_ptrc_glDebugMessageCallback) numFailed++;
	_ptrc_glDebugMessageControl = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean))IntGetProcAddress("glDebugMessageControl");
	if(!_ptrc_glDebugMessageControl) numFailed++;
	_ptrc_glDebugMessageInsert = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *))IntGetProcAddress("glDebugMessageInsert");
	if(!_ptrc_glDebugMessageInsert) numFailed++;
	_ptrc_glDispatchCompute = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glDispatchCompute");
	if(!_ptrc_glDispatchCompute) numFailed++;
	_ptrc_glDispatchComputeIndirect = (void (CODEGEN_FUNCPTR *)(GLintptr))IntGetProcAddress("glDispatchComputeIndirect");
	if(!_ptrc_glDispatchComputeIndirect) numFailed++;
	_ptrc_glFramebufferParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glFramebufferParameteri");
	if(!_ptrc_glFramebufferParameteri) numFailed++;
	_ptrc_glGetDebugMessageLog = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *))IntGetProcAddress("glGetDebugMessageLog");
	if(!_ptrc_glGetDebugMessageLog) numFailed++;
	_ptrc_glGetFramebufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetFramebufferParameteriv");
	if(!_ptrc_glGetFramebufferParameteriv) numFailed++;
	_ptrc_glGetInternalformati64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, GLint64 *))IntGetProcAddress("glGetInternalformati64v");
	if(!_ptrc_glGetInternalformati64v) numFailed++;
	_ptrc_glGetObjectLabel = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetObjectLabel");
	if(!_ptrc_glGetObjectLabel) numFailed++;
	_ptrc_glGetObjectPtrLabel = (void (CODEGEN_FUNCPTR *)(const void *, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetObjectPtrLabel");
	if(!_ptrc_glGetObjectPtrLabel) numFailed++;
	_ptrc_glGetProgramInterfaceiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLint *))IntGetProcAddress("glGetProgramInterfaceiv");
	if(!_ptrc_glGetProgramInterfaceiv) numFailed++;
	_ptrc_glGetProgramResourceIndex = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceIndex");
	if(!_ptrc_glGetProgramResourceIndex) numFailed++;
	_ptrc_glGetProgramResourceLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceLocation");
	if(!_ptrc_glGetProgramResourceLocation) numFailed++;
	_ptrc_glGetProgramResourceLocationIndex = (GLint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceLocationIndex");
	if(!_ptrc_glGetProgramResourceLocationIndex) numFailed++;
	_ptrc_glGetProgramResourceName = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramResourceName");
	if(!_ptrc_glGetProgramResourceName) numFailed++;
	_ptrc_glGetProgramResourceiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *))IntGetProcAddress("glGetProgramResourceiv");
	if(!_ptrc_glGetProgramResourceiv) numFailed++;
	_ptrc_glInvalidateBufferData = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glInvalidateBufferData");
	if(!_ptrc_glInvalidateBufferData) numFailed++;
	_ptrc_glInvalidateBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glInvalidateBufferSubData");
	if(!_ptrc_glInvalidateBufferSubData) numFailed++;
	_ptrc_glInvalidateFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLenum *))IntGetProcAddress("glInvalidateFramebuffer");
	if(!_ptrc_glInvalidateFramebuffer) numFailed++;
	_ptrc_glInvalidateSubFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glInvalidateSubFramebuffer");
	if(!_ptrc_glInvalidateSubFramebuffer) numFailed++;
	_ptrc_glInvalidateTexImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint))IntGetProcAddress("glInvalidateTexImage");
	if(!_ptrc_glInvalidateTexImage) numFailed++;
	_ptrc_glInvalidateTexSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glInvalidateTexSubImage");
	if(!_ptrc_glInvalidateTexSubImage) numFailed++;
	_ptrc_glMultiDrawArraysIndirect = (void (CODEGEN_FUNCPTR *)(GLenum, const void *, GLsizei, GLsizei))IntGetProcAddress("glMultiDrawArraysIndirect");
	if(!_ptrc_glMultiDrawArraysIndirect) numFailed++;
	_ptrc_glMultiDrawElementsIndirect = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const void *, GLsizei, GLsizei))IntGetProcAddress("glMultiDrawElementsIndirect");
	if(!_ptrc_glMultiDrawElementsIndirect) numFailed++;
	_ptrc_glObjectLabel = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLchar *))IntGetProcAddress("glObjectLabel");
	if(!_ptrc_glObjectLabel) numFailed++;
	_ptrc_glObjectPtrLabel = (void (CODEGEN_FUNCPTR *)(const void *, GLsizei, const GLchar *))IntGetProcAddress("glObjectPtrLabel");
	if(!_ptrc_glObjectPtrLabel) numFailed++;
	_ptrc_glPopDebugGroup = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glPopDebugGroup");
	if(!_ptrc_glPopDebugGroup) numFailed++;
	_ptrc_glPushDebugGroup = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLchar *))IntGetProcAddress("glPushDebugGroup");
	if(!_ptrc_glPushDebugGroup) numFailed++;
	_ptrc_glShaderStorageBlockBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glShaderStorageBlockBinding");
	if(!_ptrc_glShaderStorageBlockBinding) numFailed++;
	_ptrc_glTexBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glTexBufferRange");
	if(!_ptrc_glTexBufferRange) numFailed++;
	_ptrc_glTexStorage2DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexStorage2DMultisample");
	if(!_ptrc_glTexStorage2DMultisample) numFailed++;
	_ptrc_glTexStorage3DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexStorage3DMultisample");
	if(!_ptrc_glTexStorage3DMultisample) numFailed++;
	_ptrc_glTextureView = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glTextureView");
	if(!_ptrc_glTextureView) numFailed++;
	_ptrc_glVertexAttribBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexAttribBinding");
	if(!_ptrc_glVertexAttribBinding) numFailed++;
	_ptrc_glVertexAttribFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribFormat");
	if(!_ptrc_glVertexAttribFormat) numFailed++;
	_ptrc_glVertexAttribIFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexAttribIFormat");
	if(!_ptrc_glVertexAttribIFormat) numFailed++;
	_ptrc_glVertexAttribLFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexAttribLFormat");
	if(!_ptrc_glVertexAttribLFormat) numFailed++;
	_ptrc_glVertexBindingDivisor = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexBindingDivisor");
	if(!_ptrc_glVertexBindingDivisor) numFailed++;
	_ptrc_glBindBuffersBase = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLuint *))IntGetProcAddress("glBindBuffersBase");
	if(!_ptrc_glBindBuffersBase) numFailed++;
	_ptrc_glBindBuffersRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizeiptr *))IntGetProcAddress("glBindBuffersRange");
	if(!_ptrc_glBindBuffersRange) numFailed++;
	_ptrc_glBindImageTextures = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *))IntGetProcAddress("glBindImageTextures");
	if(!_ptrc_glBindImageTextures) numFailed++;
	_ptrc_glBindSamplers = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *))IntGetProcAddress("glBindSamplers");
	if(!_ptrc_glBindSamplers) numFailed++;
	_ptrc_glBindTextures = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *))IntGetProcAddress("glBindTextures");
	if(!_ptrc_glBindTextures) numFailed++;
	_ptrc_glBindVertexBuffers = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizei *))IntGetProcAddress("glBindVertexBuffers");
	if(!_ptrc_glBindVertexBuffers) numFailed++;
	_ptrc_glBufferStorage = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizeiptr, const void *, GLbitfield))IntGetProcAddress("glBufferStorage");
	if(!_ptrc_glBufferStorage) numFailed++;
	_ptrc_glClearTexImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLenum, const void *))IntGetProcAddress("glClearTexImage");
	if(!_ptrc_glClearTexImage) numFailed++;
	_ptrc_glClearTexSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glClearTexSubImage");
	if(!_ptrc_glClearTexSubImage) numFailed++;
	_ptrc_glBindTextureUnit = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glBindTextureUnit");
	if(!_ptrc_glBindTextureUnit) numFailed++;
	_ptrc_glBlitNamedFramebuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum))IntGetProcAddress("glBlitNamedFramebuffer");
	if(!_ptrc_glBlitNamedFramebuffer) numFailed++;
	_ptrc_glCheckNamedFramebufferStatus = (GLenum (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glCheckNamedFramebufferStatus");
	if(!_ptrc_glCheckNamedFramebufferStatus) numFailed++;
	_ptrc_glClearNamedBufferData = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLenum, const void *))IntGetProcAddress("glClearNamedBufferData");
	if(!_ptrc_glClearNamedBufferData) numFailed++;
	_ptrc_glClearNamedBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *))IntGetProcAddress("glClearNamedBufferSubData");
	if(!_ptrc_glClearNamedBufferSubData) numFailed++;
	_ptrc_glClearNamedFramebufferfi = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, const GLfloat, GLint))IntGetProcAddress("glClearNamedFramebufferfi");
	if(!_ptrc_glClearNamedFramebufferfi) numFailed++;
	_ptrc_glClearNamedFramebufferfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, const GLfloat *))IntGetProcAddress("glClearNamedFramebufferfv");
	if(!_ptrc_glClearNamedFramebufferfv) numFailed++;
	_ptrc_glClearNamedFramebufferiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, const GLint *))IntGetProcAddress("glClearNamedFramebufferiv");
	if(!_ptrc_glClearNamedFramebufferiv) numFailed++;
	_ptrc_glClearNamedFramebufferuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, const GLuint *))IntGetProcAddress("glClearNamedFramebufferuiv");
	if(!_ptrc_glClearNamedFramebufferuiv) numFailed++;
	_ptrc_glClipControl = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glClipControl");
	if(!_ptrc_glClipControl) numFailed++;
	_ptrc_glCompressedTextureSubImage1D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTextureSubImage1D");
	if(!_ptrc_glCompressedTextureSubImage1D) numFailed++;
	_ptrc_glCompressedTextureSubImage2D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTextureSubImage2D");
	if(!_ptrc_glCompressedTextureSubImage2D) numFailed++;
	_ptrc_glCompressedTextureSubImage3D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *))IntGetProcAddress("glCompressedTextureSubImage3D");
	if(!_ptrc_glCompressedTextureSubImage3D) numFailed++;
	_ptrc_glCopyNamedBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLintptr, GLintptr, GLsizeiptr))IntGetProcAddress("glCopyNamedBufferSubData");
	if(!_ptrc_glCopyNamedBufferSubData) numFailed++;
	_ptrc_glCopyTextureSubImage1D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei))IntGetProcAddress("glCopyTextureSubImage1D");
	if(!_ptrc_glCopyTextureSubImage1D) numFailed++;
	_ptrc_glCopyTextureSubImage2D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTextureSubImage2D");
	if(!_ptrc_glCopyTextureSubImage2D) numFailed++;
	_ptrc_glCopyTextureSubImage3D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTextureSubImage3D");
	if(!_ptrc_glCopyTextureSubImage3D) numFailed++;
	_ptrc_glCreateBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateBuffers");
	if(!_ptrc_glCreateBuffers) numFailed++;
	_ptrc_glCreateFramebuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateFramebuffers");
	if(!_ptrc_glCreateFramebuffers) numFailed++;
	_ptrc_glCreateProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateProgramPipelines");
	if(!_ptrc_glCreateProgramPipelines) numFailed++;
	_ptrc_glCreateQueries = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLuint *))IntGetProcAddress("glCreateQueries");
	if(!_ptrc_glCreateQueries) numFailed++;
	_ptrc_glCreateRenderbuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateRenderbuffers");
	if(!_ptrc_glCreateRenderbuffers) numFailed++;
	_ptrc_glCreateSamplers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateSamplers");
	if(!_ptrc_glCreateSamplers) numFailed++;
	_ptrc_glCreateTextures = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLuint *))IntGetProcAddress("glCreateTextures");
	if(!_ptrc_glCreateTextures) numFailed++;
	_ptrc_glCreateTransformFeedbacks = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateTransformFeedbacks");
	if(!_ptrc_glCreateTransformFeedbacks) numFailed++;
	_ptrc_glCreateVertexArrays = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glCreateVertexArrays");
	if(!_ptrc_glCreateVertexArrays) numFailed++;
	_ptrc_glDisableVertexArrayAttrib = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glDisableVertexArrayAttrib");
	if(!_ptrc_glDisableVertexArrayAttrib) numFailed++;
	_ptrc_glEnableVertexArrayAttrib = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glEnableVertexArrayAttrib");
	if(!_ptrc_glEnableVertexArrayAttrib) numFailed++;
	_ptrc_glFlushMappedNamedBufferRange = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glFlushMappedNamedBufferRange");
	if(!_ptrc_glFlushMappedNamedBufferRange) numFailed++;
	_ptrc_glGenerateTextureMipmap = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glGenerateTextureMipmap");
	if(!_ptrc_glGenerateTextureMipmap) numFailed++;
	_ptrc_glGetCompressedTextureImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, void *))IntGetProcAddress("glGetCompressedTextureImage");
	if(!_ptrc_glGetCompressedTextureImage) numFailed++;
	_ptrc_glGetCompressedTextureSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLsizei, void *))IntGetProcAddress("glGetCompressedTextureSubImage");
	if(!_ptrc_glGetCompressedTextureSubImage) numFailed++;
	_ptrc_glGetGraphicsResetStatus = (GLenum (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glGetGraphicsResetStatus");
	if(!_ptrc_glGetGraphicsResetStatus) numFailed++;
	_ptrc_glGetNamedBufferParameteri64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint64 *))IntGetProcAddress("glGetNamedBufferParameteri64v");
	if(!_ptrc_glGetNamedBufferParameteri64v) numFailed++;
	_ptrc_glGetNamedBufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetNamedBufferParameteriv");
	if(!_ptrc_glGetNamedBufferParameteriv) numFailed++;
	_ptrc_glGetNamedBufferPointerv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, void **))IntGetProcAddress("glGetNamedBufferPointerv");
	if(!_ptrc_glGetNamedBufferPointerv) numFailed++;
	_ptrc_glGetNamedBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr, void *))IntGetProcAddress("glGetNamedBufferSubData");
	if(!_ptrc_glGetNamedBufferSubData) numFailed++;
	_ptrc_glGetNamedFramebufferAttachmentParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLint *))IntGetProcAddress("glGetNamedFramebufferAttachmentParameteriv");
	if(!_ptrc_glGetNamedFramebufferAttachmentParameteriv) numFailed++;
	_ptrc_glGetNamedFramebufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetNamedFramebufferParameteriv");
	if(!_ptrc_glGetNamedFramebufferParameteriv) numFailed++;
	_ptrc_glGetNamedRenderbufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetNamedRenderbufferParameteriv");
	if(!_ptrc_glGetNamedRenderbufferParameteriv) numFailed++;
	_ptrc_glGetQueryBufferObjecti64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLintptr))IntGetProcAddress("glGetQueryBufferObjecti64v");
	if(!_ptrc_glGetQueryBufferObjecti64v) numFailed++;
	_ptrc_glGetQueryBufferObjectiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLintptr))IntGetProcAddress("glGetQueryBufferObjectiv");
	if(!_ptrc_glGetQueryBufferObjectiv) numFailed++;
	_ptrc_glGetQueryBufferObjectui64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLintptr))IntGetProcAddress("glGetQueryBufferObjectui64v");
	if(!_ptrc_glGetQueryBufferObjectui64v) numFailed++;
	_ptrc_glGetQueryBufferObjectuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLintptr))IntGetProcAddress("glGetQueryBufferObjectuiv");
	if(!_ptrc_glGetQueryBufferObjectuiv) numFailed++;
	_ptrc_glGetTextureImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLenum, GLsizei, void *))IntGetProcAddress("glGetTextureImage");
	if(!_ptrc_glGetTextureImage) numFailed++;
	_ptrc_glGetTextureLevelParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLfloat *))IntGetProcAddress("glGetTextureLevelParameterfv");
	if(!_ptrc_glGetTextureLevelParameterfv) numFailed++;
	_ptrc_glGetTextureLevelParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLint *))IntGetProcAddress("glGetTextureLevelParameteriv");
	if(!_ptrc_glGetTextureLevelParameteriv) numFailed++;
	_ptrc_glGetTextureParameterIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetTextureParameterIiv");
	if(!_ptrc_glGetTextureParameterIiv) numFailed++;
	_ptrc_glGetTextureParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetTextureParameterIuiv");
	if(!_ptrc_glGetTextureParameterIuiv) numFailed++;
	_ptrc_glGetTextureParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat *))IntGetProcAddress("glGetTextureParameterfv");
	if(!_ptrc_glGetTextureParameterfv) numFailed++;
	_ptrc_glGetTextureParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetTextureParameteriv");
	if(!_ptrc_glGetTextureParameteriv) numFailed++;
	_ptrc_glGetTextureSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *))IntGetProcAddress("glGetTextureSubImage");
	if(!_ptrc_glGetTextureSubImage) numFailed++;
	_ptrc_glGetTransformFeedbacki64_v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLint64 *))IntGetProcAddress("glGetTransformFeedbacki64_v");
	if(!_ptrc_glGetTransformFeedbacki64_v) numFailed++;
	_ptrc_glGetTransformFeedbacki_v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLint *))IntGetProcAddress("glGetTransformFeedbacki_v");
	if(!_ptrc_glGetTransformFeedbacki_v) numFailed++;
	_ptrc_glGetTransformFeedbackiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetTransformFeedbackiv");
	if(!_ptrc_glGetTransformFeedbackiv) numFailed++;
	_ptrc_glGetVertexArrayIndexed64iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLint64 *))IntGetProcAddress("glGetVertexArrayIndexed64iv");
	if(!_ptrc_glGetVertexArrayIndexed64iv) numFailed++;
	_ptrc_glGetVertexArrayIndexediv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexArrayIndexediv");
	if(!_ptrc_glGetVertexArrayIndexediv) numFailed++;
	_ptrc_glGetVertexArrayiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexArrayiv");
	if(!_ptrc_glGetVertexArrayiv) numFailed++;
	_ptrc_glGetnCompressedTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei, void *))IntGetProcAddress("glGetnCompressedTexImage");
	if(!_ptrc_glGetnCompressedTexImage) numFailed++;
	_ptrc_glGetnTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLenum, GLsizei, void *))IntGetProcAddress("glGetnTexImage");
	if(!_ptrc_glGetnTexImage) numFailed++;
	_ptrc_glGetnUniformdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLdouble *))IntGetProcAddress("glGetnUniformdv");
	if(!_ptrc_glGetnUniformdv) numFailed++;
	_ptrc_glGetnUniformfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLfloat *))IntGetProcAddress("glGetnUniformfv");
	if(!_ptrc_glGetnUniformfv) numFailed++;
	_ptrc_glGetnUniformiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLint *))IntGetProcAddress("glGetnUniformiv");
	if(!_ptrc_glGetnUniformiv) numFailed++;
	_ptrc_glGetnUniformuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLuint *))IntGetProcAddress("glGetnUniformuiv");
	if(!_ptrc_glGetnUniformuiv) numFailed++;
	_ptrc_glInvalidateNamedFramebufferData = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLenum *))IntGetProcAddress("glInvalidateNamedFramebufferData");
	if(!_ptrc_glInvalidateNamedFramebufferData) numFailed++;
	_ptrc_glInvalidateNamedFramebufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glInvalidateNamedFramebufferSubData");
	if(!_ptrc_glInvalidateNamedFramebufferSubData) numFailed++;
	_ptrc_glMapNamedBuffer = (void * (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glMapNamedBuffer");
	if(!_ptrc_glMapNamedBuffer) numFailed++;
	_ptrc_glMapNamedBufferRange = (void * (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr, GLbitfield))IntGetProcAddress("glMapNamedBufferRange");
	if(!_ptrc_glMapNamedBufferRange) numFailed++;
	_ptrc_glMemoryBarrierByRegion = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glMemoryBarrierByRegion");
	if(!_ptrc_glMemoryBarrierByRegion) numFailed++;
	_ptrc_glNamedBufferData = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizeiptr, const void *, GLenum))IntGetProcAddress("glNamedBufferData");
	if(!_ptrc_glNamedBufferData) numFailed++;
	_ptrc_glNamedBufferStorage = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizeiptr, const void *, GLbitfield))IntGetProcAddress("glNamedBufferStorage");
	if(!_ptrc_glNamedBufferStorage) numFailed++;
	_ptrc_glNamedBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr, const void *))IntGetProcAddress("glNamedBufferSubData");
	if(!_ptrc_glNamedBufferSubData) numFailed++;
	_ptrc_glNamedFramebufferDrawBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glNamedFramebufferDrawBuffer");
	if(!_ptrc_glNamedFramebufferDrawBuffer) numFailed++;
	_ptrc_glNamedFramebufferDrawBuffers = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLenum *))IntGetProcAddress("glNamedFramebufferDrawBuffers");
	if(!_ptrc_glNamedFramebufferDrawBuffers) numFailed++;
	_ptrc_glNamedFramebufferParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glNamedFramebufferParameteri");
	if(!_ptrc_glNamedFramebufferParameteri) numFailed++;
	_ptrc_glNamedFramebufferReadBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glNamedFramebufferReadBuffer");
	if(!_ptrc_glNamedFramebufferReadBuffer) numFailed++;
	_ptrc_glNamedFramebufferRenderbuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLuint))IntGetProcAddress("glNamedFramebufferRenderbuffer");
	if(!_ptrc_glNamedFramebufferRenderbuffer) numFailed++;
	_ptrc_glNamedFramebufferTexture = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLint))IntGetProcAddress("glNamedFramebufferTexture");
	if(!_ptrc_glNamedFramebufferTexture) numFailed++;
	_ptrc_glNamedFramebufferTextureLayer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glNamedFramebufferTextureLayer");
	if(!_ptrc_glNamedFramebufferTextureLayer) numFailed++;
	_ptrc_glNamedRenderbufferStorage = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLsizei, GLsizei))IntGetProcAddress("glNamedRenderbufferStorage");
	if(!_ptrc_glNamedRenderbufferStorage) numFailed++;
	_ptrc_glNamedRenderbufferStorageMultisample = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glNamedRenderbufferStorageMultisample");
	if(!_ptrc_glNamedRenderbufferStorageMultisample) numFailed++;
	_ptrc_glReadnPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *))IntGetProcAddress("glReadnPixels");
	if(!_ptrc_glReadnPixels) numFailed++;
	_ptrc_glTextureBarrier = (void (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("glTextureBarrier");
	if(!_ptrc_glTextureBarrier) numFailed++;
	_ptrc_glTextureBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint))IntGetProcAddress("glTextureBuffer");
	if(!_ptrc_glTextureBuffer) numFailed++;
	_ptrc_glTextureBufferRange = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glTextureBufferRange");
	if(!_ptrc_glTextureBufferRange) numFailed++;
	_ptrc_glTextureParameterIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLint *))IntGetProcAddress("glTextureParameterIiv");
	if(!_ptrc_glTextureParameterIiv) numFailed++;
	_ptrc_glTextureParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLuint *))IntGetProcAddress("glTextureParameterIuiv");
	if(!_ptrc_glTextureParameterIuiv) numFailed++;
	_ptrc_glTextureParameterf = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat))IntGetProcAddress("glTextureParameterf");
	if(!_ptrc_glTextureParameterf) numFailed++;
	_ptrc_glTextureParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLfloat *))IntGetProcAddress("glTextureParameterfv");
	if(!_ptrc_glTextureParameterfv) numFailed++;
	_ptrc_glTextureParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glTextureParameteri");
	if(!_ptrc_glTextureParameteri) numFailed++;
	_ptrc_glTextureParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLint *))IntGetProcAddress("glTextureParameteriv");
	if(!_ptrc_glTextureParameteriv) numFailed++;
	_ptrc_glTextureStorage1D = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei))IntGetProcAddress("glTextureStorage1D");
	if(!_ptrc_glTextureStorage1D) numFailed++;
	_ptrc_glTextureStorage2D = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glTextureStorage2D");
	if(!_ptrc_glTextureStorage2D) numFailed++;
	_ptrc_glTextureStorage2DMultisample = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTextureStorage2DMultisample");
	if(!_ptrc_glTextureStorage2DMultisample) numFailed++;
	_ptrc_glTextureStorage3D = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glTextureStorage3D");
	if(!_ptrc_glTextureStorage3D) numFailed++;
	_ptrc_glTextureStorage3DMultisample = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTextureStorage3DMultisample");
	if(!_ptrc_glTextureStorage3DMultisample) numFailed++;
	_ptrc_glTextureSubImage1D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTextureSubImage1D");
	if(!_ptrc_glTextureSubImage1D) numFailed++;
	_ptrc_glTextureSubImage2D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTextureSubImage2D");
	if(!_ptrc_glTextureSubImage2D) numFailed++;
	_ptrc_glTextureSubImage3D = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *))IntGetProcAddress("glTextureSubImage3D");
	if(!_ptrc_glTextureSubImage3D) numFailed++;
	_ptrc_glTransformFeedbackBufferBase = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glTransformFeedbackBufferBase");
	if(!_ptrc_glTransformFeedbackBufferBase) numFailed++;
	_ptrc_glTransformFeedbackBufferRange = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glTransformFeedbackBufferRange");
	if(!_ptrc_glTransformFeedbackBufferRange) numFailed++;
	_ptrc_glUnmapNamedBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glUnmapNamedBuffer");
	if(!_ptrc_glUnmapNamedBuffer) numFailed++;
	_ptrc_glVertexArrayAttribBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glVertexArrayAttribBinding");
	if(!_ptrc_glVertexArrayAttribBinding) numFailed++;
	_ptrc_glVertexArrayAttribFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexArrayAttribFormat");
	if(!_ptrc_glVertexArrayAttribFormat) numFailed++;
	_ptrc_glVertexArrayAttribIFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexArrayAttribIFormat");
	if(!_ptrc_glVertexArrayAttribIFormat) numFailed++;
	_ptrc_glVertexArrayAttribLFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexArrayAttribLFormat");
	if(!_ptrc_glVertexArrayAttribLFormat) numFailed++;
	_ptrc_glVertexArrayBindingDivisor = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glVertexArrayBindingDivisor");
	if(!_ptrc_glVertexArrayBindingDivisor) numFailed++;
	_ptrc_glVertexArrayElementBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexArrayElementBuffer");
	if(!_ptrc_glVertexArrayElementBuffer) numFailed++;
	_ptrc_glVertexArrayVertexBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLintptr, GLsizei))IntGetProcAddress("glVertexArrayVertexBuffer");
	if(!_ptrc_glVertexArrayVertexBuffer) numFailed++;
	_ptrc_glVertexArrayVertexBuffers = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizei *))IntGetProcAddress("glVertexArrayVertexBuffers");
	if(!_ptrc_glVertexArrayVertexBuffers) numFailed++;
	return numFailed;
}

typedef int (*PFN_LOADFUNCPOINTERS)(void);
typedef struct ogl_StrToExtMap_s
{
	char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} ogl_StrToExtMap;

static ogl_StrToExtMap ExtensionMap[52] = {
	{"GL_EXT_texture_compression_s3tc", &ogl_ext_EXT_texture_compression_s3tc, NULL},
	{"GL_EXT_texture_sRGB", &ogl_ext_EXT_texture_sRGB, NULL},
	{"GL_EXT_texture_filter_anisotropic", &ogl_ext_EXT_texture_filter_anisotropic, NULL},
	{"GL_ARB_compressed_texture_pixel_storage", &ogl_ext_ARB_compressed_texture_pixel_storage, NULL},
	{"GL_ARB_conservative_depth", &ogl_ext_ARB_conservative_depth, NULL},
	{"GL_ARB_ES2_compatibility", &ogl_ext_ARB_ES2_compatibility, Load_ARB_ES2_compatibility},
	{"GL_ARB_get_program_binary", &ogl_ext_ARB_get_program_binary, Load_ARB_get_program_binary},
	{"GL_ARB_explicit_uniform_location", &ogl_ext_ARB_explicit_uniform_location, NULL},
	{"GL_ARB_internalformat_query", &ogl_ext_ARB_internalformat_query, Load_ARB_internalformat_query},
	{"GL_ARB_internalformat_query2", &ogl_ext_ARB_internalformat_query2, Load_ARB_internalformat_query2},
	{"GL_ARB_map_buffer_alignment", &ogl_ext_ARB_map_buffer_alignment, NULL},
	{"GL_ARB_program_interface_query", &ogl_ext_ARB_program_interface_query, Load_ARB_program_interface_query},
	{"GL_ARB_separate_shader_objects", &ogl_ext_ARB_separate_shader_objects, Load_ARB_separate_shader_objects},
	{"GL_ARB_shading_language_420pack", &ogl_ext_ARB_shading_language_420pack, NULL},
	{"GL_ARB_shading_language_packing", &ogl_ext_ARB_shading_language_packing, NULL},
	{"GL_ARB_texture_buffer_range", &ogl_ext_ARB_texture_buffer_range, Load_ARB_texture_buffer_range},
	{"GL_ARB_texture_storage", &ogl_ext_ARB_texture_storage, Load_ARB_texture_storage},
	{"GL_ARB_texture_view", &ogl_ext_ARB_texture_view, Load_ARB_texture_view},
	{"GL_ARB_vertex_attrib_binding", &ogl_ext_ARB_vertex_attrib_binding, Load_ARB_vertex_attrib_binding},
	{"GL_ARB_viewport_array", &ogl_ext_ARB_viewport_array, Load_ARB_viewport_array},
	{"GL_ARB_arrays_of_arrays", &ogl_ext_ARB_arrays_of_arrays, NULL},
	{"GL_ARB_clear_buffer_object", &ogl_ext_ARB_clear_buffer_object, Load_ARB_clear_buffer_object},
	{"GL_ARB_copy_image", &ogl_ext_ARB_copy_image, Load_ARB_copy_image},
	{"GL_ARB_ES3_compatibility", &ogl_ext_ARB_ES3_compatibility, NULL},
	{"GL_ARB_fragment_layer_viewport", &ogl_ext_ARB_fragment_layer_viewport, NULL},
	{"GL_ARB_framebuffer_no_attachments", &ogl_ext_ARB_framebuffer_no_attachments, Load_ARB_framebuffer_no_attachments},
	{"GL_ARB_invalidate_subdata", &ogl_ext_ARB_invalidate_subdata, Load_ARB_invalidate_subdata},
	{"GL_ARB_robust_buffer_access_behavior", &ogl_ext_ARB_robust_buffer_access_behavior, NULL},
	{"GL_ARB_stencil_texturing", &ogl_ext_ARB_stencil_texturing, NULL},
	{"GL_ARB_texture_query_levels", &ogl_ext_ARB_texture_query_levels, NULL},
	{"GL_ARB_texture_storage_multisample", &ogl_ext_ARB_texture_storage_multisample, Load_ARB_texture_storage_multisample},
	{"GL_KHR_debug", &ogl_ext_KHR_debug, Load_KHR_debug},
	{"GL_ARB_buffer_storage", &ogl_ext_ARB_buffer_storage, Load_ARB_buffer_storage},
	{"GL_ARB_clear_texture", &ogl_ext_ARB_clear_texture, Load_ARB_clear_texture},
	{"GL_ARB_enhanced_layouts", &ogl_ext_ARB_enhanced_layouts, NULL},
	{"GL_ARB_multi_bind", &ogl_ext_ARB_multi_bind, Load_ARB_multi_bind},
	{"GL_ARB_query_buffer_object", &ogl_ext_ARB_query_buffer_object, NULL},
	{"GL_ARB_texture_mirror_clamp_to_edge", &ogl_ext_ARB_texture_mirror_clamp_to_edge, NULL},
	{"GL_ARB_texture_stencil8", &ogl_ext_ARB_texture_stencil8, NULL},
	{"GL_ARB_vertex_type_10f_11f_11f_rev", &ogl_ext_ARB_vertex_type_10f_11f_11f_rev, NULL},
	{"GL_ARB_seamless_cubemap_per_texture", &ogl_ext_ARB_seamless_cubemap_per_texture, NULL},
	{"GL_ARB_clip_control", &ogl_ext_ARB_clip_control, Load_ARB_clip_control},
	{"GL_ARB_conditional_render_inverted", &ogl_ext_ARB_conditional_render_inverted, NULL},
	{"GL_ARB_cull_distance", &ogl_ext_ARB_cull_distance, NULL},
	{"GL_ARB_derivative_control", &ogl_ext_ARB_derivative_control, NULL},
	{"GL_ARB_direct_state_access", &ogl_ext_ARB_direct_state_access, Load_ARB_direct_state_access},
	{"GL_ARB_get_texture_sub_image", &ogl_ext_ARB_get_texture_sub_image, Load_ARB_get_texture_sub_image},
	{"GL_ARB_shader_texture_image_samples", &ogl_ext_ARB_shader_texture_image_samples, NULL},
	{"GL_ARB_texture_barrier", &ogl_ext_ARB_texture_barrier, Load_ARB_texture_barrier},
	{"GL_KHR_context_flush_control", &ogl_ext_KHR_context_flush_control, NULL},
	{"GL_KHR_robust_buffer_access_behavior", &ogl_ext_KHR_robust_buffer_access_behavior, NULL},
	{"GL_KHR_robustness", &ogl_ext_KHR_robustness, Load_KHR_robustness},
};

static int g_extensionMapSize = 52;

static ogl_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	ogl_StrToExtMap *currLoc = ExtensionMap;
	for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if(strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}
	
	return NULL;
}

static void ClearExtensionVars(void)
{
	ogl_ext_EXT_texture_compression_s3tc = ogl_LOAD_FAILED;
	ogl_ext_EXT_texture_sRGB = ogl_LOAD_FAILED;
	ogl_ext_EXT_texture_filter_anisotropic = ogl_LOAD_FAILED;
	ogl_ext_ARB_compressed_texture_pixel_storage = ogl_LOAD_FAILED;
	ogl_ext_ARB_conservative_depth = ogl_LOAD_FAILED;
	ogl_ext_ARB_ES2_compatibility = ogl_LOAD_FAILED;
	ogl_ext_ARB_get_program_binary = ogl_LOAD_FAILED;
	ogl_ext_ARB_explicit_uniform_location = ogl_LOAD_FAILED;
	ogl_ext_ARB_internalformat_query = ogl_LOAD_FAILED;
	ogl_ext_ARB_internalformat_query2 = ogl_LOAD_FAILED;
	ogl_ext_ARB_map_buffer_alignment = ogl_LOAD_FAILED;
	ogl_ext_ARB_program_interface_query = ogl_LOAD_FAILED;
	ogl_ext_ARB_separate_shader_objects = ogl_LOAD_FAILED;
	ogl_ext_ARB_shading_language_420pack = ogl_LOAD_FAILED;
	ogl_ext_ARB_shading_language_packing = ogl_LOAD_FAILED;
	ogl_ext_ARB_texture_buffer_range = ogl_LOAD_FAILED;
	ogl_ext_ARB_texture_storage = ogl_LOAD_FAILED;
	ogl_ext_ARB_texture_view = ogl_LOAD_FAILED;
	ogl_ext_ARB_vertex_attrib_binding = ogl_LOAD_FAILED;
	ogl_ext_ARB_viewport_array = ogl_LOAD_FAILED;
	ogl_ext_ARB_arrays_of_arrays = ogl_LOAD_FAILED;
	ogl_ext_ARB_clear_buffer_object = ogl_LOAD_FAILED;
	ogl_ext_ARB_copy_image = ogl_LOAD_FAILED;
	ogl_ext_ARB_ES3_compatibility = ogl_LOAD_FAILED;
	ogl_ext_ARB_fragment_layer_viewport = ogl_LOAD_FAILED;
	ogl_ext_ARB_framebuffer_no_attachments = ogl_LOAD_FAILED;
	ogl_ext_ARB_invalidate_subdata = ogl_LOAD_FAILED;
	ogl_ext_ARB_robust_buffer_access_behavior = ogl_LOAD_FAILED;
	ogl_ext_ARB_stencil_texturing = ogl_LOAD_FAILED;
	ogl_ext_ARB_texture_query_levels = ogl_LOAD_FAILED;
	ogl_ext_ARB_texture_storage_multisample = ogl_LOAD_FAILED;
	ogl_ext_KHR_debug = ogl_LOAD_FAILED;
	ogl_ext_ARB_buffer_storage = ogl_LOAD_FAILED;
	ogl_ext_ARB_clear_texture = ogl_LOAD_FAILED;
	ogl_ext_ARB_enhanced_layouts = ogl_LOAD_FAILED;
	ogl_ext_ARB_multi_bind = ogl_LOAD_FAILED;
	ogl_ext_ARB_query_buffer_object = ogl_LOAD_FAILED;
	ogl_ext_ARB_texture_mirror_clamp_to_edge = ogl_LOAD_FAILED;
	ogl_ext_ARB_texture_stencil8 = ogl_LOAD_FAILED;
	ogl_ext_ARB_vertex_type_10f_11f_11f_rev = ogl_LOAD_FAILED;
	ogl_ext_ARB_seamless_cubemap_per_texture = ogl_LOAD_FAILED;
	ogl_ext_ARB_clip_control = ogl_LOAD_FAILED;
	ogl_ext_ARB_conditional_render_inverted = ogl_LOAD_FAILED;
	ogl_ext_ARB_cull_distance = ogl_LOAD_FAILED;
	ogl_ext_ARB_derivative_control = ogl_LOAD_FAILED;
	ogl_ext_ARB_direct_state_access = ogl_LOAD_FAILED;
	ogl_ext_ARB_get_texture_sub_image = ogl_LOAD_FAILED;
	ogl_ext_ARB_shader_texture_image_samples = ogl_LOAD_FAILED;
	ogl_ext_ARB_texture_barrier = ogl_LOAD_FAILED;
	ogl_ext_KHR_context_flush_control = ogl_LOAD_FAILED;
	ogl_ext_KHR_robust_buffer_access_behavior = ogl_LOAD_FAILED;
	ogl_ext_KHR_robustness = ogl_LOAD_FAILED;
}


static void LoadExtByName(const char *extensionName)
{
	ogl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if(entry)
	{
		if(entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if(numFailed == 0)
			{
				*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED;
		}
	}
}


static void ProcExtsFromExtList(void)
{
	GLint iLoop;
	GLint iNumExtensions = 0;
	_ptrc_glGetIntegerv(GL_NUM_EXTENSIONS, &iNumExtensions);

	for(iLoop = 0; iLoop < iNumExtensions; iLoop++)
	{
		const char *strExtensionName = (const char *)_ptrc_glGetStringi(GL_EXTENSIONS, iLoop);
		LoadExtByName(strExtensionName);
	}
}

int ogl_LoadFunctions()
{
	int numFailed = 0;
	ClearExtensionVars();
	
	_ptrc_glGetIntegerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint *))IntGetProcAddress("glGetIntegerv");
	if(!_ptrc_glGetIntegerv) return ogl_LOAD_FAILED;
	_ptrc_glGetStringi = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glGetStringi");
	if(!_ptrc_glGetStringi) return ogl_LOAD_FAILED;
	
	ProcExtsFromExtList();
	numFailed = Load_Version_4_5();
	
	if(numFailed == 0)
		return ogl_LOAD_SUCCEEDED;
	else
		return ogl_LOAD_SUCCEEDED + numFailed;
}

static int g_major_version = 0;
static int g_minor_version = 0;

static void GetGLVersion(void)
{
	glGetIntegerv(GL_MAJOR_VERSION, &g_major_version);
	glGetIntegerv(GL_MINOR_VERSION, &g_minor_version);
}

int ogl_GetMajorVersion(void)
{
	if(g_major_version == 0)
		GetGLVersion();
	return g_major_version;
}

int ogl_GetMinorVersion(void)
{
	if(g_major_version == 0) /*Yes, check the major version to get the minor one.*/
		GetGLVersion();
	return g_minor_version;
}

int ogl_IsVersionGEQ(int majorVersion, int minorVersion)
{
	if(g_major_version == 0)
		GetGLVersion();
	
	if(majorVersion < g_major_version) return 1;
	if(majorVersion > g_major_version) return 0;
	if(minorVersion <= g_minor_version) return 1;
	return 0;
}

