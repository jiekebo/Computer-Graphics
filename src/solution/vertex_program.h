#ifndef VERTEX_PROGRAM_H
#define VERTEX_PROGRAM_H
//
// Graphics Framework.
// Copyright (C) 2008 Department of Computer Science, University of Copenhagen
//
#include "graphics/graphics.h"
#include "graphics/graphics_state.h"
#include "solution/transformations.h"


namespace graphics {

/*******************************************************************\
*                                                                   *
*                       P a s s T h r o u g h                       *
*                                                                   *
\*******************************************************************/

    template<typename math_types>
    class MyIdentityVertexProgram : public VertexProgram<math_types>
    {
    public:
	typedef typename VertexProgram<math_types>::graphics_state_type  graphics_state_type;
	typedef typename math_types::vector3_type                        vector3_type;

    public:
	void run(graphics_state_type const& state,
		 vector3_type const& in_vertex,
		 vector3_type const& in_color,
		 vector3_type& out_vertex,
		 vector3_type& out_color)
	{
	    // >> ADD YOUR OWN MAGIC HERE <<
	    out_vertex = in_vertex;
	    out_color =  in_color;
	}


	void run(graphics_state_type const& state,
		 vector3_type const& in_vertex,
		 vector3_type const& in_normal,
		 vector3_type const& in_color,
		 vector3_type& out_vertex,
		 vector3_type& out_normal,
		 vector3_type& out_color)
	{
	    // >> ADD YOUR OWN MAGIC HERE <<
	    out_vertex = in_vertex;
	    out_normal = in_normal;
	    out_color =  in_color;
	}
    };


/*******************************************************************\
*                                                                   *
*                         T r a n s f o r m                         *
*                                                                   *
\*******************************************************************/

    template<typename math_types>
    class MyTransformVertexProgram : public VertexProgram<math_types>
    {
    public:
	typedef typename VertexProgram<math_types>::graphics_state_type  graphics_state_type;
	typedef typename math_types::vector3_type                        vector3_type;
	typedef typename math_types::vector4_type                        vector4_type;
	typedef typename math_types::matrix4x4_type                      matrix4x4_type;
	typedef typename math_types::real_type                           real_type;

    public:
	void run(graphics_state_type const& state,
		 vector3_type const& in_vertex,
		 vector3_type const& in_color,
		 vector3_type& out_vertex,
		 vector3_type& out_color)
	{
	    // >> ADD YOUR OWN MAGIC HERE <<
	    out_vertex = this->TransformPoint(state, in_vertex);
	    out_color =  in_color;
	}


	void run(graphics_state_type const& state,
		 vector3_type const& in_vertex,
		 vector3_type const& in_normal,
		 vector3_type const& in_color,
		 vector3_type& out_vertex,
		 vector3_type& out_normal,
		 vector3_type& out_color)
	{
	    // >> ADD YOUR OWN MAGIC HERE <<
	    out_vertex = this->TransformPoint(state,  in_vertex);
	    out_normal = this->TransformNormal(state, in_normal);
	    out_color =  gouraudColor(state,in_vertex,out_normal);
	}

	vector3_type gouraudColor(graphics_state_type const& state,
		 vector3_type const& in_position,
		 vector3_type const& in_normal)
	{
		// >> TODO ADD YOUR OWN MAGIC HERE <<

		//std::cout << "fragment: [" << in_position << "]" << std::endl;

		//out_color = in_color;

		// Compute the needed vectors: N, L, R, V - and the dot products
		vector3_type N = in_normal;
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

		return color;
	}

    private:
	vector3_type TransformPoint(graphics_state_type const& state, vector3_type const& point)
	{
	    //std::cout << "-->MyTransformVertexProgram::TransformPoint(vector3_type&)" << std::endl;
	    //std::cout << "   point = (" << point << ")^T" << std::endl;

	    vector4_type hompoint = HomVector(point);
	    //std::cout << "   hompoint = [" << hompoint << "]^T" << std::endl;

	    matrix4x4_type modeltrans = state.model();
	    //std::cout << "   state.model() = " << modeltrans << std::endl;

	    hompoint = modeltrans * hompoint;
	    //std::cout << "   Model * hompoint = " << hompoint << std::endl;

	    matrix4x4_type M = state.projection();
	    //std::cout << "   projection =" << M << std::endl;

	    hompoint = M * hompoint;
	    //std::cout << "   Transformed hompoint = [" << hompoint << "]" << std::endl;

	    vector3_type transformed_point = Vector3D(hompoint);
	    //std::cout << "   Transformed point = [" << transformed_point << "]" << std::endl;

	    //std::cout << "<--MyTransformVertexProgram::TransformPoint(vector3_type&)" << std::endl;

	    return transformed_point;
	}

	// Beware this only use the graphics_state.model() to transform the normal
	vector3_type TransformNormal(graphics_state_type const& state, vector3_type const& normal)
	{
	    //std::cout << "-->MyTransformVertexProgram::TransformNormal(vector3_type&)" << std::endl;
	    //std::cout << "   normal = (" << normal << ")^T" << std::endl;

	    vector4_type homnormal = HomNormal(normal);
	    //std::cout << "   homnormal = [" << homnormal << "]^T" << std::endl;

	    matrix4x4_type inv_modeltrans = state.inv_model();
	    //std::cout << "   state.inv_model() = " << inv_modeltrans << std::endl;

	    matrix4x4_type inv_modeltrans_transpose = inv_modeltrans.T();
	    //std::cout << "   inv_model.t() = " << inv_modeltrans_transpose << std::endl;

	    homnormal = inv_modeltrans_transpose * homnormal;
	    //std::cout << "   inv_model_transpose * homnormal = " << homnormal << std::endl;

	    vector3_type transformed_normal = Normal3D(homnormal);
	    //std::cout << "   Transformed normal = [" << transformed_normal << "]" << std::endl;


	    //std::cout << "<--MyTransformVertexProgram::TransformNormal(vector3_type&)" << std::endl;

	    return transformed_normal;
	}

	real_type Clamp(real_type const& value)
		{
		    real_type result = value;
		    if (value < 0.0) result = 0.0;
		    if (value > 1.0) result = 1.0;

		    return result;
		}
    };

}// end namespace graphics

// VERTEX_PROGRAM_H
#endif
