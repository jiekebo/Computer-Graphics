#ifndef LINE_RASTERIZER_H
#define LINE_RASTERIZER_H
//
// Graphics Framework.
// Copyright (C) 2008 Department of Computer Science, University of Copenhagen
//

#include <iostream>
#include <iomanip>
#include <cmath>

#include "graphics/graphics.h"
#include "linear_interpolator.h"


/*******************************************************************\
*                                                                   *
*                N a m e S p a c e   g r a p h i c s                *
*                                                                   *
\*******************************************************************/

namespace graphics {

/*******************************************************************\
*                                                                   *
*            C l a s s   M y L i n e R a s t e r i z e r            *
*                                                                   *
\*******************************************************************/

    template<typename math_types>
    class MyLineRasterizer : public Rasterizer<math_types>
    {

/*******************************************************************\
*                                                                   *
*                       P u l i c   T y p e s                       *
*                                                                   *
\*******************************************************************/

    public:

/*******************************************************************\
*                                                                   *
*                      v e c t o r 3 _ t y p e                      *
*                                                                   *
\*******************************************************************/

	typedef typename math_types::vector3_type      vector3_type;

/*******************************************************************\
*                                                                   *
*                         r e a l _ t y p e                         *
*                                                                   *
\*******************************************************************/

	typedef typename math_types::real_type         real_type;



/*******************************************************************\
*                                                                   *
*                    P u b l i c   M e m b e r s                    *
*                                                                   *
\*******************************************************************/

    public:

/*******************************************************************\
*                                                                   *
*                M y L i n e R a s t e r i z e r ( )                *
*                                                                   *
\*******************************************************************/

	MyLineRasterizer() : zero_normal(0, 0, 0), valid(false), Debug(false)
	{}


/*******************************************************************\
*                                                                   *
*                ~ M y L i n e R a s t e r i z e r ( )              *
*                                                                   *
\*******************************************************************/

	virtual ~MyLineRasterizer() {}


/*******************************************************************\
*                                                                   *
*           i n i t ( 4   x   v e c t o r 3 _ t y p e & )           *
*                                                                   *
\*******************************************************************/

	void init( vector3_type const& in_vertex1,
		   vector3_type const& in_color1,
		   vector3_type const& in_vertex2,
		   vector3_type const& in_color2)
	{
	    // This is a line rasterizer
	    // The vertices are in 3D screen coordinates

	    this->zero_normal = vector3_type(0, 0, 0);

	    // Convert (x, y) to integer coordinates
	    this->x_start = static_cast<int>(round(in_vertex1[1]));
	    this->y_start = static_cast<int>(round(in_vertex1[2]));
	    this->z_start = in_vertex1[3];

	    this->x_stop  = static_cast<int>(round(in_vertex2[1]));
	    this->y_stop  = static_cast<int>(round(in_vertex2[2]));
	    this->z_stop  = in_vertex2[3];

	    this->x_current = this->x_start;
	    this->y_current = this->y_start;

	    this->color_start   = in_color1;
	    this->color_stop    = in_color2;

	    // Initilize the internal variables: dx and dy
	    this->dx      = x_stop - x_start;
	    this->dy      = y_stop - y_start;

	    // Initialize the steps: DeltaX and DeltaY, step positive or negative
	    this->DeltaX  = (dx < 0) ? -1 : 1;
	    this->DeltaY  = (dy < 0) ? -1 : 1;

	    // Initialize helper variables: abs_2dx == 2 * |dx|, abs_2dy == 2 * |dy|
	    this->abs_2dx  = std::abs(this->dx) << 1;
	    this->abs_2dy  = std::abs(this->dy) << 1;

	    // Determine if the line is x-dominat or y-dominant
	    if (this->abs_2dx > this->abs_2dy) {
		// The line is x-dominant
		this->leftrightscan  = (DeltaX > 0) ? true : false;
		this->distance       = this->abs_2dy - (this->abs_2dx >> 1);
		this->valid          = (this->x_start != this->x_stop);

		// Initialize the depth_interpolator
		this->depth_interpolator.init(this->x_start, this->x_stop, this->z_start, this->z_stop);


		// Initialize the normal_interpolator
		// this->normal_interpolator.init(this->x_start, this->x_stop, this->normal_start, this->normal_stop);

		// Initialize the worldpoint_interpolator
		// this->worldpoint_interpolator.init(this->x_start, this->x_stop, this->worldpoint_start, this->worldpoint_stop);

		// Initialize the color_interpolator
		this->color_interpolator.init(this->x_start, this->x_stop, this->color_start, this->color_stop);

		// Set up a pointer for the inner-loop for an x-donmiant line
		// This is a pointer to a private member function!
		// Thererore it looks very strange, but this is how to do it!
		this->innerloop = &MyLineRasterizer::x_dominant_innerloop;
	    }
	    else {
		// The line is y-dominant
		this->leftrightscan  = (DeltaY > 0) ? true : false;
		this->distance       = this->abs_2dx - (this->abs_2dy >> 1);
		this->valid          = (this->y_start != this->y_stop);

                // Initialize the depth_interpolator
		this->depth_interpolator.init(this->y_start, this->y_stop, this->z_start, this->z_stop);

                // Initialize the color_interpolator
		this->color_interpolator.init(this->y_start, this->y_stop, this->color_start, this->color_stop);

                // Set up a pointer for the inner-loop for a y-donmiant line
		// This is a pointer to a private member function!
		// Thererore it looks very strange, but this is how to do it!
		this->innerloop = &MyLineRasterizer::y_dominant_innerloop;
	    }

	    //this->Debug = false;
	    //this->print_variables();
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
*                               x ( )                               *
*                                                                   *
\*******************************************************************/

	int x() const
	{
	    if (!this->valid) {
		throw std::runtime_error("MyLineRasterizer::x():Invalid State/Not Initialized");
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
		throw std::runtime_error("MyLineRasterizer::y():Invalid State/Not Initialized");
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
	    if (!this->valid) {
		throw std::runtime_error("MyLineRasterizer::depth():Invalid State/Not Initialized");
	    }
	    return this->depth_interpolator.value();
	}


/*******************************************************************\
*                                                                   *
*                        p o s i t i o n ( )                        *
*                                                                   *
\*******************************************************************/

	vector3_type position() const
        {
	    if (!this->valid) {
		throw std::runtime_error("MyLineRasterizer::position():Invalid State/Not Initialized");
	    }
	    return vector3_type(this->x(), this->y(), this->depth());
	}


/*******************************************************************\
*                                                                   *
*                          n o r m a l ( )                          *
*                                                                   *
\*******************************************************************/

	vector3_type const& normal() const
	{
	    if (!this->valid) {
		throw std::runtime_error("MyLineRasterizer::normal():Invalid State/Not Initialized");
	    }
	    return this->zero_normal;    // The program should never come here!
	}


/*******************************************************************\
*                                                                   *
*                           c o l o r ( )                           *
*                                                                   *
\*******************************************************************/

	vector3_type const& color() const
	{
	    if (!this->valid) {
		throw std::runtime_error("MyLineRasterizer::color():Invalid State/Not Initialized");
	    }

	    // std::cout << "line_rasterizer::color():color() = " << this->color() << std::endl;

	    return this->color_interpolator.value();
	}


/*******************************************************************\
*                                                                   *
*                  m o r e _ f r a g m e n t s ( )                  *
*                                                                   *
\*******************************************************************/


	bool more_fragments() const
	{
            // Usage:
	    //    Assume a pointer variable rasterizer is set up probably
	    //     while (rasterizer->more_fragments()) {
	    //         int x = rasterize->x();
	    //         int y = rasterizer->y();
	    //         MyMathTypes::real_type depth         = rasterizer->depth();
	    //         MyMathTypes::vector3_type position   = rasterizer->position();
	    //         MyMathTypes::vector3_type color      = rasterizer->color();
	    //             ...
	    //             use the values ...
	    //             ...
	    //         rasterizer->next_fragment();
	    //    }

	    return this->valid;
	}


/*******************************************************************\
*                                                                   *
*                   n e x t _ f r a g m e n t ( )                   *
*                                                                   *
\*******************************************************************/

	void next_fragment()
	{
	    // Dereference a pointer to a private member function.
	    // It looks strange, but it is the way to do it!
	    (this->*innerloop)();

	    this->depth_interpolator.next_value();
	    this->color_interpolator.next_value();
     	}


/*******************************************************************\
*                                                                   *
*                 p r i n t _ v a r i a b l e s ( )                 *
*                                                                   *
\*******************************************************************/

	void print_variables()
	{
	    std::cout << "MyLineRasterizer: local variables" << std::endl;
	    std::cout << "=================================" << std::endl;
	    std::cout << "\tvalid     == " << this->valid    << std::endl;
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
	    std::cout << "\tDeltaX    == " << this->DeltaX    << std::endl;
	    std::cout << "\tDeltaY    == " << this->DeltaY    << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tleftrightscan = " << this->leftrightscan << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tdx        == " << this->dx        << std::endl;
	    std::cout << "\tdy        == " << this->dy        << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tabs_2dx   == " << this->abs_2dx   << std::endl;
	    std::cout << "\tabs_2dy   == " << this->abs_2dy   << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tdistance  == " << this->distance  << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tcolor_start ==   " << this->color_start   << std::endl;
	    std::cout << "\tcolor_stop  ==   " << this->color_stop    << std::endl;
	    std::cout << "\tcolor_current == " << this->color_current << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tDebug == " << this->Debug << std::endl;
	    std::cout << std::endl;
	}

    protected:


/*******************************************************************\
*                                                                   *
*                   P r i v a t e   M e m b e r s                   *
*                                                                   *
\*******************************************************************/

    private:


/*******************************************************************\
*                                                                   *
*                x _ d o m i n a n t _ i n n e r l o o p ( )        *
*                                                                   *
\*******************************************************************/

	void x_dominant_innerloop()
	{
	    if (this->x_current == this->x_stop)
		this->valid = false;
	    else {
		if ((this->distance > 0) || ((this->distance == 0) && this->leftrightscan)) {
		    this->y_current += this->DeltaY;
		    this->distance  -= this->abs_2dx;
		}
		this->x_current += this->DeltaX;
		this->distance  += this->abs_2dy;
	    }
	}


/*******************************************************************\
*                                                                   *
*            y _ d o m i n a n t _ i n n e r l o o p ( )            *
*                                                                   *
\*******************************************************************/

	void y_dominant_innerloop()
	{
	    if (this->y_current == this->y_stop)
		this->valid = false;
	    else {
		if ((this->distance > 0) || ((this->distance == 0) && this->leftrightscan)) {
		    this->x_current += this->DeltaX;
		    this->distance  -= this->abs_2dy;
		}
		this->y_current += this->DeltaY;
		this->distance  += this->abs_2dx;
	    }
	}


/*******************************************************************\
*                                                                   *
*                 P r i v a t e   V a r i a b l e s                 *
*                                                                   *
\*******************************************************************/

	// This looks strange, but it is the definition of a pointer to a
	// private member function! That is how it is done!
	void         (MyLineRasterizer::*innerloop)();

	int          x_start;
	int          y_start;
	real_type    z_start;

	int          x_stop;
	int          y_stop;
	real_type    z_stop;

	int          x_current;
	int          y_current;

	vector3_type color_start;
	vector3_type color_stop;

	bool         leftrightscan;

	int          dx;
	int          dy;
	int          abs_2dx;
	int          abs_2dy;
	int          distance;

	int          DeltaX;
	int          DeltaY;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::real_type> depth_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	// LinearInterpolator<math_types, typename math_types::vector3_type> normal_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	// LinearInterpolator<math_types, typename math_types::vector3_type> worldpoint_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::vector3_type> color_interpolator;

	vector3_type zero_normal;

	bool         valid;

	bool         Debug;
    };

}// end namespace graphics

// LINE_RASTERIZER_H
#endif
