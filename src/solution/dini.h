#ifndef DINI_H
#define DINI_H

#include "solution/math_types.h"
#include "solution/transformations.h"


/*******************************************************************\
*                                                                   *
*                     D i n i ' s  S u r f a c e                    *
*                                                                   *
\*******************************************************************/

class DiniSurface {
public:
    typedef MyMathTypes::real_type    real_type;
    typedef MyMathTypes::vector3_type vector3_type;

public:
    DiniSurface()
    {}

    virtual ~DiniSurface()
    {}

    void init()
    {}


    vector3_type Dini(real_type const& u, real_type const& v, real_type const& a, real_type const& b) const
    {
	vector3_type point;
	point[1] = a * cos(u) * sin(v);
	point[2] = a * sin(u) * sin(v);
	point[3] = a * (cos(v) + log(tan(0.5 * v))) + b * u ;

	return point;
    }

    vector3_type DiniNormal(real_type const& u,real_type const&  v, real_type const& a, real_type const& b) const
    {
    vector3_type uDeriv;
    uDeriv[1] = - a * sin(u) * sin(v);
    uDeriv[2] = a * cos(u) * sin(v);
    uDeriv[3] = b;

    vector3_type vDeriv;
    vDeriv[1] = a * cos(u) * cos(v);
    vDeriv[2] = a * sin(u) * cos(v);
    vDeriv[3] = a * (-sin(v) + (1 + pow(tan(0.5 * v),2))/(2 * tan(0.5 * v)));

	vector3_type normal;
	normal = Cross(uDeriv, vDeriv);

	if (!Zero(normal)) normal /= Norm(normal);
	normal = - normal;

	//std::cout << "KleinBottomNormal(" << u << ", " << v << ") == [" << normal << "]" << std::endl;

	return normal;
    }

private:

};

#endif
