#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H


#include "graphics/graphics.h"
#include "solution/math_types.h"


/*******************************************************************\
*                                                                   *
*                       B o u n d i n g B o x                       *
*                                                                   *
\*******************************************************************/

template <typename math_types>
class BoundingBox {
public:
    typedef typename math_types::real_type    real_type;
    typedef typename math_types::vector3_type vector3_type;

public:
    BoundingBox() : first_item(true)
    {}

    virtual ~BoundingBox()
    {}

    void Reset()
    {
	this->first_item = true;
    }

    vector3_type const& Lower_Left() const
    {
	return this->Min;
    }

    vector3_type const& Upper_Right() const
    {
        return this->Max;
    }

    void Submit(vector3_type const& value)
    { 
	if (this->first_item) {
	    this->Min = value;
	    this->Max = value;
	    this->first_item = false;
	}
	else {
	    for (int i = 1; i <= 3; ++i) {
		if (this->Min[i] > value[i]) this->Min[i] = value[i];
		if (this->Max[i] < value[i]) this->Max[i] = value[i];
	    }
	}
    }
    
private:
    bool         first_item;
    vector3_type Min;
    vector3_type Max;
};

#endif
