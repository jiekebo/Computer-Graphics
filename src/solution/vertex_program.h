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
	    out_color =  in_color;
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
    };

}// end namespace graphics

// VERTEX_PROGRAM_H
#endif
