// Notes:
// GL_MULTISAMPLE is initially turned off and controlled by line width, being
// enabled briefly for each solid shape drawn.
// 
// arc(int x, int y, int w, int h, double a1, double a2)
// gives slightly different results from the native Quartz renderer on my Mac.
// 
// OpenGL line antialiasing is fairly ugly. Line smoothing may work better, but
// is often unsupported. Polygon smoothing is *usually* unsupported. Full-screen
// antialiasing can also be implemented via the accumulation buffer, at a cost
// in performance. AA settings should probably be configurable.

#include "GL_GraphicsDriver.h"
#include "fltk3/draw.h"

#include <cmath>
#include <iostream>

using namespace std;

// #define LOG(s) cerr << "GL_GraphicsDriver::" << __func__ << s << endl
#define LOG(s) 
#define LOG_UNIMPLEMENTED(s) cerr << "*GL_GraphicsDriver::" << __func__ << s << endl
// #define LOG_UNIMPLEMENTED(s)

GL_GraphicsDriver::GL_GraphicsDriver(fltk3::Rectangle * rect):
    fltk3::GraphicsDriver()
{
    // We can't predict what some custom widgets might touch, so save everything here.
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    
    viewW = rect->w();
    viewH = rect->h();
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    // Could use the modelview matrix to do point transformations. For simplicity, not done at present.
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, viewW, 0, viewH, -1, 1);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    lineWidth = 1;
    glLineWidth(lineWidth);
    glDisable(GL_LINE_STIPPLE);
    
    glDisable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    
    // Not a good way to do antialiasing, really need multisampling, but this may be a useful option...
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_LINE_SMOOTH);
    // glEnable(GL_POLYGON_SMOOTH);
    // glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    install();
}

GL_GraphicsDriver::~GL_GraphicsDriver()
{
    uninstall();
    
    glPopAttrib();
    glPopClientAttrib();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void GL_GraphicsDriver::install() {
    replacedDriver = fltk3::DisplayDevice::display_device()->driver();
    fltk3::DisplayDevice::display_device()->driver(this);
    fltk3::DisplayDevice::display_device()->set_current();
}

void GL_GraphicsDriver::uninstall() {
    fltk3::DisplayDevice::display_device()->driver(replacedDriver);
    fltk3::DisplayDevice::display_device()->set_current();
}

void GL_GraphicsDriver::gl_vertex(double x, double y)
{
    glVertex2d(to_gl_x(x), to_gl_y(y));
}

void GL_GraphicsDriver::color(fltk3::Color c) {
    uninstall();
    GraphicsDriver::color(c);
    gl_color(c);
    install();
    LOG("(c)");
}
void GL_GraphicsDriver::color(uchar r, uchar g, uchar b) {
    uninstall();
    fltk3::Color c = fltk3::rgb_color(r, g, b);
    GraphicsDriver::color(c);
    gl_color(c);
    install();
    LOG("(r, g, b)");
}

void GL_GraphicsDriver::font(fltk3::Font face, fltk3::Fontsize size) {
    uninstall();
    GraphicsDriver::font(face, size);
    gl_font(face, size);
    install();
    LOG("()");
}

void GL_GraphicsDriver::line_style(int style, int width, char * dashes)
{
    cerr << "line_style " << style << ", " << width << ", ";
    if(dashes) {
        while(*dashes)
            cerr << (int)*dashes++;
    }
    cerr << endl;
    
    // FIXME finish implementation
    // SOLID = 0, DASH = 1, DOT = 2, DASHDOT = 3, DASHDOTDOT = 4
    // CAP_FLAT = 0x100, CAP_ROUND = 0x200, CAP_SQUARE = 0x300
    // JOIN_MITER = 0x1000, JOIN_ROUND = 0x2000, JOIN_BEVEL = 0x3000
    switch(style & 0xFF) {
        case fltk3::SOLID: glDisable(GL_LINE_STIPPLE); break;
        case fltk3::DASH: glEnable(GL_LINE_STIPPLE); glLineStipple(1, 0x7777); break;
        case fltk3::DOT: glEnable(GL_LINE_STIPPLE); glLineStipple(1, 0x5555); break;
        case fltk3::DASHDOT: glEnable(GL_LINE_STIPPLE); glLineStipple(1, 0x2727); break;
        case fltk3::DASHDOTDOT: glEnable(GL_LINE_STIPPLE); glLineStipple(1, 0x5757); break;
        default: glDisable(GL_LINE_STIPPLE); break;
    }
    
    lineWidth = max(1, width);
    glLineWidth(lineWidth);
    
    if(lineWidth < 1.5)
        glDisable(GL_MULTISAMPLE);
    else
        glEnable(GL_MULTISAMPLE);
    
    LOG_UNIMPLEMENTED("()");
}

void GL_GraphicsDriver::StartSolid() {
    if(lineWidth < 1.5)
        glEnable(GL_MULTISAMPLE);
}
void GL_GraphicsDriver::EndSolid() {
    if(lineWidth < 1.5)
        glDisable(GL_MULTISAMPLE);
}


// ****************************************************************************
// Drawing commands
// ****************************************************************************

void GL_GraphicsDriver::RectVertices(double x, double y, double w, double h)
{
    if(w < 0) {
        w = -w;
        x -= w;
    }
    if(h < 0) {
        h = -h;
        y -= h;
    }
    double x0 = x + origin_x();
    double y0 = y + origin_y();
    double x1 = x0 + w;
    double y1 = y0 + h;
    gl_vertex(x0, y0);
    gl_vertex(x0, y1);
    gl_vertex(x1, y1);
    gl_vertex(x1, y0);
}


void GL_GraphicsDriver::rect(int x, int y, int w, int h)
{
    glBegin(GL_LINE_LOOP);
    RectVertices(x, y, w - 1, h - 1);
    glEnd();
    LOG("()");
}

void GL_GraphicsDriver::rectf(int x, int y, int w, int h)
{
    StartSolid();
    glBegin(GL_POLYGON);
    // Note offset, required for clear drawing/pixel alignment
    RectVertices(x - 0.5, y - 0.5, w, h);
    glEnd();
    EndSolid();
    LOG("()");
}


void GL_GraphicsDriver::xyline(int x, int y, int x1)
{
    int ox = origin_x(), oy = origin_y();
    if(x1 > x)
        x1 += 1;
    else
        x1 -= 1;
    
    glBegin(GL_LINES);
    gl_vertex(ox + x, oy + y);
    gl_vertex(ox + x1, oy + y);
    glEnd();
    LOG("(x)");
}

void GL_GraphicsDriver::xyline(int x, int y, int x1, int y2)
{
    int ox = origin_x(), oy = origin_y();
    if(y2 > y)
        y2 += 1;
    else
        y2 -= 1;
    glBegin(GL_LINE_STRIP);
    gl_vertex(ox + x, oy + y);
    gl_vertex(ox + x1, oy + y);
    gl_vertex(ox + x1, oy + y2);
    glEnd();
    LOG("(xy)");
}

void GL_GraphicsDriver::xyline(int x, int y, int x1, int y2, int x3)
{
    int ox = origin_x(), oy = origin_y();
    if(x3 > x1)
        x3 += 1;
    else
        x3 -= 1;
    glBegin(GL_LINE_STRIP);
    gl_vertex(ox + x, oy + y);
    gl_vertex(ox + x1, oy + y);
    gl_vertex(ox + x1, oy + y2);
    gl_vertex(ox + x3, oy + y2);
    glEnd();
    LOG("(xyx)");
    
}

void GL_GraphicsDriver::yxline(int x, int y, int y1)
{
    int ox = origin_x(), oy = origin_y();
    if(y1 > y)
        y1 += 1;
    else
        y1 -= 1;
    
    glBegin(GL_LINES);
    gl_vertex(ox + x, oy + y);
    gl_vertex(ox + x, oy + y1);
    glEnd();
    LOG("(y)");
}

void GL_GraphicsDriver::yxline(int x, int y, int y1, int x2)
{
    int ox = origin_x(), oy = origin_y();
    if(x2 > x)
        x2 += 1;
    else
        x2 -= 1;
    
    glBegin(GL_LINE_STRIP);
    gl_vertex(ox + x, oy + y);
    gl_vertex(ox + x, oy + y1);
    gl_vertex(ox + x2, oy + y1);
    glEnd();
    LOG("(yx)");
}

void GL_GraphicsDriver::yxline(int x, int y, int y1, int x2, int y3)
{
    int ox = origin_x(), oy = origin_y();
    if(y3 > y1)
        y3 += 1;
    else
        y3 -= 1;
    glBegin(GL_LINE_STRIP);
    gl_vertex(ox + x, oy + y);
    gl_vertex(ox + x, oy + y1);
    gl_vertex(ox + x2, oy + y1);
    gl_vertex(ox + x2, oy + y3);
    glEnd();
    LOG("(yxy)");
}

void GL_GraphicsDriver::line(int x, int y, int x1, int y1)
{
    int ox = origin_x(), oy = origin_y();
    
    glBegin(GL_LINES);
    gl_vertex(ox + x, oy + y);
    gl_vertex(ox + x1, oy + y1);
    glEnd();
    // Lines consistently seem one pixel short. Plop a point down to finish them.
    glBegin(GL_POINTS);
    gl_vertex(ox + x1, oy + y1);
    glEnd();
    LOG("()");
}

void GL_GraphicsDriver::line(int x, int y, int x1, int y1, int x2, int y2)
{
    int ox = origin_x(), oy = origin_y();
    glBegin(GL_LINE_STRIP);
    gl_vertex(ox + x, oy + y);
    gl_vertex(ox + x1, oy + y1);
    gl_vertex(ox + x2, oy + y2);
    glEnd();
    LOG("(2)");
}


void GL_GraphicsDriver::point(int x, int y)
{
    x += origin_x();
    y += origin_y();
    glBegin(GL_POINTS);
    gl_vertex(x, y);
    glEnd();
    LOG("()");
}


void GL_GraphicsDriver::loop(int x0, int y0, int x1, int y1, int x2, int y2)
{
    int ox = origin_x(), oy = origin_y();
    glBegin(GL_LINE_STRIP);
    gl_vertex(ox + x0, oy + y0);
    gl_vertex(ox + x1, oy + y1);
    gl_vertex(ox + x2, oy + y2);
    gl_vertex(ox + x0, oy + y0);
    glEnd();
    LOG("()");
}

void GL_GraphicsDriver::loop(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3)
{
    int ox = origin_x(), oy = origin_y();
    glBegin(GL_LINE_STRIP);
    gl_vertex(ox + x0, oy + y0);
    gl_vertex(ox + x1, oy + y1);
    gl_vertex(ox + x2, oy + y2);
    gl_vertex(ox + x3, oy + y3);
    gl_vertex(ox + x0, oy + y0);
    glEnd();
    LOG("()");
}

void GL_GraphicsDriver::polygon(int x0, int y0, int x1, int y1, int x2, int y2)
{
    int ox = origin_x(), oy = origin_y();
    StartSolid();
    glBegin(GL_TRIANGLES);
    gl_vertex(ox + x0, oy + y0);
    gl_vertex(ox + x1, oy + y1);
    gl_vertex(ox + x2, oy + y2);
    glEnd();
    EndSolid();
    LOG("()");
}

void GL_GraphicsDriver::polygon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3)
{
    int ox = origin_x(), oy = origin_y();
    StartSolid();
    glBegin(GL_QUADS);
    gl_vertex(ox + x0, oy + y0);
    gl_vertex(ox + x1, oy + y1);
    gl_vertex(ox + x2, oy + y2);
    gl_vertex(ox + x3, oy + y3);
    glEnd();
    EndSolid();
    LOG("()");
}

void GL_GraphicsDriver::circle(double x, double y, double r)
{
    int n = min(360.0, M_PI*r);
    double cx = x + origin_x();
    double cy = y + origin_y();
    glBegin(GL_LINE_LOOP);
    for(int j = 0; j < n; ++j) {
        double th = (2.0*M_PI*j)/n;
        gl_vertex(cx + cos(th)*r, cy + sin(th)*r);
    }
    glEnd();
    LOG("()");
}

void GL_GraphicsDriver::arc(int x, int y, int w, int h, double a1, double a2)
{
    w -= 1; h -= 1;
    // Arcs are apparently drawn 1 pixel smaller than specified...line width related?
    int n = min(360.0, M_PI*(w + h)/4.0*(a2 - a1)/360.0);
    double xr = w/2.0;
    double yr = h/2.0;
    double cx = x + origin_x() + xr;
    double cy = y + origin_y() + yr;
    glBegin(GL_LINE_STRIP);
    for(int j = 0; j < n; ++j) {
        double th = (2.0*M_PI/360.0)*(((double)j/(n-1))*(a2 - a1) + a1);
        gl_vertex(cx + cos(th)*xr, cy - sin(th)*yr);
    }
    glEnd();
    LOG("()");
}

void GL_GraphicsDriver::pie(int x, int y, int w, int h, double a1, double a2)
{
    int n = min(360.0, M_PI*(w + h)/4.0*(a2 - a1)/360.0);
    double xr = w/2.0;
    double yr = h/2.0;
    // Note offset, required for clear drawing/pixel alignment
    double cx = x + origin_x() + xr - 0.5;
    double cy = y + origin_y() + yr - 0.5;
    StartSolid();
    glBegin(GL_TRIANGLE_FAN);
    gl_vertex(cx, cy);
    for(int j = 0; j < n; ++j) {
        double th = (2.0*M_PI/360.0)*(((double)j/(n-1))*(a2 - a1) + a1);
        gl_vertex(cx + cos(th)*xr, cy - sin(th)*yr);
    }
    glEnd();
    EndSolid();
    LOG("()");
}


void GL_GraphicsDriver::end_points()
{
    glBegin(GL_POINTS);
    XPOINT * p = vertices();
    for(int j = 0, n = vertex_no(); j < n; ++j)
        gl_vertex(p[j].x, p[j].y);
    glEnd();
    LOG("()");
}

void GL_GraphicsDriver::end_line()
{
    StartSolid();// Not actually solid, but treated as such for AA
    glBegin(GL_LINE_STRIP);
    XPOINT * p = vertices();
    for(int j = 0, n = vertex_no(); j < n; ++j)
        gl_vertex(p[j].x, p[j].y);
    glEnd();
    EndSolid();
    LOG("()");
}

void GL_GraphicsDriver::end_loop()
{
    StartSolid();// Not actually solid, but treated as such for AA
    glBegin(GL_LINE_LOOP);
    XPOINT * p = vertices();
    for(int j = 0, n = vertex_no(); j < n; ++j)
        gl_vertex(p[j].x, p[j].y);
    glEnd();
    EndSolid();
    LOG("()");
}

void GL_GraphicsDriver::end_polygon()
{
    StartSolid();
    glBegin(GL_POLYGON);
    XPOINT * p = vertices();
    for(int j = 0, n = vertex_no(); j < n; ++j)
        gl_vertex(p[j].x, p[j].y);
    glEnd();
    EndSolid();
    LOG("()");
}


// void GL_GraphicsDriver::begin_complex_polygon()
// {
//     fltk3::GraphicsDriver::begin_complex_polygon();
//     LOG("()");
// }

static void TessCombineCB(GLdouble coords[3], GLvoid * points[4], GLfloat weights[4], GLvoid ** vertData, GLvoid * userData)
{
    // Tesselator generated a new vertex, needs somewhere to put it
    double * newPt = new double[3];
    ((std::vector<double *>*)userData)->push_back(newPt);
    newPt[0] = coords[0];
    newPt[1] = coords[1];
    newPt[2] = coords[2];
    *vertData = newPt;
}

static void TessErrorCB(GLenum errorCode)
{
    cerr << "GLU tesselation error: " << gluErrorString(errorCode) << endl;
}

void GL_GraphicsDriver::end_complex_polygon()
{
    XPOINT * p = vertices();
    std::vector<double> cpolyPts;
    std::vector<double *> newPts;
    
    // Copy points to temp buffer
    for(int j = 0, n = vertex_no(); j < n; ++j) {
        cpolyPts.push_back(to_gl_x(p[j].x));
        cpolyPts.push_back(to_gl_y(p[j].y));
        cpolyPts.push_back(0.0);
    }
    
    // Tesselate polygon
    StartSolid();
    GLUtriangulatorObj * cpoly = gluNewTess();
    gluTessCallback(cpoly, GLU_TESS_BEGIN, (GLvoid (*)())glBegin);
    gluTessCallback(cpoly, GLU_TESS_VERTEX, (GLvoid (*)())glVertex3dv);
    gluTessCallback(cpoly, GLU_TESS_COMBINE_DATA, (GLvoid (*)())TessCombineCB);
    gluTessCallback(cpoly, GLU_TESS_END, (GLvoid (*)())glEnd);
    gluTessCallback(cpoly, GLU_TESS_ERROR, (GLvoid (*)())TessErrorCB);
    
    gluTessBeginPolygon(cpoly, (GLvoid*)&newPts);
    gluTessBeginContour(cpoly);
    
    std::vector<int>::iterator cont = cpolyContours.begin();
    for(int j = 0, n = vertex_no(); j < n; ++j)
    {
        if(cont != cpolyContours.end() && *cont == j) {
            gluTessEndContour(cpoly);
            gluTessBeginContour(cpoly);
            ++cont;
        }
        gluTessVertex(cpoly, &(cpolyPts[j*3]), &(cpolyPts[j*3]));
    }
    
    gluTessEndContour(cpoly);
    gluTessEndPolygon(cpoly);
    gluDeleteTess(cpoly);
    
    std::vector<double *>::iterator np = newPts.begin();
    while(np != newPts.end()) {
        delete[] *np;
        ++np;
    }
    
    cpolyContours.clear();
    EndSolid();
    LOG("()");
}

void GL_GraphicsDriver::gap()
{
    // As far as I can tell, "gap" breaks to a new contour polygon.
    // GraphicsDriver::gap() does something rather obscure, perhaps
    // closing the previous contour.
    // Instead, here we just make a note of the index.
    if(!cpolyContours.empty() && vertex_no() == cpolyContours.back())
        return;// No points since last call, ignore
    
    cpolyContours.push_back(vertex_no());
    LOG("()");
}


// ****************************************************************************
// Clipping
// This needs attention. Clipping might best be done with the stencil buffer.
// ****************************************************************************

void GL_GraphicsDriver::push_clip(int x, int y, int w, int h)
{
    LOG_UNIMPLEMENTED("()");
}
int GL_GraphicsDriver::clip_box(int x, int y, int w, int h, int & X, int & Y, int & W, int & H) {
    LOG_UNIMPLEMENTED("()");
    // Simply return given bounding rect
    X = x;
    Y = y;
    W = w;
    H = h;
    return 0;
}
int GL_GraphicsDriver::not_clipped(int x, int y, int w, int h) {
    LOG_UNIMPLEMENTED("()");
    return 1;// Rect is assumed to be visible.
}
void GL_GraphicsDriver::restore_clip() {
    LOG_UNIMPLEMENTED("()");
}


// ****************************************************************************
// Images
// ****************************************************************************

void GL_GraphicsDriver::draw_image(const uchar * buf, int X, int Y, int W, int H, int D, int L)
{
    if(L == 0)
        L = W*D;
    
    glPixelZoom((D < 0)? -1 : 1, (L < 0)? 1 : -1);
    
    if(D < 0) {
        D = -D;
        X += W;
    }
    if(L < 0) {
        L = -L;
        Y -= H;
    }
    
    glRasterPos2i(origin_x() + X, viewH - (origin_y() + Y));
    glPixelStorei(GL_UNPACK_ROW_LENGTH, L/D);
    switch(D)
    {
        case 1: glDrawPixels(W, H, GL_LUMINANCE, GL_UNSIGNED_BYTE, buf); break;
        case 2: glDrawPixels(W, H, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, buf); break;
        case 3: glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, buf); break;
        case 4: glDrawPixels(W, H, GL_RGBA, GL_UNSIGNED_BYTE, buf); break;
        default:
            cerr << __func__ << "Image depth not supported by GL_GraphicsDriver" << endl;
    }
    LOG("(const uchar * buf)");
}
void GL_GraphicsDriver::draw_image_mono(const uchar * buf, int X, int Y, int W, int H, int D, int L) {
    draw_image(buf, X, Y, W, H, D, L);
    LOG("(const uchar * buf)");
}

void GL_GraphicsDriver::draw_image(fltk3::DrawImageCb cb, void * data, int X, int Y, int W, int H, int D) {
    // Reconstruct image
    uint8_t * buf = new uint8_t[D*W*H];
    for(int y = 0; y < H; ++y)
        cb(data, 0, y, W, buf + D*W*y);
    
    draw_image(buf, X, Y, W, H, D, 0);
    delete[] buf;
    LOG("(fltk3::DrawImageCb cb)");
}
void GL_GraphicsDriver::draw_image_mono(fltk3::DrawImageCb cb, void * data, int X, int Y, int W, int H, int D) {
    draw_image(cb, data, X, Y, W, H, D);
    LOG("(fltk3::DrawImageCb cb)");
}


void draw_empty(fltk3::Image * img, int X, int Y) {
    if(img->w() > 0 && img->h() > 0) {
        fltk3::color(fltk3::FOREGROUND_COLOR);
        fltk3::rect(X, Y, img->w(), img->h());
        fltk3::line(X, Y, X + img->w() - 1, Y + img->h() - 1);
        fltk3::line(X, Y + img->h() - 1, X + img->w() - 1, Y);
    }
}
void GL_GraphicsDriver::draw(fltk3::RGBImage * rgb, int X, int Y, int W, int H, int cx, int cy) {
    // depth must be > 0, must have data
    if(rgb->d() == 0 || !rgb->data()) {
        draw_empty(rgb, X, Y);
        return;
    }
    if(W > rgb->w() - cx)
        W = rgb->w() - cx;
    if(H > rgb->h() - cy)
        H = rgb->h() - cy;
    
    // FIXME: Not at all sure if this is the right behavior...
    const uint8_t * data = (const uint8_t *)rgb->data() + rgb->d()*(cy*rgb->ld() + cx);
    draw_image(data, X, Y, W, H, rgb->d(), rgb->ld());
    LOG("(fltk3::RGBImage)");
}
void GL_GraphicsDriver::draw(fltk3::Pixmap * pxm, int XP, int YP, int WP, int HP, int cx, int cy) {
    LOG_UNIMPLEMENTED("(fltk3::Pixmap)");
}
void GL_GraphicsDriver::draw(fltk3::Bitmap * bm, int XP, int YP, int WP, int HP, int cx, int cy) {
    LOG_UNIMPLEMENTED("(fltk3::Bitmap)");
}

void GL_GraphicsDriver::copy_offscreen(int x, int y, int w, int h, fltk3::Offscreen pixmap, int srcx, int srcy)
{
    LOG_UNIMPLEMENTED("()");
}


// ****************************************************************************
// Text
// ****************************************************************************

void GL_GraphicsDriver::draw(const char * str, int n, int x, int y) {
    uninstall();
    x += origin_x();
    y += origin_y();
    gl_draw(str, n, (int)to_gl_x(x), (int)to_gl_y(y));
    install();
    LOG("()");
}
void GL_GraphicsDriver::draw(int angle, const char * str, int n, int x, int y) {
    uninstall();
    // FIXME
    x += origin_x();
    y += origin_y();
    gl_draw(str, n, (int)to_gl_x(x), (int)to_gl_y(y));
    install();
    LOG_UNIMPLEMENTED("(angle)");
}
void GL_GraphicsDriver::rtl_draw(const char * str, int n, int x, int y) {
    uninstall();
    // FIXME
    x += origin_x();
    y += origin_y();
    gl_draw(str, n, (int)to_gl_x(x), (int)to_gl_y(y));
    install();
    LOG_UNIMPLEMENTED("()");
}


// FIXME: make this non-ugly. Doing this properly probably will require changes in FLTK...
double GL_GraphicsDriver::width(const char * str, int n) {
    uninstall();
    int w = gl_width(str, n);
    install();
    LOG("()");
    return w;
}
void GL_GraphicsDriver::text_extents(const char * str, int n, int & dx, int & dy, int & w, int & h) {
    uninstall();
    dx = 0;
    dy = gl_descent();
    w = gl_width(str, n);
    h = gl_height();
    install();
    LOG("()");
}
int GL_GraphicsDriver::height() {
    uninstall();
    int h = gl_height();
    install();
    LOG("()");
    return h;
}
int GL_GraphicsDriver::descent() {
    uninstall();
    int d = gl_descent();
    install();
    LOG("()");
    return d;
}


// ****************************************************************************
// Misc
// ****************************************************************************

char GL_GraphicsDriver::can_do_alpha_blending() {
    LOG_UNIMPLEMENTED("()");
    return 1;
    // return 0;
}

