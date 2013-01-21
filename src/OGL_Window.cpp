
#include "OGL_Window.h"
#include "pixfmt.h"
#include "GL_GraphicsDriver.h"

OGL_Window::OGL_Window(int wx, int wy, int ww, int wh, const char * label):
    fltk3::GLWindow(wx, wy, ww, wh, label)
{
    // We really need multisampling for decent results. Standard mode does not
    // support it.
    CustomGL_Visual(this);
    // fltk3::GLWindow() calls end(), but we don't want to end.
    begin();
}

void OGL_Window::draw()
{
    GL_GraphicsDriver glgd(this);
    redraw();
    fltk3::Window::draw();
}


void OGL_Window::resize(int wx, int wy, int ww, int wh)
{
    fltk3::GLWindow::resize(wx, wy, ww, wh);
    glViewport(0, 0, w(), h());
    redraw();
}

