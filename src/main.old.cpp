//
// Graphics Framework.
// Copyright (C) 2008 Department of Computer Science, University of Copenhagen
//



/*******************************************************************\
*                                                                   *
*                              M e n u                              *
*                                                                   *
\*******************************************************************/

int Menu;

/*******************************************************************\
*                                                                   *
*        D r a w   t h e   C o o r d i n a t e   S y s t e m        *
*                                                                   *
\*******************************************************************/

#define DRAWCOORDINATESYSTEM 0


/*******************************************************************\
*                                                                   *
*           D r a w   L i n e s   o r   T r i a n g l e s           *
*                                                                   *
\*******************************************************************/

#define DRAWGRID            1
#define DRAWPOINTS          1
#define DEBUGPOINTS         1
#define DRAWLINES           1
#define DEBUGLINES          1
#define DRAWTRIANGLES       1
#define DEBUGTRIANGLES      1
#define GOURAUDTRIANGLES    1
#define HIDDENSURFACES      1
#define PHONGTRIANGLES      1
#define FOLEYFIGURES        1 
#define KLEINBOTTLE         1
#define KLEINBOTTLEINTERIOR 1
#define TEAPOT              1


/*******************************************************************\
*                                                                   *
*                      K l e i n   B o t t l e                      *
*                                                                   *
\*******************************************************************/

#define KLEINBOTTOMWIREFRAME 0
#define KLEINHANDLEWIREFRAME 0
#define KLEINTOPWIREFRAME    0
#define KLEINMIDDLEWIREFRAME 0

#define KLEINNORMALS         0

/*******************************************************************\
*                                                                   *
*                     P h o n g   S u r f a c e                     *
*                                                                   *
\*******************************************************************/

#define PHONGNORMALS         0

/*******************************************************************\
*                                                                   *
*                   B e z i e r   S u r f a c e s                   *
*                                                                   *
\*******************************************************************/

#define BEZIERNORMALS        0


/*******************************************************************\
*                                                                   *
*               L i n e s     o r   T r i a n g l e s               *
*                                                                   *
\*******************************************************************/

static char figure       = '0';

static int  GridVisible  =  0;
static int  UnitLength   =  1;
static int  XSpacing     =  1;
static int  YSpacing     =  1;
static int  v            =  0;
static int  l            =  0;

// icosahedron triangle number
static int  t            = -1;
static int  tmax         = 20;

static int  t_subdiv     =  -1;
static int  t_subdiv_max =   4;


/*******************************************************************\
*                                                                   *
*                       W i n d o w   S i z e                       *
*                                                                   *
\*******************************************************************/

int winWidth  = 1024;
int winHeight = 768;


/*******************************************************************\
*                                                                   *
*                  B i l l   G a t e s '   S h i t                  *
*                                                                   *
\*******************************************************************/

#ifdef WIN32
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#  undef WIN32_LEAN_AND_MEAN
#  undef NOMINMAX
#endif

/*******************************************************************\
*                                                                   *
*                     I n c l u d e   F i l e s                     *
*                                                                   *
\*******************************************************************/

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <sstream>


//For MAC OS X
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif


#include "graphics/graphics.h"
#include "solution/linear_interpolator.h"
#include "solution/point_rasterizer.h"
#include "solution/line_rasterizer.h"
#include "solution/triangle_rasterizer.h"
#include "solution/math_types.h"
#include "solution/camera.h"
#include "solution/vertex_program.h"
#include "solution/fragment_program.h"
#include "solution/transformations.h"
#include "solution/boundingbox.h"
#include "solution/readbezierpatches.h"


/*******************************************************************\
*                                                                   *
*                      G l o b a l   T y p e s                      *
*                                                                   *
\*******************************************************************/

typedef enum { ControlGrid = 1, ShadedPatch = 2 } DrawStyle;


/*******************************************************************\
*                                                                   *
*                  G l o b a l   V a r i a b l e s                  *
*                                                                   *
\*******************************************************************/

using namespace graphics;

MyCamera<MyMathTypes>                  camera;
RenderPipeline<MyMathTypes>            render_pipeline;
MyIdentityVertexProgram<MyMathTypes>   identity_vertex_program;
MyTransformVertexProgram<MyMathTypes>  transform_vertex_program;
MyIdentityFragmentProgram<MyMathTypes> identity_fragment_program;
MyPhongFragmentProgram<MyMathTypes>    phong_fragment_program;


//LinearInterpolator<MyMathTypes,       MyMathTypes::vector3_type> linear_interpolator;
//MyLineRasterizer<MyMathTypes, linear_interpolator>     line_rasterizer;

MyPointRasterizer<MyMathTypes>        point_rasterizer;
MyLineRasterizer<MyMathTypes>         line_rasterizer;
MyTriangleRasterizer<MyMathTypes>     triangle_rasterizer;


/*******************************************************************\
*                                                                   *
*                            C o l o r s                            *
*                                                                   *
\*******************************************************************/

MyMathTypes::vector3_type cblack(0.0, 0.0, 0.0);
MyMathTypes::vector3_type cgray(0.5, 0.5, 0.5);
MyMathTypes::vector3_type cwhite(1.0, 1.0, 1.0);
MyMathTypes::vector3_type cred(1.0, 0.0, 0.0);
MyMathTypes::vector3_type cgreen(0.0, 1.0, 0.0);
MyMathTypes::vector3_type cblue(0.0, 0.0, 1.0);
MyMathTypes::vector3_type cyellow(255.0 / 255.0, 245.0 / 255.0, 6.0 / 255.0);



/*******************************************************************\
*                                                                   *
*                  C l a m p ( r e a l _ t y p e )                  *
*                                                                   *
\*******************************************************************/

MyMathTypes::real_type Clamp(MyMathTypes::real_type const& value)
{
    MyMathTypes::real_type result = value;
    if (value < 0.0) result = 0.0;
    if (value > 1.0) result = 1.0;
    
    return result;
}


/*******************************************************************\
*                                                                   *
*          D r a w W o r l C o o r d i n a t e S y s t e m          *
*                                                                   *
\*******************************************************************/

void DrawWorldCoordinateSystem(MyMathTypes::real_type ScaleFactor = 10.0)
{
    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    MyMathTypes::vector3_type Origo(0, 0, 0);
    MyMathTypes::vector3_type Xaxis(1, 0, 0);
    MyMathTypes::vector3_type Yaxis(0, 1, 0);
    MyMathTypes::vector3_type Zaxis(0, 0, 1);

    render_pipeline.draw_line(Origo, cred,   ScaleFactor * Xaxis, cred);
    render_pipeline.draw_line(Origo, cgreen, ScaleFactor * Yaxis, cgreen);
    render_pipeline.draw_line(Origo, cblue,  ScaleFactor * Zaxis, cblue);
}


/*******************************************************************\
*                                                                   *
*                 I n i t i a l i z e   C a m e r a                 *
*                                                                   *
\*******************************************************************/

void InitCamera()
{
    render_pipeline.set_resolution(winWidth, winHeight);
    MyMathTypes::vector3_type vrp(0, 0, -winWidth / 2.0);
    MyMathTypes::vector3_type vpn(0, 0, 1);
    MyMathTypes::vector3_type vup(0, 1, 0);
    MyMathTypes::vector3_type prp(0, 0, winWidth / 2.0);

    MyMathTypes::vector2_type lower_left(-winWidth / 2.0, -winHeight / 2.0);
    MyMathTypes::vector2_type upper_right(winWidth / 2.0,  winHeight / 2.0);

    MyMathTypes::real_type    front_plane(-100.0);
    MyMathTypes::real_type    back_plane(0.0);

    camera.set_projection(vrp, vpn, vup, prp,
                          lower_left, upper_right,
                          front_plane, back_plane,
                          winWidth, winHeight);
}


/*******************************************************************\
*                                                                   *
*                    R e s e t   C a m e r a ( )                    *
*                                                                   *
\*******************************************************************/

void ResetCamera()
{
    
    render_pipeline.set_resolution(winWidth, winHeight);
    camera.init(render_pipeline);
    camera.reset();
}


/*******************************************************************\
*                                                                   *
*                        D r a w G r i d ( )                        *
*                                                                   *
\*******************************************************************/

void DrawGrid()
{
    // Do your magic stuff here if you need to - for future expansion.
}


/*******************************************************************\
*                                                                   *
*                       D r a w P o i n t  ( )                      *
*                                                                   *
\*******************************************************************/

void DrawPoint()
{
    render_pipeline.load_rasterizer(point_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    // Do the fun stuff here

    int x_start = 150;
    int x_width = 400;
    int x_stop  = x_start + x_width;

    int y_start = 200;
    int y_width = 400;
    int y_stop  = y_start + y_width;

#ifdef KENNY_ZBUFFER
    MyMathTypes::real_type depth =  0.5;
#else
    MyMathTypes::real_type depth = -0.5;
#endif

    for (int x = x_start; x < x_stop; ++x) {
	for (int y = y_start; y < y_stop; ++y) {
	    MyMathTypes::vector3_type point(x, y, depth);
	    render_pipeline.draw_point(point, cyellow);
	}
    }

    x_start = 250;
    x_width = 500;
    x_stop  = x_start + x_width;

    y_start = 100;
    y_width = 400;
    y_stop  = y_start + y_width;

#ifdef KENNY_ZBUFFER
    depth =  0.75;
#else
    depth = -0.75;
#endif

    for (int x = x_start; x < x_stop; ++x) {
	for (int y = y_start; y < y_stop; ++y) {
	    MyMathTypes::vector3_type point(x, y, depth);
	    render_pipeline.draw_point(point, cred);
	}
    }
}


/*******************************************************************\
*                                                                   *
*                      D e b u g P o i n t  ( )                     *
*                                                                   *
\*******************************************************************/

void DebugPoint()
{
    UnitLength = 50;
    XSpacing   = 1;
    YSpacing   = 1;

    render_pipeline.load_rasterizer(point_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    render_pipeline.DebugOn();
    render_pipeline.unit_length(UnitLength);

    // Do the fun stuff here

    int x_start = 150 / UnitLength;
    int x_width = 400 / UnitLength;
    int x_stop  = x_start + x_width;

    int y_start = 200 / UnitLength;
    int y_width = 400 / UnitLength;
    int y_stop  = y_start + y_width;

#ifdef KENNY_ZBUFFER
    int depth =  0.5;
#else
    int depth = -0.5;
#endif

    for (int x = x_start; x < x_stop; ++x) {
	for (int y = y_start; y < y_stop; ++y) {
	    MyMathTypes::vector3_type point(x, y, depth);
	    render_pipeline.draw_point(point, cyellow);
	}
    }

    x_start = 250 / UnitLength;
    x_width = 500 / UnitLength;
    x_stop  = x_start + x_width;

    y_start = 100 / UnitLength;
    y_width = 400 / UnitLength;
    y_stop  = y_start + y_width;

#ifdef KENNY_ZBUFFER
    depth =  0.75;
#else
    depth = -0.75;
#endif

    for (int x = x_start; x < x_stop; ++x) {
	for (int y = y_start; y < y_stop; ++y) {
	    MyMathTypes::vector3_type point(x, y, depth);
	    render_pipeline.draw_point(point, cred);
	}
    }

    render_pipeline.draw_grid(XSpacing, YSpacing, cblack);
    render_pipeline.DebugOff();
    render_pipeline.unit_length(1);
}


/*******************************************************************\
*                                                                   *
*                       D r a w L i n e s ( )                       *
*                                                                   *
\*******************************************************************/

void DrawLines()
{
    // TestProgram: It tests drawing of lines.
    MyMathTypes::vector3_type  l0(  100.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  l1(  150.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  l2(  100.0, 150.0, 0.0 );
    MyMathTypes::vector3_type  l3(   50.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  l4(  100.0,  50.0, 0.0 );
    MyMathTypes::vector3_type  l5(  200.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  l6(  100.0, 200.0, 0.0 );


    render_pipeline.load_rasterizer( line_rasterizer );
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);


    render_pipeline.draw_line( l0, cwhite, l1, cwhite );
    render_pipeline.draw_line( l0, cwhite, l2, cwhite );
    render_pipeline.draw_line( l0, cwhite, l3, cwhite );
    render_pipeline.draw_line( l0, cwhite, l4, cwhite );

    render_pipeline.draw_line( l1, cwhite, l2, cwhite );
    render_pipeline.draw_line( l2, cwhite, l3, cwhite );
    render_pipeline.draw_line( l3, cwhite, l4, cwhite );
    render_pipeline.draw_line( l4, cwhite, l1, cwhite );


    // Draw a circle
    double Nedges =  50;
    double Radius = 300;

    double DeltaAngle = 2.0 * M_PI / Nedges;
    double AngleStop  = 2.0 * M_PI;
    MyMathTypes::vector3_type Center(winWidth / 2, winHeight / 2, 0);
    MyMathTypes::vector3_type Pnew;
    MyMathTypes::vector3_type Pold(Center[1] + Radius, Center[2], 0.0);
    MyMathTypes::vector3_type Pstart = Pold;

    for (double Angle = DeltaAngle; Angle < AngleStop; Angle += DeltaAngle) {
	render_pipeline.draw_line(Center, cblack, Pold, cwhite);
	Pnew[1] = Center[1] + Radius * cos(Angle);
	Pnew[2] = Center[2] + Radius * sin(Angle);
	Pnew[3] = 0.0;
	render_pipeline.draw_line(Pold, cwhite, Pnew, cwhite);
	Pold = Pnew;
    }
    render_pipeline.draw_line(Center, cblack, Pold, cwhite);
    render_pipeline.draw_line(Pold, cwhite, Pstart, cwhite);
}


/*******************************************************************\
*                                                                   *
*                   L o c a l   L i n e   D a t a                   *
*                                                                   *
\*******************************************************************/

#if 1
MyMathTypes::vector3_type LineStop[] = {
    MyMathTypes::vector3_type(18.0,  7.0, 0.0),
//
    MyMathTypes::vector3_type(18.0,  7.0, 0.0),
    MyMathTypes::vector3_type(18.0,  8.0, 0.0),
    MyMathTypes::vector3_type(18.0,  9.0, 0.0),
    MyMathTypes::vector3_type(18.0, 10.0, 0.0),
    MyMathTypes::vector3_type(18.0, 11.0, 0.0),
    MyMathTypes::vector3_type(18.0, 12.0, 0.0),
    MyMathTypes::vector3_type(18.0, 13.0, 0.0),
//
    MyMathTypes::vector3_type(17.0, 13.0, 0.0),
    MyMathTypes::vector3_type(16.0, 13.0, 0.0),
    MyMathTypes::vector3_type(15.0, 13.0, 0.0),
    MyMathTypes::vector3_type(14.0, 13.0, 0.0),
    MyMathTypes::vector3_type(13.0, 13.0, 0.0),
    MyMathTypes::vector3_type(12.0, 13.0, 0.0),
    MyMathTypes::vector3_type(11.0, 13.0, 0.0),
    MyMathTypes::vector3_type(10.0, 13.0, 0.0),
//
    MyMathTypes::vector3_type( 9.0, 13.0, 0.0),
    MyMathTypes::vector3_type( 8.0, 13.0, 0.0),
    MyMathTypes::vector3_type( 7.0, 13.0, 0.0),
    MyMathTypes::vector3_type( 6.0, 13.0, 0.0),
    MyMathTypes::vector3_type( 5.0, 13.0, 0.0),
    MyMathTypes::vector3_type( 4.0, 13.0, 0.0),
    MyMathTypes::vector3_type( 3.0, 13.0, 0.0),
    MyMathTypes::vector3_type( 2.0, 13.0, 0.0),
//
    MyMathTypes::vector3_type( 2.0, 12.0, 0.0),
    MyMathTypes::vector3_type( 2.0, 11.0, 0.0),
    MyMathTypes::vector3_type( 2.0, 10.0, 0.0),
    MyMathTypes::vector3_type( 2.0,  9.0, 0.0),
    MyMathTypes::vector3_type( 2.0,  8.0, 0.0),
    MyMathTypes::vector3_type( 2.0,  7.0, 0.0),
    MyMathTypes::vector3_type( 2.0,  6.0, 0.0),
    MyMathTypes::vector3_type( 2.0,  5.0, 0.0),
    MyMathTypes::vector3_type( 2.0,  4.0, 0.0),
    MyMathTypes::vector3_type( 2.0,  3.0, 0.0),
    MyMathTypes::vector3_type( 2.0,  2.0, 0.0),
//
    MyMathTypes::vector3_type( 3.0,  2.0, 0.0),
    MyMathTypes::vector3_type( 4.0,  2.0, 0.0),
    MyMathTypes::vector3_type( 5.0,  2.0, 0.0),
    MyMathTypes::vector3_type( 6.0,  2.0, 0.0),
    MyMathTypes::vector3_type( 7.0,  2.0, 0.0),
    MyMathTypes::vector3_type( 8.0,  2.0, 0.0),
    MyMathTypes::vector3_type( 9.0,  2.0, 0.0),
    MyMathTypes::vector3_type(10.0,  2.0, 0.0),
    MyMathTypes::vector3_type(11.0,  2.0, 0.0),
    MyMathTypes::vector3_type(12.0,  2.0, 0.0),
    MyMathTypes::vector3_type(13.0,  2.0, 0.0),
    MyMathTypes::vector3_type(14.0,  2.0, 0.0),
    MyMathTypes::vector3_type(15.0,  2.0, 0.0),
    MyMathTypes::vector3_type(16.0,  2.0, 0.0),
    MyMathTypes::vector3_type(17.0,  2.0, 0.0),
    MyMathTypes::vector3_type(18.0,  2.0, 0.0),
//
    MyMathTypes::vector3_type(18.0,  3.0, 0.0),
    MyMathTypes::vector3_type(18.0,  4.0, 0.0),
    MyMathTypes::vector3_type(18.0,  5.0, 0.0),
    MyMathTypes::vector3_type(18.0,  6.0, 0.0)
};

#endif

int lmax = sizeof(LineStop) / sizeof(MyMathTypes::vector3_type);


/*******************************************************************\
*                                                                   *
*                      D e b u g L i n e s ( )                      *
*                                                                   *
\*******************************************************************/

void DebugLines()
{
    UnitLength = 50;
    XSpacing   = 1;
    YSpacing   = 1;

    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    render_pipeline.DebugOn();
    render_pipeline.unit_length(UnitLength);

    MyMathTypes::vector3_type LineStart(10, 7, 0);

    // The variable l is global, and don't worry about it - display() updates it!

    render_pipeline.draw_line(LineStart, cblack, LineStop[l], cwhite);

    render_pipeline.draw_grid(XSpacing, YSpacing, cblack);
    render_pipeline.draw_debugline(LineStart, LineStop[l], cwhite);
    render_pipeline.DebugOff();
    render_pipeline.unit_length(1);
}


/*******************************************************************\
*                                                                   *
*                   D r a w T r i a n g l e s ( )                   *
*                                                                   *
\*******************************************************************/

//#define NEWTRIANGLES

void DrawTriangles()
{
    // TestProgram: It tests drawing of triangles.

    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);


#ifdef NEWTRIANGLES
    // The new test which should show better that the trianles work.

    // The Center of the window is (400, 400)

    // Lower triangle
    MyMathTypes::vector3_type  v1( 400.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  v2( 700.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  v3( 550.0, 400.0, 0.0 );    

    MyMathTypes::vector3_type  n1( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n2( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n3( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c1( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c2( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c3( 1.0, 0.0, 0.0 );

    render_pipeline.draw_triangle(v1,  n1, c1,  v2,  n2, c2,  v3,  n3, c3);    

    // Upper triangle
    MyMathTypes::vector3_type  v4( 550.0, 400.0, 0.0 );
    MyMathTypes::vector3_type  v5( 700.0, 700.0, 0.0 );
    MyMathTypes::vector3_type  v6( 400.0, 700.0, 0.0 );    

    MyMathTypes::vector3_type  n4( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n5( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n6( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c4( 0.0, 1.0, 0.0 );
    MyMathTypes::vector3_type  c5( 0.0, 1.0, 0.0 );
    MyMathTypes::vector3_type  c6( 0.0, 1.0, 0.0 );

    render_pipeline.draw_triangle(v4,  n4, c4,
				  v5,  n5, c5,
				  v6,  n6, c6);  

    // Left triangle
    MyMathTypes::vector3_type  v7( 400.0, 400.0, 0.0 );
    MyMathTypes::vector3_type  v8( 100.0, 700.0, 0.0 );
    MyMathTypes::vector3_type  v9( 400.0, 100.0, 0.0 );    

    MyMathTypes::vector3_type  n7( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n8( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n9( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c7( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  c8( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  c9( 0.0, 0.0, 1.0 );

    render_pipeline.draw_triangle(v7,  n7, c7,
				  v8,  n8, c8,
				  v9,  n9, c9);

#else

    // Old Triangles

    // Lower left triangle with a lower horizontal edge
    MyMathTypes::vector3_type  v1( 100.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  v2( 400.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  v3( 250.0, 300.0, 0.0 );    

    MyMathTypes::vector3_type  n1( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n2( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n3( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c1( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c2( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c3( 1.0, 0.0, 0.0 );

#if 1
    render_pipeline.draw_triangle(  v1,  n1, cyellow,
				    v2,  n2, cyellow,
				    v3,  n3, cyellow);    
#endif

    // Upper left triangle with a upper horizontal edge
    MyMathTypes::vector3_type  v4( 100.0, winHeight - v1[2], 0.0 );
    MyMathTypes::vector3_type  v5( 250.0, winHeight - v3[2], 0.0 );
    MyMathTypes::vector3_type  v6( 400.0, winHeight - v2[2], 0.0 );    

    MyMathTypes::vector3_type  n4( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n5( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n6( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c4( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c5( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c6( 1.0, 0.0, 0.0 );

#if 1
    render_pipeline.draw_triangle(  v4,  n4, cyellow,
				    v5,  n5, cyellow,
				    v6,  n6, cyellow); 
#endif

    // General triangle with the third point the the right
    MyMathTypes::vector3_type  v7( winWidth /  2 + 200.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  v8( winWidth /  2 + 100.0, 400.0, 0.0 );
    MyMathTypes::vector3_type  v9( winWidth /  2 + 300.0, 250.0, 0.0 );    

    MyMathTypes::vector3_type  n7( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n8( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n9( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c7( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c8( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c9( 1.0, 0.0, 0.0 );

#if 1
    render_pipeline.draw_triangle(  v7,  n7, cyellow,
				    v8,  n8, cyellow,
				    v9,  n9, cyellow); 
#endif

    // General triangle with the third point the the left
    MyMathTypes::vector3_type  v10( winWidth /  2 + 250.0, winHeight - 100.0, 0.0 );
    MyMathTypes::vector3_type  v11( winWidth /  2 + 300.0, winHeight - 400.0, 0.0 );
    MyMathTypes::vector3_type  v12( winWidth /  2 + 100.0, winHeight - 250.0, 0.0 );    

    MyMathTypes::vector3_type  n10( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n11( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n12( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c10( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c11( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c12( 1.0, 0.0, 0.0 );

#if 1
    render_pipeline.draw_triangle( v10, n10, cyellow,
				   v11, n11, cyellow,
				   v12, n12, cyellow);    
#endif
#endif
}


/*******************************************************************\
*                                                                   *
*               L o c a l   T r i a n g l e   D a t a               *
*                                                                   *
\*******************************************************************/

#if 1
MyMathTypes::vector3_type Vertex1[] = {
    MyMathTypes::vector3_type(9.0, 1.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
//
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
//
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
//
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
//
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
//
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
//
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0)
};

MyMathTypes::vector3_type Vertex2[] = {
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
//
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
//
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
//
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
//
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
//
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
//
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0)
};

MyMathTypes::vector3_type Vertex3[] = {
    MyMathTypes::vector3_type(1.0, 1.0, 0.0),
    MyMathTypes::vector3_type(1.0, 1.0, 0.0),
    MyMathTypes::vector3_type(2.0, 1.0, 0.0),
    MyMathTypes::vector3_type(3.0, 1.0, 0.0),
    MyMathTypes::vector3_type(4.0, 1.0, 0.0),
//
    MyMathTypes::vector3_type(5.0, 1.0, 0.0),
    MyMathTypes::vector3_type(6.0, 1.0, 0.0),
    MyMathTypes::vector3_type(7.0, 1.0, 0.0),
    MyMathTypes::vector3_type(8.0, 1.0, 0.0),
    MyMathTypes::vector3_type(9.0, 2.0, 0.0),
//
    MyMathTypes::vector3_type(9.0, 3.0, 0.0),
    MyMathTypes::vector3_type(9.0, 4.0, 0.0),
    MyMathTypes::vector3_type(9.0, 5.0, 0.0),
    MyMathTypes::vector3_type(9.0, 6.0, 0.0),
    MyMathTypes::vector3_type(9.0, 7.0, 0.0),
//
    MyMathTypes::vector3_type(9.0, 8.0, 0.0),
    MyMathTypes::vector3_type(9.0, 9.0, 0.0),
    MyMathTypes::vector3_type(8.0, 9.0, 0.0),
    MyMathTypes::vector3_type(7.0, 9.0, 0.0),
    MyMathTypes::vector3_type(6.0, 9.0, 0.0),
//
    MyMathTypes::vector3_type(5.0, 9.0, 0.0),
    MyMathTypes::vector3_type(4.0, 9.0, 0.0),
    MyMathTypes::vector3_type(3.0, 9.0, 0.0),
    MyMathTypes::vector3_type(2.0, 9.0, 0.0),
    MyMathTypes::vector3_type(1.0, 9.0, 0.0),
//
    MyMathTypes::vector3_type(1.0, 8.0, 0.0),
    MyMathTypes::vector3_type(1.0, 7.0, 0.0),
    MyMathTypes::vector3_type(1.0, 6.0, 0.0),
    MyMathTypes::vector3_type(1.0, 5.0, 0.0),
    MyMathTypes::vector3_type(1.0, 4.0, 0.0),
//
    MyMathTypes::vector3_type(1.0, 3.0, 0.0),
    MyMathTypes::vector3_type(1.0, 2.0, 0.0)
};

#endif

int vmax = sizeof(Vertex3) / sizeof(MyMathTypes::vector3_type);

/*******************************************************************\
*                                                                   *
*                  D e b u g T r i a n g l e s ( )                  *
*                                                                   *
\*******************************************************************/

void DebugTriangles()
{
    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);


    MyMathTypes::vector3_type ZeroNormal(0.0, 0.0, 0.0);

    UnitLength = 50;
    XSpacing   = 1;
    YSpacing   = 1;

    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_rasterizer(triangle_rasterizer);

    render_pipeline.DebugOn();
    render_pipeline.unit_length(UnitLength);

//#define ONETRIANGLE
#ifdef ONETRIANGLE
    // Only one triangle
    MyMathTypes::vector3_type V1(1, 2, 0);
    MyMathTypes::vector3_type V2(9, 2 ,0);
    MyMathTypes::vector3_type V3(1, 9 ,0);

    render_pipeline.draw_triangle(V1, ZeroNormal, cwhite,
				  V2, ZeroNormal, cwhite,
				  V3, ZeroNormal, cwhite);

    render_pipeline.draw_grid(XSpacing, YSpacing, cblack);

    render_pipeline.draw_debugline(V1, V2, cwhite);
    render_pipeline.draw_debugline(V2, V3, cwhite);
    render_pipeline.draw_debugline(V3, V1, cwhite); // was cgreen
    
#else
    // Many triangles

    // The variable v is global, and don't worry about it - display() updates it!

    render_pipeline.draw_triangle(Vertex1[v], ZeroNormal, cwhite,
				  Vertex2[v], ZeroNormal, cwhite,
				  Vertex3[v], ZeroNormal, cwhite);

    render_pipeline.draw_grid(XSpacing, YSpacing, cblack);

    render_pipeline.draw_debugline(Vertex1[v], Vertex2[v], cwhite);
    render_pipeline.draw_debugline(Vertex2[v], Vertex3[v], cwhite);
    render_pipeline.draw_debugline(Vertex3[v], Vertex1[v], cwhite); // was cgreen

    render_pipeline.unit_length(1);
    render_pipeline.DebugOff();
#endif
}


/*******************************************************************\
*                                                                   *
*            D r a w G o u r a u d T r i a n g l e s ( )            *
*                                                                   *
\*******************************************************************/

void DrawGouraudTriangles()
{
    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);
    
    
    render_pipeline.clear(-1.0, cgray);

    // Old Triangles

    // Lower left triangle with a lower horizontal edge
    MyMathTypes::vector3_type  v1( 100.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  v2( 400.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  v3( 250.0, 300.0, 0.0 );    

    MyMathTypes::vector3_type  n1( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n2( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n3( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c1( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c2( 0.0, 1.0, 0.0 );
    MyMathTypes::vector3_type  c3( 0.0, 0.0, 1.0 );

#if 1
    render_pipeline.draw_triangle(  v1,  n1, cred,
				    v2,  n2, cgreen,
				    v3,  n3, cblue);    
#endif

// Upper left triangle with a upper horizontal edge
    MyMathTypes::vector3_type  v4( 100.0, winHeight - v1[2], 0.0 );
    MyMathTypes::vector3_type  v5( 250.0, winHeight - v3[2], 0.0 );
    MyMathTypes::vector3_type  v6( 400.0, winHeight - v2[2], 0.0 );    

    MyMathTypes::vector3_type  n4( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n5( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n6( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c4( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c5( 0.0, 1.0, 0.0 );
    MyMathTypes::vector3_type  c6( 0.0, 0.0, 1.0 );

#if 1
    render_pipeline.draw_triangle(  v4,  n4, cred,
				    v5,  n5, cgreen,
				    v6,  n6, cblue);
#endif

// General triangle with the third point the the right
    MyMathTypes::vector3_type  v7( winWidth /  2 + 200.0, 100.0, 0.0 );
    MyMathTypes::vector3_type  v8( winWidth /  2 + 100.0, 400.0, 0.0 );
    MyMathTypes::vector3_type  v9( winWidth /  2 + 300.0, 250.0, 0.0 );    

    MyMathTypes::vector3_type  n7( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n8( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n9( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c7( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c8( 0.0, 1.0, 0.0 );
    MyMathTypes::vector3_type  c9( 0.0, 0.0, 1.0 );

#if 1
    render_pipeline.draw_triangle(  v7,  n7, cred,
				    v8,  n8, cgreen,
				    v9,  n9, cblue);
#endif

// General triangle with the third point the the left
    MyMathTypes::vector3_type  v10( winWidth /  2 + 250.0, winHeight - 100.0, 0.0 );
    MyMathTypes::vector3_type  v11( winWidth /  2 + 300.0, winHeight - 400.0, 0.0 );
    MyMathTypes::vector3_type  v12( winWidth /  2 + 100.0, winHeight - 250.0, 0.0 );    

    MyMathTypes::vector3_type  n10( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n11( 0.0, 0.0, 1.0 );
    MyMathTypes::vector3_type  n12( 0.0, 0.0, 1.0 );

    MyMathTypes::vector3_type  c10( 1.0, 0.0, 0.0 );
    MyMathTypes::vector3_type  c11( 0.0, 1.0, 0.0 );
    MyMathTypes::vector3_type  c12( 0.0, 0.0, 1.0 );

#if 1
    render_pipeline.draw_triangle( v10, n10, cred,
				   v11, n11, cgreen,
				   v12, n12, cblue);    
#endif
}


/*******************************************************************\
*                                                                   *
*                D r a w H i d d e n S u r f a c e s                *
*                                                                   *
\*******************************************************************/

void DrawHiddenSurfaces()
{
    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    // Dummy normal
    MyMathTypes::vector3_type n(0.0, 0.0, 0.0);

    // First Triangle - red
    MyMathTypes::vector3_type v_11(200.0, 200.0, -0.9); 
    MyMathTypes::vector3_type v_12(800.0, 150.0, -0.1); 
    MyMathTypes::vector3_type v_13(300.0, 700.0, -0.7);
    
    render_pipeline.draw_triangle(v_11, n, cred,
				  v_12, n, cred,
				  v_13, n, cred);

    // Second Triangle - green
    MyMathTypes::vector3_type v_21(300.0, 250.0, -0.1); 
    MyMathTypes::vector3_type v_22(875.0, 200.0, -0.9); 
    MyMathTypes::vector3_type v_23(850.0, 500.0, -0.9); 
    
    render_pipeline.draw_triangle(v_21, n, cgreen,
				  v_22, n, cgreen,
				  v_23, n, cgreen);

    // Draw a straght line through the triangles
    render_pipeline.load_rasterizer(line_rasterizer);
    MyMathTypes::vector3_type l_start(700.0, 100.0, -0.01);
    MyMathTypes::vector3_type l_stop( 200.0, 600.0, -0.99);

    render_pipeline.draw_line(l_start, cwhite, l_stop, cwhite);
}


/*******************************************************************\
*                                                                   *
*                D r a w P h o n g T r i a n g l e s                *
*                                                                   *
\*******************************************************************/

void DrawPhongTriangles()
{
    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(phong_fragment_program);

    // Vertices
    MyMathTypes::vector3_type v_1(-33.978017, -34.985076,  50.214926);
    MyMathTypes::vector3_type v_2( 84.192943, -13.784394, -50.214926);
    MyMathTypes::vector3_type v_3(-16.236910,  83.754546, -50.214926);
    MyMathTypes::vector3_type normal = Cross(v_2 - v_1, v_3 - v_1);
                              normal /= Norm(normal);      

    // Light Sources
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;
    
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);
    MyMathTypes::vector3_type I_p_position(266.395325, 274.291267, -43.696048);
    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;

    // Set up the Light source parameters
    render_pipeline.state().light_position() = I_p_position;

    // Material Properties
    MyMathTypes::real_type    k_a = 0.5;
    MyMathTypes::vector3_type O_a(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_d = 0.75;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.9;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 20.0;
    MyMathTypes::real_type    f_att = 1.0;   // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


    // Viewing Parameters
    MyMathTypes::vector3_type VRP(0.0, 0.0, 125.0);
    MyMathTypes::vector3_type VPN(0.0, 0.0, 1.0);
    MyMathTypes::vector3_type VUP(0.0, 1.0, 0.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 50.0);

    MyMathTypes::real_type    F =   10.0;
    MyMathTypes::real_type    B = -800.0;

    MyMathTypes::vector2_type lower_left( -25.0, -25.0);
    MyMathTypes::vector2_type upper_right( 25.0,  25.0);
    
    // Set up the Camera
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winWidth, winHeight);

    // Set the Eye coordinates
    render_pipeline.state().eye_position() = VRP + PRP;

    // Draw the triangle
    render_pipeline.draw_triangle(v_1, normal, k_d * O_d,
				  v_2, normal, k_d * O_d,
				  v_3, normal, k_d * O_d);

    
}


/*******************************************************************\
*                                                                   *
*                         H o u s e D a t a                         *
*                                                                   *
\*******************************************************************/

// Front Wall
MyMathTypes::real_type const front_depth = 54.0;
MyMathTypes::vector3_type FrontWall[] = {
    MyMathTypes::vector3_type( 0.0,  0.0, front_depth),
    MyMathTypes::vector3_type(16.0,  0.0, front_depth),
    MyMathTypes::vector3_type(16.0, 10.0, front_depth),
    MyMathTypes::vector3_type( 8.0, 16.0, front_depth),
    MyMathTypes::vector3_type( 0.0, 10.0, front_depth)
};
int const NFrontWallVertices = sizeof(FrontWall) / sizeof(MyMathTypes::vector3_type);

// Back Wall
MyMathTypes::real_type const back_depth = 30.0;
//MyMathTypes::real_type const back_depth = -100.0;
MyMathTypes::vector3_type BackWall[] = {
    MyMathTypes::vector3_type( 0.0,  0.0, back_depth),
    MyMathTypes::vector3_type(16.0,  0.0, back_depth),
    MyMathTypes::vector3_type(16.0, 10.0, back_depth),
    MyMathTypes::vector3_type( 8.0, 16.0, back_depth),
    MyMathTypes::vector3_type( 0.0, 10.0, back_depth)
};
int const NBackWallVertices = sizeof(BackWall) / sizeof(MyMathTypes::vector3_type);


/*******************************************************************\
*                                                                   *
*                       D r a w H o u s e ( )                       *
*                                                                   *
\*******************************************************************/

void DrawHouse()
{
    // Test program for the transformations and projections

    // Draw the front wall
    for (int i = 1; i < NFrontWallVertices; ++i) {
	render_pipeline.draw_line( FrontWall[i-1], cwhite,
				   FrontWall[i], cwhite );
    }
    render_pipeline.draw_line( FrontWall[NFrontWallVertices-1], cwhite, 
			       FrontWall[0], cwhite );

    // Draw the back wall
    for (int i = 1; i < NBackWallVertices; ++i) {
	render_pipeline.draw_line(BackWall[i-1], cred, 
				  BackWall[i], cred );
    }
    render_pipeline.draw_line( BackWall[NBackWallVertices-1], cred, 
			       BackWall[0], cred );

    // Draw the side walls
    for (int i = 0; i < NFrontWallVertices; ++i) {
	render_pipeline.draw_line( FrontWall[i], cgreen, 
				   BackWall[i], cgreen );
    }

    // Reset the framebuffer resolution to the original values
    
    camera.reset();
}


/*******************************************************************\
*                                                                   *
*                  F o l e y _ F i g _ 6 _ 2 7 ( )                  *
*                                                                   *
\*******************************************************************/

void Foley_Fig_6_27()
{
    std::cout << "Draw the House in Foley, Fig 6.27." << std::endl;

    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    MyMathTypes::vector3_type vrp(0, 0, 0);
    MyMathTypes::vector3_type vpn(0, 0, 1);
    MyMathTypes::vector3_type vup(0, 1, 0);
    MyMathTypes::vector3_type prp(8, 6, 84);

    MyMathTypes::vector2_type lower_left( -50.0, -50.0);
    MyMathTypes::vector2_type upper_right( 50.0,  50.0);

    MyMathTypes::real_type    front_plane(60.0);
    MyMathTypes::real_type    back_plane( 25.0);

    camera.set_projection(vrp, vpn, vup, prp,
			  lower_left, upper_right,
			  front_plane, back_plane,
			  winWidth, winHeight);

    DrawHouse();
}


/*******************************************************************\
*                                                                   *
*                  F o l e y _ F i g _ 6 _ 2 8 ( )                  *
*                                                                   *
\*******************************************************************/

void Foley_Fig_6_28()
{
    std::cout << "Draw the House in Foley, Fig 6.28." << std::endl;

    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    MyMathTypes::vector3_type vrp(0, 0, 54);
    MyMathTypes::vector3_type vpn(0, 0, 1);
    MyMathTypes::vector3_type vup(0, 1, 0);
    MyMathTypes::vector3_type prp(8, 6, 30);

    MyMathTypes::vector2_type lower_left( -1.0, -1.0);
    MyMathTypes::vector2_type upper_right(17.0, 17.0);

    MyMathTypes::real_type    front_plane(  1.0);
    MyMathTypes::real_type    back_plane( -35.0);

    camera.set_projection(vrp, vpn, vup, prp,
			  lower_left, upper_right,
			  front_plane, back_plane,
			  winWidth, winHeight);

    DrawHouse();
}


/*******************************************************************\
*                                                                   *
*                  F o l e y _ F i g _ 6 _ 3 1 ( )                  *
*                                                                   *
\*******************************************************************/

void Foley_Fig_6_31()
{
    std::cout << "Draw the House in Foley, Fig 6.31." << std::endl;

    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    MyMathTypes::vector3_type vrp(16, 0, 54);
    MyMathTypes::vector3_type vpn(0, 0, 1);
    MyMathTypes::vector3_type vup(0, 1, 0);
    MyMathTypes::vector3_type prp(20, 25, 20);

    MyMathTypes::vector2_type lower_left( -20.0, -5.0);
    MyMathTypes::vector2_type upper_right( 20.0,  35.0);

    MyMathTypes::real_type    front_plane(  1.0);
    MyMathTypes::real_type    back_plane( -35.0);

    camera.set_projection(vrp, vpn, vup, prp,
			  lower_left, upper_right,
			  front_plane, back_plane,
			  winWidth, winHeight);

    DrawHouse();
}


/*******************************************************************\
*                                                                   *
*                  F o l e y _ F i g _ 6 _ 2 2 ( )                  *
*                                                                   *
\*******************************************************************/

void Foley_Fig_6_22()
{
    std::cout << "Draw the House in Foley, Fig 6.22." << std::endl;

    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    MyMathTypes::vector3_type vrp(16, 0, 54);
    MyMathTypes::vector3_type vpn(1, 0, 1);
    MyMathTypes::vector3_type vup(0, 1, 0);
    MyMathTypes::vector3_type prp(0, 25, 20 * sqrt(2.0));

    MyMathTypes::vector2_type lower_left( -20.0, -5.0);
    MyMathTypes::vector2_type upper_right( 20.0,  35.0);

    MyMathTypes::real_type    front_plane(  1.0);
    MyMathTypes::real_type    back_plane( -35.0);

    camera.set_projection(vrp, vpn, vup, prp,
			  lower_left, upper_right,
			  front_plane, back_plane,
			  winWidth, winHeight);

    DrawHouse();
}


/*******************************************************************\
*                                                                   *
*                  F o l e y _ F i g _ 6 _ 3 4 ( )                  *
*                                                                   *
\*******************************************************************/

void Foley_Fig_6_34()
{
    std::cout << "Draw the House in Foley, Fig 6.34." << std::endl;

    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    MyMathTypes::vector3_type vrp(16, 0, 54);
    MyMathTypes::vector3_type vpn(1, 0, 1);
    MyMathTypes::vector3_type vup(-sin(10.0 * M_PI / 180.0),
				   cos(10.0 * M_PI / 180.0),
				   sin(10.0 * M_PI / 180.0));
    //std::cout << "vup = [" << vup << "]^T" << std::endl;
    MyMathTypes::vector3_type prp(0, 25, 20 * sqrt(2.0));

    MyMathTypes::vector2_type lower_left( -20.0, -5.0);
    MyMathTypes::vector2_type upper_right( 20.0,  35.0);

    MyMathTypes::real_type    front_plane(  1.0);
    MyMathTypes::real_type    back_plane( -35.0);

    camera.set_projection(vrp, vpn, vup, prp,
			  lower_left, upper_right,
			  front_plane, back_plane,
			  winWidth, winHeight);

    DrawHouse();
}


/*******************************************************************\
*                                                                   *
*                      B e z i e r   C u r v e                      *
*                                                                   *
\*******************************************************************/

/*******************************************************************\
*                                                                   *
*              D r a w B e z i e r C u r v e F W D ( )              *
*                                                                   *
\*******************************************************************/

void DrawBezierCurveFWD()
{
    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    
    int N = 30;

    MyMathTypes::vector3_type G_1(100.0, 100.0, 0.0);
    MyMathTypes::vector3_type G_2(250.0, 500.0, 0.0);
    MyMathTypes::vector3_type G_3(600.0, 500.0, 0.0);
    MyMathTypes::vector3_type G_4(900.0, 100.0, 0.0);

    MyMathTypes::bezier_curve bcurve(G_1, G_2, G_3, G_4);

    // Draw the control grid
    render_pipeline.draw_line(bcurve[1], cred, bcurve[2], cred);
    render_pipeline.draw_line(bcurve[2], cred, bcurve[3], cred);
    render_pipeline.draw_line(bcurve[3], cred, bcurve[4], cred);

    // Now draw the real curve using forward differences.
}


/*******************************************************************\
*                                                                   *
*         S u b D i v i d e B e z i e r C u r v e ( . . . )         *
*                                                                   *
\*******************************************************************/

void SubDivideBezierCurve(MyMathTypes::bezier_curve const& G, int N)
{
    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    // Subdivide the Bezier curve recursively.
}


/*******************************************************************\
*                                                                   *
*        D r a w B e z i e r C u r v e S u b D i v i d e ( )        *
*                                                                   *
\*******************************************************************/

void DrawBezierCurveSubDivide()
{
    int SubDiv = 5;

    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(identity_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    MyMathTypes::vector3_type G_1(100.0, 100.0, 0.0);
    MyMathTypes::vector3_type G_2(250.0, 500.0, 0.0);
    MyMathTypes::vector3_type G_3(600.0, 500.0, 0.0);
    MyMathTypes::vector3_type G_4(900.0, 100.0, 0.0);

    MyMathTypes::bezier_curve bcurve(G_1, G_2, G_3, G_4);

    // Draw the control grid
    render_pipeline.draw_line(bcurve[1], cred, bcurve[2], cred);
    render_pipeline.draw_line(bcurve[2], cred, bcurve[3], cred);
    render_pipeline.draw_line(bcurve[3], cred, bcurve[4], cred);

    // Draw the Castaljo thing
    MyMathTypes::vector3_type L_1(bcurve[1]);
    MyMathTypes::vector3_type R_1(bcurve[4]);
    MyMathTypes::vector3_type L_2((bcurve[1] + bcurve[2]) / 2.0);
    MyMathTypes::vector3_type R_2((bcurve[3] + bcurve[4]) / 2.0); 
    MyMathTypes::vector3_type H((bcurve[2] + bcurve[3]) / 2.0);
    MyMathTypes::vector3_type L_3((L_2 + H) / 2.0);
    MyMathTypes::vector3_type R_3((R_2 + H) / 2.0);
    MyMathTypes::vector3_type L_4((L_3 + R_3) / 2.0);

    render_pipeline.draw_line(L_1, cgreen,  L_2, cgreen);
    render_pipeline.draw_line(L_2, cgreen,  H  , cgreen);
    render_pipeline.draw_line(H  , cgreen,  R_2, cgreen);
    render_pipeline.draw_line(R_2, cgreen,  R_1, cgreen);
    render_pipeline.draw_line(L_3, cyellow, R_3, cyellow);

    // Now for the real curve
    SubDivideBezierCurve(bcurve, SubDiv);
}


/*******************************************************************\
*                                                                   *
*                  T h e   K l e i n   B o t t l e                  *
*                                                                   *
\*******************************************************************/

MyMathTypes::real_type const klein_epsilon = 1.0e-4;


#include "solution/kleinbottle.h"

KleinBottle Klein;

/*******************************************************************\
*                                                                   *
*              D r a w T h e K l e i n B o t t o m ( )              *
*                                                                   *
\*******************************************************************/

void DrawTheKleinBottom()
{
    int N = 20;       // Tesselation in the u-parameter
    int M = N / 2;    // Tesselation in the v-parameter

    // Draw the bottom of the Klein bottle.
}


/*******************************************************************\
*                                                                   *
*              D r a w T h e K l e i n H a n d l e ( )              *
*                                                                   *
\*******************************************************************/

void DrawTheKleinHandle()
{
    int N = 20;       // Tesselation in the u-parameter
    int M = N / 2;    // Tesselation in the v-parameter

    // Draw the handle of the Klein bottle.
}


/*******************************************************************\
*                                                                   *
*                 D r a w T h e K l e i n T o p ( )                 *
*                                                                   *
\*******************************************************************/

void DrawTheKleinTop()
{
    int N = 20;       // Tesselation in the u-parameter
    int M = N / 2;    // Tesselation in the v-parameter

    // Draw the top of the Klein bottle.
}


/*******************************************************************\
*                                                                   *
*              D r a w T h e K l e i n M i d d l e ( )              *
*                                                                   *
\*******************************************************************/

void DrawTheKleinMiddle()
{
    int N = 20;       // Tesselation in the u-parameter
    int M = N / 2;    // Tesselation in the v-parameter

    // Draw the middle of the Klein bottle.
}


/*******************************************************************\
*                                                                   *
*                 D r a w K l e i n B o t t l e ( )                 *
*                                                                   *
\*******************************************************************/

void DrawKleinBottle()
{

/*******************************************************************\
*                                                                   *
*                V i e w i n g   P a r a m e t e r s                *
*                                                                   *
\*******************************************************************/

    MyMathTypes::vector3_type VRP(5.0, 0.0, 6.0);
    MyMathTypes::vector3_type VPN(cos(30.0 * M_PI / 180.0), 0.0, sin(30.0 * M_PI / 180.0));
    MyMathTypes::vector3_type VUP(0.0, 0.0, 1.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 50.0);

    MyMathTypes::vector2_type lower_left(-10.0, -10.0);
    MyMathTypes::vector2_type upper_right(10.0, 10.0);

    MyMathTypes::real_type    F =   5.0;
    MyMathTypes::real_type    B = -15.0;
    
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winWidth, winHeight);


/*******************************************************************\
*                                                                   *
*                      L i g h t   S o u r c e                      *
*                                                                   *
\*******************************************************************/

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);
    MyMathTypes::vector3_type I_p_position(300.0, 300.0, -300.0);

    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;


/*******************************************************************\
*                                                                   *
*               M a t e r i a l   P r o p e r t i e s               *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    k_a = 0.5;
    MyMathTypes::vector3_type O_a(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_d = 0.75;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.9;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 75.0;
    MyMathTypes::real_type    f_att = 1.0;   // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   P i p e l i n e               *
*                                                                   *
\*******************************************************************/

    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(phong_fragment_program);


/*******************************************************************\
*                                                                   *
*                   D r a w   t h e   B o t t l e                   *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type rotation_angle = 45.0 * M_PI /  180.0;
    render_pipeline.state().model()     = Z_Rotate(rotation_angle);
    render_pipeline.state().inv_model() = Inv_Z_Rotate(rotation_angle);

    DrawTheKleinBottom();
    DrawTheKleinHandle();
    DrawTheKleinTop();
    
    if (figure != 'm') {
	DrawTheKleinMiddle();
    }

    render_pipeline.state().model()     = Identity();
    render_pipeline.state().inv_model() = Identity();
}


/*******************************************************************\
*                                                                   *
*        T h e   P h o n g   R e f l e c t i o n   M o d e l        *
*                                                                   *
\*******************************************************************/

MyMathTypes::real_type const phong_epsilon = 1.0e-4;


/*******************************************************************\
*                                                                   *
*                  D r a w P h o n g S u r f a c e                  *
*                                                                   *
\*******************************************************************/

#include "solution/phongsurface.h"

void DrawPhongSurface()
{

/*******************************************************************\
*                                                                   *
*                V i e w i n g   P a r a m e t e r s                *
*                                                                   *
\*******************************************************************/

    camera.reset();
    render_pipeline.state().model() = Identity();
    render_pipeline.state().inv_model() = Identity();
    
    MyMathTypes::vector3_type VRP(5.0, 0.0, 6.0);
    MyMathTypes::vector3_type VPN(cos(30.0 * M_PI / 180.0), 0.0, sin(30.0 * M_PI / 180.0));
    MyMathTypes::vector3_type VUP(0.0, 0.0, 1.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 50.0); // was 100

    MyMathTypes::vector2_type lower_left(-10.0, -10.0);
    MyMathTypes::vector2_type upper_right(10.0, 10.0);

    MyMathTypes::real_type    F =   5.0;   // Original
    MyMathTypes::real_type    B = -15.0;   // Original
    
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winWidth, winHeight);


/*******************************************************************\
*                                                                   *
*                      L i g h t   S o u r c e                      *
*                                                                   *
\*******************************************************************/

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);

    MyMathTypes::vector3_type I_p_position(300.0, -300.0, 300.0);   // Use this one

    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;


/*******************************************************************\
*                                                                   *
*               M a t e r i a l   P r o p e r t i e s               *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    k_a = 0.5;
    MyMathTypes::vector3_type O_a(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_d = 0.75;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.9;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 50.0; //20.0;
    MyMathTypes::real_type    f_att =  1.0; // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   P i p e l i n e               *
*                                                                   *
\*******************************************************************/

    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(phong_fragment_program);


/*******************************************************************\
*                                                                   *
*            D r a w   t h e   P h o n g   S u r f a c e            *
*                                                                   *
\*******************************************************************/

#if DRAWCOORDINATESYSTEM
    // draw the coordinate system
    render_pipeline.clear(-1.0, cgray);
    DrawWorldCoordinateSystem(15);
#endif
    
    int N = 25;       // Tesselation in the phi-parameter
    int M = 25;       // Tesselation in the theta-parameter

    MyMathTypes::real_type ScaleFactor = 14.0; // Scale the Surface

    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(phong_fragment_program);

    // Sample the Phong surface and draw it.

    render_pipeline.state().model()     = Identity();
    render_pipeline.state().inv_model() = Identity();
}


/*******************************************************************\
*                                                                   *
*                    B e z i e r   P a t c h e s                    *
*                                                                   *
\*******************************************************************/

/*******************************************************************\
*                                                                   *
*              S u b d i v i d e B e z i e r P a t c h              *
*                                                                   *
\*******************************************************************/

void SubdivideBezierPatch(MyMathTypes::bezier_patch const& Patch, int SubdivLevel, 
			  bool InvertNormals, DrawStyle VisualizationStyle)
{
    // Subdivide the Bezier patch recursively.
}


/*******************************************************************\
*                                                                   *
*                D r a w B e z i e r P a t c h e s                  *
*                                                                   *
\*******************************************************************/

void DrawBezierPatches(std::vector<MyMathTypes::bezier_patch> const& BezierPatches, int SubdivLevel, 
		       std::vector<bool> const& InvertNormals, DrawStyle VisualizationStyle)
{
    int p = 0;
    for (std::vector<MyMathTypes::bezier_patch>::const_iterator Bpatch = BezierPatches.begin();
	 Bpatch != BezierPatches.end(); ++Bpatch)
    {
	MyMathTypes::bezier_patch Patch = *Bpatch;
	SubdivideBezierPatch(Patch, SubdivLevel, InvertNormals[p], VisualizationStyle);

	++p;
    }
}


/*******************************************************************\
*                                                                   *
*                    D r a w U T A H T e a p o t                    *
*                                                                   *
\*******************************************************************/

void DrawUTAHTeapot()
{

/*******************************************************************\
*                                                                   *
*                V i e w i n g   P a r a m e t e r s                *
*                                                                   *
\*******************************************************************/

    MyMathTypes::vector3_type VRP(5.0, 0.0, 5.0);
    MyMathTypes::vector3_type VPN(cos(30.0 * M_PI / 180.0), 0.0, sin(30.0 * M_PI / 180.0));
    MyMathTypes::vector3_type VUP(0.0, 0.0, 1.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 50.0);

    MyMathTypes::vector2_type lower_left( -4.0, -4.0);
    MyMathTypes::vector2_type upper_right( 4.0,  4.0);

    MyMathTypes::real_type    F =   5.0;
    MyMathTypes::real_type    B = -10.0;
    
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winWidth, winHeight);


/*******************************************************************\
*                                                                   *
*                      L i g h t   S o u r c e                      *
*                                                                   *
\*******************************************************************/

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);
    //MyMathTypes::vector3_type I_p_position(266.395325, 274.291267, -43.696048);
    MyMathTypes::vector3_type I_p_position(300.0, 300.0, 300.0);

    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;


/*******************************************************************\
*                                                                   *
*               M a t e r i a l   P r o p e r t i e s               *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    k_a = 0.5;
    MyMathTypes::vector3_type O_a(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_d = 0.75;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.9;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 75.0; //20.0;
    MyMathTypes::real_type    f_att = 1.0;   // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   P i p e l i n e               *
*                                                                   *
\*******************************************************************/

    render_pipeline.load_vertex_program(transform_vertex_program);


/*******************************************************************\
*                                                                   *
*              D r a w   t h e   U T A H   T e a p o t              *
*                                                                   *
\*******************************************************************/

    std::vector<MyMathTypes::bezier_patch> BezierPatches;
    
    int fail = ReadBezierPatches("./src/data/teapot.data", BezierPatches);
    if (fail) {
	throw std::runtime_error("DrawUTAHTeapot: failed to read the file: ./src/data/teapot.data");
    }
    
    // std::cout << "The Bezier Patches read:" << std::endl;
    // std::cout << "========================" << std::endl;
    // for (int i = 0; i < BezierPatches.size(); ++i) {
    // 	std::cout << "Bezier Patch[" << std::setw(2) << i+1 << "]: [" << BezierPatches[i] << "]"
    // 		  << std::endl << std::endl;
    // }

    //std::cout << "Draw The Bezier Patches:" << std::endl;
    //std::cout << "========================" << std::endl;

    std::vector<bool> InvertNormals(BezierPatches.size(), true);
    
#if 1
    MyMathTypes::real_type rotation_angle = 45.0 * M_PI /  180.0;
    render_pipeline.state().model()     = Z_Rotate(rotation_angle);
    render_pipeline.state().inv_model() = Inv_Z_Rotate(rotation_angle);
#endif

    int  SubdivLevel;

    SubdivLevel = 3;
    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_fragment_program(phong_fragment_program);
    DrawBezierPatches(BezierPatches, SubdivLevel, InvertNormals, ShadedPatch);

    render_pipeline.state().model()     = Identity();
    render_pipeline.state().inv_model() = Identity();
}



/*******************************************************************\
*                                                                   *
*                        D r a w R o c k e t                        *
*                                                                   *
\*******************************************************************/

void DrawRocket()
{

/*******************************************************************\
*                                                                   *
*                V i e w i n g   P a r a m e t e r s                *
*                                                                   *
\*******************************************************************/

    MyMathTypes::vector3_type VRP(5.0, 0.0, 5.0);
    MyMathTypes::vector3_type VPN(cos(30.0 * M_PI / 180.0), 0.0, sin(30.0 * M_PI / 180.0));
    MyMathTypes::vector3_type VUP(0.0, 0.0, 1.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 50.0);

    MyMathTypes::vector2_type lower_left( -4.0, -4.0);
    MyMathTypes::vector2_type upper_right( 4.0,  4.0);

    MyMathTypes::real_type    F =   5.0;
    MyMathTypes::real_type    B = -10.0;
    
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winWidth, winHeight);


/*******************************************************************\
*                                                                   *
*                      L i g h t   S o u r c e                      *
*                                                                   *
\*******************************************************************/

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);
    //MyMathTypes::vector3_type I_p_position(266.395325, 274.291267, -43.696048);
    MyMathTypes::vector3_type I_p_position(300.0, 300.0, 300.0);

    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;


/*******************************************************************\
*                                                                   *
*               M a t e r i a l   P r o p e r t i e s               *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    k_a = 0.5;
    MyMathTypes::vector3_type O_a(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_d = 0.75;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.9;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 75.0;
    MyMathTypes::real_type    f_att = 1.0;   // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   P i p e l i n e               *
*                                                                   *
\*******************************************************************/

    render_pipeline.load_vertex_program(transform_vertex_program);


/*******************************************************************\
*                                                                   *
*                   D r a w   t h e   R o c k e t                   *
*                                                                   *
\*******************************************************************/

    std::vector<MyMathTypes::bezier_patch> BezierPatches;
    
    int fail = ReadBezierPatches("./src/data/rocket.data", BezierPatches);
    if (fail) {
	throw std::runtime_error("DrawRocket: failed to read the file: ./src/data/rocket.data");
    }
    
    // std::cout << "The Bezier Patches read:" << std::endl;
    // std::cout << "========================" << std::endl;
    // for (int i = 0; i < BezierPatches.size(); ++i) {
    // 	std::cout << "Bezier Patch[" << std::setw(2) << i+1 << "]: [" << BezierPatches[i] << "]"
    // 		  << std::endl << std::endl;
    // }

    //std::cout << "Draw The Bezier Patches:" << std::endl;
    //std::cout << "========================" << std::endl;

    //render_pipeline.clear(-1.0, cgray);
    //DrawWorldCoordinateSystem();


    std::vector<bool> InvertNormals(BezierPatches.size(), true);

#if 1
    MyMathTypes::real_type rotation_angle = 90.0 * M_PI /  180.0;
    MyMathTypes::vector3_type t(0.0, 0.0, 0.0);
    MyMathTypes::matrix4x4_type M    = X_Rotate(rotation_angle) * Translate(t);
    MyMathTypes::matrix4x4_type InvM = InvTranslate(t) * Inv_X_Rotate(rotation_angle);

    render_pipeline.state().model()     = M;
    render_pipeline.state().inv_model() = InvM;
#endif


    // Translate the patch and scale it.
    MyMathTypes::real_type    ScaleFactor = 2.0;
    MyMathTypes::vector3_type T(0.0, 1.0, 0.0);

    std::vector<MyMathTypes::bezier_patch> TransformedBezierPatches;
    for (std::vector<MyMathTypes::bezier_patch>::const_iterator Bpatch = BezierPatches.begin();
	 Bpatch != BezierPatches.end(); ++Bpatch)
    {
	MyMathTypes::bezier_patch Patch = *Bpatch;
	for (int row = 1; row <= 4; ++row) {
	    for (int col = 1; col <= 4; ++col) {
		Patch[row][col] = (Patch[row][col] + T) * ScaleFactor;
	    }
	}
	TransformedBezierPatches.push_back(Patch);
    }

    
    int  SubdivLevel;

    SubdivLevel = 3;
    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_fragment_program(phong_fragment_program);
    DrawBezierPatches(TransformedBezierPatches, SubdivLevel, InvertNormals, ShadedPatch);

    render_pipeline.state().model()     = Identity();
    render_pipeline.state().inv_model() = Identity();
}


/*******************************************************************\
*                                                                   *
*                      D r a w S a i l b o a t                      *
*                                                                   *
\*******************************************************************/

void DrawSailboat()
{

/*******************************************************************\
*                                                                   *
*                V i e w i n g   P a r a m e t e r s                *
*                                                                   *
\*******************************************************************/

    MyMathTypes::vector3_type VRP(0.0, 3.0, 0.0); // was (5.0, 0.0, 5.0)
    MyMathTypes::vector3_type VPN(cos(30.0 * M_PI / 180.0), 0.0, sin(30.0 * M_PI / 180.0));
    MyMathTypes::vector3_type VUP(0.0, 1.0, 0.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 10.0); // was 50

    MyMathTypes::vector2_type lower_left( -5.0, -5.0);
    MyMathTypes::vector2_type upper_right( 5.0,  5.0);

    MyMathTypes::real_type    F =  5.0;
    MyMathTypes::real_type    B = -5.0;
    
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winWidth, winHeight);


/*******************************************************************\
*                                                                   *
*                      L i g h t   S o u r c e                      *
*                                                                   *
\*******************************************************************/

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);
    MyMathTypes::vector3_type I_p_position(300.0, 300.0, -50.0); // was (300.0, 300.0, 300.0)

    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;


/*******************************************************************\
*                                                                   *
*               M a t e r i a l   P r o p e r t i e s               *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    k_a = 0.5;
    MyMathTypes::vector3_type O_a(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_d = 0.75;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.9;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 75.0;
    MyMathTypes::real_type    f_att = 1.0;   // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   P i p e l i n e               *
*                                                                   *
\*******************************************************************/

    render_pipeline.load_vertex_program(transform_vertex_program);
    

/*******************************************************************\
*                                                                   *
*                 D r a w   t h e   S a i l b o a t                 *
*                                                                   *
\*******************************************************************/

    std::vector<MyMathTypes::bezier_patch> BezierPatches;

    int fail = ReadBezierPatches("./src/data/patches.data", BezierPatches);
    if (fail) {
	throw std::runtime_error("DrawSailboat: failed to read the file: ./src/data/patches.data");
    }
    
    // std::cout << "The Bezier Patches read:" << std::endl;
    // std::cout << "========================" << std::endl;
    // for (int i = 0; i < BezierPatches.size(); ++i) {
    // 	std::cout << "Bezier Patch[" << std::setw(2) << i+1 << "]: [" << BezierPatches[i] << "]"
    // 		  << std::endl << std::endl;
    // }

    //std::cout << "Draw The Bezier Patches:" << std::endl;
    //std::cout << "========================" << std::endl;

    std::vector<bool> InvertNormals(BezierPatches.size(), true);
    InvertNormals[31] = false; // Let the major sail have the front side to the viewer.
    InvertNormals[32] = false; // Let the jib have the back side to the viewer.

    // Translate the patch and scale it.
    MyMathTypes::real_type    ScaleFactor = 0.4;
    MyMathTypes::vector3_type T(0.0, 0.0, 0.0);

    std::vector<MyMathTypes::bezier_patch> TransformedBezierPatches;
    for (std::vector<MyMathTypes::bezier_patch>::const_iterator Bpatch = BezierPatches.begin();
	 Bpatch != BezierPatches.end(); ++Bpatch)
    {
	MyMathTypes::bezier_patch Patch = *Bpatch;
	for (int row = 1; row <= 4; ++row) {
	    for (int col = 1; col <= 4; ++col) {
		Patch[row][col] = (Vector3D(Y_Rotate(90.0 * M_PI / 180.0) * HomVector(Patch[row][col]))
				   + T) * ScaleFactor;
	    }
	}
	TransformedBezierPatches.push_back(Patch);
    }
    
    int  SubdivLevel   = 2;
    
    
#if 0
    //SubdivLevel = 1; 
    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);
    DrawBezierPatches(TransformedBezierPatches, SubdivLevel, InvertNormals, ControlGrid);
#endif

#if 1
    //SubdivLevel = 1;
    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(phong_fragment_program);
    DrawBezierPatches(TransformedBezierPatches, SubdivLevel, InvertNormals, ShadedPatch);
#endif

    render_pipeline.state().model()     = Identity();
    render_pipeline.state().inv_model() = Identity();
}


/*******************************************************************\
*                                                                   *
*                          D r a w P a i n                          *
*                                                                   *
\*******************************************************************/

void DrawPain()
{

/*******************************************************************\
*                                                                   *
*                V i e w i n g   P a r a m e t e r s                *
*                                                                   *
\*******************************************************************/

    MyMathTypes::vector3_type VRP(5.0, 0.0, 5.0);
    MyMathTypes::vector3_type VPN(cos(30.0 * M_PI / 180.0), 0.0, sin(30.0 * M_PI / 180.0));
    MyMathTypes::vector3_type VUP(0.0, 0.0, 1.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 50.0);

    MyMathTypes::vector2_type lower_left( -4.0, -4.0);
    MyMathTypes::vector2_type upper_right( 4.0,  4.0);

    MyMathTypes::real_type    F =   5.0;
    MyMathTypes::real_type    B = -10.0;
    
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winWidth, winHeight);


/*******************************************************************\
*                                                                   *
*                      L i g h t   S o u r c e                      *
*                                                                   *
\*******************************************************************/

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);
    MyMathTypes::vector3_type I_p_position(300.0, 300.0, 300.0);

    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;


/*******************************************************************\
*                                                                   *
*               M a t e r i a l   P r o p e r t i e s               *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    k_a = 0.5;
    MyMathTypes::vector3_type O_a(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_d = 0.75;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.9;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 75.0;
    MyMathTypes::real_type    f_att = 1.0;   // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   P i p e l i n e               *
*                                                                   *
\*******************************************************************/

    render_pipeline.load_vertex_program(transform_vertex_program);


/*******************************************************************\
*                                                                   *
*                     D r a w   t h e   P a i n                     *
*                                                                   *
\*******************************************************************/

    std::vector<MyMathTypes::bezier_patch> BezierPatches;
    
    int fail = ReadBezierPatches("./src/data/pain.data", BezierPatches);
    if (fail) {
	throw std::runtime_error("DrawPain: failed to read the file: ./src/data/pain.data");
    }
    
    // std::cout << "The Bezier Patches read:" << std::endl;
    // std::cout << "========================" << std::endl;
    // for (int i = 0; i < BezierPatches.size(); ++i) {
    // 	std::cout << "Bezier Patch[" << std::setw(2) << i+1 << "]: [" << BezierPatches[i] << "]"
    // 		  << std::endl << std::endl;
    // }

    //std::cout << "Draw The Bezier Patches:" << std::endl;
    //std::cout << "========================" << std::endl;

    MyMathTypes::real_type ScaleFactor = 2.0;
    for (int i = 0; i < BezierPatches.size(); ++i) {
	BezierPatches[i] = BezierPatches[i] * ScaleFactor;
    }

    std::vector<bool> InvertNormals(BezierPatches.size(), false);

#if 1
    MyMathTypes::real_type rotation_angle = 45.0 * M_PI /  180.0;
    render_pipeline.state().model()     = Z_Rotate(rotation_angle);
    render_pipeline.state().inv_model() = Inv_Z_Rotate(rotation_angle);
#endif

    int  SubdivLevel;

#if 0
    SubdivLevel = 3; 
    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_fragment_program(identity_fragment_program);
    DrawBezierPatches(BezierPatches, SubdivLevel, InvertNormals, ControlGrid);
#endif

    SubdivLevel = 5;
    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_fragment_program(phong_fragment_program);
    DrawBezierPatches(BezierPatches, SubdivLevel, InvertNormals, ShadedPatch);

    render_pipeline.state().model()     = Identity();
    render_pipeline.state().inv_model() = Identity();
}


/*******************************************************************\
*                                                                   *
*                       I c o s a h e d r o n                       *
*                                                                   *
\*******************************************************************/

/*******************************************************************\
*                                                                   *
*                       S c a l e F a c t o r                       *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    ScaleFactor = 1.5;

/*******************************************************************\
*                                                                   *
*                       T r a n s l a t i o n                       *
*                                                                   *
\*******************************************************************/

    MyMathTypes::vector3_type Translation(2.0, 0.5, 0.0);


//#include "solution/nicosahedron.h"
#include "solution/icosahedron.h"

Icosahedron icosahedron;
//Icosahedron icosahedron(ScaleFactor, Translation);

/*******************************************************************\
*                                                                   *
*              D r a w   t h e   I c o s a h e d r o n              *
*                                                                   *
\*******************************************************************/

void DrawIcosahedron()
{

/*******************************************************************\
*                                                                   *
*                V i e w i n g   P a r a m e t e r s                *
*                                                                   *
\*******************************************************************/

    //MyMathTypes::vector3_type VRP(5.0, 0.0, 5.0);
    MyMathTypes::vector3_type VRP(0.0, 0.0, 0.0);
    MyMathTypes::vector3_type VPN(1.0, 1.0, 1.0);
    MyMathTypes::vector3_type VUP(0.0, 1.0, 0.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 50.0);

    MyMathTypes::vector2_type lower_left( -4.0, -4.0);
    MyMathTypes::vector2_type upper_right( 4.0,  4.0);

    MyMathTypes::real_type    F =  10.0;
    MyMathTypes::real_type    B = -10.0;
    
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winHeight, winHeight);


/*******************************************************************\
*                                                                   *
*                      L i g h t   S o u r c e                      *
*                                                                   *
\*******************************************************************/

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);
    MyMathTypes::vector3_type I_p_position(30.0, 30.0, 30.0);

    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;


/*******************************************************************\
*                                                                   *
*               M a t e r i a l   P r o p e r t i e s               *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    k_a = 0.5;
    MyMathTypes::vector3_type O_a(0.5, 0.0, 0.0);

    MyMathTypes::real_type    k_d = 0.75;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.9;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 75.0;
    MyMathTypes::real_type    f_att = 1.0;   // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   P i p e l i n e               *
*                                                                   *
\*******************************************************************/

    render_pipeline.load_rasterizer(point_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(phong_fragment_program);

    render_pipeline.state().model()     = Identity();
    render_pipeline.state().inv_model() = Identity();


/*******************************************************************\
*                                                                   *
*              D r a w   t h e   I c o s a h e d r o n              *
*                                                                   *
\*******************************************************************/

    BoundingBox<MyMathTypes> BB;

    render_pipeline.clear(-1.0, cblack);

    //DrawWorldCoordinateSystem(3.0);
    //render_pipeline.load_rasterizer(point_rasterizer);

    //render_pipeline.DebugOn();
    //render_pipeline.unit_length(5);

    DrawWorldCoordinateSystem(3.0);

    // Draw the defining planes
    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    if (icosahedron.OriginalIcosahedron()) {
	// Draw the red defining plane
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ul_red), cred,
				  icosahedron.Vertex(icosahedron.ll_red), cred);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ll_red), cred,
				  icosahedron.Vertex(icosahedron.lr_red), cred);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.lr_red), cred,
				  icosahedron.Vertex(icosahedron.ur_red), cred);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ur_red), cred,
				  icosahedron.Vertex(icosahedron.ul_red), cred);

	// Draw the green defining plane
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ul_green), cgreen,
				  icosahedron.Vertex(icosahedron.ll_green), cgreen);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ll_green), cgreen,
				  icosahedron.Vertex(icosahedron.lr_green), cgreen);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.lr_green), cgreen,
				  icosahedron.Vertex(icosahedron.ur_green), cgreen);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ur_green), cgreen,
				  icosahedron.Vertex(icosahedron.ul_green), cgreen);

	// Draw the blue defining plane
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ul_blue), cblue,
				  icosahedron.Vertex(icosahedron.ll_blue), cblue);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ll_blue), cblue,
				  icosahedron.Vertex(icosahedron.lr_blue), cblue);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.lr_blue), cblue,
				  icosahedron.Vertex(icosahedron.ur_blue), cblue);
	render_pipeline.draw_line(icosahedron.Vertex(icosahedron.ur_blue), cblue,
				  icosahedron.Vertex(icosahedron.ul_blue), cblue);
    }
    // Draw the defining planes
    // just to check if the parameters work - remove the ScaleFactor and Translation
    // Draw the triangles as wireframes
    std::cout << "Draw triangles as wire frames" << std::endl;

    render_pipeline.load_rasterizer(line_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);

    for (int i = 1; i <= t; ++i) {
	render_pipeline.load_rasterizer(line_rasterizer);
	render_pipeline.load_vertex_program(transform_vertex_program);
	render_pipeline.load_fragment_program(identity_fragment_program);

	Icosahedron::triangle T = icosahedron.Triangle(i);
	Icosahedron::vertex v1 = T[0];
	Icosahedron::vertex v2 = T[1];
	Icosahedron::vertex v3 = T[2];

	render_pipeline.draw_line(v1, cyellow, v2, cyellow);
	render_pipeline.draw_line(v2, cyellow, v3, cyellow);
	render_pipeline.draw_line(v3, cyellow, v1, cyellow);

	// Draw the normal
	MyMathTypes::vector3_type N = icosahedron.Normal(i);
	MyMathTypes::vector3_type C = (v1 + v2 + v3) / 3.0;
	render_pipeline.draw_line(C, cred, C + N, cyellow);
    }

    // Draw the triangles as shaded triangles
    std::cout << "Draw the triangles as shaded triangles" << std::endl;

    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(phong_fragment_program);

    for (int i = 1; i <= t; ++i) {
	Icosahedron::triangle T = icosahedron.Triangle(i);
	Icosahedron::vertex v1 = T[0];
	Icosahedron::vertex v2 = T[1];
	Icosahedron::vertex v3 = T[2];

	// Just one normal per triangle.
	Icosahedron::normal   N = icosahedron.Normal(i);
	if (Zero(N))
	    std::cout << "DrawIcosahedron(): N == [" << N << "]" << std::endl;
	else {
	    N /= Norm(N);
	}

	render_pipeline.load_rasterizer(triangle_rasterizer);
	render_pipeline.load_vertex_program(transform_vertex_program);
	render_pipeline.load_fragment_program(phong_fragment_program);

	render_pipeline.draw_triangle(v1, N, cred, v2, N, cred, v3, N, cred); 

	// Draw the Normal
	// Just one normal per triangle.
	render_pipeline.load_rasterizer(line_rasterizer);
	render_pipeline.load_vertex_program(transform_vertex_program);
	render_pipeline.load_fragment_program(identity_fragment_program);

	Icosahedron::vertex C = (v1 + v2 + v3) / 3.0;
	render_pipeline.draw_line(C, cred, C + N, cyellow);
    }
}


/*******************************************************************\
*                                                                   *
*            S u b D i v i d e T r i a n g l e ( . . . )            *
*                                                                   *
\*******************************************************************/

void SubDivideTriangle(Icosahedron::triangle const& T, int t_subdiv)
{
    if (t_subdiv <= 0) {
	render_pipeline.load_rasterizer(triangle_rasterizer);
	render_pipeline.load_vertex_program(transform_vertex_program);
	render_pipeline.load_fragment_program(identity_fragment_program);

	// compute and draw the normals
	// one normal per vertex
	render_pipeline.load_rasterizer(line_rasterizer);
	render_pipeline.load_vertex_program(transform_vertex_program);
	render_pipeline.load_fragment_program(identity_fragment_program);

	Icosahedron::normal N_0(T[0]);
	if (!Zero(N_0)) N_0 /= Norm(N_0);
	Icosahedron::normal N_1(T[1]);
	if (!Zero(N_1)) N_1 /= Norm(N_1);
	Icosahedron::normal N_2(T[2]);
	if (!Zero(N_2)) N_2 /= Norm(N_2);

	// draw the triangle
	render_pipeline.load_rasterizer(triangle_rasterizer);
	render_pipeline.load_vertex_program(transform_vertex_program);
	render_pipeline.load_fragment_program(phong_fragment_program);
	//render_pipeline.load_fragment_program(identity_fragment_program);

	render_pipeline.draw_triangle(T[0], N_0, cred, T[1], N_1, cred, T[2], N_2, cred);	
    }
    else {
	// Something is really wrong here !
	//std::cout << "SubDivideTriangle(...)::SplitTheTriangle(...): t_subdiv == " << t_subdiv << std::endl;

	//render_pipeline.load_rasterizer(triangle_rasterizer);
	//render_pipeline.load_vertex_program(transform_vertex_program);
	//render_pipeline.load_fragment_program(phong_fragment_program);
	//render_pipeline.load_fragment_program(identity_fragment_program);

	render_pipeline.state().model()     = Identity();
	render_pipeline.state().inv_model() = Identity();

	Icosahedron::vertex v1 = T[0];
	Icosahedron::vertex v2 = T[1];
	Icosahedron::vertex v3 = T[2];

	// 3 New Vertices
	Icosahedron::vertex v12_m = (v1 + v2) / 2.0;
	v12_m = (v12_m / Norm(v12_m)) * icosahedron.Radius();

	Icosahedron::vertex v23_m = (v2 + v3) / 2.0;
	v23_m = (v23_m / Norm(v23_m)) * icosahedron.Radius();

	Icosahedron::vertex v31_m = (v3 + v1) / 2.0;
	v31_m = (v31_m / Norm(v31_m)) * icosahedron.Radius();

	// Subdivide the 4 new triangles
	Icosahedron::triangle T1;
	T1.push_back(v1); T1.push_back(v12_m); T1. push_back(v31_m);
	SubDivideTriangle(T1, t_subdiv - 1);

	Icosahedron::triangle T2;
	T2.push_back(v12_m); T2.push_back(v2); T2.push_back(v23_m);
	SubDivideTriangle(T2, t_subdiv - 1);
	
	Icosahedron::triangle T3;
	T3.push_back(v31_m); T3.push_back(v12_m); T3.push_back(v23_m);
	SubDivideTriangle(T3, t_subdiv - 1);

	Icosahedron::triangle T4;
	T4.push_back(v31_m); T4.push_back(v23_m); T4.push_back(v3);
	SubDivideTriangle(T4, t_subdiv - 1);
    }

}


/*******************************************************************\
*                                                                   *
*       D r a w S u b D i v i d e d I c o s a h e d r o n ( )       *
*                                                                   *
\*******************************************************************/

void DrawSubDividedIcosahedron()
{
/*******************************************************************\
*                                                                   *
*                       S c a l e F a c t o r                       *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    ScaleFactor = 1.0;


/*******************************************************************\
*                                                                   *
*                       T r a n s l a t i o n                       *
*                                                                   *
\*******************************************************************/

    MyMathTypes::vector3_type Translation(0.0, 0.0, 0.0);

/*******************************************************************\
*                                                                   *
*                            R a d i u s                            *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type R = icosahedron.Radius();
    std::cout << "Icosahedron radius ==  " << R << std::endl;

    BoundingBox<MyMathTypes> BB;


/*******************************************************************\
*                                                                   *
*                V i e w i n g   P a r a m e t e r s                *
*                                                                   *
\*******************************************************************/

    MyMathTypes::vector3_type VRP(0.0, 0.0, 0.0);
    MyMathTypes::vector3_type VPN(1.0, 1.0, 1.0);
    MyMathTypes::vector3_type VUP(0.0, 1.0, 0.0);
    MyMathTypes::vector3_type PRP(0.0, 0.0, 50.0);

    MyMathTypes::vector2_type lower_left( -4.0, -4.0);
    MyMathTypes::vector2_type upper_right( 4.0,  4.0);

    MyMathTypes::real_type    F =  4.8;
    MyMathTypes::real_type    B = -4.8;
    
    camera.set_projection(VRP, VPN, VUP, PRP,
			  lower_left, upper_right,
			  F, B,
			  winHeight, winHeight);


/*******************************************************************\
*                                                                   *
*                      L i g h t   S o u r c e                      *
*                                                                   *
\*******************************************************************/

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_a(0.5, 0.5, 0.5);
    render_pipeline.state().I_a() = I_a;

    // Set up the Light source parameters in graphics_state
    MyMathTypes::vector3_type I_p(1.0, 1.0, 1.0);
    MyMathTypes::vector3_type I_p_position(30.0, 30.0, 10.0);

    render_pipeline.state().I_p() = I_p;
    render_pipeline.state().light_position() = I_p_position;

/*******************************************************************\
*                                                                   *
*               M a t e r i a l   P r o p e r t i e s               *
*                                                                   *
\*******************************************************************/

    MyMathTypes::real_type    k_a = 0.2;
    MyMathTypes::vector3_type O_a(0.5, 0.0, 0.0);

    MyMathTypes::real_type    k_d = 0.8;
    MyMathTypes::vector3_type O_d(0.0, 1.0, 0.0);

    MyMathTypes::real_type    k_s = 0.5;
    MyMathTypes::vector3_type O_s(1.0, 1.0, 1.0);

    MyMathTypes::real_type    n     = 50.0;
    MyMathTypes::real_type    f_att = 1.0;   // Not used!
    
    // Set up the Material parameters
    render_pipeline.state().ambient_intensity()  = k_a;
    render_pipeline.state().ambient_color()      = O_a;

    render_pipeline.state().diffuse_intensity()  = k_d;
    render_pipeline.state().diffuse_color()      = O_d;

    render_pipeline.state().specular_intensity() = k_s;
    render_pipeline.state().specular_color()     = O_s;

    render_pipeline.state().fall_off() = n;


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   P i p e l i n e               *
*                                                                   *
\*******************************************************************/

    render_pipeline.load_rasterizer(triangle_rasterizer);
    render_pipeline.load_vertex_program(transform_vertex_program);
    render_pipeline.load_fragment_program(identity_fragment_program);
    
    render_pipeline.state().model()     = Identity();
    render_pipeline.state().inv_model() = Identity();


/*******************************************************************\
*                                                                   *
*         S u b D i v i d e   t h e   I c o s a h e d r o n         *
*                                                                   *
\*******************************************************************/

    Icosahedron icosahedron;
    
    DrawWorldCoordinateSystem(3.0);

    for (int i = 1; i <= icosahedron.NTriangles(); ++i) {
	Icosahedron::triangle T = icosahedron.Triangle(i);
	BB.Submit(T[0]); BB.Submit(T[1]); BB.Submit(T[2]);
	SubDivideTriangle(T, t_subdiv);
    }
}



/*******************************************************************\
*                                                                   *
*                     S y s t e m   T h i n g s                     *
*                                                                   *
\*******************************************************************/

/*******************************************************************\
*                                                                   *
*                         r e s h a p e ( )                         *
*                                                                   *
\*******************************************************************/

void reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glutPostRedisplay();
};


/*******************************************************************\
*                                                                   *
*                           u s a g e ( )                           *
*                                                                   *
\*******************************************************************/

void usage()
{
    std::cout << std::endl << std::flush;
    std::cout << "Legal keystrokes in the Window:"     << std::endl << std::flush;
    std::cout << "==============================="     << std::endl << std::flush;
    //std::cout << "\th : Help Message"               << std::endl << std::flush;
    std::cout << "\t? : Help Message"                  << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\tq : Quit"                          << std::endl << std::flush;
    std::cout << "\tc : Clear Screen"                  << std::endl << std::flush;
    std::cout << "\td : Redisplay the Screen"          << std::endl << std::flush;
    //std::cout << "\tg : Toggle Grid on/off (debug)" << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\ti : Initialize the Camera (does not work)"      << std::endl << std::flush;
    std::cout << "\tr : Reset the Camera"              << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\tPoints:"                           << std::endl << std::flush;
    std::cout << "\t-------"                           << std::endl << std::flush;
    std::cout << "\tp : Draw Points"                   << std::endl << std::flush;
    std::cout << "\tP : Debug Points"                  << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\tLines:"                            << std::endl << std::flush;
    std::cout << "\t------"                            << std::endl << std::flush;
    std::cout << "\tl : Draw Lines"                    << std::endl << std::flush;
    std::cout << "\tL : Debug Lines - forward"         << std::endl << std::flush;
    std::cout << "\tK : Debug Lines - backward"        << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\tTriangles:"                        << std::endl << std::flush;
    std::cout << "\t----------"                        << std::endl << std::flush;
    std::cout << "\tt : Draw Triangles"                << std::endl << std::flush;
    std::cout << "\tT : Debug Triangles - forward"     << std::endl << std::flush;
    std::cout << "\tR : Debug Triangles - backward"    << std::endl << std::flush;
    std::cout << "\ts : Gouraud Shaded Triangles"      << std::endl << std::flush;
    std::cout << "\th : Hidden Surfaces"               << std::endl << std::flush;
    std::cout << "\ta : Phong Shaded Triangles"        << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\tDraw a Wire Frame House:"          << std::endl << std::flush;
    std::cout << "\t------------------------"          << std::endl << std::flush;
    std::cout << "\t1 : Foley, Fig. 6.27"              << std::endl << std::flush;
    std::cout << "\t2 : Foley, Fig. 6.28"              << std::endl << std::flush;
    std::cout << "\t3 : Foley, Fig. 6.31"              << std::endl << std::flush;
    std::cout << "\t4 : Foley, Fig. 6.22"              << std::endl << std::flush;
    std::cout << "\t5 : Foley, Fig. 6.34"              << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\tDraw a Bezier Curves"              << std::endl << std::flush;
    std::cout << "\t--------------------"              << std::endl << std::flush;
    std::cout << "\tf : Draw a Bezier Curve using FWD" << std::endl << std::flush;
    std::cout << "\tw : Draw a Bezier Curve using SubDivision" << std::endl << std::flush;

    std::cout << "\tDraw General Parametric Surfaces:" << std::endl << std::flush;
    std::cout << "\t---------------------------------" << std::endl << std::flush;
    std::cout << "\tk : Klein Bottle"                  << std::endl << std::flush;
    std::cout << "\tm : Klein Bottle interior"         << std::endl << std::flush;
    std::cout << "\tx : Phong Surface"                 << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\tDraw Bezier Surfaces:"             << std::endl << std::flush;
    std::cout << "\t---------------------"             << std::endl << std::flush;
    std::cout << "\tn : UTAH Teapot"                   << std::endl << std::flush;
    std::cout << "\tz : Rocket"                        << std::endl << std::flush;
    std::cout << "\tv : Sailboat"                      << std::endl << std::flush;
    std::cout << "\tb : Pain"                          << std::endl << std::flush;
    std::cout << std::endl << std::flush;

    std::cout << "\tDraw an Icosahedron:"              << std::endl << std::flush;
    std::cout << "\t--------------------"              << std::endl << std::flush;
    std::cout << "\te : Icosahedron"                   << std::endl << std::flush;
    std::cout << "\tE : Subdivice Icosahedron"         << std::endl << std::flush;

    std::cout << "Keystroke: "                         << std::flush;
}


/*******************************************************************\
*                                                                   *
*         k e y b o a r d ( u c h a r ,   i n t ,   i n t )         *
*                                                                   *
\*******************************************************************/

void keyboard(unsigned char Key, int Xmouse, int Ymouse)
{
    glutSetWindowTitle("Foundations of Computer Graphics");

    std::cout << "You typed '" << Key << "': ";
    switch (Key) {
    //case 'h':
    case '?':
    case 'u':
	usage();
	break;
    case 'q':
    case 'Q':
        // quit the program
        std::cout << "Quit" << std::endl << std::endl << std::flush;
        exit(0);
        break;
    case 'c':
    case 'C':
	// clear the screen
	std::cout << "Clear Screen" << std::endl << std::flush;
	figure = 'c';
	glutPostRedisplay();
	break;
    case 'd':
    case 'D':
        // call the DisplayCallback as soon as possible
        std::cout << "Redisplay" << std::endl << std::flush;
        glutPostRedisplay();
        break;
    case 'g':
    case 'G':
        // toggle the grid on and off --- does not work right now --- turned off!
        std::cout << "Toggle Grid";
	figure = 'g';
        GridVisible = GridVisible ? 0 : 1;
	glutPostRedisplay();
        break;
    case 'i':
    case 'I':
	// initialize the camera
	std::cout << "Initialize Camera as given in InitCamera()" << std::endl << std::flush;
	figure = 'i';
	glutPostRedisplay();
	break;
    case 'r':
	// reset the camera
	std::cout << "Reset Camera to default" << std::endl << std::flush;
	figure = 'r';
	glutPostRedisplay();
	break;
    case 'p':
	// draw points
	std::cout << "Draw Point" << std::endl << std::flush;
	figure = 'p';
	glutPostRedisplay();
	break;
    case 'P':
	// debug points
	std::cout << "Debug Point" << std::endl << std::flush;
	figure = 'P';
	glutPostRedisplay();
	break;
    case 'l':
	// draw lines
	std::cout << "Draw Lines" << std::endl << std::flush;
	figure = 'l';
	glutPostRedisplay();
	break;
    case 'L':
	// debug lines
	std::cout << "Debug Lines - forward" << std::endl << std::flush;
	++l;
	if (l >= lmax) l = 1;
	std::cout << "l = " << l << std::endl << std::flush;
	figure = 'L';
	glutPostRedisplay();
	break;
    case 'K':
	// debug lines
	std::cout << "Debug Lines - backward" << std::endl << std::flush;
	--l;
	if (l <= 0) l = lmax - 1;
	std::cout << "l = " << l << std::endl << std::flush;
	figure = 'K';
	glutPostRedisplay();
	break;
    case 't':
	// draw triangles
	std::cout << "Draw Triangles" << std::endl << std::flush;
	figure = 't';
	glutPostRedisplay();
	break;
    case 'T':
	// debug triangles - forward
	std::cout << "Debug Triangles - forward" << std::endl << std::flush;
	++v;
	if (v >= vmax) v = 1;
	std::cout << "v = " << v << std::endl << std::flush;
	figure = 'T';
	glutPostRedisplay();
	break;
    case 'R':
 	// debug triangles - backward
	std::cout << "Debug Triangles - backward" << std::endl << std::flush;
	--v;
	if (v <= 0) v = vmax - 1;
	std::cout << "v = " << v << std::endl << std::flush;
	figure = 'T';
	glutPostRedisplay();
	break;
    case 's':
	// Draw Gouraud shaded triangles
	figure = 's';
	glutPostRedisplay();
	break;
    case 'h':
	// Hidden Surfaces
	figure = 'h';
	glutPostRedisplay();
	break;
    case 'a':
	// Phong shaded triangles
	figure = 'a';
	glutPostRedisplay();
	break;
    case '1':
	// draw the house
	figure = '1';
	//Foley_Fig_6_27();
	glutPostRedisplay();
	break;
    case '2':
	// draw the house
	figure = '2';
	//Foley_Fig_6_28();
	glutPostRedisplay();
	break;
    case '3':
	// draw the house
	figure = '3';
	//Foley_Fig_6_31();
	glutPostRedisplay();
	break;
    case '4':
	// draw the house
	figure = '4';
	//Foley_Fig_6_22();
	glutPostRedisplay();
	break;
    case '5':
	// draw the house
	figure = '5';
	//Foley_Fig_6_34();
	glutPostRedisplay();
	break;
    case 'f':
	// draw Bezier Curve using FWD
	figure = 'f';
	glutPostRedisplay();
	break;
    case 'w':
	// draw Bezier Curve using Subdivision
	figure = 'w';
	glutPostRedisplay();
	break;
    case 'k':
	// draw a Klein Bottle
	figure = 'k';
	glutPostRedisplay();
	break;
    case 'm':
	// draw Klein Bottle interior
	figure = 'm';
	glutPostRedisplay();
	break;
    case 'x':
	// draw the Phong Surface
	figure = 'x';
	glutPostRedisplay();
	break;
    case 'n':
	// draw UTAH Teapot
	figure = 'n';
	glutPostRedisplay();
	break;
    case 'z':
	// draw Rocket
	figure = 'z';
	glutPostRedisplay();
	break;
    case 'v':
	// draw Sailboat
	figure = 'v';
	glutPostRedisplay();
	break;
    case 'b':
	// draw Pain
	figure = 'b';
	glutPostRedisplay();
	break;
    case 'e':
	// draw Icosahedron
	++t;
	if (t > icosahedron.NTriangles()) t = 0;
	//std::cout << "t = " << t << std::endl << std::flush;
	figure = 'e';
	glutPostRedisplay();
	break;
    case 'E':
	// subdivide an Icosahedron
	++t_subdiv;
	if (t_subdiv > t_subdiv_max) t_subdiv = 0;
	figure = 'E';
	glutPostRedisplay();
	break;
    default:
	std::cout << "No action defined for key '" << Key << "'" << std::endl << std::flush;
	std::cout << std::endl << std::flush;
    }
    std::cout << std::endl << std::flush;
    usage();
    //glutPostRedisplay();
}


/*******************************************************************\
*                                                                   *
*                         d i s p l a y ( )                         *
*                                                                   *
\*******************************************************************/

void display()
{
    // This is where things happen! - all of your drawings should go here!

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode( GL_MODELVIEW );
    //////////////////////////////////////////////////////////////////
    
    // >> TODO USE YOUR SOFTWARE RENDERPIPEINE TO DRAW << 

#ifdef KENNY_ZBUFFER
    MyMathTypes::real_type     infinty  =  1.0;
#else
    MyMathTypes::real_type     infinity = -1.0;
#endif

    MyMathTypes::vector3_type  color( 0.0, 0.0, 1.0 );   // Blue Screen of Death Color
    
#if 0
    //  camera.set_model_view( ... );
    //  camera.set_projection( ... );
#endif
   
    render_pipeline.clear( infinity, color );


/*******************************************************************\
*                                                                   *
*               I n i t i a l i z e   C a m e r a ( )               *
*                                                                   *
\*******************************************************************/

    if (figure == 'i') {
	InitCamera();
	figure = 'c';
    }


/*******************************************************************\
*                                                                   *
*                    R e s e t   C a m e r a ( )                    *
*                                                                   *
\*******************************************************************/

    if (figure == 'r') {
	ResetCamera();
	figure = 'c';
	glutPostRedisplay();
    }


/*******************************************************************\
*                                                                   *
*                        D r a w G r i d ( )                        *
*                                                                   *
\*******************************************************************/

#if DRAWGRID
    //std::cout << "GridVisible == " << GridVisible << std::endl;
    if (GridVisible) DrawGrid();
#endif


/*******************************************************************\
*                                                                   *
*                      D r a w P o i n t s ( )                      *
*                                                                   *
\*******************************************************************/

#if DRAWPOINTS
    if (figure == 'p') {
	DrawPoint();
    }
#endif

/*******************************************************************\
*                                                                   *
*                     D e b u g P o i n t s ( )                     *
*                                                                   *
\*******************************************************************/

#if DEBUGPOINTS
    if (figure == 'P') {
	DebugPoint();
    }
#endif

/*******************************************************************\
*                                                                   *
*                      D r a w L i n e s ( )                        *
*                                                                   *
\*******************************************************************/

#if DRAWLINES
    if (figure == 'l') {
	DrawLines();
    }
#endif

    // Remember that you can load different rasterizers (line, triangle) by the call:
    // render_pipeline.load_rasterizer(new_rasterizer);
    //
    // The same is true for vertex_programs and fragment_programs

    // std::cout << "Load line_rasterizer" << std::endl;
    // render_pipeline.load_rasterizer( line_rasterizer );
    // render_pipeline.draw_line( v1, c1, v2, c2 );

    // std::cout << "Load triangle_rasterizer" << std::endl;
    // std::cout << "Draw a triangle" << std::endl;
    // render_pipeline.draw_triangle( v1, n1, c1, v2, n2, c2, v3, n3, c3);


/*******************************************************************\
*                                                                   *
*                      D e b u g L i n e s ( )                      *
*                                                                   *
\*******************************************************************/

#if DEBUGLINES
    if (figure == 'L') {
	DebugLines();
    }
    if (figure == 'K') {
	DebugLines();
    }
#endif


/*******************************************************************\
*                                                                   *
*                   D r a w T r i a n t l e s ( )                   *
*                                                                   *
\*******************************************************************/

#if DRAWTRIANGLES
    if (figure == 't') {
	DrawTriangles();
    }
#endif


/*******************************************************************\
*                                                                   *
*                  D e b u g T r i a n g l e s ( )                  *
*                                                                   *
\*******************************************************************/

#if DEBUGTRIANGLES
    if (figure == 'T') {
	DebugTriangles();
    }
    if (figure == 'R') {
	//--v;
	DebugTriangles();
    }
#endif


/*******************************************************************\
*                                                                   *
*                G o u r a u d T r i a n g l e s ( )                *
*                                                                   *
\*******************************************************************/

#if GOURAUDTRIANGLES
    if (figure == 's') {
	DrawGouraudTriangles();
    }
#endif


/*******************************************************************\
*                                                                   *
*                  H i d d e n S u r f a c e s ( )                  *
*                                                                   *
\*******************************************************************/

#if HIDDENSURFACES
    if (figure == 'h') {
	DrawHiddenSurfaces();
    }
#endif


/*******************************************************************\
*                                                                   *
*              D r a w P h o n g T r i a n g l e s ( )              *
*                                                                   *
\*******************************************************************/

#if PHONGTRIANGLES
    if (figure == 'a') {
	DrawPhongTriangles();
    }
#endif


/*******************************************************************\
*                                                                   *
*                       D r a w H o u s e ( )                       *
*                                                                   *
\*******************************************************************/

    if (figure == '1') {
	Foley_Fig_6_27();
    }


    if (figure == '2') {
	Foley_Fig_6_28();
    }

    if (figure == '3') {
	Foley_Fig_6_31();
    }

    if (figure == '4') {
	Foley_Fig_6_22();
    }
    
    if (figure == '5') {
	Foley_Fig_6_34();
    }

/*******************************************************************\
*                                                                   *
*              D r a w B e z i e r C u r v e F W D ( )              *
*                                                                   *
\*******************************************************************/

    if (figure == 'f') {
	DrawBezierCurveFWD();
    }


/*******************************************************************\
*                                                                   *
*        D r a w B e z i e r C u r v e S u b D i v i d e ( )        *
*                                                                   *
\*******************************************************************/

    if (figure == 'w') {
	DrawBezierCurveSubDivide();
    }


/*******************************************************************\
*                                                                   *
*                 D r a w K l e i n B o t t l e ( )                 *
*                                                                   *
\*******************************************************************/

    if (figure == 'k') {
	DrawKleinBottle();
    }


/*******************************************************************\
*                                                                   *
*         D r a w K l e i n B o t t l e I n t e r i o r ( )         *
*                                                                   *
\*******************************************************************/

    if (figure == 'm') {
	DrawKleinBottle();
    }

/*******************************************************************\
*                                                                   *
*                D r a w P h o n g S u r f a c e ( )                *
*                                                                   *
\*******************************************************************/

    if (figure == 'x') {
	DrawPhongSurface();
    }

/*******************************************************************\
*                                                                   *
*                  D r a w U T A H T e a p o t ( )                  *
*                                                                   *
\*******************************************************************/

    if (figure == 'n') {
	DrawUTAHTeapot();
    }

/*******************************************************************\
*                                                                   *
*                      D r a w R o c k e t ( )                      *
*                                                                   *
\*******************************************************************/

    if (figure == 'z') {
	DrawRocket();
    }

/*******************************************************************\
*                                                                   *
*                    D r a w S a i l b o a t ( )                    *
*                                                                   *
\*******************************************************************/

    if (figure == 'v') {
	DrawSailboat();
    }

/*******************************************************************\
*                                                                   *
*                        D r a w P a i n ( )                        *
*                                                                   *
\*******************************************************************/

    if (figure == 'b') {
	DrawPain();
    }

/*******************************************************************\
*                                                                   *
*                 D r a w I c o s a h e d r o n ( )                 *
*                                                                   *
\*******************************************************************/

    if (figure == 'e') {
	DrawIcosahedron();
    }

/*******************************************************************\
*                                                                   *
*       S u b d i v i d e   t h e   I c o s a h e d r o n ( )       *
*                                                                   *
\*******************************************************************/

    if (figure == 'E') {
	DrawSubDividedIcosahedron();
    }
    
/*******************************************************************\
*                                                                   *
*  F i n i s h   u p   t h e   d i s p l a y ( )   f u n c t i o n  *
*                                                                   *
\*******************************************************************/

    render_pipeline.flush();
    
    glFinish();
    bool AnyErrors = false;
    GLenum ErrorCode = GL_NO_ERROR;
    while ((ErrorCode = glGetError()) != GL_NO_ERROR) {
	std::cout << "display(): " << gluErrorString(ErrorCode) << std::endl;
        AnyErrors = true;
    }
    if (AnyErrors) {
	std::ostringstream errormessage;
        //errormessage << "file " << __FILE__ << ": line " << __LINE__ << ':' << std::endl;
        //errormessage << "    " << typeid(*this).name() << "::draw(): " << std::endl;
        errormessage << "display(): " << "OpenGL Rendering Failure" << std::ends;

        throw std::runtime_error(errormessage.str());
    }

    glutSwapBuffers();
    //glutPostRedisplay();
}


/*******************************************************************\
*                                                                   *
*                 p r o g r a m _ c l e a n u p ( )                 *
*                                                                   *
\*******************************************************************/

void program_cleanup()
{
    std::cout << "Bye, bye..." << std::endl;
    std::cout << std::endl << std::flush;
}


/*******************************************************************\
*                                                                   *
*                     R e f r e s h M e n u ( )                     *
*                                                                   *
\*******************************************************************/

void RefreshMenu()
{
    glutChangeToMenuEntry(1,  "   Help", 1);
    glutChangeToMenuEntry(2,  "   Quit", 2);
    glutChangeToMenuEntry(3,  "   Clear Screen", 3);
    glutChangeToMenuEntry(4,  "   Redisplay", 4);
#if 1
    glutChangeToMenuEntry(5,  "   Toggle Grid", 5);
    glutChangeToMenuEntry(6,  "   Init Camera", 6);
    glutChangeToMenuEntry(7,  "   Reset Camera", 7);
#endif
    glutChangeToMenuEntry(8,  "   Draw Points", 8);
    glutChangeToMenuEntry(9,  "   Debug Points", 9);
    glutChangeToMenuEntry(10, "   Draw Lines", 10);
    glutChangeToMenuEntry(11, "   Debug Lines Forward", 11);
    glutChangeToMenuEntry(12, "   Debug Lines Backward", 12);
    glutChangeToMenuEntry(13, "   Draw Triangles", 13);
    glutChangeToMenuEntry(14, "   Debug Triangles Forward", 14);
    glutChangeToMenuEntry(15, "   Debug Triangles Backward", 15);
    glutChangeToMenuEntry(16, "   Draw Gouraud Triangles", 16);
    glutChangeToMenuEntry(17, "   Draw Phong Triangles", 17);
    glutChangeToMenuEntry(18, "   Draw Hidden Surfaces", 18);
    glutChangeToMenuEntry(19, "   Draw Foley 6.27", 19);
    glutChangeToMenuEntry(20, "   Draw Foley 6.28", 20);
    glutChangeToMenuEntry(21, "   Draw Foley 6.31", 21);
    glutChangeToMenuEntry(22, "   Draw Foley 6.22", 22);
    glutChangeToMenuEntry(23, "   Draw Foley 6.34", 23);
    glutChangeToMenuEntry(24, "   Draw Bezier Curve FWD", 24);
    glutChangeToMenuEntry(25, "   Draw Bezier Curve Subdivide", 25);
    glutChangeToMenuEntry(26, "   Draw Klein Bottle", 26);
    glutChangeToMenuEntry(27, "   Draw Klein Bottle interior", 27);
    glutChangeToMenuEntry(28, "   Draw Phong Surface", 28);
    glutChangeToMenuEntry(29, "   Draw UTAH Teapot", 29);
    glutChangeToMenuEntry(30, "   Draw Rocket", 30);
    glutChangeToMenuEntry(31, "   Draw Sailboat", 31);
    glutChangeToMenuEntry(32, "   Draw Pain", 32);
    glutChangeToMenuEntry(33, "   Draw Icosahedron", 33);
    glutChangeToMenuEntry(34, "   SubDivide Icosahedron", 34);
}


/*******************************************************************\
*                                                                   *
*               S e l e c t M e n u I t e m ( i n t )               *
*                                                                   *
\*******************************************************************/

void SelectMenuItem(int value)
{
    switch (value) {
    case 1:
        RefreshMenu();
        glutChangeToMenuEntry(1, "-> Help", 1);
	usage();
        break;
    case 2:
        RefreshMenu();
        glutChangeToMenuEntry(2, "-> Quit", 2);
	exit(0);
        break;
    case 3:
        RefreshMenu();
        glutChangeToMenuEntry(3, "-> Clear Screen", 3);
	figure = 'c';
	glutPostRedisplay();
        break;
    case 4:
        RefreshMenu();
        glutChangeToMenuEntry(4, "-> Redisplay", 4);
	glutPostRedisplay();
        break;
#if 0
    case 5:
	RefreshMenu();
	glutChangeToMenuEntry(5, "-> Toggle Grid", 5);
	figure = 'g';
        GridVisible = GridVisible ? 0 : 1;
	break;
    case 6:
	RefreshMenu();
	glutChangeToMenuEntry(6, "-> Init Camera", 6);
	figure = 'i';
	glutPostRedisplay();
	break;
    case 7:
	RefreshMenu();
	glutChangeToMenuEntry(7, "-> Reset Camera", 7);
	figure = 'r';
	glutPostRedisplay();
	break;
#endif
    case 8:
	RefreshMenu();
	glutChangeToMenuEntry(8, "-> Draw Points", 8);
	figure = 'p';
	glutPostRedisplay();
	break;
    case 9:
	RefreshMenu();
	glutChangeToMenuEntry(9, "-> Debug Points", 9);
	figure = 'P';
	glutPostRedisplay();
	break;
    case 10:
	RefreshMenu();
	glutChangeToMenuEntry(10, "-> Draw Lines", 10);
	figure = 'l';
	glutPostRedisplay();
	break;
    case 11:
	RefreshMenu();
	glutChangeToMenuEntry(11, "-> Debug Lines Forward", 11);
	++l;
	if (l >= lmax) l = 1;
	figure = 'L';
	glutPostRedisplay();
	break;
    case 12:
	RefreshMenu();
	glutChangeToMenuEntry(12, "-> Debug Lines Backward", 12);
	--l;
	if (l <= 0) l = lmax - 1;
	figure = 'K';
	glutPostRedisplay();
	break;
    case 13:
	RefreshMenu();
	glutChangeToMenuEntry(13, "-> Draw Triangles", 13);
	figure = 't';
	glutPostRedisplay();
	break;
    case 14:
	RefreshMenu();
	glutChangeToMenuEntry(14, "-> Debug Triangles Forward", 14);
	++v;
	if (v >= vmax) v = 1;
	figure = 'T';
	glutPostRedisplay();
	break;
    case 15:
	RefreshMenu();
	glutChangeToMenuEntry(15, "-> Debug Triangles Backward", 15);
	--v;
	if (v <= 0) v = vmax - 1;
	figure = 'T';
	glutPostRedisplay();
	break;
    case 16:
	RefreshMenu();
	glutChangeToMenuEntry(16, "-> Draw Gouraud Triangles", 16);
	figure = 's';
	glutPostRedisplay();
	break;
    case 17:
	RefreshMenu();
	glutChangeToMenuEntry(17, "-> Draw Phong Triangles", 17);
	figure = 'a';
	glutPostRedisplay();
	break;
    case 18:
	RefreshMenu();
	glutChangeToMenuEntry(18, "-> Draw Hidden Surfaces", 18);
	figure = 'h';
	glutPostRedisplay();
	break;
    case 19:
	RefreshMenu();
	glutChangeToMenuEntry(19, "-> Draw Foley 6.27", 19);
	figure = '1';
	glutPostRedisplay();
	break;
    case 20:
	RefreshMenu();
	glutChangeToMenuEntry(20, "-> Draw Foley 6.28", 20);
	figure = '2';
	glutPostRedisplay();
	break;
    case 21:
	RefreshMenu();
	glutChangeToMenuEntry(21, "-> Draw Foley 6.31", 21);
	figure = '3';
	glutPostRedisplay();
	break;
    case 22:
	RefreshMenu();
	glutChangeToMenuEntry(22, "-> Draw Foley 6.22", 22);
	figure = '4';
	glutPostRedisplay();
	break;
    case 23:
	RefreshMenu();
	glutChangeToMenuEntry(23, "-> Draw Foley 6.34", 23);
	figure = '5';
	glutPostRedisplay();
	break;
    case 24:
	RefreshMenu();
	glutChangeToMenuEntry(24, "-> Draw Bezier Curve FWD", 24);
	figure = 'f';
	glutPostRedisplay();
	break;
    case 25:
	RefreshMenu();
	glutChangeToMenuEntry(25, "-> Draw Bezier Curve Subdivide", 25);
	figure = 'w';
	glutPostRedisplay();
	break;
    case 26:
	RefreshMenu();
	glutChangeToMenuEntry(26, "-> Draw Klein Bottle", 26);
	figure = 'k';
	glutPostRedisplay();
	break;
    case 27:
	RefreshMenu();
	glutChangeToMenuEntry(27, "-> Draw Klein Bottle interior", 27);
	figure = 'm';
	glutPostRedisplay();
	break;
    case 28:
	RefreshMenu();
	glutChangeToMenuEntry(28, "-> Draw Phong Surface", 28);
	figure = 'x';
	glutPostRedisplay();
	break;
    case 29:
	RefreshMenu();
	glutChangeToMenuEntry(29, "-> Draw UTAH Teapot", 29);
	figure = 'n';
	glutPostRedisplay();
	break;
    case 30:
	RefreshMenu();
	glutChangeToMenuEntry(30, "-> Draw Rocket", 30);
	figure = 'z';
	glutPostRedisplay();
	break;
    case 31:
	RefreshMenu();
	glutChangeToMenuEntry(31, "-> Draw Sailboat", 31);
	figure = 'v';
	glutPostRedisplay();
	break;
    case 32:
	RefreshMenu();
	glutChangeToMenuEntry(32, "-> Draw Pain", 32);
	figure = 'b';
	glutPostRedisplay();
	break;
    case 33:
	RefreshMenu();
	glutChangeToMenuEntry(33, "-> Draw Icosahedron", 33);
	figure = 'e';
	glutPostRedisplay();
	break;
    case 34:
	RefreshMenu();
	glutChangeToMenuEntry(34, "-> SubDivide Icosahedron", 34);
	figure = 'E';
	glutPostRedisplay();
	break;
    }
}


/*******************************************************************\
*                                                                   *
*                      C r e a t e M e n u ( )                      *
*                                                                   *
\*******************************************************************/

void CreateMenu()
{
    Menu = glutCreateMenu(SelectMenuItem);
    glutAddMenuEntry("   Help", 1);
    glutAddMenuEntry("   Quit", 2);
    glutAddMenuEntry("   Clear Screen", 3);
    glutAddMenuEntry("   Redisplay", 4);
#if 1
    glutAddMenuEntry("   Toggle Grid", 5);
    glutAddMenuEntry("   Init Camera", 6);
    glutAddMenuEntry("   Reset Camera", 7);
#endif
    glutAddMenuEntry("   Draw Points", 8);
    glutAddMenuEntry("   Debug Points", 9);
    glutAddMenuEntry("   Draw Lines", 10);
    glutAddMenuEntry("   Debug Lines Forward", 11);
    glutAddMenuEntry("   Debug Lines Backward", 12);
    glutAddMenuEntry("   Draw Triangles", 13);
    glutAddMenuEntry("   Debug Triangles Forward", 14);
    glutAddMenuEntry("   Debug Triangles Backward", 15);
    glutAddMenuEntry("   Draw Gouraud Triangles", 16);
    glutAddMenuEntry("   Draw Phong Triangles", 17);
    glutAddMenuEntry("   Draw Hidden Surfaces", 18);
    glutAddMenuEntry("   Draw Foley 6.27", 19);
    glutAddMenuEntry("   Draw Foley 6.28", 20);
    glutAddMenuEntry("   Draw Foley 6.31", 21);
    glutAddMenuEntry("   Draw Foley 6.22", 22);
    glutAddMenuEntry("   Draw Foley 6.34", 23);
    glutAddMenuEntry("   Draw Bezier Curve FWD", 24);
    glutAddMenuEntry("   Draw Bezier Curve Subdivide", 25);
    glutAddMenuEntry("   Draw Klein Bottle", 26);
    glutAddMenuEntry("   Draw Klein Bottle interior", 27);
    glutAddMenuEntry("   Draw Phong Surface", 28);
    glutAddMenuEntry("   Draw UTAH Teapot", 29);
    glutAddMenuEntry("   Draw Rocket", 30);
    glutAddMenuEntry("   Draw Sailboat", 31);
    glutAddMenuEntry("   Draw Pain", 32);
    glutAddMenuEntry("   Draw Icosahedron", 33);
    glutAddMenuEntry("   SubDivide Icosahedron", 34);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*******************************************************************\
*                                                                   *
*                            m a i n ( )                            *
*                                                                   *
\*******************************************************************/

int main( int argc, char **argv )
{
    try {
	atexit(program_cleanup);

	glutInit( &argc, argv );
    
	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
	glutInitWindowSize( winWidth, winHeight );
	glutInitWindowPosition( 50, 50 );
	glutCreateWindow( "Blue Screen of Death" );
    
	glClearColor( 1, 1, 1, 0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glutSwapBuffers();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glFinish();

	//////////////////////////////////////////////////////////////////
    
	// >> TODO INITIALIZE YOUR RENDERPIPELINE << 
    
	//--- connect hardware
	render_pipeline.load_vertex_program( identity_vertex_program );
	render_pipeline.load_fragment_program( identity_fragment_program );

	// Load the appropriate rasterizer when it is needed,
	// i.e. render_pipeline.load_rasterizer(line_rasterizer | triangle_rasterizer);
        // old: render_pipeline.load_rasterizer( rasterizer);
	
	//--- allocate memory
	render_pipeline.set_resolution(winWidth, winHeight );
    
	//--- set up graphics state
	render_pipeline.state().ambient_intensity() = 0.5;
	
	//--- init camera
	camera.init( render_pipeline );
	
	usage();

	//////////////////////////////////////////////////////////////////
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	CreateMenu();
	glutChangeToMenuEntry(1, "-> Help", 1);

	glutMainLoop();
    }
    catch (std::exception const& Exception) {
	std::cout << Exception.what() << std::endl;
    }

    return 0;
}
