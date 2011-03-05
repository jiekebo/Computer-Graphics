#ifndef GRAPHICS_STATE_H
#define GRAPHICS_STATE_H
//
// Graphics Framework.
// Copyright (C) 2007 Department of Computer Science, University of Copenhagen
//

#include "graphics/graphics.h"
#include "solution/transformations.h"


namespace graphics
{
    /**
     * Keeps the information of what state the graphics pipeline is in at all times.
     */
    template< typename math_types >
    class GraphicsState
    {
    public:
	/**
	 * The basic type which is the the type of the elements of vectors and matrices.
	 */
	typedef typename math_types::real_type      real_type;

	/**
	 * A vector with 2 entries both of type real_type.
	 */
	typedef typename math_types::vector2_type   vector2_type;

        /**
	 * A vector with 3 entries both of type real_type.
	 */
	typedef typename math_types::vector3_type   vector3_type;

        /**
	 * A vector with 4 entries both of type real_type.
	 */
	typedef typename math_types::vector4_type   vector4_type;

	/**
	 * A matrix of dimension 4 x 4 with entries of real_type.
	 */
	typedef typename math_types::matrix4x4_type matrix4x4_type;

    public:
	/**
	 * Default constructor. set all transformations to the identity.
	 */
	GraphicsState() {
	    /// Reset all the transformation matrices to the identity.

	    matrix4x4_type Id = Identity();

	    this->m_model     = Id;
	    this->m_inv_model = Id;
	    
	    this->m_view_orientation     = Id;
	    this->m_inv_view_orientation = Id;

	    this->m_view_projection     = Id;
	    this->m_inv_view_projection = Id;

	    this->m_window_viewport     = Id;
	    this->m_inv_window_viewport = Id;

	    this->m_projection     = Id;
	    this->m_inv_projection = Id;

	    /// Reset all the Phong parameters to some useful values.
	    /// ...
	}

	/**
	 * Transform from Model-coordinates to World-coordinates.
	 * @return A read-only reference to the matrix which transforms from the Model-coordinate system to the World-coordinate system.
	 */
	matrix4x4_type const& model() const { return this->m_model; }

	/**
	 * Transform from Model-coordinates to World-coordinates.
	 * @return A writable reference to the matrix which transforms from the Model-coordinate system to the World-coordinate system.
	 */
	matrix4x4_type&       model()       { return this->m_model; }

	/**
	 * The inverse model transformation
	 */
	matrix4x4_type const& inv_model() const { return this->m_inv_model; }

	/**
	 * The inverse model transformation
	 */
	matrix4x4_type&       inv_model()       { return this->m_inv_model; }

        /**
	 * Transform from World-coordinates to Eye-coordinates.
	 * @return A read-only reference to the matrix which transforms from the World-coordinate system to the Screen-coordinate system.
	 */
	matrix4x4_type const& view_orientation() const { return this->m_view_orientation; }

	/**
	 * Transform from World-coordinates to Eye-coordinates.
	 * @return A writable reference to the matrix which transforms from the World-coordinate system to the Eye-coordinate system.
	 */
	matrix4x4_type&       view_orientation()       { return this->m_view_orientation; }

	/**
	 * The inverse of the view_orientation matrix.
	 */
	matrix4x4_type const& inv_view_orientation() const { return this->m_inv_view_orientation; }

        /**
	 * The inverse of the view_orientation matrix.
	 */
	matrix4x4_type& inv_view_orientation()         { return this->m_inv_view_orientation; }
        /**
	 * Transform from Eye-coordinates to Canonical-coordinates.
	 * @return A read-only reference to the matrix which transforms from the Eye-coordinate system to the Canonical-coordinate system.
	 */
	matrix4x4_type const& view_projection() const { return this->m_view_projection; }

	/**
	 * Transform from Eye-coordinates to Canonical-coordinates.
	 * @return A writable reference to the matrix which transforms from the Eye-coordinate system to the Canonical-coordinate system.
	 */
	matrix4x4_type&       view_projection()       { return this->m_view_projection; }

        /**
	 * The inverse of the view_projection matrix.
	 */
	matrix4x4_type const& inv_view_projection() const { return this->m_inv_view_projection; }

	/**
	 * The inverse of the view_projection matrix.
	 */
	matrix4x4_type&       inv_view_projection()       { return this->m_inv_view_projection; }

        /**
	 * Transform from Canonical-coordinates to Screen-coordinates.
	 * @return A read-only reference to the matrix which transforms from the Canonical-coordinate system to the Screen-coordinate system.
	 */
	matrix4x4_type const& window_viewport() const { return this->m_window_viewport; }

	/**
	 * Transform from Canonical-coordinates to Screen-coordinates.
	 * @return A writable reference to the matrix which transforms from the Canonical-coordinate system to the Screen-coordinate system.
	 */
	matrix4x4_type&       window_viewport()       { return this->m_window_viewport; }

	/**
	 * The inverse of the window_viewport transformation.
	 */
	matrix4x4_type const& inv_window_viewport() const { return this->m_inv_window_viewport; }

	/**
	 * The inverse of the window_viewport transformation.
	 */
	matrix4x4_type&       inv_window_viewport()       { return this->m_inv_window_viewport; }

        /**
	 * Transform from World-coordinates to Canonical-coordinates.
	 * @return A read-only reference to the matrix which transforms from the World-coordinate system to the Canonical-coordinate system.
	 */
	matrix4x4_type const& projection() const { return this->m_projection; }

        /**
	 * Transform from World-coordinates to Canonical-coordinates.
	 * @return A writable reference to the matrix which transforms from the World-coordinate system to the Screen-coordinate system.
	 */
	matrix4x4_type&       projection()       { return this->m_projection; }

	/**
	 * The inverse of the projection transformation.
	 */
	matrix4x4_type const& inv_projection() const { return this->m_inv_projection; }

	/**
	 * The inverse of the projection transformation.
	 */
	matrix4x4_type&       inv_projection()       { return this->m_inv_projection; }

	// The eye coordinate system
	/**
	 * The x-axis of the eye coordinate system.
	 * @return A read-only reference to the x-axis of the eye coordinate system.
	 */
	vector3_type const& x_eye_axis() const { return this->m_x_eye_axis; }

        /**
	 * The x-axis of the eye coordinate system.
	 * @return A writable reference to the x-axis of the eye coordinate system.
	 */
	vector3_type&       x_eye_axis()       { return this->m_x_eye_axis; }

	/**
	 * The y-axis of the eye coordinate system.
	 * @return A read-only reference to the y-axis of the eye coordinate system.
	 */
	vector3_type const& y_eye_axis() const { return this->m_y_eye_axis; }

        /**
	 * The y-axis of the eye coordinate system.
	 * @return A writable reference to the y-axis of the eye coordinate system.
	 */
	vector3_type&       y_eye_axis()       { return this->m_y_eye_axis; }

	/**
	 * The z-axis of the eye coordinate system.
	 * @return A read-only reference to the z-axis of the eye coordinate system.
	 */
	vector3_type const& z_eye_axis() const { return this->m_z_eye_axis; }

        /**
	 * The z-axis of the eye coordinate system.
	 * @return A writable reference to the z-axis of the eye coordinate system.
	 */
	vector3_type&       z_eye_axis()       { return this->m_z_eye_axis; }

	/**
	 * The position of the eye.
	 * @return A read-only reference to the coordinates of the eye.
	 */
	vector3_type const& eye_position() const { return this->m_eye_position; }

        /**
	 * The position of the eye.
	 * @return A writable reference to the coordinates of the eye.
	 */
	vector3_type&      eye_position()        { return this->m_eye_position; }

	// The Phong Light model parameters
	/**
	 * The color of the ambient light source I_a
	 */
	vector3_type const& I_a() const { return this->m_I_a; }

	/**
	 * The color of the ambient light source I_a
	 */
	vector3_type&       I_a()       { return this->m_I_a; }

        /**
	 * The color of the point light source I_p
	 */
	vector3_type const& I_p() const { return this->m_I_p; }

	/**
	 * The color of the point light source I_p
	 */
	vector3_type&       I_p()       { return this->m_I_p; }

	/**
	 * The position of the light source.
	 * @return A read-only reference to the coordinates of the light source.
	 */
	vector3_type const& light_position() const { return this->m_light_position; }

        /**
	 * The position of the light source.
	 * @return A writable reference to the coordinates of the light source.
	 */
	vector3_type&       light_position()       { return this->m_light_position; }

	
	/**
	 * The value of the ambient color.
	 * @return A read-only reference to the ambient color.
	 */
	vector3_type const& ambient_color()  const { return this->m_ambient_color;  }

        /**
	 * The value of the ambient color.
	 * @return A writable reference to the ambient color.
	 */
	vector3_type&       ambient_color()        { return this->m_ambient_color;  }
    
        /**
	 * The value of the diffuse color.
	 * @return A read-only reference to the diffuse color.
	 */
	vector3_type const& diffuse_color()  const { return m_diffuse_color;  }

        /**
	 * The value of the diffuse color.
	 * @return A writable reference to the diffuse color.
	 */
	vector3_type&       diffuse_color()        { return this->m_diffuse_color;  }

	/**
	 * The value of the specular color.
	 * @return A read-only reference to the specular color.
	 */
	vector3_type const& specular_color() const { return this->m_specular_color; }
        /**
	 * The value of the specular color.
	 * @return A writable reference to the specular color.
	 */
	vector3_type&       specular_color()       { return this->m_specular_color; }

	/**
	 * The value of the ambient intensity (k_a, in Foley)
	 * @return A read-only reference to the scale factor of the ambient light source.
	 */
	real_type const& ambient_intensity()  const { return this->m_ambient_intensity;  }

	/**
	 * The value of the ambient intensity (k_a, in Foley)
	 * @return A writable reference to the scale factor of the ambient light source.
	 */
	real_type&       ambient_intensity()        { return this->m_ambient_intensity;  }
    
        /**
	 * The value of the ambient intensity (k_d, in Foley)
	 * @return A read-only reference to the scale factor of the diffuse light source.
	 */
	real_type const& diffuse_intensity()  const { return this->m_diffuse_intensity;  }

	/**
	 * The value of the ambient intensity (k_d, in Foley)
	 * @return A writable reference to the scale factor of the diffuse light source.
	 */
	real_type&       diffuse_intensity()        { return this->m_diffuse_intensity;  }
    
	/**
	 * The value of the specular intensity (k_s, in Foley)
	 * @return A read-only reference to the scale factor of the specular light source.
	 */
	real_type const& specular_intensity() const { return this->m_specular_intensity; }

	/**
	 * The value of the specular intensity (k_s, in Foley)
	 * @return A writable reference to the scale factor of the specular light source.
	 */
	real_type&       specular_intensity()       { return this->m_specular_intensity; }

	/**
	 * The exponent of the specular light in Phong's equation. Big means narrow, and small means broad.
	 * @return A read-only reference to the fall-off exponent in Phong's equation.
	 */
	real_type const& fall_off () const { return m_fall_off; }

        /**
	 * The exponent of the specular light in Phong's equation. Big means narrow, and small means broad.
	 * @return A writable reference to the fall-off exponent in Phong's equation.
	 */
	real_type&       fall_off ()       { return this->m_fall_off; }



	// Should be changed from < to >= by kaiip 06.12.2008 - 00:44
	// But it has many consequences - so for now, I just leave it as is!
	// This is the original

	/**
	 * The Z-buffer test.
	 * Maybe it should be changed from < to >= by kaiip 06.12.2008 - 00:44
	 * But it has many consequences - so for now, I just leave it as is!
	 * This is the original.
	 * @param z_old
	 * @param z_new
	 * @return true if (z_new < z_old), else false.
	 */
#ifdef KENNY_ZBUFFER
	bool ztest(real_type const& z_old, real_type const& z_new) { return (z_new < z_old); }
#else
	bool ztest(real_type const& z_old, real_type const& z_new) { return (z_new > z_old); }
#endif

    protected:

	///  Model to world transformation matrix.
	matrix4x4_type m_model;
	matrix4x4_type m_inv_model;
	

	/// World to eye space transformation matrix (view-orientation).
	matrix4x4_type m_view_orientation;
	matrix4x4_type m_inv_view_orientation;

	/// Eye to Canonical view-volume transformation matrix.
	matrix4x4_type m_view_projection;
	matrix4x4_type m_inv_view_projection;

        /// Canonical volume to screen transformation matrix.
	matrix4x4_type m_window_viewport;
	matrix4x4_type m_inv_window_viewport;

	/// World to screen space transformation matrix (projection) all the way.
	matrix4x4_type m_projection;
	matrix4x4_type m_inv_projection;

	
	/// x-axis of the eye coordinate system
	vector3_type    m_x_eye_axis;

	
	/// y-axis of the eye coordinate system
	vector3_type    m_y_eye_axis;

	
	/// z-axis of the eye coordinate system
	vector3_type    m_z_eye_axis;

	/// Eye position
	vector3_type    m_eye_position;

	/**
	 * The parameters of Phong's reflection model
	 */
	/// The color the the ambient light source I_a.
	vector3_type    m_I_a;

	/// The color the the point light source I_p.
	vector3_type    m_I_p;

	/// Light source position.
	vector3_type    m_light_position;
	
	/// Ambient color.
	vector3_type    m_ambient_color;

	/// Diffuse color.
	vector3_type    m_diffuse_color;

	/// Specular color.
	vector3_type    m_specular_color;

	/// Ambient coeffecient k_a.
	real_type       m_ambient_intensity;

	/// Diffuse coeffecient k_d.
	real_type       m_diffuse_intensity;

	/// Specular coeffecient k_s.
	real_type       m_specular_intensity;

	/// Specular exponent n.
	real_type       m_fall_off;
    };

}// end namespace graphics

// GRAPHICS_STATE_H
#endif
