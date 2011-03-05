#ifndef KLEINBOTTLE_H
#define KLEINBOTTLE_H

#include "solution/math_types.h"
#include "solution/transformations.h"


/*******************************************************************\
*                                                                   *
*                       K l e i n B o t t l e                       *
*                                                                   *
\*******************************************************************/

class KleinBottle {
public:
    typedef MyMathTypes::real_type    real_type;
    typedef MyMathTypes::vector3_type vector3_type;

public:
    KleinBottle()
    {}

    virtual ~KleinBottle()
    {}

    void init()
    {}


    vector3_type BottomVertex(real_type const& u, real_type const& v) const
    {
	vector3_type point;
	point[1] = (2.5 + 1.5 * cos(v)) * cos(u);
	point[2] = (2.5 + 1.5 * cos(v)) * sin(u);
	point[3] = -2.5 * sin(v);

	//std::cout << "KleinBottomPoint(" << u << ", " << v << ") == [" << point << "]" << std::endl;

	return point;
    }
    
    vector3_type BottomNormal(real_type const& u,real_type const&  v) const
    {
	vector3_type normal;
	normal[1] = (-6.25 - 3.75 * cos(v)) * cos(v) * cos(u);
	normal[2] = (-6.25 - 3.75 * cos(v)) * cos(v) * sin(u);
	normal[3] = ( 3.75 + 2.25 * cos(v)) * sin(v);
	
	if (!Zero(normal)) normal /= Norm(normal);
	normal = - normal;

	//std::cout << "KleinBottomNormal(" << u << ", " << v << ") == [" << normal << "]" << std::endl;

	return normal;
    }

    vector3_type HandleVertex(real_type const& u ,real_type const&  v) const
    {
	vector3_type point;
	point[1] = 2.0 - 2.0 * cos(v) + sin(u);
	point[2] = cos(u);
	point[3] = 3 * v;

	//std::cout << "KleinHandlePoint(" << u << ", " << v << ") == [" << point << "]" << std::endl;

	return point;
    }
    
    vector3_type HandleNormal(real_type const& u, real_type const&  v) const
    {
	vector3_type normal;
	normal[1] = - 3.0 * sin(u);
	normal[2] = - 3.0 * cos(u);
	normal[3] =   2.0 * sin(u) * sin(v);

	if (!Zero(normal)) normal /= Norm(normal);
	normal = - normal;

	//std::cout << "KleinHandleNormal(" << u << ", " << v << ") == [" << normal << "]" << std::endl;
    
	return normal;
    }

    vector3_type TopVertex(real_type const& u, real_type const&  v) const
    {
	vector3_type point;
	point[1] = 2.0 + (2.0 + cos(u)) * cos(v);
	point[2] = sin(u);
	point[3] = 3.0 * M_PI + (2.0 + cos(u)) * sin(v);

	//std::cout << "KleinTopPoint(" << u << ", " << v << ") == [" << point << "]" << std::endl;

	return point;
    }
    
    vector3_type TopNormal(real_type const& u, real_type const&  v) const
    {
	vector3_type normal;
	normal[1]= (2.0 + cos(u)) * cos(u) * cos(v);
	normal[2]= (2.0 + cos(u)) * sin(u);
	normal[3]= (2.0 + cos(u)) * cos(u) * sin(v);

	//std::cout << "Top Normal == " << normal << std::endl;

	if (!Zero(normal)) normal /= Norm(normal);

	//std::cout << "KleinTopNormal(" << u << ", " << v << ") == [" << normal << "]" << std::endl;

	return normal;
	
    }

    vector3_type MiddleVertex(real_type const& u, real_type const&  v) const
    {
	vector3_type point;
	point[1] = (2.5 + 1.5 * cos(v)) * cos(u);
	point[2] = (2.5 + 1.5 * cos(v)) * sin(u);
	point[3] = 3 * v;

	//std::cout << "KleinMiddlePoint(" << u << ", " << v << ") == [" << point << "]" << std::endl;

	return point;
    }
    
    vector3_type MiddleNormal(real_type const& u, real_type const&  v) const
    {
	vector3_type normal;
	normal[1] = (7.5 + 4.5 * cos(v)) * cos(u);
	normal[2] = (7.5 + 4.5 * cos(v)) * sin(u);
	normal[3] = (3.75 + 2.25 * cos(v)) * sin(v);

	if (!Zero(normal)) normal /= Norm(normal);

	//std::cout << "KleinMiddleNormal(" << u << ", " << v << ") == [" << normal << "]" << std::endl;

	return normal;
    }

private:

};

#endif
