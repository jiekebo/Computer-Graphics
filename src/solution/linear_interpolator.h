#ifndef LINEARINTERPOLATOR
#define LINEARINTERPOLATOR

#include <iostream>
#include <iomanip>
#include <cmath>

#include "graphics/graphics.h"
#include "solution/math_types.h"
#include "solution/interpolator.h"


namespace graphics {

/*******************************************************************\
*                                                                   *
*                L i n e a r I n t e r p o l a t o r                *
*                                                                   *
\*******************************************************************/

    template <typename math_types, typename value_type>
    class LinearInterpolator : public Interpolator<math_types, value_type> {
    public:
	typedef typename math_types::vector3_type vector3_type;
	typedef typename math_types::real_type    real_type;

    public:
	LinearInterpolator() : Interpolator<math_types, value_type>()
	{
	    // Do your own magic here

	    this->v_current = value_type();
	    this->valid = false;
	}

	LinearInterpolator(LinearInterpolator const& new_linearinterpolator)
	    : Interpolator<math_types, value_type>(new_linearinterpolator)
	      
	{
	    this->t_start   = new_linearinterpolator.t_start;
	    this->t_stop    = new_linearinterpolator.t_stop;
	    this->t_current = new_linearinterpolator.t_current;
	    this->Delta_t   = new_linearinterpolator.Delta_t;


	    this->v_start   = new_linearinterpolator.v_start;
	    this->v_stop    = new_linearinterpolator.v_stop;
	    this->v_current = new_linearinterpolator.v_current;
	    this->Delta_v   = new_linearinterpolator.Delta_v;

	    this->valid     = new_linearinterpolator.valid;
	}

/*******************************************************************\
*                                                                   *
*                ~ L i n e a r I n t e r p o l a t o r ( )          *
*                                                                   *
\*******************************************************************/

	virtual ~LinearInterpolator() {}


/*******************************************************************\
*                                                                   *
*     i n i t ( 2   x   i n t ,   2   x   v a l u e _ t y p e &)    *
*                                                                   *
\*******************************************************************/

    // Usage:
    //    while (more_values()) {
    //        value_type this_value = value();
    //        ...
    //        use this_value
    //        ...
    //        next_value();
    //    }


	void init(int t_start, int t_stop, value_type const& Vstart, value_type const& Vstop)
        {
	    // Do your own magic here.

	    this->v_current = value_type();
	    this->valid = true;
	}


/*******************************************************************\
*                                                                   *
*                           V a l u e ( )                           *
*                                                                   *
\*******************************************************************/

	value_type const& value() const
	{
	  //if (!(this->valid)) {
	  //    throw std::runtime_error("LinearInterpolator::value(): Invalid State/Not Initialized");
	  //}

	    return this->v_current;
	}


/*******************************************************************\
*                                                                   *
*                 p r i n t _ v a r i a b l e s ( )                 *
*                                                                   *
\*******************************************************************/

	void print_variables()
	{
	    std::cout << "LinearInterpolator: local variables" << std::endl;
	    std::cout << "===================================" << std::endl;
	    std::cout << "\tvalid       == "     << this->valid   << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tt_start     == " << this->t_start   << std::endl;
	    std::cout << "\tt_current   == " << this->t_current << std::endl;
	    std::cout << "\tt_stop      == " << this->t_stop    << std::endl;
	    std::cout << "\tDelta_t     == " << this->Delta_t   << std::endl;
	    std::cout << std::endl;
	    std::cout << "\tv_start     == " << this->v_start   << std::endl;
	    std::cout << "\tv_current   == " << this->v_current << std::endl;
	    std::cout << "\tv_stop      == " << this->v_stop    << std::endl;
	    std::cout << "\tDelta_v     == " << this->Delta_v   << std::endl;
	    std::cout << std::endl;
	}


/*******************************************************************\
*                                                                   *
*                     m o r e _ v a l u e s ( )                     *
*                                                                   *
\*******************************************************************/

	bool more_values() const
	{
            // Do your own magic here.
	    //return this->valid;

	    return false;
	}

/*******************************************************************\
*                                                                   *
*                      n e x t _ v a l u e ( )                      *
*                                                                   *
\*******************************************************************/

	void next_value()
	{
	    // Do your own magic here.
	   
	    this->valid = false;
	}
	
/*******************************************************************\
*                                                                   *
*     o p e r a t o r = ( L i n e a r I n t e r p o l a t o r )     *
*                                                                   *
\*******************************************************************/

	LinearInterpolator<math_types, value_type> const&
	operator=(LinearInterpolator const& newlinearinterpolator)
	{
	    if (this != &newlinearinterpolator) {
		this->Interpolator<math_types, value_type>::operator=(newlinearinterpolator);

		this->t_start   = newlinearinterpolator.t_start;
		this->t_stop    = newlinearinterpolator.t_stop;
		this->t_current = newlinearinterpolator.t_current;
		this->Delta_t   = newlinearinterpolator.Delta_t;
		
		this->v_start   = newlinearinterpolator.v_start;
		this->v_stop    = newlinearinterpolator.v_stop;
		this->v_current = newlinearinterpolator.v_current;
		this->Delta_v   = newlinearinterpolator.Delta_v;

		this->valid     = newlinearinterpolator.valid;
	    }

	    return *this;
	}


/*******************************************************************\
*                                                                   *
*                 P r o t e c t e d   M e m b e r s                 *
*                                                                   *
\*******************************************************************/


    protected:


/*******************************************************************\
*                                                                   *
*                   P r i v a t e   M e m b e r s                   *
*                                                                   *
\*******************************************************************/

    private:
	int t_start;
	int t_stop;
	int t_current;
	int Delta_t;

	value_type v_start;
	value_type v_stop;
	value_type v_current;
	value_type Delta_v;

	bool valid;
    };
}

#endif
