
#include <config.h>
#include <fltk3/run.h>

#if defined(__APPLE_QUARTZ__)
#include <OpenGL/gl.h>
#include <AGL/agl.h>
#else
#include <GL/glx.h>
#endif

#include "pixfmt.h"

#include <iostream>


void CustomGL_Visual(fltk3::GLWindow * wind)
{
#if defined(WIN32)
    uint32_t m = fltk3::DOUBLE | fltk3::RGB8 | fltk3::ALPHA | fltk3::DEPTH | fltk3::MULTISAMPLE;
    if(wind)
        wind->mode(m);
    else
        fltk3::gl_visual(m);
#else
    #if defined(USE_X11)
    GLint glVisualDesc[64] = {
        GLX_RGBA, GLX_GREEN_SIZE, 8, GLX_ALPHA_SIZE, 8,
        // GLX_ACCUM_GREEN_SIZE, 8, GLX_ACCUM_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        // GLX_STENCIL_SIZE, 1,
    #if defined(GLX_VERSION_1_1) && defined(GLX_SGIS_multisample)
        GLX_MULTISAMPLE, GLX_SAMPLES_SGIS, 8,
    #endif
        GLX_DOUBLEBUFFER,
        // GLX_STEREO,
        (int)NULL
    };
    #elif defined(__APPLE_QUARTZ__)
    GLint glVisualDesc[64] = {
        AGL_NO_RECOVERY,
        AGL_RGBA,
        AGL_GREEN_SIZE, 8,
        AGL_ALPHA_SIZE, 8,
        // AGL_ACCUM_GREEN_SIZE, 8,
        // AGL_ACCUM_ALPHA_SIZE, 8,
        AGL_DEPTH_SIZE, 24,
        // AGL_STENCIL_SIZE, 1,
        AGL_SAMPLE_BUFFERS_ARB, 1, AGL_SAMPLES_ARB, 8,
        AGL_MULTISAMPLE,
        AGL_DOUBLEBUFFER,
        // AGL_STEREO,
        (int)NULL
    };
    #endif
    if(wind)
        wind->mode(glVisualDesc);
    else
        fltk3::gl_visual(0, glVisualDesc);
#endif
}


