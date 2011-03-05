#ifndef CAMERA_H
#define CAMERA_H
//
// Graphics Framework.
// Copyright (C) 2007 Department of Computer Science, University of Copenhagen
//
#include "graphics/graphics.h"
#include "solution/transformations.h"

#define PRINT_MATRICES 0


namespace graphics {
    
    /**
     * A virtual pin-hole camera.
     * The camera class produces the matrices necessary for the virtual world
     * to be rasterized.
     */
    template<typename math_types>
    class MyCamera : public Camera<math_types>
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
	MyCamera() : Camera<math_types>()
	{}
	


	/**
	 * Destroys the Camera.
	 */
	virtual ~MyCamera()
	{}

    protected:

	/**
	 * Computes a matrix which transforms coordinates from the World-coordinate system 
	 * to the Eye-coordinate system.
	 * Foley et al. (p.229-284; 2nd edition)
	 * @param vrp The View Reference Point.
	 * @param vpn The View Plane Normal.
	 * @param vup The View Up Vector.
	 * @return    The view_orientation matrix.
	 */
	matrix4x4_type compute_view_orientation_matrix(vector3_type const& vrp,
						       vector3_type const& vpn,
						       vector3_type const& vup)
	{
#if 0
	    matrix4x4_type M;    // The identity matrix.
	    M[1][1] = 1, M[1][2] = 0, M[1][3] = 0, M[1][4] = 0;
	    M[2][1] = 0, M[2][2] = 1, M[2][3] = 0, M[2][4] = 0;
	    M[3][1] = 0, M[3][2] = 0, M[3][3] = 1, M[3][4] = 0;
	    M[4][1] = 0, M[4][2] = 0, M[4][3] = 0, M[4][4] = 1;

	    return M;
#else

	    /// Translate -vrp, and its inverse
	    matrix4x4_type T    = Translate(-vrp);
	    matrix4x4_type InvT = Translate(vrp);

#if PRINT_MATRICES
	    std::cout << "T(-vrp) = T(" << -vrp << ") = " << T << std::endl;
	    std::cout << "InvT(" << -vrp << ") = T(" << vrp << ") = " << InvT << std::endl;
#endif

	    // Rotate so the Eye coordinate system coinsides with the Workd coordinate system
	    vector3_type n = vpn;
	    n /= Norm(n);

	    vector3_type u = Cross(vup, vpn);
	    u /= Norm(u);

	    vector3_type v = Cross(n, u);
	    v /= Norm(v);

	    matrix4x4_type R = Identity();
	    for (int i = 1; i <= 3; ++i) {
		R[1][i] = u[i];
		R[2][i] = v[i];
		R[3][i] = n[i];
	    }

	    /// and the inverse
	    matrix4x4_type InvR = R.T();

	    // save x_eye, y_eye, z_eye in graphics_state
	    this->Camera<math_types>::m_state->x_eye_axis() = u;
	    this->Camera<math_types>::m_state->y_eye_axis() = v;
	    this->Camera<math_types>::m_state->z_eye_axis() = n;

#if PRINT_MATRICES
	    std::cout << "R    = " << R << std::endl;
	    std::cout << "InvR = R.T() = " << InvR << std::endl;
#endif

	    matrix4x4_type ViewOrient = R * T;
	    this->m_state->view_orientation() = ViewOrient;

#if PRINT_MATRICES
	    std::cout << "ViewOrient = R * T(-vrp) = " << ViewOrient << std::endl;
#endif

	    matrix4x4_type InvViewOrient = InvT * InvR;
	    this->m_state->inv_view_orientation() = InvViewOrient;

#if PRINT_MATRICES
	    std::cout << "InvViewOrient = T(vrp) * R.T() = " << InvViewOrient << std::endl;
#endif

	    return ViewOrient;
#endif
	}

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
	matrix4x4_type compute_view_projection_matrix(vector3_type const& prp,
						      vector2_type const& lower_left,
						      vector2_type const& upper_right,
						      real_type    const& front_plane,
						      real_type    const& back_plane)
        {
#if 0
	    matrix4x4_type M;    // The identity matrix.
	    M[1][1] = 1, M[1][2] = 0, M[1][3] = 0, M[1][4] = 0;
	    M[2][1] = 0, M[2][2] = 1, M[2][3] = 0, M[2][4] = 0;
	    M[3][1] = 0, M[3][2] = 0, M[3][3] = 1, M[3][4] = 0;
	    M[4][1] = 0, M[4][2] = 0, M[4][3] = 0, M[4][4] = 1;

	    return M;
#else
	    // Transform the the user view-volumem the the canonical view-volume
	    // There are a number of steps:

	    // Translate prp to the origin
	    matrix4x4_type T    = Translate(-prp);
	    matrix4x4_type InvT = Translate(prp);

#if PRINT_MATRICES
	    std::cout << "T(-prp) = T(" << -prp << ") = " << T << std::endl;
#endif

	    // Shear_xy so the view volume is symmetric around the z-axis
	    vector2_type tmp = (upper_right + lower_left) / 2.0;
	    vector3_type cw(tmp[1], tmp[2], 0.0);
	    vector3_type dop = prp - cw;
	    real_type Sh_x = - dop[1] / dop[3];
	    real_type Sh_y = - dop[2] / dop[3];

	    matrix4x4_type Sh_xy    = XY_Shear(Sh_x, Sh_y);
	    matrix4x4_type InvSh_xy = XY_Shear(-Sh_x, -Sh_y);

#if PRINT_MATRICES
	    //std::cout << "lower_left  = " << lower_left << std::endl;
	    //std::cout << "upper_right = " << upper_right << std::endl;
	    //std::cout << "cw  = " << cw  << std::endl;
	    //std::cout << "prp = " << prp << std::endl;
	    //std::cout << "dop = prp - cw = " << dop << std::endl;
	    std::cout << "Sh_xy(-dop_u/dop_n, -dop_v/dop_n) = Sh_xy("
		      << - dop[1] / dop[3] << " " << - dop[2] / dop[3] << ") = " << Sh_xy << std::endl;
#endif


	    // Scale such that the planes make 45 degrees the z-axis
	    // First step: scale in x- and y-directions
	    vector2_type window_dimensions = upper_right - lower_left;
	    real_type s_x = 2.0 * prp[3] / window_dimensions[1];
	    real_type s_y = 2.0 * prp[3] / window_dimensions[2];

	    matrix4x4_type S_xy    = Scale(s_x, s_y, 1.0);
	    matrix4x4_type InvS_xy = Scale(1.0 / s_x, 1.0 / s_y, 1.0);

#if PRINT_MATRICES  
	    std::cout << "upper_right - lower_left = [" << window_dimensions << "]^T" << std::endl;
	    std::cout << "S(s_x, s_y, 1) = S(" << s_x << " " << s_y << " 1) = " << S_xy << std::endl;
#endif

	    // Second step: scale uniformly so the view-volume gets the right dimensions, i.e.
	    // Scale uniformly so the back plane is mapped to -1.
	    real_type s = -1.0 / (back_plane - prp[3]);
	    if (Zero(s)) s = 1.0;
	    matrix4x4_type S_uniform    = Scale(s, s, s);
	    matrix4x4_type InvS_uniform = Scale(1.0 / s, 1.0 / s, 1.0 / s);

#if PRINT_MATRICES	    
	    std::cout << "back_plane = " << back_plane << std::endl;
	    std::cout << "prp = [" << prp << "]^T" << std::endl;
	    std::cout << "s = " << s << std::endl;
	    std::cout << "S_uniform(s) = S(" << s << ") = " << S_uniform << std::endl;
	    
#endif

	    // Now, transform the perspective view-volume to the parallel view-volume
	    real_type z_max = - (front_plane - prp[3]) / (back_plane - prp[3]);

	    matrix4x4_type Perp2Par    = Perspective_to_Parallel(z_max);
	    matrix4x4_type InvPerp2Par = InvPerspective_to_Parallel(z_max);
	    
#if PRINT_MATRICES
	    std::cout << "Perp2Par = " << Perp2Par << std::endl;
#endif

	    matrix4x4_type ViewProject    = Perp2Par * S_uniform * S_xy * Sh_xy * T;
	    matrix4x4_type InvViewProject = InvT * InvSh_xy * InvS_uniform * InvPerp2Par;

	    this->m_state->view_projection()     = ViewProject;
	    this->m_state->inv_view_projection() = InvViewProject;

#if PRINT_MATRICES
	    std::cout << "ViewProject = " << ViewProject << std::endl;
#endif

	    return ViewProject;
#endif
	}


	/**
	 * Computes a matrix which transforms from the Canonical-Volume to the Screen coordinates.
	 * @param viewport_width  The width of the viewport on the screen.
	 * @param viewport_height The height of the viewport on the screen.
	 */
	virtual matrix4x4_type compute_window_viewport_matrix(real_type const& viewport_width,
							      real_type const& viewport_height,
	                                                      vector3_type const& translation
							                          = vector3_type(0.0, 0.0, 0.0))
	{
	    /// The translation and its inverse

	    matrix4x4_type T_wv(Translate(1, 1, 0));
	    matrix4x4_type InvT_wv(Translate(-1, -1, 0));

#if PRINT_MATRICES
	    std::cout << "T_wv(1.0, 1.0, 0.0) = " << T_wv << std::endl;
#endif

	    /// The scaling and its inverse
	    matrix4x4_type S_screen(Scale(viewport_width / 2.0, viewport_height / 2.0, 1.0));
	    matrix4x4_type InvS_screen(Scale(2.0 / viewport_width, 2.0 / viewport_height, 1.0));

#if PRINT_MATRICES
	    std::cout << "S_screen("
		      << viewport_width  / 2.0 << " "
		      << viewport_height / 2.0 << " "
		      << 0.0
		      << ") = " << S_screen << std::endl;
#endif

	    /// The translation on the screen and its inverse
	    matrix4x4_type T_screen(Translate(translation));
	    matrix4x4_type InvT_screen(Translate(-translation));


	    /// The final window-viewport transformation
	    matrix4x4_type WindowViewport = T_screen * S_screen * T_wv;
	    this->m_state->window_viewport() = WindowViewport;

#if PRINT_MATRICES
	    std::cout << "WindowViewport = " << WindowViewport << std::endl;
#endif


	    matrix4x4_type InvWindowViewport = InvT_wv * InvS_screen * InvT_screen;
	    this->m_state->inv_window_viewport() = InvWindowViewport;

	    return WindowViewport;
	}
// #endif


	/**
	 * Computes a projection matrix using the parameters for a perspective camera 
	 * given by Foley et al. (p.229-284; 2nd edition)
	 * The projection matrix should transforms from world coordinates to 
	 * normalized projection coordinates.
	 * This is done in a number of steps: compute_view_orientation, compute_view_projection, and compute_window_viewport.
	 * The final matrix is stored in the graphics_state.
	 *
	 * @param vrp	        View reference point
	 * @param vpn           View-plane normal 
	 * @param vup	        View up vector
	 * @param prp	        Projection reference point
	 * @param lower_left    Lower left corner of the view-plane
	 * @param upper_right   Upper right corner of the view-plane
	 * @param front_plane   Distance to front clipping plane
	 * @param back_plane    Distance to back clipping plane
	 * @param screen_width  The width in pixels of the screen
	 * @param screen_height The height in pixels of the screen
	 */
	matrix4x4_type compute_projection_matrix(vector3_type const& vrp, vector3_type const& vpn,
						 vector3_type const& vup, vector3_type const& prp,
						 vector2_type const& lower_left,
						 vector2_type const& upper_right,
						 real_type    const& front_plane,
						 real_type    const& back_plane,
						 real_type    const& screen_width,
						 real_type    const& screen_height)
	{
	    // ToDo add your magic here!
#if 0
	    matrix4x4_type M;    // The identity matrix.
	    M[1][1] = 1, M[1][2] = 0, M[1][3] = 0, M[1][4] = 0;
	    M[2][1] = 0, M[2][2] = 1, M[2][3] = 0, M[2][4] = 0;
	    M[3][1] = 0, M[3][2] = 0, M[3][3] = 1, M[3][4] = 0;
	    M[4][1] = 0, M[4][2] = 0, M[4][3] = 0, M[4][4] = 1;

	    return M;
#else
	    matrix4x4_type Id = Identity();
	    return Id;
#endif
	}
    };

}// end namespace graphics

// CAMERA_H
#endif
