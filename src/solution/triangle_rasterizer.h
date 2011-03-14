#ifndef TRIANGLE_RASTERIZER_H
#define TRIABGLE_RASTERIZER_H
//
// Graphics Framework.
// Copyright (C) 2010 Department of Computer Science, University of Copenhagen
//

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include "graphics/graphics.h"
#include "solution/edge_rasterizer.h"
#include "solution/linear_interpolator.h"
#include "solution/transformations.h"


// remove: #define NEWSCANLINESEARCH


namespace graphics {

    template<typename math_types>
    class MyTriangleRasterizer : public Rasterizer<math_types>
    {
    public:
	typedef typename math_types::vector3_type vector3_type;
	typedef typename math_types::real_type    real_type;


    public:


/*******************************************************************\
*                                                                   *
*            M y T r i a n g l e R a s t e r i z e r ( )            *
*                                                                   *
\*******************************************************************/

	MyTriangleRasterizer() : valid(false), Debug(false)
	{
	    //std::cout << "-->MyTriangleRasterizer" << std::endl;
	    //std::cout << "<--MyTriangleRasterizer" << std::endl;
	}

/*******************************************************************\
*                                                                   *
*           ~ M y T r i a n g l e R a s t e r i z e r ( )           *
*                                                                   *
\*******************************************************************/

	virtual ~MyTriangleRasterizer()
	{}


/*******************************************************************\
*                                                                   *
*                         i n i t ( . . . )                         *
*                                                                   *
\*******************************************************************/

	void init( vector3_type const& in_vertex1,
		   vector3_type const& in_normal1,
		   vector3_type const& in_worldpoint1,
		   vector3_type const& in_color1,
		   vector3_type const& in_vertex2,
		   vector3_type const& in_normal2,
		   vector3_type const& in_worldpoint2,
		   vector3_type const& in_color2,
		   vector3_type const& in_vertex3,
		   vector3_type const& in_normal3,
		   vector3_type const& in_worldpoint3,
		   vector3_type const& in_color3) 
	{
	    // This is a triangle rasterizer

	    // Save the original parameters
	    this->org_vertex[0] = in_vertex1;
	    this->org_vertex[1] = in_vertex2;
	    this->org_vertex[2] = in_vertex3;

	    this->org_normal[0] = in_normal1;
	    this->org_normal[1] = in_normal2;
	    this->org_normal[2] = in_normal3;

	    this->org_worldpoint[0] = in_worldpoint1;
	    this->org_worldpoint[1] = in_worldpoint2;
	    this->org_worldpoint[2] = in_worldpoint3;

	    this->org_color[0]  = in_color1;
	    this->org_color[1]  = in_color2;
	    this->org_color[2]  = in_color3;

	    this->cred    = vector3_type(1.0, 0.0, 0.0);
	    this->cgreen  = vector3_type(0.0, 1.0, 0.0);
	    this->cyellow = vector3_type(225.0 / 255.0, 245.0 / 255.0, 6.0 / 255.0);

	    this->color_current = this->org_color[0]; // in_color1;
//	    std::cout << std::endl;
//	    std::cout << "triangle_rasterizer::color_current == " << this->color_current << std::endl;

	    if (this->degenerate()) {
		this->valid = false;
		//throw std::runtime_error("MyTriangleRasterizer:: The triangle is degenerate, i.e. all three points are collinear");
	    }
	    else {
		//std::cout << "MyTriangleRasterizer::init(...): Triangle not degenerate" << std::endl;
		this->initialize_triangle();
	    }
//	    this->Debug = false;
//	    this->print_variables();
	}


/*******************************************************************\
*                                                                   *
*                         D e b u g O n ( )                         *
*                                                                   *
\*******************************************************************/

	bool DebugOn()
	{
	    bool oldvalue = this->Debug;
	    this->Debug = true;

	    return oldvalue;
	}


/*******************************************************************\
*                                                                   *
*                        D e b u g O f f ( )                        *
*                                                                   *
\*******************************************************************/

	bool DebugOff()
	{
	    bool oldvalue = this->Debug;
	    this->Debug = false;

	    return oldvalue;
	}


/*******************************************************************\
*                                                                   *
*                           V a l i d ( )                           *
*                                                                   *
\*******************************************************************/

	bool Valid() const
	{
	    return !(this->degenerate());
	}


/*******************************************************************\
*                                                                   *
*                      D e g e n e r a t e ( )                      *
*                                                                   *
\*******************************************************************/

	bool Degenerate() const
	{
	    return this->degenerate();
	}


/*******************************************************************\
*                                                                   *
*                               x ( )                               *
*                                                                   *
\*******************************************************************/

	int x() const
	{
	    if (!this->valid) {
                throw std::runtime_error("MyTriangleRasterizer::x(): Invalid State/Not Initialized");
            }
            return this->x_current;
	}


/*******************************************************************\
*                                                                   *
*                               y ( )                               *
*                                                                   *
\*******************************************************************/

	int y() const
	{
	    if (!this->valid) {
                throw std::runtime_error("MyTriangleRasterizer::y(): Invalid State/Not Initialized");
            }
            return this->y_current;
	}


/*******************************************************************\
*                                                                   *
*                           d e p t h ( )                           *
*                                                                   *
\*******************************************************************/

	real_type depth() const     
	{
	  //	    if (!this->valid) {
	  //                throw std::runtime_error("MyTriangleRasterizer::depth(): Invalid State/Not Initialized");
	  //            }
            //return this->z_current;
	  //	    if (!this->depth_interpolator.more_values()) {
	  //		throw std::runtime_error("MyTriangleRasterizer::depth(): Invalid depth_interpolator");
	  //	    }
	    return this->depth_interpolator.value();
	}


/*******************************************************************\
*                                                                   *
*                        p o s i t i o n ( )                        *
*                                                                   *
\*******************************************************************/

	vector3_type position() const 
        {
	  //	    if (!this->valid) {
	  //                throw std::runtime_error("MyTriangleRasterizer::position(): Invalid State/Not Initialized");
	  //            }
	    //return vector3_type(this->x(), this->y(), this->depth());
	  //	    if (!this->worldpoint_interpolator.more_values()) {
	  //		throw std::runtime_error("MyEdgeRasterizer::position(): Invalid worldpoint_interpolator");
	  //	    }
	    return this->worldpoint_interpolator.value();
	}


/*******************************************************************\
*                                                                   *
*                          n o r m a l ( )                          *
*                                                                   *
\*******************************************************************/

	vector3_type const& normal() const     
	{
	  //	    if (!this->valid) {
	  //                throw std::runtime_error("MyTriangleRasterizer::normal(): Invalid State/Not Iitialized");
	  //            }
	    //return this->Ncurrent;
	  //	    if (!this->normal_interpolator.more_values()) {
	  //		throw std::runtime_error("MyTriangleRasterizer::normal(): Invalid normal_interpolator");
	  //	    }
	    return this->normal_interpolator.value();
	}


/*******************************************************************\
*                                                                   *
*                           c o l o r ( )                           *
*                                                                   *
\*******************************************************************/

	vector3_type const& color() const 
	{
	  //	    if (!this->valid) {
	  //                throw std::runtime_error("MyTriangleRasterizer::color(): Invalid State/Not Initialized");
	  //            }

	    vector3_type return_color;
	    if (this->Debug) {
		return this->color_current;
	    }

	    //	    if (!this->color_interpolator.more_values()) {
	    //		throw std::runtime_error("MyEdgeRasterizer::color(): Invalid color_interpolator");
	    //	    }
	    return this->color_interpolator.value();
	}


/*******************************************************************\
*                                                                   *
*                 p r i n t _ v a r i a b l e s ( )                 *
*                                                                   *
\*******************************************************************/

	void print_variables()
        {
	    std::cout << "MyTriangleRasterizer: local variables" << std::endl;
	    std::cout << "=====================================" << std::endl;
	    std::cout << "\tvalid     == " << this->valid    << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tV1 = [" << this->org_vertex[0] << "]" << std::endl;
	    std::cout << "\tV2 = [" << this->org_vertex[1] << "]" << std::endl;
	    std::cout << "\tV3 = [" << this->org_vertex[2] << "]" << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tx_start   == " << this->x_start   << std::endl;
	    std::cout << "\ty_start   == " << this->y_start   << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tx_current == " << this->x_current << std::endl;
	    std::cout << "\ty_current == " << this->y_current << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tx_stop    == " << this->x_stop    << std::endl;
	    std::cout << "\ty_stop    == " << this->y_stop    << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tcolor_left    == " << this->color_left    << std::endl;
	    std::cout << "\tcolor_right   == " << this->color_right   << std::endl;
	    std::cout << "\tcolor_current == " << this->color_current << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tDebug == " << this->Debug << std::endl;
	    std::cout << std::endl;
	}


/*******************************************************************\
*                                                                   *
*                  m o r e _ f r a g m e n t s ( )                  *
*                                                                   *
\*******************************************************************/

	bool more_fragments() const
	{
	    return this->valid;
	}


/*******************************************************************\
*                                                                   *
*                   n e x t _ f r a g m e n t ( )                   *
*                                                                   *
\*******************************************************************/

	void next_fragment()
	{
	    // The new algorithm - and it does work for horizontal bottom lines!

	    if (this->x_current < this->x_stop) {
		this->x_current += 1;

		this->depth_interpolator.next_value();
		this->normal_interpolator.next_value();
		this->worldpoint_interpolator.next_value();
		this->color_interpolator.next_value();
	    }
	    else {
		// this->x_current >= this->x_stop, so find the next NonEmptyScanline
		this->valid = this->SearchForNonEmptyScanline();
	    }
// This must be changed
	    if (this->Debug) {
//		std::cout << "triangle_rasterizer::choose_color(int)" << std::endl;
		this->choose_color(this->x_current);
	    }
	    else {
//		std::cout << "Get the right color" << std::endl;
		this->color_current = this->org_color[0];
	    }

	    this->valid = this->leftedge.more_fragments();
	}



    private:

/*******************************************************************\
*                                                                   *
*             i n i t i a l i z e _ t r i a n g l e ( )             *
*                                                                   *
*         Initialize the current triangle for rasterization         *
*                                                                   *
\*******************************************************************/

	void initialize_triangle()
	{
	    //std::cout << "-->MyTriangleRasterizer::initialize_triangle()" << std::endl;

	    // Maybe the problem is here
	    //this->color_current = this->org_color[0];

	    this->lower_left = this->LowerLeft();
	    this->upper_left = this->UpperLeft();
	    this->the_other  = 3 - lower_left - upper_left;
            // Let ll be the integer representation of the lower_left vertex of the triangle,
	    // disregarding the z-component.
	    vector3_type ll(static_cast<int>(round(this->org_vertex[this->lower_left][1])),
			    static_cast<int>(round(this->org_vertex[this->lower_left][2])),
			    0);

            // Let ul be the integer representation of the upper_left vertex of the triangle,
	    // disregarding the z-component.
	    vector3_type ul(static_cast<int>(round(this->org_vertex[this->upper_left][1])),
			    static_cast<int>(round(this->org_vertex[this->upper_left][2])),
			    0);

            // Let ot be the integer representation of the the_other vertex of the triangle,
	    // disregarding the z-component.
	    vector3_type ot(static_cast<int>(round(this->org_vertex[this->the_other][1])),
			    static_cast<int>(round(this->org_vertex[this->the_other][2])),
			    0);

#if 0
	    std::cout << "MyTriangleRaserizer::initialize_triangle():" << std::endl;
	    std::cout << "    ll == [" << ll << "]" << std::endl;
	    std::cout << "    ul == [" << ul << "]" << std::endl;
	    std::cout << "    ot == [" << ot << "]" << std::endl;
	    std::cout << std::endl;
#endif

            // Let u be the vector from 'lower_left' to 'upper_left' vertices.
	    vector3_type u(ul - ll);

	    // Let v be the vector from 'lower_left' to 'the_other'.
	    vector3_type v(ot - ll);

	    // If the cross product (u x v) has a positive
	    // z-component then the point 'the_other' is to the left of u, else it is to the
	    // right of u.
	    real_type z_component_of_the_cross_product = u[1] * v[2]- u[2] * v[1];

	    if (Zero(z_component_of_the_cross_product)) {
		std::cout << "MyTriangleRasterizer::initialize_triangle(): The triangle is degenerate" << std::endl;
		//throw std::runtime_error("The triangle is degenerate");
	    }

	    if (z_component_of_the_cross_product > 0) {
		// The vertex the_other is to the left of the longest vector u.
		// Therefore, the leftedge has two edges associated to it
		// (lower_left -> the_other), and (the_other -> upper_left),
		// while the right edge has only one (lower_left -> upper_left).

		// Here there is no need to check for a horizontal edge, because it
		// would be a top/bottom edge, and therefore it would not be drawn anyway.

		this->leftedge.init(this->org_vertex[this->lower_left],    // first vertex
				    this->org_normal[this->lower_left],
				    this->org_worldpoint[this->lower_left],
				    this->org_color[this->lower_left],
				    this->org_vertex[this->the_other],     // second vertex
				    this->org_normal[this->the_other],
				    this->org_worldpoint[this->the_other],
				    this->org_color[this->the_other],
				    this->org_vertex[this->upper_left],    // third vertex
				    this->org_normal[this->upper_left],
				    this->org_worldpoint[this->upper_left],
				    this->org_color[this->upper_left]);

		this->rightedge.init(this->org_vertex[this->lower_left],   // first vertex
				     this->org_normal[this->lower_left],
				     this->org_worldpoint[this->lower_left],
				     this->org_color[this->lower_left],
				     this->org_vertex[this->upper_left],   // second vertex
				     this->org_normal[this->upper_left],
				     this->org_worldpoint[this->upper_left],
				     this->org_color[this->upper_left]);
	    }
	    else {
                // The vertex the_other is to the right of the longest vector u.
		// Therefore, the leftedge has only one edge assigned to it
		// (lower_left -> upper_left), while the  rightedge has two edges
		// associated to it (lower_left -> the_other), and (the_other -> upper_left).

		// Here there is no need to check for a horizontal edge, because it
		// would be a top/bottom edge, and therefore it would not be drawn anyway.

		this->leftedge.init(this->org_vertex[this->lower_left],    // first vertex
				    this->org_normal[this->lower_left],
				    this->org_worldpoint[this->lower_left],
				    this->org_color[this->lower_left],
				    this->org_vertex[this->upper_left],    // second vertex
				    this->org_normal[this->upper_left],
				    this->org_worldpoint[this->upper_left],
				    this->org_color[this->upper_left]);

		this->rightedge.init(this->org_vertex[this->lower_left],   // first vertex
				     this->org_normal[this->lower_left],
				     this->org_worldpoint[this->lower_left],
				     this->org_color[this->lower_left],
				     this->org_vertex[this->the_other],    // second vertex
				     this->org_normal[this->the_other],
				     this->org_worldpoint[this->the_other],
				     this->org_color[this->the_other],
				     this->org_vertex[this->upper_left],   // third vertex
				     this->org_normal[this->upper_left],
				     this->org_worldpoint[this->upper_left],
				     this->org_color[this->upper_left]);
	    }

	    // Now the leftedge and rightedge `edge_rasterizers' are initialized, so they are
	    // ready for use.

	    this->x_start   = this->leftedge.x();
	    this->y_start   = this->leftedge.y();
	    this->z_start   = this->leftedge.depth();

	    this->x_current = this->x_start;
	    this->y_current = this->y_start;
	    this->z_current = this->z_start;

	    this->x_stop    = this->rightedge.x() - 1;
	    this->y_stop    = static_cast<int>(round(this->org_vertex[this->upper_left][2]));
	    this->z_stop    = this->rightedge.depth();

	    // Now, the vertices and edges are in place, go initialize the interpolators

	    // Depth Interpolator
	    this->depth_interpolator.init(this->x_start, this->x_stop,
	    				  this->leftedge.depth(), this->rightedge.depth());

            // Normal Interpolator
	    this->normal_interpolator.init(this->x_start, this->x_stop,
	    				   this->leftedge.normal(),  this->rightedge.normal());

	    // World Point Interpolator
	    this->worldpoint_interpolator.init(this->x_start, this->x_stop,
	    				       this->leftedge.position(), this->rightedge.position());

            // Color Interpolator
	    this->color_interpolator.init(this->x_start, this->x_stop,
	    				  this->leftedge.color(), this->rightedge.color());



	    this->valid = true;    // necessary?

// This must be changed
	    if (this->Debug) {
		this->choose_color(this->x_start); // this->org_color[0];  // this->cgreen;
	    }
	    else {
//		std::cout << "Get the right color" << std::endl;
		this->color_current = this->org_color[0];
	    }

	    if (this->x_current < this->x_stop)
		this->valid = true;
	    else {
		// Should only be called if the scanline is empty
		this->valid = this->SearchForNonEmptyScanline();
	    }
	    //std::cout << "<--MyTriangleRasterizer::initialize_triangle()" << std::endl;
	}

/*******************************************************************\
*                                                                   *
*                      d e g e n e r a t e ( )                      *
*                                                                   *
*     A triangle is degenerate if all three points are co-linear    *
*                                                                   *
\*******************************************************************/

	bool degenerate()
	{
	    bool result = false;

	    vector3_type int_vertex1(static_cast<int>(round(this->org_vertex[0][1])),
				     static_cast<int>(round(this->org_vertex[0][2])),
				     0);
	    vector3_type int_vertex2(static_cast<int>(round(this->org_vertex[1][1])),
				     static_cast<int>(round(this->org_vertex[1][2])),
				     0);
	    vector3_type int_vertex3(static_cast<int>(round(this->org_vertex[2][1])),
				     static_cast<int>(round(this->org_vertex[2][2])),
				     0);

	    vector3_type u(int_vertex2 - int_vertex1);
	    vector3_type v(int_vertex3 - int_vertex1);

	    real_type z_component_of_the_cross_product = u[1] * v[2] - u[2] * v[1];

#if 0
	    vector3_type cross_product = Cross(int_vertex2 - int_vertex1,
					       int_vertex3 - int_vertex1);
	    vector3_type zero_vector(0, 0, 0);
#endif
	    if (Zero(z_component_of_the_cross_product)) {
		/*std::cout << "triangle_rasterizer::degenerate():The triangle is degenerate" << std::endl;
		std::cout << "    vertex[1] = [" << int_vertex1 << "]" << std::endl;
		std::cout << "    vertex[2] = [" << int_vertex2 << "]" << std::endl;
		std::cout << "    vertex[3] = [" << int_vertex3 << "]" << std::endl;
		std::cout << "    This triangle will be ignored..."    << std::endl;*/

		// Look here tomorrow: 26.03.2010-05:05
		// Try to use render_pipeline.draw_debugline(...) to mark it in red.
		// But the render_pipeline must be given as a parameter to the constructor, and
		// saved in a private variable.

		result = true;
	    }

	    return result;
	}


/*******************************************************************\
*                                                                   *
*                       L o w e r L e f t ( )                       *
*                                                                   *
\*******************************************************************/

	// LowerLeft() returns the index of the vertex with the smallest y-coordinate
	// If there is a horizontal edge, the vertex with the smallest 
	// x-coordinate is chosen.
	// The computations should be done in integer coordinates.
	int LowerLeft()
	{
	    int ll = 0;
	    for (int i = ll + 1; i < 3; ++i) {
		if
		    (
			(
			    static_cast<int>(round(this->org_vertex[i][2]))
			    <
			    static_cast<int>(round(this->org_vertex[ll][2]))
			)
			||
			(
			    (
				static_cast<int>(round(this->org_vertex[i][2]))
				==
				static_cast<int>(round(this->org_vertex[ll][2]))
			    )
			    &&
			    (
				static_cast<int>(round(this->org_vertex[i][1]))
				<
				static_cast<int>(round(this->org_vertex[ll][1]))
			    )
			)
		    )
		{
		    ll = i;
		}
	    }
	    return ll;
	}


/*******************************************************************\
*                                                                   *
*                       U p p e r L e f t ( )                       *
*                                                                   *
\*******************************************************************/

	// UpperLeft() returns the index of the vertex with the greatest y-coordinate
	// If there is a horizontal edge, the vertex with the smallest 
	// x-coordinate is chosen.
	// The computations should be done in integer coordinates.
	int UpperLeft()
	{
	    int ul = 0;
	    for (int i = ul + 1; i < 3; ++i) {
		if
		    (
			(
			    static_cast<int>(round(this->org_vertex[i][2]))
			    >
			    static_cast<int>(round(this->org_vertex[ul][2]))
			)
			||
		        (
			    (
				static_cast<int>(round(this->org_vertex[i][2]))
				==
				static_cast<int>(round(this->org_vertex[ul][2]))
			    )
			    &&
			    (
				static_cast<int>(round(this->org_vertex[i][1]))
				<
				static_cast<int>(round(this->org_vertex[ul][1]))
			    )
			)
		    )
		{
		    ul = i;
		}
	    }
	    return ul;
	}


/*******************************************************************\
*                                                                   *
*       S e a r c h F o r N o n E m p t y S c a n l i n e ( )       *
*                                                                   *
\*******************************************************************/

	bool SearchForNonEmptyScanline()
	{
	    //std::cout << "-->SearchForNonEmptyScanline()" << std::endl;

	    // Assumes that the current scanline is empty!
	    this->valid = false;
	    this->leftedge.next_fragment();
	    this->rightedge.next_fragment();

	    this->valid = (this->leftedge.more_fragments()) && (this->rightedge.more_fragments());

	    bool NonEmptyScanlineFound = false;
	    while (!NonEmptyScanlineFound && this->valid) {
		this->x_start   = this->leftedge.x();
		this->x_current = this->x_start;
		this->y_current = this->leftedge.y();

		this->x_stop    = this->rightedge.x() - 1;

		//std::cout << "scanline: " << this->y_current
		//	  << " [" << this->x_start << ", " << this->x_stop << "]" << std::endl;

		if (this->x_current <= this->x_stop) {
		    //std::cout << "   SearchForNonEmptyScanline():NonEmpty scanline found" << std::endl;
		    NonEmptyScanlineFound = true;

		    this->x_start   = this->leftedge.x();
		    this->x_stop    = this->rightedge.x();
		    this->x_current = this->x_start;

		    //std::cout << "Scanline y = " << this->y_current << ", (x_l, x_r) = ("
		    //	      << this->x_start << ", " << this->x_stop << ")" << std::endl;

		    // Initialize the interpolators using the new values from the edges.

		    // Depth Interpolator
#if 0
		    std::cout << "depth_interpolator.init("
			      << this->x_start << ", "
			      << this->x_stop
			      << ")";
		    std::cout << std::endl;
#endif
#if 0
		    std::cout << "LeftEdge: (x, y, depth)  = ("
			      << this->leftedge.x() << ", "
			      << this->leftedge.y() << ", "
			      << this->leftedge.depth() << ")"
			      << std::endl;
		    std::cout << "RightEdge: (x, y, depth) = ("
			      << this->rightedge.x() << ", "
			      << this->rightedge.y() << ", "
			      << this->rightedge.depth() << ")"
			      << std::endl;
#endif
		    this->depth_interpolator.init(this->x_start, this->x_stop,
		    				  this->leftedge.depth(), this->rightedge.depth());

		    // Normal Interpolator
		    //std::cout << "Now the normals" << std::endl;
		    this->normal_interpolator.init(this->x_start, this->x_stop,
		    				   this->leftedge.normal(),  this->rightedge.normal());

		    // World Point Interpolator
		    this->worldpoint_interpolator.init(this->x_start, this->x_stop,
		    				       this->leftedge.position(), this->rightedge.position());

		    // Color Interpolator
		    this->color_interpolator.init(this->x_start, this->x_stop,
		    				  this->leftedge.color(), this->rightedge.color());

		    this->valid = true;
		}
		else {
		    NonEmptyScanlineFound = false;
		    this->leftedge.next_fragment();
		    this->rightedge.next_fragment();
		    this->valid =
			(this->leftedge.more_fragments()) && (this->rightedge.more_fragments());
		}
	    }

	    if (this->valid) {
		this->y_current = this->leftedge.y();
		this->x_start   = this->leftedge.x();
		this->x_current = this->x_start;
		this->x_stop    = this->rightedge.x() - 1;

		this->z_start   = this->leftedge.depth();
		this->z_current = this->z_start;
		this->z_stop    = this->rightedge.depth();

		this->Nleft     = this->leftedge.normal();
		this->Ncurrent  = this->Nleft;
		this->Nright    = this->rightedge.normal();

		this->Worldleft    = this->leftedge.position();
		this->Worldcurrent = this->Worldleft;
		this->Worldright   = this->rightedge.position();

		this->color_left    = this->leftedge.color();
		this->color_current = this->color_left;
		this->color_right   = this->rightedge.color();

		if (this->Debug) {
		    this->choose_color(this->x_start);
		}
	    }
	    //std::cout << "<--SearchForNonEmptyScanline()" << std::endl;

	    return this->valid;
	}


/*******************************************************************\
*                                                                   *
*               c h o o s e _ c o l o r ( i n t   x )               *
*                                                                   *
\*******************************************************************/

	void choose_color(int x)
	{
	    // x is the position on a scanline in a triangle - they all have different colors:
	    //    xstart  : green
	    //    xcurrent: yellow
	    //    xstop   : red
	    // This is like a trafic-light: green->go ahead, yellow->be carefull, red->stop!

	    //std::cout << "-->triangle_rasterizer::choose_color(int)" << std::endl;

	    this->color_current = this->cyellow;
	    if (x == this->x_stop) {
		this->color_current = this->cred;
	    }
	    if (x == this->x_start) {
		this->color_current = this->cgreen;
	    }

	    //std::cout << "<--triangle_rasterizer::choose_color(int)" << std::endl;
	}


/*******************************************************************\
*                                                                   *
*                 P r i v a t e   V a r i a b l e s                 *
*                                                                   *
\*******************************************************************/

	// The Debug variable
	bool Debug;

	// The original 3D vertices
	vector3_type org_vertex[3];

	// The original 3D normals
	vector3_type org_normal[3];

	// The original 3D world coordinates
	vector3_type org_worldpoint[3];

	// The original vertex colors
	vector3_type org_color[3];

	// Indices into the vertex table
	int lower_left;
	int upper_left;
	int the_other;

        // Screen coordinates
	int       x_start;
	int       y_start;
	real_type z_start;

	int       x_stop;
	int       y_stop;
	real_type z_stop;

	int       x_current;
	int       y_current;
	real_type z_current;


	vector3_type Nleft;
	vector3_type Nright;
	vector3_type Ncurrent;

	vector3_type Worldleft;
	vector3_type Worldright;
	vector3_type Worldcurrent;
	
	vector3_type color_left;
	vector3_type color_right;
	vector3_type color_current;

	vector3_type cred;
	vector3_type cgreen;
	vector3_type cyellow;

	MyEdgeRasterizer<math_types> leftedge;
	MyEdgeRasterizer<math_types> rightedge;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::real_type> depth_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::vector3_type> normal_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::vector3_type> worldpoint_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::vector3_type> color_interpolator;

	bool valid;
    };

}// end namespace graphics

// TRIANGLE_RASTERIZER_H
#endif
