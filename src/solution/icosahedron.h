#ifndef ISOCAHEDRON_H
#define ISOCAHEDRON_H

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <vector>

#include "solution/math_types.h"
#include "solution/transformations.h"

/*******************************************************************\
*                                                                   *
*                       I c o s a h e d r o n                       *
*                                                                   *
*             C e n t e r e d   a t   t h e   O r i g i n           *
*                                                                   *
\*******************************************************************/

class Icosahedron {
public:
    typedef MyMathTypes::real_type    real_type;
    typedef MyMathTypes::vector3_type vector3_type;

    typedef MyMathTypes::vector3_type vertex;
    typedef std::vector<vertex>       vertex_list;

    typedef std::vector<vertex>       triangle;
    typedef std::vector<triangle>     triangle_list;

    typedef MyMathTypes::vector3_type normal;
    typedef std::vector<normal>       normal_list;

    // Naming of the vertices of the defining planes
    int const ul_red;
    int const ll_red;
    int const lr_red;
    int const ur_red;

    int const ul_green;
    int const ll_green;
    int const lr_green;
    int const ur_green;

    int const ul_blue;
    int const ll_blue;
    int const lr_blue;
    int const ur_blue;

public:
    Icosahedron() : ScaleFactor(1.0), Translation(0.0, 0.0, 0.0),
		    org_icosahedron(true), golden_ratio((1.0 + std::sqrt(5.0)) / 2.0),
		    ul_red(1),   ll_red(2),   lr_red(3),   ur_red(4),
		    ul_green(5), ll_green(6), lr_green(7), ur_green(8),
		    ul_blue(9),  ll_blue(10), lr_blue(11), ur_blue(12)
    {
	this->MakeVertices();
	this->MakeTriangles();
    }

    Icosahedron(real_type ScaleFactor, vector3_type const& Translation)
	: ScaleFactor(ScaleFactor), Translation(Translation),
		    org_icosahedron(true), golden_ratio((1.0 + std::sqrt(5.0)) / 2.0),
		    ul_red(1),   ll_red(2),   lr_red(3),   ur_red(4),
		    ul_green(5), ll_green(6), lr_green(7), ur_green(8),
		    ul_blue(9),  ll_blue(10), lr_blue(11), ur_blue(12)
    {
	this->MakeVertices();
	this->MakeTriangles();
    }


    virtual ~Icosahedron()
    {}


    bool OriginalIcosahedron() const
    {
	return this->org_icosahedron;
    }


    real_type Radius() const
    {
	return this->radius;
    }


    int NVertices() const
    {
	return this->Vertex_List.size();
    }


    vertex Vertex(int i) const
    {
	if ((i < 1) || (i > this->NVertices() + 1)) {
	    std::ostringstream errormessage;
	    errormessage << "Icosahedron::vertex(" << i 
			 << "): The index i == " << i << " should be in the range [1, " 
			 << this->NVertices() + 1 << "]" << std::ends;
	    throw std::range_error(errormessage.str());
	}

	return this->Vertex_List[i - 1];

    }


    int NNormals() const
    {
	return this->Normal_List.size();
    }


    normal Normal(int i) const
    {
	if ((i < 1) || (i > this->NNormals() + 1)) {
	    std::ostringstream errormessage;
	    errormessage << "Icosahedron::Normal(" << i
			 << "): should be in the range [1, "
			 << this->NVertices() << "]" << std::ends;
	    throw std::range_error(errormessage.str());
	}

	return this->Normal_List[i - 1];
    }


    int NTriangles() const
    {
	return this->Triangle_List.size();
    }


    triangle Triangle(int i)
    {
	if ((i < 0) || (i > this->NTriangles() + 1)) {
	    std::ostringstream errormessage;
	    errormessage << "Icosahedron::Triangle(" << i 
			 << "): should be in the range [1, "
			 << this->NTriangles() << "]" << std::ends;
	    throw std::range_error(errormessage.str());
	}

	//std::vector<vector3_type> triangle;
	triangle triangle = this->Triangle_List[i - 1];
	
        //std::cout << "Not Implemented yet" << std::endl;

	return triangle;
    }


    void PrintVertices() const
    {
	int v = 1;
	for (vertex_list::const_iterator vertex = this->Vertex_List.begin();
	     vertex != this->Vertex_List.end(); ++vertex)
	{
	    std::cout << "PrintVertices(): Vertex[" << std::setw(2) << v << "] == ["
		      << std::setw(8) << *vertex << "]" << std::endl;
	    ++v;
	}
    }


    void PrintTriangles() const
    {
	int t = 1;
	for (triangle_list::const_iterator triang = this->Triangle_List.begin();
	     triang != this->Triangle_List.end(); ++triang)
	{
	    triangle T = *triang;
	    std::cout << "PrintTriangles(): Triangle[" << std::setw(2) << t << "] == [" << std::endl
	    	      << "\t[" << std::setw(8) << T[0] << "]" << std::endl
		      << "\t[" << std::setw(8) << T[1] << "]" << std::endl
		      << "\t[" << std::setw(8) << T[2] << "]" << std::endl
		      << "\tNormal == [" << std::setw(8) << this->Normal(t) << "]" << std::endl
	    << "]" << std::endl;
	    ++t;
	}
    }

    void PrintNormals() const
    {
	int n = 1;
	for (normal_list::const_iterator normal = this->Normal_List.begin();
	     normal != this->Normal_List.end(); ++normal)
	{
	    std::cout << "PrintNormals(): Normal[" << std::setw(2) << n << "] == ["
		      << std::setw(8) << *normal << "]" << std::endl;
	    ++n;
	}
    }
private:
    void MakeVertices()
    {
	// The Red defining plane
	// vertex  1 - ul_red
	this->Vertex_List.push_back(this->make_vertex(vertex(0.0,   1.0,   this->golden_ratio)));

	// vertex  2 - ll_red
	this->Vertex_List.push_back(this->make_vertex(vertex(0.0, - 1.0,   this->golden_ratio)));
	
	// vertex  3 - lr_red
	this->Vertex_List.push_back(this->make_vertex(vertex(0.0, - 1.0, - this->golden_ratio)));

	// vertex  4 - ur_red
	this->Vertex_List.push_back(this->make_vertex(vertex(0.0,   1.0, - this->golden_ratio)));

	// The Green defining plane
	// vertex  5 - ul_green
	this->Vertex_List.push_back(this->make_vertex(vertex(- this->golden_ratio, 0.0, - 1.0)));

	// vertex  6 - ll_green
	this->Vertex_List.push_back(this->make_vertex(vertex(- this->golden_ratio, 0.0,   1.0)));

        // vertex  7 - lr_green
	this->Vertex_List.push_back(this->make_vertex(vertex(  this->golden_ratio, 0.0,   1.0)));
	
        // vertex  8 - ur_green
	this->Vertex_List.push_back(this->make_vertex(vertex(  this->golden_ratio, 0.0, - 1.0)));

	// The Blue defining plane
	// vertex  9 - ul_blue
	this->Vertex_List.push_back(this->make_vertex(- vertex(1.0, - this->golden_ratio, 0.0)));

	// vertex 10 - ll_blue
	this->Vertex_List.push_back(this->make_vertex(- vertex(1.0,   this->golden_ratio, 0.0)));
	
        // vertex 11 - lr_blue
	this->Vertex_List.push_back(this->make_vertex(  vertex(1.0, - this->golden_ratio, 0.0)));

        // vertex 12 - ur_blue
	this->Vertex_List.push_back(this->make_vertex(  vertex(1.0,   this->golden_ratio, 0.0)));
    }


    vertex make_vertex(vertex const& v)
    {
	vertex transformed_vertex = this->ScaleFactor * v + this->Translation;
	return transformed_vertex;
    }


    void MakeTriangles()
    {
	//std::cout << "icosahedron::MakeTriangles(): Under Construction: does not work yet!" << std::endl;

	bool const InvertNormal = true;
	{
	    // Triangle #1
	    vertex v1 = this->Vertex(this->ul_blue);
	    vertex v2 = this->Vertex(this->ul_green);
	    vertex v3 = this->Vertex(this->ll_green);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #2
	    vertex v1 = this->Vertex(this->ul_blue);
	    vertex v2 = this->Vertex(this->ur_red);
	    vertex v3 = this->Vertex(this->ul_green);
	    this->MakeTriangle(v1, v2, v3);
	}  

	{
	    // Triangle #3
	    vertex v1 = this->Vertex(this->ul_green);
	    vertex v2 = this->Vertex(this->ur_red);
	    vertex v3 = this->Vertex(this->lr_red);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #4
	    vertex v1 = this->Vertex(this->ll_green);
	    vertex v2 = this->Vertex(this->ul_green);
	    vertex v3 = this->Vertex(this->ll_blue);
	    this->MakeTriangle(v1, v2, v3);
	}
	
	{
	    // Triangle #5
	    vertex v1 = this->Vertex(this->ll_blue);
	    vertex v2 = this->Vertex(this->ul_green);
	    vertex v3 = this->Vertex(this->lr_red);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #6
	    vertex v1 = this->Vertex(this->ul_blue);
	    vertex v2 = this->Vertex(this->ur_blue);
	    vertex v3 = this->Vertex(this->ur_red);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #7
	    vertex v1 = this->Vertex(this->lr_red);
	    vertex v2 = this->Vertex(this->ur_red);
	    vertex v3 = this->Vertex(this->ur_green);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #8
	    vertex v1 = this->Vertex(this->ur_blue);
	    vertex v2 = this->Vertex(this->ur_green);
	    vertex v3 = this->Vertex(this->ur_red);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #9
	    vertex v1 = this->Vertex(this->ll_green);
	    vertex v2 = this->Vertex(this->ll_blue);
	    vertex v3 = this->Vertex(this->ll_red);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #10
	    vertex v1 = this->Vertex(this->ll_blue);
	    vertex v2 = this->Vertex(this->lr_blue);
	    vertex v3 = this->Vertex(this->ll_red);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #11
	    vertex v1 = this->Vertex(this->ll_blue);
	    vertex v2 = this->Vertex(this->lr_red);
	    vertex v3 = this->Vertex(this->lr_blue);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #12 - identitical to #11
	    vertex v1 = this->Vertex(this->lr_blue);
	    vertex v2 = this->Vertex(this->lr_red);
	    vertex v3 = this->Vertex(this->ur_green);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #13
	    vertex v1 = this->Vertex(this->lr_green);
	    vertex v2 = this->Vertex(this->lr_blue);
	    vertex v3 = this->Vertex(this->ur_green);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #14
	    vertex v1 = this->Vertex(this->lr_green);
	    vertex v2 = this->Vertex(this->ur_green);
	    vertex v3 = this->Vertex(this->ur_blue);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #15
	    vertex v1 = this->Vertex(this->ll_green);
	    vertex v2 = this->Vertex(this->ul_red);
	    vertex v3 = this->Vertex(this->ul_blue);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #16
	    vertex v1 = this->Vertex(this->ll_red);
	    vertex v2 = this->Vertex(this->ul_red);
	    vertex v3 = this->Vertex(this->ll_green);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #17
	    vertex v1 = this->Vertex(this->ll_red);
	    vertex v2 = this->Vertex(this->lr_blue);
	    vertex v3 = this->Vertex(this->lr_green);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #18
	    vertex v1 = this->Vertex(this->ul_red);
	    vertex v2 = this->Vertex(this->ll_red);
	    vertex v3 = this->Vertex(this->lr_green);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #19
	    vertex v1 = this->Vertex(this->ul_blue);
	    vertex v2 = this->Vertex(this->ul_red);
	    vertex v3 = this->Vertex(this->ur_blue);
	    this->MakeTriangle(v1, v2, v3);
	}

	{
	    // Triangle #20
	    vertex v1 = this->Vertex(this->ul_red);
	    vertex v2 = this->Vertex(this->lr_green);
	    vertex v3 = this->Vertex(this->ur_blue);
	    this->MakeTriangle(v1, v2, v3);
	}
    }
	

    void MakeTriangle(vertex const& v1, vertex const& v2, vertex const& v3, bool InvertNormal = false)
    {
	// Make the triangle
	triangle T;
	T.push_back(v1);
	T.push_back(v2);
	T.push_back(v3);
	this->Triangle_List.push_back(T);

	this->radius = Norm(v1);
	//std::cout << "Icosahedron::MakeTriangle::radius(): radius == " << this->Radius() << std::endl;

	// Make the normal of the triangle
	MyMathTypes::vector3_type e1 = v2 - v1;
	MyMathTypes::vector3_type e2 = v3 - v1;
	MyMathTypes::vector3_type N = Cross(e1, e2);
	if (InvertNormal) N = - N;
	if (!Zero(N))
	    N /= Norm(N);
	else {
	    std::cout << "Icosahedron::MakeTriangle(...): Normal == [" << N << "]" << std::endl;
	}
	this->Normal_List.push_back(N);
    }


    // Private variables
    bool      const org_icosahedron;
    real_type const golden_ratio;

    real_type    const ScaleFactor;
    vector3_type const Translation;

    real_type       radius;
    vertex_list     Vertex_List;
    triangle_list   Triangle_List;
    normal_list     Normal_List;
};

#endif
