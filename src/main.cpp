
#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cfloat>

#include <boost/format.hpp>

#include <config.h>
#include <fltk3/fltk3.h>
#include <fltk3gl/GLWindow.h>
#include <fltk3gl/gl.h>
#include <fltk3/Box.h>

#include "GL_GraphicsDriver.h"
#include "pixfmt.h"

#include "fltk3utils.h"

using namespace std;
using boost::format;

class DiffWindow;

GL_GraphicsDriver * glGraphicsDriver = nullptr;
DiffWindow * diffWindow = nullptr;


class DiffWindow: public flu::Window {
    uint8_t * imageA;
    uint8_t * imageB;
  public:
    DiffWindow(int wx, int wy, int ww, int wh, const char * label = nullptr):
        flu::Window(wx, wy, ww, wh, label),
        imageA(nullptr),
        imageB(nullptr)
    {}
    
    void set_image_a(uint8_t * img) {
        if(imageA) delete[] imageA;
        imageA = img;
        redraw();
    }
    void set_image_b(uint8_t * img) {
        if(imageB) delete[] imageB;
        imageB = img;
        redraw();
    }
    
    void draw() {
        size_t n = 3*w()*h();
        std::vector<uint8_t> buf(n, 0);
        if(imageA && imageB) {
            cerr << "Have images, computing difference" << endl;
            for(size_t x = 0; x < w(); ++x)
            for(size_t y = 0; y < h(); ++y)
            {
                // for(size_t c = 0; c < 3; ++c)
                // {
                //     int a = imageA[3*((h() - 1 - y)*w() + x) + c];
                //     int b = imageB[3*(y*w() + x) + c];
                //     buf[3*(y*w() + x) + c] = min(255, abs(a - b));
                // }
                int a = imageA[3*((h() - 1 - y)*w() + x) + 0];
                int b = imageB[3*(y*w() + x) + 0];
                buf[3*(y*w() + x) + 0] = a;
                buf[3*(y*w() + x) + 1] = b;
                buf[3*(y*w() + x) + 2] = 255;
            }
        }
        fltk3::draw_image(&buf[0], 0, 0, w(), h(), 3, 0);
    }
};

class Sketch: public flu::Widget {
    std::function<void()> drawFn;
  public:
    Sketch(int wx, int wy, int ww, int wh, const char * label = nullptr):
        flu::Widget(wx, wy, ww, wh, label)
    {}
    
    template<typename fn_t>
    void to_draw(const fn_t & fn) {drawFn = fn;}
    
    void draw() {if(drawFn) drawFn();}
};

class GLView: public flu::FLU<fltk3::GLWindow> {
  public:
    GLView(int wx, int wy, int ww, int wh, const char * label = nullptr);
    
    void InitGL();
    void draw();
    
    void resize(int wx, int wy, int ww, int wh) {
        flu::FLU<fltk3::GLWindow>::resize(wx, wy, ww, wh);
        glViewport(0, 0, w(), h());
        redraw();
    }
};

GLView::GLView(int wx, int wy, int ww, int wh, const char * label):
    flu::FLU<fltk3::GLWindow>(wx, wy, ww, wh, label)
{
    CustomGL_Visual(this);
    // Needs a kick after the mode change to display properly
    hide();
    show();
}

void GLView::InitGL()
{
    glViewport(0, 0, w(), h());
    
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    // glClearColor(0.0, 0.0, 0.5, 1.0);
    glClearColor(0.75, 0.75, 0.75, 1.0);
}

void GLView::draw()
{
    if(!valid())
        InitGL();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // cout << "GLView::draw()" << endl;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w(), 0, h(), -1, 1);
    
    // Not a good way to do antialiasing, really need multisampling, but this may be a useful option...
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_LINE_SMOOTH);
    // glEnable(GL_POLYGON_SMOOTH);
    // glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    glGraphicsDriver->install(w(), h());
    redraw();
    draw_children();
    glGraphicsDriver->uninstall();
    
    glFlush();
    size_t n = 4*w()*h();
    uint8_t * buf = new uint8_t[n];
    glReadPixels(0, 0, w(), h(), GL_RGB, GL_UNSIGNED_BYTE, buf);
    diffWindow->set_image_a(buf);
}



class CaptureWindow: public flu::Window {
  public:
    CaptureWindow(int wx, int wy, int ww, int wh, const char * label = nullptr):
        flu::Window(wx, wy, ww, wh, label)
    {}
    
    void draw() {
        redraw();
        flu::Window::draw();
        size_t n = 4*w()*h();
        uint8_t * buf = new uint8_t[n];
        
        fltk3::read_image(buf, 0, 0, w(), h(), 0);
        diffWindow->set_image_b(buf);
    }
};



const int kButtonH = 20;

void PopulateWindow(fltk3::Window * win)
{
    int hmargin = 30;
    int w = win->w() - hmargin*2;
    int h = 20;
    int ym = 10;
    int x = hmargin;
    int y = 150;
    
    h = 90;
    flu::Widget * box = new flu::Widget(x+16, y, w-32, h, "Testing...");
    box->box(fltk3::UP_BOX);
    box->labelfont(fltk3::BOLD + fltk3::ITALIC);
    box->labelsize(36);
    box->labeltype(fltk3::SHADOW_LABEL);
    // box->on_enter([]() -> int {cout << "Enter!" << endl; return 1;});
    // box->on_leave([]() -> int {cout << "Exit!" << endl; return 1;});
    y += h + ym;
    
    flu::Button * consoleBtn = new flu::Button(x, y, w, kButtonH, "Show console");
    y += kButtonH + ym;
    // consoleBtn->callback([&ui, console] {ui.PushPane(console);});
    
    flu::Button * optionsBtn = new flu::Button(x, y, w, kButtonH, "Options");
    y += kButtonH + ym;
    // optionsBtn->callback([&ui, optionsUI] {ui.PushPane(optionsUI);});
    
    flu::CheckButton * fullscreenCB = new flu::CheckButton(x, y, w, kButtonH, "Fullscreen");
    y += kButtonH + ym;
    // fullscreenCB->callback([&fullscreen, win] {
    //     if(fullscreen)
    //         win->fullscreen_off();
    //     else
    //         win->fullscreen();
    //     fullscreen = !fullscreen;
    // });
    
    Sketch * sketch = new Sketch(0, win->h()/2, win->w(), win->h()/2);
    sketch->to_draw([sketch]{
        cerr << "####################################################" << endl;
        fltk3::color(0xFF, 0xFF, 0xFF);
        fltk3::rectf(0, 0, sketch->w(), sketch->h());
        fltk3::color(0, 0, 0);
        fltk3::rect(8, 8, sketch->w() - 16, sketch->h() - 16);
        int x = 20, y = 20;
        fltk3::rect(x, y, 16, 16);
        x += 20;
        fltk3::rectf(x, y, 16, 16);
        
        x += 20;
        fltk3::xyline(x, y + 3, x + 16);
        fltk3::xyline(x + 16, y + 7, x);
        
        x += 20;
        fltk3::xyline(x, y + 3, x + 16, y + 7);
        
        x += 20;
        fltk3::xyline(x, y + 3, x + 16, y + 7, x);
        
        x += 20;
        fltk3::yxline(x + 3, y, y + 16);
        fltk3::yxline(x + 7, y + 16, y);
        
        x += 20;
        fltk3::yxline(x + 3, y, y + 16, x + 7);
        
        x += 20;
        fltk3::yxline(x + 3, y, y + 16, x + 7, y);
        
        x += 20;
        fltk3::line(x, y, x + 6, y + 6);
        fltk3::line(x + 16, y, x + 16 - 6, y + 6);
        fltk3::line(x, y + 16, x + 6, y + 16 - 6);
        fltk3::line(x + 16, y + 16, x + 16 - 6, y + 16 - 6);
        
        // fltk3::draw(const char * str, int n, int x, int y);
        // fltk3::draw(int angle, const char * str, int n, int x, int y);
        // fltk3::rtl_draw(const char * str, int n, int x, int y);
        
        x += 20;
        fltk3::point(x + 6, y + 6);
        fltk3::point(x + 14, y + 6);
        fltk3::point(x + 14, y + 14);
        fltk3::point(x + 6, y + 14);
        
        x += 20;
        fltk3::loop(x + 6, y + 6, x + 14, y + 6, x + 14, y + 14);
        
        x += 20;
        fltk3::loop(x + 6, y + 6, x + 14, y + 6, x + 14, y + 14, x + 6, y + 14);
        
        x += 20;
        fltk3::polygon(x + 6, y + 6, x + 14, y + 6, x + 14, y + 14);
        
        x += 20;
        fltk3::polygon(x + 6, y + 6, x + 14, y + 6, x + 14, y + 14, x + 6, y + 14);
        
        x += 20;
        fltk3::circle(x + 10, y + 10, 8);
        
        x += 20;
        fltk3::arc(x + 2, y + 2, 16, 16, 0, 270);
        
        x += 20;
        fltk3::pie(x + 2, y + 2, 16, 16, 0, 270);
        fltk3::pie(40, 250, 80, 80, 0, 270);
        
        x = 20;
        y += 40;
        for(int j = 0, n = 16; j < n; ++j) {
            double th = (2.0*M_PI*j)/n;
            double c = cos(th), s = sin(th);
            fltk3::line(x + 20 + c*8, y + 20 + s*8, x + 20 + c*16, y + 20 + s*16);
        }
        
        
        x += 20;
        // fltk3::begin_points();
        // fltk3::vertex();
        // fltk3::transformed_vertex();
        // fltk3::end_points();
        
        // fltk3::begin_line();
        // fltk3::begin_loop();
        // fltk3::begin_polygon();
        
        // fltk3::begin_complex_polygon();
        // fltk3::curve(double X0, double Y0, double X1, double Y1, double X2, double Y2, double X3, double Y3);
        // fltk3::arc(double x, double y, double r, double start, double end);
        
        // TODO: Clipping, images, text...
        cerr << "====================================================" << endl;
    });
    
    win->end();
}

int main(int argc, char * argv[])
{
    CustomGL_Visual();
    flu::initialize();
    
    glGraphicsDriver = new GL_GraphicsDriver;
    
    diffWindow = new DiffWindow(1024+64, 0, 512, 720);
    diffWindow->end();
    diffWindow->show();
    
    GLView * glView = new GLView(0+64, 0, 512, 720);
    glView->begin();
    PopulateWindow(glView);
    glView->show();
    
    flu::Window * standardView = new CaptureWindow(512+64, 0, 512, 720);
    PopulateWindow(standardView);
    standardView->show();
    
    return fltk3::run();
}
