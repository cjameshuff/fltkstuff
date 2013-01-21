
#ifndef OGL_WINDOW_H
#define OGL_WINDOW_H

#include <fltk3/fltk3.h>
#include <fltk3gl/GLWindow.h>

class OGL_Window: public fltk3::GLWindow {
  public:
    OGL_Window(int wx, int wy, int ww, int wh, const char * label = nullptr);
    
    // Calling this is only guaranteed valid from within constructor!
    void init_mode(int m, const int * a) {
        mode_ = m;
        alist = a;
        g = 0;
    }
    
    void draw();
    
    void resize(int wx, int wy, int ww, int wh);
};

#endif // OGL_WINDOW_H
