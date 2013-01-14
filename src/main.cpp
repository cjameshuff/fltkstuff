
#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cfloat>

#include <boost/format.hpp>
#include "targa_io.h"

#include <fltk3/fltk3.h>
#include <fltk3gl/GLWindow.h>
#include <fltk3gl/gl.h>
#include <fltk3/Box.h>

#include "GL_GraphicsDriver.h"

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
            for(size_t c = 0; c < 3; ++c)
            {
                int a = imageA[3*((h() - 1 - y)*w() + x) + c];
                int b = imageB[3*(y*w() + x) + c];
                buf[3*(y*w() + x) + c] = min(255, abs(a - b));
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
    GLView(int wx, int wy, int ww, int wh, const char * label = nullptr):
        flu::FLU<fltk3::GLWindow>(wx, wy, ww, wh, label)
    {}
    
    void InitGL();
    void draw();
    
    void resize(int wx, int wy, int ww, int wh) {
        flu::FLU<fltk3::GLWindow>::resize(wx, wy, ww, wh);
        glViewport(0, 0, w(), h());
        redraw();
    }
};

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
    
    glClearColor(0.0, 0.0, 0.5, 1.0);
}

void GLView::draw()
{
    if(!valid())
        InitGL();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    
    // cout << "GLView::draw()" << endl;
    
    // fltk3::GraphicsDriver * drv = fltk3::DisplayDevice::surface()->driver();
    // fltk3::DisplayDevice::surface()->driver(glGraphicsDriver);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w(), 0, h(), -1, 1);
    
    glGraphicsDriver->install(w(), h());
    // fltk3::Window::draw();
    redraw();
    draw_children();
    glGraphicsDriver->uninstall();
    
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
        // fltk3::Offscreen offscreen = fl_create_offscreen(w(), h());
        // fl_begin_offscreen(offscreen);
        redraw();
        flu::Window::draw();
        size_t n = 4*w()*h();
        uint8_t * buf = new uint8_t[n];
        
        fltk3::read_image(buf, 0, 0, w(), h(), 0);
        diffWindow->set_image_b(buf);
        // fl_end_offscreen();
        // fl_delete_offscreen(offscreen);
        // make_current();
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
        cerr << "====================================================" << endl;
    });
    
    win->end();
    win->resizable(win);
}

int main(int argc, char * argv[])
{
    flu::initialize();
    
    fltk3::gl_visual(fltk3::DOUBLE | fltk3::RGB8 | fltk3::ALPHA | fltk3::DEPTH | fltk3::MULTISAMPLE);
    
    glGraphicsDriver = new GL_GraphicsDriver;
    
    
    // flu::on_shortcut([]() -> int {cout << "shortcut!" << endl; return 1;});
    
    // UI_Controller ui;
    // bool fullscreen = false;
    diffWindow = new DiffWindow(1024+64, 0, 512, 720);
    diffWindow->end();
    diffWindow->show();
    
    // flu::Window * window = new flu::Window(50, 250, 1024, 720);
    // window->callback([&ui] {
    //     if(fltk3::event() == fltk3::SHORTCUT && fltk3::event_key() == fltk3::EscapeKey)
    //     {
    //         ui.PopPane();
    //         return;
    //     }
    //     exit(0);
    // });
    
    
    GLView * glView = new GLView(0+64, 0, 512, 720);
    glView->begin();
    PopulateWindow(glView);
    glView->show();
    
    flu::Window * standardView = new CaptureWindow(512+64, 0, 512, 720);
    PopulateWindow(standardView);
    standardView->show();
    
    // window->show(argc, argv);
    
    return fltk3::run();
}
