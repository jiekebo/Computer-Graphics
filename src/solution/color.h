#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <iomanip>

#include "solution/math_types.h"

namespace graphics {

/*******************************************************************\
*                                                                   *
*                             C o l o r                             *
*                                                                   *
\*******************************************************************/

    template <typename math_types>
    class Color {
    public:
	typedef math_types::real_type    real_type;
	typedef math_types::vector3_type vector3_type;

    public:

/*******************************************************************\
*                                                                   *
*                           C o l o r ( )                           *
*                                                                   *
\*******************************************************************/

	Color()
	{}

/*******************************************************************\
*                                                                   *
*                          ~ C o l o r ( )                          *
*                                                                   *
\*******************************************************************/

	virtual ~Color()
	{}

/*******************************************************************\
*                                                                   *
*                         i n i t ( . . . )                         *
*                                                                   *
\*******************************************************************/

	void init(real_type ka, vector3_type Oa,
	          real_type kd, vector3_type Od,
	          real_type ks, vector3_type Os,
	          int shininess)
	{
	    this->ka = ka;
	    this->Oa = Oa;
	    this->ambient_color = this->ka * this->Oa;

	    this->kd = kd;
	    this->Od = Od;
	    this->diffuse_color = this->kd * this->Od;

	    this->ks = ks;
	    this->Os = Os;
	    this->specular_color = this->ks * this->Os;
	    this->shininess      = shininess;
	}

/*******************************************************************\
*                                                                   *
*                   A m b i e n t C o l o r ( )                     *
*                                                                   *
\*******************************************************************/

	vector3_type AmbientColor()
	{
	    return this->ambient_color;
	}

/*******************************************************************\
*                                                                   *
*                    D i f f u s e C o l o r ( )                    *
*                                                                   *
\*******************************************************************/

	vector3_type DiffuseColor()
	{
	    return this->diffuse_color;
	}

/*******************************************************************\
*                                                                   *
*                   S p e c u l a r C o l o r ( )                   *
*                                                                   *
\*******************************************************************/

	vector3_type SpecularColor()
	{
	    return this->specular_color;
	}

/*******************************************************************\
*                                                                   *
*                       S h i n i n e s s ( )                       *
*                                                                   *
\*******************************************************************/

	int Shininess()
	{
	    return this->shininess;
	}


    protected:

/*******************************************************************\
*                                                                   *
*                 P r i v a t e   V a r i a b l e s                 *
*                                                                   *
\*******************************************************************/

    private:
	real_type    ka;
	vector3_type Oa;
	vector3_type ambient_color;

	real_type    kd;
	vector3_type Oa;
	vector3_type diffuse_color;

	real_type    ks;
	vector3_type Oa;
	vector3_type specular_color;
	int          shininess;
    };
}

#endif
