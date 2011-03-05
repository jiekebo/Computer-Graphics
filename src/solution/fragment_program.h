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
	    // >> TODO ADD YOUR OWN MAGIC HERE <<

	    // Implement the Phong reflection model using the values saved in state

	    out_color = in_color;
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
