
#ifndef GL_GRAPHICSDRIVER_H
#define GL_GRAPHICSDRIVER_H

#include "fltk3/Device.h"
#include "fltk3gl/gl.h"
#include "fltk3gl/glu.h"
#include <vector>

// Correct call sequence?
// glGraphicsDriver->set_current();
// draw widgets...
// fltk3::DisplayDevice::display_device()->set_current();

class GL_GraphicsDriver: public fltk3::GraphicsDriver {
    fltk3::GraphicsDriver * replacedDriver;
    int viewW, viewH;
    double lineWidth;
    std::vector<int> cpolyContours;
    
  protected:
    void RectVertices(double x, double y, double w, double h);
    
    void StartSolid();
    void EndSolid();
    
    double to_gl_x(double x) {return x + 0.5;}
    double to_gl_y(double y) {return viewH - 0.5 - y;}
    
    void gl_vertex(double x, double y);
    
  public:
    GL_GraphicsDriver(fltk3::Rectangle * rect);
    virtual ~GL_GraphicsDriver();
    
    void install();
    void uninstall();
    
    virtual void rect(int x, int y, int w, int h);
    virtual void rectf(int x, int y, int w, int h);
    virtual void line_style(int style, int width=0, char * dashes=0);
    virtual void xyline(int x, int y, int x1);
    virtual void xyline(int x, int y, int x1, int y2);
    virtual void xyline(int x, int y, int x1, int y2, int x3);
    virtual void yxline(int x, int y, int y1);
    virtual void yxline(int x, int y, int y1, int x2);
    virtual void yxline(int x, int y, int y1, int x2, int y3);
    virtual void line(int x, int y, int x1, int y1);
    virtual void line(int x, int y, int x1, int y1, int x2, int y2);
    virtual void draw(const char * str, int n, int x, int y);
    virtual void draw(int angle, const char * str, int n, int x, int y);
    virtual void rtl_draw(const char * str, int n, int x, int y);
    virtual void color(fltk3::Color c);
    virtual void color(uchar r, uchar g, uchar b);
    virtual void point(int x, int y);
    virtual void loop(int x0, int y0, int x1, int y1, int x2, int y2);
    virtual void loop(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);
    virtual void polygon(int x0, int y0, int x1, int y1, int x2, int y2);
    virtual void polygon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);
    // virtual void begin_points();
    // virtual void begin_line();
    // virtual void begin_loop();
    // virtual void begin_polygon();
    // virtual void vertex(double x, double y);
    // virtual void curve(double X0, double Y0, double X1, double Y1, double X2, double Y2, double X3, double Y3);
    virtual void circle(double x, double y, double r);
    // virtual void arc(double x, double y, double r, double start, double end);
    virtual void arc(int x, int y, int w, int h, double a1, double a2);
    virtual void pie(int x, int y, int w, int h, double a1, double a2);
    virtual void end_points();
    virtual void end_line();
    virtual void end_loop();
    virtual void end_polygon();
    // virtual void begin_complex_polygon();
    virtual void gap();
    virtual void end_complex_polygon();
    // virtual void transformed_vertex(double xf, double yf);
    
    virtual void push_clip(int x, int y, int w, int h);
    virtual int clip_box(int x, int y, int w, int h, int & X, int & Y, int & W, int & H);
    virtual int not_clipped(int x, int y, int w, int h);
    // virtual void push_no_clip();
    // virtual void pop_clip();
    virtual void restore_clip();
    
    // Images
    virtual void draw_image(const uchar * buf, int X, int Y, int W, int H, int D=3, int L=0);
    virtual void draw_image_mono(const uchar * buf, int X, int Y, int W, int H, int D=1, int L=0);
    virtual void draw_image(fltk3::DrawImageCb cb, void * data, int X, int Y, int W, int H, int D=3);
    virtual void draw_image_mono(fltk3::DrawImageCb cb, void * data, int X, int Y, int W, int H, int D=1);
    
    virtual void draw(fltk3::RGBImage * rgb, int XP, int YP, int WP, int HP, int cx, int cy);
    virtual void draw(fltk3::Pixmap * pxm, int XP, int YP, int WP, int HP, int cx, int cy);
    virtual void draw(fltk3::Bitmap * bm, int XP, int YP, int WP, int HP, int cx, int cy);
    
    virtual void font(fltk3::Font face, fltk3::Fontsize size);
    
    virtual double width(const char * str, int n);
    virtual void text_extents(const char * str, int n, int & dx, int & dy, int & w, int & h);
    virtual int height();
    virtual int descent();
    
    virtual void copy_offscreen(int x, int y, int w, int h, fltk3::Offscreen pixmap, int srcx, int srcy);
    virtual char can_do_alpha_blending();
};

#endif // GL_GRAPHICSDRIVER_H
