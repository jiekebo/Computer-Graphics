#ifndef PHONGSURFACE_H
#define PHONGSURFACE_H

#include "solution/math_types.h"
#include "solution/transformations.h"


/*******************************************************************\
*                                                                   *
*                      P h o n g S u r f a c e                      *
*                                                                   *
\*******************************************************************/

class PhongSurface {
public:
    typedef MyMathTypes::real_type    real_type;
    typedef MyMathTypes::vector3_type vector3_type;

public:
    PhongSurface()
    {
	this->ScaleFactor = 1.0;

	this->I_a = 0.2;
	this->k_a = 0.0;
	this->O_a = 0.2;
    
	this->I_p = 1.0;
	this->k_d = 0.5;
	this->O_d = 1.0;

	this->k_s = 0.5;
	this->O_s = 1.0;
	this->exponent = 20.0;

	this->N_local = vector3_type(0, 0, 1);
	if (!Zero(N_local)) N_local /= Norm(N_local);

	this->L_local = vector3_type(-0.8, -0.5, 1);
	if (!Zero(L_local)) L_local /= Norm(L_local);

	this->NdotL_local = Dot(this->N_local, this->L_local);

	this->R_local = this->N_local * Dot(this->N_local, this->L_local) * 2.0 - this->L_local;
	if (!Zero(this->R_local)) this->R_local /= Norm(this->R_local);	
    }

    virtual ~PhongSurface()
    {}

    void init(real_type const& ScaleFactor,
	      real_type const& I_a,
	      real_type const& k_a,
	      real_type const& O_a,
	      real_type const& I_p,
	      real_type const& k_d,
	      real_type const& O_d,
	      real_type const& k_s,
	      real_type const& O_s,
	      real_type const& exponent,
	      vector3_type const& N,
	      vector3_type const& L)
    {
	this->ScaleFactor = ScaleFactor;

	this->I_a = I_a;
	this->k_a = k_a;
	this->O_a = O_a;
    
	this->I_p = I_p;
	this->k_d = k_d;
	this->O_d = O_d;

	this->k_s = k_s;
	this->O_s = O_s;
	this->exponent = exponent;

	this->N_local = N;
	if (!Zero(N_local)) N_local /= Norm(N_local);

	this->L_local = L;
	if (!Zero(L_local)) L_local /= Norm(L_local);

	this->NdotL_local = Dot(this->N_local, this->L_local);

	this->R_local = this->N_local * Dot(this->N_local, this->L_local) * 2.0 - this->L_local;
	if (!Zero(this->R_local)) this->R_local /= Norm(this->R_local);
    }


    vector3_type Vertex(real_type const& phi,real_type const&  theta) const
    {
	vector3_type vertex;
	vertex = this->V(phi, theta) * this->P(phi, theta);

	return this->ScaleFactor * vertex;
    }


    vector3_type Normal(real_type const& phi, real_type const& theta) const
    {
	// With this definition of the surface, normals points inside the surface.
        // Multiply with -1 to make them point outward.

	vector3_type Dphi = this->dPdphi(phi, theta) * this->V(phi, theta)
	                    + this->P(phi, theta) * this->dVdphi(phi, theta);

	vector3_type Dtheta = this->dPdtheta(phi, theta) * this->V(phi, theta)
	                      + this->P(phi, theta) * this->dVdtheta(phi, theta);

	vector3_type normal = Cross(Dphi, Dtheta);

	//std::cout << " ";
	if (Zero(normal))
	    std::cout << "PhongSurface::Normal(" << phi << ", " << theta << "): Normal is zero." << std::endl;
	else {
	    normal /= Norm(normal);
	    //std::cout << "Normal = [" << normal << "]" << std::endl;
	}

	return - normal;
    }


private:

/*******************************************************************\
*                                                                   *
*                 P r i v a t e   F u n c t i o n s                 *
*                                                                   *
\*******************************************************************/

    vector3_type V(real_type const& phi, real_type const& theta) const
    {
	vector3_type point;
	point[1] = cos(phi) * cos(theta);
	point[2] = cos(phi) * sin(theta);
	point[3] = sin(phi);

	return point;
    }

    vector3_type dVdphi(real_type const& phi, real_type const& theta) const
    {
	vector3_type vector;
	vector[1] = - sin(phi) * cos(theta);
	vector[2] = - sin(phi) * sin(theta);
	vector[3] =   cos(phi);

	return vector;
    }

    vector3_type dVdtheta(real_type const& phi, real_type const& theta) const
    {
	vector3_type vector;
	vector[1] = - cos(phi) * sin(theta);
	vector[2] =   cos(phi) * cos(theta);
	vector[3] =   0.0;

	return vector;
    }

    real_type P(real_type const& phi, real_type const& theta) const
    {
	real_type value;
	//real_type NdotL = Dot(this->N_local, this->L_local);
	
	vector3_type V_local = this->V(phi, theta);
	if (!Zero(V_local)) V_local /= Norm(V_local);
	real_type RdotV = Dot(this->R_local, V_local);

	value = this->k_a * this->O_a * this->I_a;
	value += this->I_p * (this->k_d * this->O_d * this->NdotL_local);
	value += this->k_s * this->O_s * pow(RdotV, this->exponent) * this->I_p;

	return value;
			
    }

    real_type dPdphi(real_type const& phi, real_type const& theta) const
    {
	real_type value;
	real_type RdotV = Dot(this->R_local, this->V(phi, theta));

	value = this->I_p * this->k_s * this->O_s
	    * this->exponent * static_cast<real_type>(pow(RdotV, this->exponent - 1))
	    * Dot(this->R_local, this->dVdphi(phi, theta));

	return value;
    }

    real_type dPdtheta(real_type const& phi, real_type const& theta) const
    {
	real_type    value;
	real_type RdotV(Dot(this->R_local, this->V(phi, theta)));
	
	value = this->I_p * this->k_s * this->O_s
	    * this->exponent * static_cast<real_type>(pow(RdotV, this->exponent - 1))
	    * Dot(this->R_local, this->dVdtheta(phi, theta));

	return value;		   
    }

    real_type Clamp(real_type const& value) const
    {
	real_type result = value;
	if (value < 0.0) result = 0.0;
	if (value > 1.0) result = 1.0;
    
	return result;
    }




/*******************************************************************\
*                                                                   *
*                 P r i v a t e   V a r i a b l e s                 *
*                                                                   *
\*******************************************************************/

    real_type ScaleFactor;

    real_type I_a;
    real_type k_a;
    real_type O_a;
    
    real_type I_p;
    real_type k_d;
    real_type O_d;

    real_type k_s;
    real_type O_s;
    real_type exponent;

    vector3_type N_local;
    vector3_type L_local;

    real_type    NdotL_local;

    vector3_type R_local;
};

#endif
