#ifndef FRAGMENT_PROGRAM_H
#define FRAGMENT_PROGRAM_H
//
// Graphics Framework.
// Copyright (C) 2007 Department of Computer Science, University of Copenhagen
//
#include <iostream>
#include <iomanip>
#include "graphics/graphics.h"

namespace graphics {

/*******************************************************************\
*                                                                   *
*                       P a s s T h r o u g h                       *
*                                                                   *
\*******************************************************************/

    template<typename math_types>
    class MyIdentityFragmentProgram : public FragmentProgram<math_types>
    {
    public:

	typedef typename FragmentProgram<math_types>::graphics_state_type graphics_state_type;
	typedef typename math_types::vector3_type                         vector3_type;
	typedef typename math_types::real_type                            real_type;

    public:
	void run(graphics_state_type const& state,
		 vector3_type const& in_position,
		 vector3_type const& in_normal,
		 vector3_type const& in_color,
		 vector3_type&       out_color)
	{
	    // >> TODO ADD YOUR OWN MAGIC HERE <<

	    //std::cout << "fragment: [" << in_position << "]" << std::endl;
	    out_color = in_color;
	}
    };


/*******************************************************************\
*                                                                   *
*                             P h o n g                             *
*                                                                   *
\*******************************************************************/

template<typename math_types>
    class MyPhongFragmentProgram : public FragmentProgram<math_types>
    {
    public:

	typedef typename FragmentProgram<math_types>::graphics_state_type graphics_state_type;
	typedef typename math_types::vector3_type                         vector3_type;
	typedef typename math_types::real_type                            real_type;

    public:
	void run(graphics_state_type const& state,
		 vector3_type const& in_position,
		 vector3_type const& in_normal,
		 vector3_type const& in_color,
		 vector3_type&       out_color)
	{
		if (state.light_position() == vector3_type(0, 0, 0)) {
			out_color = in_color;
			return;
		}

		vector3_type normal;

		if (Norm(in_normal) == 0) {
			normal = vector3_type(0, 0, 1);
		} else {
			normal = in_normal / Norm(in_normal);
		}

		vector3_type A = state.ambient_intensity() * state.ambient_color() * vector3_type(0.5, 0.5, 0.5);

		vector3_type L = state.light_position() - in_position;
        L = L / Norm(L);
		vector3_type D = state.diffuse_color() * state.diffuse_intensity() * Dot(normal, L);

		vector3_type R = (normal * 2) * Dot(normal, L) - L;
		vector3_type V = state.eye_position() - in_position;
		if (Norm(V) == 0){}
		V = V / Norm(V);

		vector3_type S = state.specular_intensity() * state.specular_color() * pow(Dot(R, V), 20);

		vector3_type light = A + D + S;

		if(light[1] > 1)
			light[1]=0;
		if(light[2] > 1)
			light[2]=0;
		if(light[3] > 1)
			light[3]=0;

	    out_color = vector3_type(in_color[1] * light[1], in_color[2] * light[2], in_color[3] * light[3]);
	}


    private:

/*******************************************************************\
*                                                                   *
*                  C l a m p ( r e a l _ t y p e )                  *
*                                                                   *
\*******************************************************************/

	real_type Clamp(real_type const& value)
	{
	    real_type result = value;
	    if (value < 0.0) result = 0.0;
	    if (value > 1.0) result = 1.0;
	    
	    return result;
	}
    };


}// end namespace graphics

// FRAGMENT_PROGRAM_H
#endif
