#ifndef GRAPHICS_CAMERA_H
#define GRAPHICS_CAMERA_H
//
// Graphics Framework.
// Copyright (C) 2008 Department of Computer Science, University of Copenhagen
//

#include "graphics/graphics.h"
#include "solution/transformations.h"


namespace graphics
{
    /**
     * A virtual pin-hole camera.
     * The camera class produces the matrices necessary for the virtual world
     * to be rasterized.
     */
    template<typename math_types>
    class Camera
    {
    public:
	/// The actual type of the elements of vectors and matrices.
	typedef typename math_types::real_type      real_type;

	/// The actual type of a vector2.
	typedef typename math_types::vector2_type   vector2_type;

	/// The actual type of a vector3.
	typedef typename math_types::vector3_type   vector3_type;

	/// The actual type of a matrix4x4.
	typedef typename math_types::matrix4x4_type matrix4x4_type;

    public:
      
	/**
	 * Creates a Camera with a default (empty) state.
	 */
	Camera() : m_state(0)
	{}

	/**
	 * Destroys the Camera.
	 */
	virtual ~Camera()
	{}

	/**
	 * Initialize the Camera with a RenderPipeline.
	 */
	void init( RenderPipeline<math_types>& R )
	{ 
	    this->m_state = &(R.state());
	    this->m_renderpipeline = &R;
	    this->reset();
	}

	
        /**
	 * Reset the Camera, so all transformation matrices are the identity.
	 * After a call to reset() you just have a screen you can write to --- no transformations.
	 */
	void reset() {
	    matrix4x4_type I = Identity();
	    this->m_state->model()                = I;
	    this->m_state->inv_model()            = I;
	    this->m_state->view_orientation()     = I;
	    this->m_state->inv_view_orientation() = I;
	    this->m_state->view_projection()      = I;
	    this->m_state->inv_view_projection()  = I;
	    this->m_state->window_viewport()      = I;
	    this->m_state->inv_window_viewport()  = I;
	    this->m_state->projection()           = I;
	    this->m_state->inv_projection()       = I;
	}


	/**
	 * Set the projection matrix.
	 *
	 * @param vrp	      View reference point
	 * @param vpn         View-plane normal 
	 * @param vup	      View up vector
	 * @param prp	      Projection reference point
	 * @param lower_left  Lower left corner of the view-plane
	 * @param upper_right Upper right corner of the view-plane
	 * @param front_plane Distance to front clipping plane
	 * @param back_plane  Distance to back clipping plane
	 */
	void set_projection(vector3_type const& vrp, vector3_type const& vpn,
			    vector3_type const& vup, vector3_type const& prp,
			    vector2_type const& lower_left,
			    vector2_type const& upper_right, 
			    real_type    const& front_plane,
			    real_type    const& back_plane,
			    real_type    const& viewport_width,
			    real_type    const& viewport_height,
	                    vector3_type const& translation = vector3_type(0.0, 0.0, 0.0))
	{
	    this->m_state->view_orientation() = this->compute_view_orientation_matrix(vrp, vpn, vup);

	    // Compute and Set the eye position - new matrix
	    vector3_type eye_position = Vector3D(this->m_state->inv_view_orientation() * HomVector(prp));
	    //std::cout << "New matrix eye_position = [" << eye_position << "]" << std::endl;

	    this->m_state->eye_position() = eye_position;

	    this->m_state->view_projection()  = this->compute_view_projection_matrix(prp,
										     lower_left,
										     upper_right,
										     front_plane,
										     back_plane);

	     this->m_state->window_viewport() = this->compute_window_viewport_matrix(viewport_width,
										     viewport_height,
		                                                                     translation);

	    matrix4x4_type M;
	    M = this->m_state->window_viewport() * this->m_state->view_projection() * this->m_state->view_orientation();

	    this->m_state->projection() = M;
	}

	/**
	 * Get the projection matrix.
	 * @return The current projection matrix from world to screen.
	 */
	matrix4x4_type get_projection() const
	{
	    return this->m_state->projection();
	}

	/**
	 * Get the inverse projection matrix.
	 * @return The current inverse projection matrix from screen to world.
	 */
	matrix4x4_type get_inv_projection() const
	{
	    return this->m_state->inv_projection();
	}

	/**
	 * Get the View-Orientation.
	 * @return The transformation matrix which transforms from the world-coordinate system to the eye-coordinate system.
	 */
	matrix4x4_type get_view_orientation() const
        {
	    return this->m_state->view_orientation();
	}

	/**
	 * Get the inverse View-Orientation.
	 * @return The transformation matrix which transforms from the eye-coordinate system to the world-coordinate system.
	 */
	matrix4x4_type get_inv_view_orientation() const
        {
	    return this->m_state->inv_view_orientation();
	}

	/**
	 * Get the View-Projection.
	 * @return The transformation matrix which transforms from the eye-coordinate system to the canonical view-volume.
	 */
	matrix4x4_type get_view_projection() const
        {
	    return this->m_state->view_projection();
	}

	/**
	 * Get the inverse View-Projection.
	 * @return The transformation matrix which transforms from the canonical view-volume to the canonical  eye-coordinate system.
	 */
	matrix4x4_type get_inv_view_projection() const
        {
	    return this->m_state->inv_view_projection();
	}

	/**
	 * Get the window-viewport transformation.
	 * @return The transformation matrix which transforms from the canonical view-volume to the screen-coordinate system.
	 */
	matrix4x4_type get_window_viewport() const
        {
	    return this->m_state->window_viewport();
	}

	void set_inv_window_viewport(matrix4x4_type M){
		this->m_state->inv_window_viewport() = M;
	}

	/**
	 * Get the inverse window-viewport transformation.
	 * @return The transformation matrix which transforms from the screen-coordinate system to the canonical view-volume.
	 */
	matrix4x4_type get_inv_window_viewport() const
        {
	    return this->m_state->inv_window_viewport();
	}

	/**
	 * Set the model-view matrix.
	 *
	 *@param M	Model-View matrix.
	 */
	void set_model_view(matrix4x4_type const& M)
	{
	    this->m_state->model() = M;
	}

	/**
	 * Set the inverse model-view matrix.
	 *
	 *@param M	Inverse Model-View matrix.
	 */
	void set_inv_model_view(matrix4x4_type const& M)
	{
	    this->m_state->inv_model() = M;
	}

    protected:

	/**
	 * Computes a matrix which transforms coordinates from the World-coordinate system to the Eye-coordinate system.
	 * Foley et al. (p.229-284; 2nd edition)
	 * @param vrp The View Reference Point.
	 * @param vpn The View Plane Normal.
	 * @param vup The View Up Vector.
	 * @return    The view_orientation matrix.
	 */
	virtual matrix4x4_type compute_view_orientation_matrix(vector3_type const& vrp,
							       vector3_type const& vpn,
							       vector3_type const& vup) = 0;

	/**
	 * Computes a matrix which transforms from the Eye-coordinate system to the Canonical View Volume.
	 * Foley et al. (p.229-284; 2nd edition)
	 * @param prp         The Projection Reference Point in Eye-coordinates.
	 * @param lower_left  The lower left corner of the window in Eye-coordinates.
	 * @param upper_right The upper right corner of the window in Eye-coordinates.
	 * @param front_plane The closest distance from the origin in Eye-coordinates.
	 * @param back_plane  The farthest distance from the origin in Eye-coordinates.
	 * @return The view_projection matrix.
	 */
	virtual matrix4x4_type compute_view_projection_matrix(vector3_type const& prp,
							      vector2_type const& lower_left,
							      vector2_type const& upper_right,
							      real_type    const& front_plane,
							      real_type    const& back_plane) = 0;

	/**
	 * Computes a matrix which transforms from the Canonical-Volume to the Screen coordinates.
	 * @param window_width  The width of the window on the screen.
	 * @param window_height The height of the window on the screen.
	 */
	virtual matrix4x4_type compute_window_viewport_matrix(real_type const& viewport_width,
							      real_type const& viewport_height,
	                                                      vector3_type const& translation) = 0;

	/**
	 * Computes a projection matrix using the parameters for a perspective camera 
	 * given by Foley et al. (p.229-284; 2nd edition)
	 * The projection matrix should transforms from world coordinates to 
	 * normalized projection coordinates
	 *
	 * @param vrp	      View reference point
	 * @param vpn         View-plane normal 
	 * @param vup	      View up vector
	 * @param prp	      Projection reference point
	 * @param lower_left  Lower left corner of the view-plane
	 * @param upper_right Upper right corner of the view-plane
	 * @param front_plane Distance to front clipping plane
	 * @param back_plane  Distance to back clipping plane
	 */
	virtual matrix4x4_type compute_projection_matrix(vector3_type const& vrp,
							 vector3_type const& vpn,
							 vector3_type const& vup,
							 vector3_type const& prp,
							 vector2_type const& lower_left,
							 vector2_type const& upper_right,
							 real_type    const& front_plane,
							 real_type    const& back_plane,
							 real_type    const& screen_width,
							 real_type    const& screen_height) = 0;

    protected:
	
	/**
	 * Stores a pointer to the state of the graphics pipeline.
	 */
	GraphicsState<math_types>* m_state;

	/**
	 * Stores a pointer to the graphics render pipeline.
	 * so we can get the resolution of the framebuffer.
	 */
	RenderPipeline<math_types>* m_renderpipeline;

    };

}// end namespace graphics

// GRAPHICS_CAMERA_H
#endif
