#ifndef LINEARINTERPOLATOR
#define LINEARINTERPOLATOR

#include <iostream>
#include <iomanip>
#include <cmath>

#include "graphics/graphics.h"
#include "solution/math_types.h"
#include "solution/interpolator.h"

// Maybe this class should contain all the variables it uses! ?

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
	    this->t_start   = 0;
	    this->t_stop    = 0;
	    this->t_current = this->t_start;
	    this->Delta_t   = 1;

	    this->v_start   = value_type();
	    this->v_stop    = value_type();
	    this->v_current = this->v_start;
	    this->Delta_v   = value_type();

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


    // I think this comment is pure crap in some sense -- no, but should be edited!
    //
    // value = v_start + (v_stop - v_start) * t;   t in [0, 1]
    //
    // t = (t_current - t_start) / (t_stop - t_start);    t_current in [t_start, t_stop]
    //
    // value = v_start + (v_stop - v_start) * (t_current - t_start) / (t_stop - t_start)
    //
    // Notice: (t_current - t_start) / (t_stop - t_start) in [0, 1] --- always
    //
    // If (t_stop - t_start) >= 0 the following equation is correct
    // value = v_start + (v_stop - v_start) * (t_current - t_start) / (t_stop - t_start)
    //       = v_start - (v_stop - v_start) / (t_stop - t_start) * t_start
    //                 + (v_stop - v_start) / (t_stop - t_start) * t_current
    //       = A + B * t_current
    // where
    //    A = v_start - (v_stop - v_start) / (t_stop - t_start) * t_start
    //    B = (v_stop - v_start) / (t_stop - t_start)
    // If t_current is a positeve integer then
    //    Delta_value = B = (v_stop - v_start) / (t_stop - t_start)
    //
    // If (t_stop - t_start) < 0 the following equation should be used
    // value = v_start + (v_stop - v_start) * (t_start - t_current) / (t_start - t_stop)
    //       = v_start + (v_stop - v_start) / (t_start - t_stop) * t_start
    //                 - (v_stop - v_start) / (t_start - t_stop) * t_current
    //       = A + B * t_current
    // where
    //    A = v_start + (v_stop - v_start) / (t_start - t_stop) * t_start
    //    B = - (v_stop - v_start) / (t_start - t_stop) * t_current
    //
    // If t_current is a positive integer then
    //    Delta_value = B = (v_stop - v_start) / (t_start - t_stop)
    //
    // The conclusion is that Delta_value can be written
    //    Delta_value = (v_stop - v_start) / fabs(t_stop - t_start)


	void init(int t_start, int t_stop, value_type const& Vstart, value_type const& Vstop)
        {
	    //std::cout << std::endl << "-->LinearInterpolator::init(...)" << std::endl;

	    if (t_start <= t_stop) {
		this->t_start   = t_start;
		this->t_stop    = t_stop;
	    }
	    else {
		this->t_start   = t_stop;
		this->t_stop    = t_start;
	    }
	    this->t_current = this->t_start;

	    if (this->t_start == this->t_stop)
		this->Delta_t = 0;
	    else {
		this->Delta_t = (this->t_start <= this->t_stop) ? 1 : -1;
	    }

	    this->v_start   = Vstart;
	    this->v_stop    = Vstop;
	    this->v_current = Vstart;

	    // This is a hack to get it to work --- come back and fix right!
	    // This hack did not fix it, just for a try!
	    if (this->Delta_t == 0) {
		//std::cout << "LinearInterpolator::init(...): Delta_t == 0" << std::endl;
		this->Delta_v = value_type();
		this->valid = false;
	    }
	    else {
		this->Delta_v =
		    (this->v_stop - this->v_start)
		    /
		    std::fabs(static_cast<real_type>( this->t_stop  - this->t_start));
		this->valid = true;
	    }

#if 0
	    std::cout << "   LinearInterpolator::init(...):" << std::endl;
	    std::cout << "       [t_start, t_stop, t_current, Delta_t] = ["
		      << this->t_start << ", " << this->t_stop << ", " << this->t_current
		      << ", " << this->Delta_t << "]" << std::endl;
	    std::cout << "       [v_start, v_stop, v_current, Delta_v] = ["
		      << this->v_start << ", " << this->v_stop << ", " << this->v_current
		      << ", " << this->Delta_v << "]"
		      << std::endl;
#endif
	    //this->valid = true;

	    //std::cout << "<--LinearInterpolator::init(...)" << std::endl;
	}


/*******************************************************************\
*                                                                   *
*                           V a l u e ( )                           *
*                                                                   *
\*******************************************************************/

	value_type const& value() const
	{
#if 0
	    std::cout << std::endl;
	    std::cout << "-->LinearInterpolator::value(): " << std::endl;
	    std::cout << "   [t_start, t_stop, t_current, Delta_t] = ["
		      << this->t_start << ", " << this->t_stop << ", "
		      << this->t_current << ", " << this->Delta_t << "]"
		      << std::endl;
	    std::cout << "   [Vstart, Vstop, Vcurrent, Delta_v] = ["
		      << this->v_start << ", " << this->v_stop << ", " << this->v_current
		      << this->Delta_v << "]"
		      << std::endl;
#endif
	    if (!(this->valid)) {
#if 0
		std::cout << std::endl;
		std::cout << "-->LinearInterpolator::value(): " << std::endl;
		std::cout << "   [t_start, t_stop, t_current, Delta_t] = ["
			  << this->t_start << ", " << this->t_stop << ", "
			  << this->t_current << ", " << this->Delta_t << "]"
			  << std::endl;
		std::cout << "   [Vstart, Vstop, Vcurrent, Delta_v] = ["
			  << this->v_start << ", " << this->v_stop << ", " << this->v_current
			  << this->Delta_v << "]"
			  << std::endl;
#endif
		throw std::runtime_error("LinearInterpolator::value(): Invalid State/Not Initialized");

		//std::cout << "<--LinearInterpolator::value(): " << std::endl;
	    }
	    //std::cout << "<--LinearInterpolator::value(): " << std::endl;

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
	    return this->valid;
	}

/*******************************************************************\
*                                                                   *
*                      n e x t _ v a l u e ( )                      *
*                                                                   *
\*******************************************************************/

	void next_value()
	{
	    //std::cout << "-->LinearInterpolator::next_value():" << std::endl;
	    if (this->t_current == this->t_stop) {
		this->valid = false;
		//std::cout << "    valid = " << this->valid << std::endl;
	    }
	    else {
		this->t_current += this->Delta_t;
		this->v_current += this->Delta_v;
		//this->valid = (this->t_current <= this->t_stop);

#if 0
		std::cout << "    valid = " << this->valid
			  << std::endl;
		std::cout << "    t_start   = " << this->t_start   << std::endl
			  << "    t_stop    = " << this->t_stop    << std::endl
			  << "    t_current = " << this->t_current << std::endl
			  << "    Delta_t   = " << this->Delta_t   << std::endl;
		std::cout << std::endl;
		std::cout << "    v_start   = " << this->v_start   << std::endl
			  << "    v_stop    = " << this->v_stop    << std::endl
			  << "    v_current = " << this->v_current << std::endl
			  << "    Delta_v   = " << this->Delta_v   << std::endl;
		std::cout << std::endl;
#endif
	    }
	    //std::cout << "<--LinearInterpolator::next_value():" << std::endl;
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
