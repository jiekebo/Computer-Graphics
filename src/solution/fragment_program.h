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
		for (int i = 1; i <= 3; ++i) {
			out_color[i] = this->Clamp(in_color[i]);
		}
	}

	real_type Clamp(real_type const& value)
	{
		real_type result = value;
		if (value < 0.0) result = 0.0;
		if (value > 1.0) result = 1.0;

		return result;
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

	    //std::cout << "fragment: [" << in_position << "]" << std::endl;

	    //out_color = in_color;

		int direction = Dot(state.z_eye_axis(), in_normal)<-0.15?-1:1;

	    // Compute the needed vectors: N, L, R, V - and the dot products
	    vector3_type N = in_normal * direction;
	    if (!Zero(N))
		N /= Norm(N);
	    else {
		std::cout << "MyPhongFragmentProgram: Zero Normal" << std::endl;
	    }

	    //std::cout << "phong fragment program::light_position = [" << state.light_position() << "]" << std::endl;
	    vector3_type L = state.light_position() - in_position;
	    if (!Zero(L))
		L /= Norm(L);
	    else {
		std::cout << "MyPhongFragmentProgram: Zero Light Vector" << std::endl;
	    }
	    //std::cout << "phong fragment program::light_vector = [" << L << "]" << std::endl;

	    vector3_type V = state.eye_position() - in_position;
	    if (!Zero(V))
		V /= Norm(V);
	    else {
		std::cout << "MyPhongFragmentProgram: Zero View Vector" << std::endl;
	    }
	    //std::cout << "V = [" << V << "]" << std::endl;



	    vector3_type Ambient_term;
	    vector3_type Diffuse_term;
	    vector3_type Specular_term;

	    // The Ambient term should always be computed

		for (int i = 1; i <= 3; ++i) {
			Ambient_term[i]  = state.I_a()[i] * state.ambient_intensity()  * state.ambient_color()[i];
			Ambient_term[i]  = this->Clamp(Ambient_term[i]);
		}

	    // Only compute the Diffuse and Specular terms of L, N, and V are on the same side of the surface.
	    if ((Dot(L, N) > 0.0) && (Dot(V, N) > 0.0)) //{
	    {
		// Both L and V on the same side of the surface as N

		vector3_type R = N * (Dot(N, L) * 2.0) - L;
		if (Dot(R, N) < 0.0) {
		    R = vector3_type(0.0, 0.0, 0.0); // was N but it might be wrong - just a try.
		}
		if (!Zero(R))
		    R /= Norm(R);
		else {
		    std::cout << "MyPhongFragmentProgram: Zero Reflection Vector" << std::endl;
		}
		//std::cout << "R = [" << R << "]" << std::endl;

		real_type    LdotN = Dot(L, N);
		             LdotN = this->Clamp(LdotN);

		real_type    RdotV = Dot(R, V);
		//std::cout << "R * V == " << RdotV << ", Clamp(R * V) == " << Clamp(RdotV) << std::endl;
	                     RdotV = this->Clamp(RdotV);

	        //std::cout << "state.fall_off() == " << state.fall_off() << std::endl;
		real_type    powRdotV = pow(RdotV, state.fall_off());
		             powRdotV = this->Clamp(powRdotV);

		//std::cout << "N == [" << N << "]" << std::endl;
		//std::cout << "L == [" << L << "]" << std::endl;
	        //std::cout << "R == [" << R << "]" << std::endl;
	        //std::cout << "V == [" << V << "]" << std::endl;
	        //std::cout << "L * N     == " << LdotN << std::endl;
	        //std::cout << "R * V     == " << RdotV << std::endl;
	        //std::cout << "(R * V)^n == " << powRdotV << std::endl;




		for (int i = 1; i <= 3; ++i) {
		    Diffuse_term[i]  = state.I_p()[i] * state.diffuse_intensity()  * state.diffuse_color()[i] * LdotN;
		    Diffuse_term[i]  = this->Clamp(Diffuse_term[i]);

		    Specular_term[i] = state.I_p()[i] * state.specular_intensity() * state.specular_color()[i] * powRdotV;
		    Specular_term[i] = this->Clamp(Specular_term[i]);
		}
	    }
	    //std::cout << "Ambient  == [" << Ambient_term  << "]" << std::endl;
	    //std::cout << "Diffuse  == [" << Diffuse_term  << "]" << std::endl;
	    //std::cout << "Specular == [" << Specular_term << "]" << std::endl;

	    //out_color = Ambient_term;
	    //out_color = Diffuse_term + Specular_term;
	    //out_color = Specular_term;

	    vector3_type color = Ambient_term + Diffuse_term + Specular_term;
	    for (int i = 1; i <= 3; ++i) {
		color[i] = this->Clamp(color[i]);
	    }

	    out_color = color;
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
