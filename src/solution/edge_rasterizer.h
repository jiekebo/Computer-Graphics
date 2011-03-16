#ifndef EDGE_RASTERIZER_H
#define EDGE_RASTERIZER_H
//
// Graphics Framework.
// Copyright (C) 2010 Department of Computer Science, University of Copenhagen
//

#include <iostream>
#include <iomanip>
#include <cmath>
#include "graphics/graphics.h"
#include "solution/linear_interpolator.h"


namespace graphics {

    // Draws an edge from V1 to V2, or two edges - one from V1 to V2 and one from V2 to V3.
    // Edges are assumed NOT to be horizontal!
    template<typename math_types>
    class MyEdgeRasterizer
    {
    public:
	typedef typename math_types::real_type    real_type;
	typedef typename math_types::vector3_type vector3_type;

    public:
/*******************************************************************\
*                                                                   *
*                M y E d g e R a s t e r i z e r ( )                *
*                                                                   *
\*******************************************************************/

	MyEdgeRasterizer() : valid(false), twoedges(false)
	{
	    //std::cout << "MyEdgeRasterizer::MyEdgeRasterizer(): called" << std::endl;
	}


/*******************************************************************\
*                                                                   *
*               ~ M y E d g e R a s t e r i z e r ( )               *
*                                                                   *
\*******************************************************************/

	virtual ~MyEdgeRasterizer()
	{}


/*******************************************************************\
*                                                                   *
*                  i n i t ( " o n e   e d g e " )                  *
*                                                                   *
\*******************************************************************/

	void init(vector3_type const& in_vertex1,
		  vector3_type const& in_normal1,
		  vector3_type const& in_worldpoint1,
		  vector3_type const& in_color1,
		  vector3_type const& in_vertex2,
		  vector3_type const& in_normal2,
		  vector3_type const& in_worldpoint2,
		  vector3_type const& in_color2)
        {
	    // Save the original parameters
	    this->org_vertex[0] = in_vertex1;
	    this->org_vertex[1] = in_vertex2;

	    this->org_normal[0] = in_normal1;
	    this->org_normal[1] = in_normal2;

	    this->org_worldpoint[0] = in_worldpoint1;
	    this->org_worldpoint[1] = in_worldpoint2;

	    this->org_color[0]  = in_color1;
	    this->org_color[1]  = in_color2;

	    // There is only one edge
	    this->twoedges = false;

	    this->initialize_current_edge(0, 1);
        }
	

/*******************************************************************\
*                                                                   *
*                i n i t ( ! " t w o   e d g e s " )                *
*                                                                   *
\*******************************************************************/

	void init(vector3_type const& in_vertex1,
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

	    // There are two edges! One edge from (x1, y1) to (x2, y2),
	    // and the other edge going from (x2, y2) to (x3, y3)

	    // The assumption is that the triangle is NOT degenerate, which means that not all
	    // vertices are co-linear.
	    // This is taken care of in the triangle_rasterizer.
            // But there can still be a horizontal edge.
	    // So, there are two edges, except if one of them is horizontal

	    bool edge1_is_horizontal = this->horizontal_edge(0, 1);
	    bool edge2_is_horizontal = this->horizontal_edge(1, 2);

	    //std::cout << "edge_rasterizer::init(...)" << std::endl;
	    if ((!edge1_is_horizontal) && (!edge2_is_horizontal)) {
		//std::cout << "   no horizontal edges"     << std::endl;
		this->twoedges = true;
		this->initialize_current_edge(0, 1);
	    }
	    else {
		if (edge1_is_horizontal && (!edge2_is_horizontal)) {
		    //std::cout << "    edge 1 is horizontal, so throw it away" << std::endl;
		    this->twoedges = false;
		    this->initialize_current_edge(1, 2);
		}
		if ((!edge1_is_horizontal) && edge2_is_horizontal) {
		    //std::cout << "    edge 2 is horizontal, so throw it away" << std::endl;
		    this->twoedges = false;
		    this->initialize_current_edge(0, 1);
		}
	    }
        }
	

/*******************************************************************\
*                                                                   *
*                               x ( )                               *
*                                                                   *
\*******************************************************************/

	int x() const
	{
	    if (!this->valid) {
		throw std::runtime_error("MyEdgeRasterizer::x(): Invalid State/Not Initialized");
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
		throw std::runtime_error("MyEdgeRasterizer::y(): Invalid State/Not Initialized");
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
	    //std::cout << "MyEdgeRasterizer::depth():(x_current, y_current) = ("
	    //	      << this->x_current << ", " << this->y_current << ", "
	    //	      << ")" << std::endl;

	    if (!this->valid) {
		throw std::runtime_error("MyEdgeRasterizer::depth(): Invalid State/Not Initialized");
            }

	    //return this->z_current;

	    if (!this->depth_interpolator.more_values()) {
		//std::cout << "MyEdgeRasterizer::depth(): more_values() == false" << std::endl;
		//std::cout << "(x_current, y_current) = ("
		//	  << this->x_current << ", " << this->y_current << ")" << std::endl;
		throw std::runtime_error("MyEdgeRasterizer::depth(): Invalid depth_interpolator");
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
                throw std::runtime_error("MyEdgeRasterizer::position():Invalid State/Not Initialized");
            }

            //return vector3_type(this->x(), this->y(), this->depth());

	    if (!this->worldpoint_interpolator.more_values()) {
		throw std::runtime_error("MyEdgeRasterizer::position(): Invalid worldpoint_interpolator");
	    }
	    return this->worldpoint_interpolator.value();
        }


/*******************************************************************\
*                                                                   *
*                          n o r m a l ( )                          *
*                                                                   *
\*******************************************************************/

	vector3_type normal() const
        {
            if (!this->valid) {
                throw std::runtime_error("MyEdgeRasterizer::normal():Invalid State/Not Initialized");
            }

            //return this->Ncurrent;

	    if (!this->normal_interpolator.more_values()) {
		throw std::runtime_error("MyEdgeRasterizer::normal(): Invalid normal_interpolator");
	    }
	    return this->normal_interpolator.value();
        }


/*******************************************************************\
*                                                                   *
*                           c o l o r ( )                           *
*                                                                   *
\*******************************************************************/

	vector3_type color() const
        {
            if (!this->valid) {
                throw std::runtime_error("MyEdgeRasterizer::color():Invalid State/Not Initialized");
            }

            //return this->color_current;

	    if (!this->color_interpolator.more_values()) {
		throw std::runtime_error("MyEdgeRasterizer::color(): Invalid color_interpolator");
	    }
	    return this->color_interpolator.value();
        }


/*******************************************************************\
*                                                                   *
*                 p r i n t _ v a r i a b l e s ( )                 *
*                                                                   *
\*******************************************************************/

	void print_variables()
        {
	    std::cout << "MyEdgeRasterizer: local variables" << std::endl;
	    std::cout << "=================================" << std::endl;
	    std::cout << "\tvalid       == "     << this->valid   << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tV1 = [" << this->org_vertex[0] << "]" << std::endl;
	    std::cout << "\tV2 = [" << this->org_vertex[1] << "]" << std::endl;
	    if (this->twoedges) {
		std::cout << "\tV3 = [" << this->org_vertex[2] << "]" << std::endl;
	    }
	    std::cout << std::endl;
	    std::cout << "\tx_start     == "   << this->x_start << std::endl;
	    std::cout << "\ty_start     == "   << this->y_start << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tx_current   == " << this->x_current << std::endl;
	    std::cout << "\ty_current   == " << this->y_current << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tx_stop      == "   << this->x_stop  << std::endl;
	    std::cout << "\ty_stop      == "   << this->y_stop  << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tDeltaX      == " << this->DeltaX    << std::endl;
	    std::cout << "\tDeltaY      == " << this->DeltaY    << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tNumerator   == " << this->Numerator   << std::endl;
	    std::cout << "\tDenominator == " << this->Denominator << std::endl;
	    std::cout << "\tAccumulator == " << this->Accumulator << std::endl;
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
	    //return (this->y_current < this->y_stop);
	}


/*******************************************************************\
*                                                                   *
*                   n e x t _ f r a g m e n t ( )                   *
*                                                                   *
\*******************************************************************/

	void next_fragment()
        {
#if 0
	    if (this->y_current >= this->y_stop) {
		this->valid = false;
	    }
	    else {
		this->Accumulator += this->Numerator;
		while (this->Accumulator > this->Denominator) {
		    this->x_current += this->DeltaX;
		    this->Accumulator -= this->Denominator;
		}
		this->y_current += this->DeltaY;

		this->depth_interpolator.next_value();
		this->normal_interpolator.next_value();
		this->worldpoint_interpolator.next_value();
		this->color_interpolator.next_value();
	    }

	    if (this->y_current >= this->y_stop) {
		if (!(this->twoedges)) {
		    this->valid = false;
		}
		else {
		    this->initialize_current_edge(1, 2);
		    this->twoedges = false;
		}
	    }
#else
	    this->y_current += this->DeltaY;
	    if (this->y_current >= this->y_stop) {
		if (!(this->twoedges)) {
		    this->valid = false;
		}
		else {
		    //std::cout << "MyEdgeRasterizer::next_fragment(): shift edges" << std::endl;
		    this->initialize_current_edge(1, 2);
		    this->twoedges = false;
		}
	    }
	    else {
		this->Accumulator += this->Numerator;
		while (this->Accumulator > this->Denominator) {
		    this->x_current += this->DeltaX;
		    this->Accumulator -= this->Denominator;
		}
		this->depth_interpolator.next_value();
		this->normal_interpolator.next_value();
		this->worldpoint_interpolator.next_value();
		this->color_interpolator.next_value();
	    }
#endif
	}

    protected:

    private:

/*******************************************************************\
*                                                                   *
* i n i t i a l i z e _ c u r r e n t _ e d g e ( i n t ,   i n t ) *
*                                                                   *
\*******************************************************************/

	void initialize_current_edge(int start_index, int stop_index)
	{
	    // Ensure that the edge has its first vertex lower than the second one
	    int Ystart = static_cast<int>(round(this->org_vertex[start_index][2]));
	    int Ystop  = static_cast<int>(round(this->org_vertex[stop_index][2]));

	    if (Ystart == Ystop) {
		throw std::runtime_error("MyEdgeRasterizer::initialize_current_edge(int, int): Horizontal Edge - Invalid");
	    }

	    if (Ystart < Ystop) {
		// usual case - first vertex is lower than the second one
		this->x_start   = static_cast<int>(round(this->org_vertex[start_index][1]));
		this->x_current = this->x_start;
		this->x_stop    = static_cast<int>(round(this->org_vertex[stop_index][1]));

		this->y_start   = Ystart;
		this->y_current = this->y_start;
		this->y_stop    = Ystop;

		this->z_start   = this->org_vertex[start_index][3];
		this->z_current = this->z_start;
		this->z_stop    = this->org_vertex[stop_index][3];

		this->Nstart    = this->org_normal[start_index];
		this->Ncurrent  = this->Nstart;
		this->Nstop     = this->org_normal[stop_index];

		this->WorldPointstart   = org_worldpoint[start_index];
		this->WorldPointcurrent = this->WorldPointstart;
		this->WorldPointstop    = org_worldpoint[stop_index];

		this->color_start   = this->org_color[start_index];
		this->color_current = this->color_start;
		this->color_start   = this->org_color[stop_index];
	    }
	    else {
		// first vertex has a higher y-coordinate than the second one
		// swap start and stop values
		this->x_start   = static_cast<int>(round(this->org_vertex[stop_index][1]));
		this->x_current = this->x_start;
		this->x_stop    = static_cast<int>(round(this->org_vertex[start_index][1]));

		this->y_start   = Ystop;
		this->y_current = this->y_start;
		this->y_stop    = Ystart;

		this->z_start   = this->org_vertex[stop_index][3];
		this->z_current = this->z_start;
		this->z_stop    = this->org_vertex[start_index][3];

		this->Nstart    = this->org_normal[stop_index];
		this->Ncurrent  = this->Nstart;
		this->Nstop     = this->org_normal[start_index];

		this->WorldPointstart   = org_worldpoint[stop_index];
		this->WorldPointcurrent = this->WorldPointstart;
		this->WorldPointstop    = org_worldpoint[start_index];

		this->color_start   = this->org_color[stop_index];
		this->color_current = this->color_start;
		this->color_stop    = this->org_color[start_index];
	    }

	    // Now, the vertices are in place, go initialize the interpolators

	    // Depth Interpolator --- this is wrong it is should be in the eye coordinate system
	    // I think it is OK 18.03.2010-23:43

	    this->z_start   = this->org_vertex[start_index][3];
	    this->z_current = this->z_start;
	    this->z_stop    = this->org_vertex[stop_index][3];
	    this->depth_interpolator.init(this->y_start, this->y_stop,
					  this->z_start, this->z_stop);

	    // Normal Interpolator
	    this->Nstart   = this->org_normal[start_index];
	    this->Ncurrent = this->Nstart;
	    this->Nstop    = this->org_normal[stop_index];
	    this->normal_interpolator.init(this->y_start, this->y_stop,
	    				   this->Nstart,  this->Nstop);

	    // World Point Interpolator
	    this->WorldPointstart   = this->org_worldpoint[start_index];
	    this->WorldPointcurrent = this->WorldPointstart;
	    this->WorldPointstop    = this->org_worldpoint[stop_index];
	    this->worldpoint_interpolator.init(this->y_start, this->y_stop,
	    				       this->WorldPointstart, this->WorldPointstop);

	    // Color Interpolator
#if 0
	    std::cout << "initialize_current_edge(" << start_index << ", " << stop_index << ")"
		      << std::endl;
	    std::cout << "    color_start = [" << this->org_color[start_index] << "]" << std::endl;
	    std::cout << "    color_stop  = [" << this->org_color[stop_index]  << "]" << std::endl;
#endif
	    this->color_start   = this->org_color[start_index];
	    this->color_current = this->color_start;
	    this->color_stop    = this->org_color[stop_index];
	    this->color_interpolator.init(this->y_start, this->y_stop,
	    				  this->color_start, this->color_stop);

	    // Now the vertices are in correct order, so now comes the real initialization of the edge
	    int dx = this->x_stop - this->x_start;
	    int dy = this->y_stop - this->y_start;

	    this->DeltaX      = (dx < 0) ? -1 : 1;
	    this->DeltaY      = 1;
	    this->Numerator   = (dx < 0) ? -dx : dx;
	    this->Denominator = dy;
	    this->Accumulator = (DeltaX > 0) ? Denominator : 1;

	    this->x_current = this->x_start;
	    this->y_current = this->y_start;

	    this->valid = (this->y_current < this->y_stop);
	}


/*******************************************************************\
*                                                                   *
*         h o r i z o n t a l _ e d g e ( i n t ,   i n t )         *
*                                                                   *
\*******************************************************************/

	bool horizontal_edge(int start_index, int stop_index) {
	    if (static_cast<int>(round(this->org_vertex[start_index][2]))
		== 
		static_cast<int>(round(this->org_vertex[stop_index][2])))
	    {
		// The edge is horizontal
//		this->twoedges = false;
		return true;
	    }
	}


/*******************************************************************\
*                                                                   *
*                 P r i v a t e   V a r i a b l e s                 *
*                                                                   *
\*******************************************************************/

	bool valid;
	bool twoedges;

	// Original vertices
	vector3_type org_vertex[3];

	// Original normals
	vector3_type org_normal[3];

	// Original worldpoint[3]
	vector3_type org_worldpoint[3];

	//Original colors
	vector3_type org_color[3];

	// Work variables
	int       x_start;
	int       y_start;
	real_type z_start;

	int       x_stop;
	int       y_stop;
	real_type z_stop;

	int       x_current;
	int       y_current;
	real_type z_current;

	int       DeltaX;
	int       DeltaY;
	int       Numerator;
	int       Denominator;
	int       Accumulator;

	vector3_type Nstart;
	vector3_type Nstop;
	vector3_type Ncurrent;

	vector3_type WorldPointstart;
	vector3_type WorldPointstop;
	vector3_type WorldPointcurrent;

	vector3_type color_start;
	vector3_type color_stop;
	vector3_type color_current;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::real_type> depth_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::vector3_type> normal_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::vector3_type> worldpoint_interpolator;

	// The LinearInterpolator is hard-coded into this rasterizer -- must be changed!
	LinearInterpolator<math_types, typename math_types::vector3_type> color_interpolator;
    };

}// end namespace graphics

// EDGE_RASTERIZER_H
#endif
