#ifndef MATH_TYPES_H
#define MATH_TYPES_H
//
// Graphics Framework.
// Copyright (C) 2007 Department of Computer Science, University of Copenhagen
//

#define INSTANTIATE
#  include <matrix/matrix.h>
#undef INSTANTIATE

namespace graphics {

  class MyMathTypes
  {
  public:
      typedef float                         real_type;

      typedef ColumnVector<real_type,2>     vector2_type;
      typedef ColumnVector<real_type,3>     vector3_type;
      typedef ColumnVector<real_type,4>     vector4_type;

      typedef RowVector<real_type,2>        vector2row_type;
      typedef RowVector<real_type,3>        vector3row_type;
      typedef RowVector<real_type,4>        vector4row_type;

      typedef Matrix<real_type, 4, 4>       matrix4x4_type;

      typedef RowVector<vector3_type, 4>    bezier_curve;
      typedef ColumnVector<vector3_type, 4> vectorfwd_type;

      typedef RowVector<vector3_type, 4>    bezier_row;
      typedef ColumnVector<vector3_type, 4> bezier_column;
      typedef Matrix<vector3_type, 4, 4>    bezier_patch;
  };


/*******************************************************************\
*                                                                   *
*   U t i l i t y   O p e r a t o r s   B e z i e r   C u r v e s   *
*                                                                   *
\*******************************************************************/

/*******************************************************************\
*                                                                   *
*          b e z i e r _ c u r v e   *   r e a l _ t y p e          *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_curve operator*(MyMathTypes::bezier_curve const& curve,
					MyMathTypes::real_type    const& factor)
    {
	MyMathTypes::bezier_curve result(curve);
	int dim = curve.Dimension();
	for (int i = 1; i <= dim; ++i) {
	    result[i] *= factor;
	}
	return result;
    }


/*******************************************************************\
*                                                                   *
*          r e a l _ t y p e   *   b e z i e r _ c u r v e          *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_curve operator*(MyMathTypes::real_type    const& factor,
					MyMathTypes::bezier_curve const& curve)
    {
	return curve * factor;
    }


/*******************************************************************\
*                                                                   *
*     b e z i e r _ c u r v e   *   m a t r i x 4 x 4 _ t y p e     *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_curve operator*(MyMathTypes::bezier_curve   const& curve,
					MyMathTypes::matrix4x4_type const& matrix)
    {
	int Ncols = curve.Dimension();
	int Nrows = matrix.Rows();
	MyMathTypes::bezier_row result;

	for (int col = 1; col <= Ncols; ++col) {
	    MyMathTypes::vector3_type Sum;
	    for (int k = 1; k <= Nrows; ++k) {
		Sum += (curve[k] * matrix[k][col]);
	    }
	    result[col] = Sum;
	}

	return result;
    }


/*******************************************************************\
*                                                                   *
*  U t i l i t y   O p e r a t o r s   B e z i e r   P a t c h e s  *
*                                                                   *
\*******************************************************************/

/*******************************************************************\
*                                                                   *
*          b e z i e r _ p a t c h   *   r e a l _ t y p e          *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_patch operator*(MyMathTypes::bezier_patch const& G,
					MyMathTypes::real_type    const& Factor)
    {
	MyMathTypes::bezier_patch result;
	for (int row = 1; row <= 4; ++row) {
	    for (int col = 1; col <= 4; ++col) {
		result[row][col] = G[row][col] * Factor;
	    }
	}
	return result;
    }


/*******************************************************************\
*                                                                   *
*          r e a l _ t y p e   *   b e z i e r _ p a t c h          *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_patch operator*(MyMathTypes::real_type    const& Factor,
					MyMathTypes::bezier_patch const& G)
    {
	return G * Factor;
    }


/*******************************************************************\
*                                                                   *
*    v e c t o r 4 r o w _ t y p e   *   b e z i e r _ p a t c h    *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_row operator*(MyMathTypes::vector4row_type const& Row,
				      MyMathTypes::bezier_patch    const& G)
    {
	int Ncols = Row.Dimension();
	int Nrows = G.Rows();
	MyMathTypes::bezier_row result;

	for (int col = 1; col <= Ncols; ++col) {
	    MyMathTypes::vector3_type Entry;
	    for (int k = 1; k <= Nrows; ++k) {
		Entry += Row[k] * G[k][col];
	    }
	    result[col] = Entry;
	}

	return result;
    }


/*******************************************************************\
*                                                                   *
*       b e z i e r _ p a t c h   *   v e c t o r 4 _ t y p e       *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_column operator*(MyMathTypes::bezier_patch const& G,
					 MyMathTypes::vector4_type const& Col)
    {
	int Ncols = G.Columns();
	int Nrows = Col.Dimension();
	MyMathTypes::bezier_column result;

	for (int row = 1; row <= Ncols; ++row) {
	    MyMathTypes::vector3_type Entry;
	    for (int k = 1; k <= Nrows; ++k) {
		Entry +=G[row][k] * Col[k];
	    }
	    result[row] = Entry;
	}

	return result;
    }


/*******************************************************************\
*                                                                   *
*     m a t r i x 4 x 4 _ t y p e   *   b e z i e r _ p a t c h     *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_patch operator*(MyMathTypes::matrix4x4_type const& M,
					MyMathTypes::bezier_patch   const& G)
    {
	int Nrows = M.Rows();
	int Ncols = G.Columns();
	MyMathTypes::bezier_patch result;

	for (int row = 1; row <= Nrows; ++row) {
	    for (int col = 1; col <= Ncols; ++col) {
		MyMathTypes::vector3_type Entry;
		for (int k = 1; k <= Nrows; ++k) {
		    Entry += M[row][k] * G[k][col];
		}
		result[row][col] = Entry;
	    }
	}

	return result;
    }


/*******************************************************************\
*                                                                   *
*     b e z i e r _ p a t c h   *   m a t r i x 4 x 4 _ t y p e     *
*                                                                   *
\*******************************************************************/

    MyMathTypes::bezier_patch operator*(MyMathTypes::bezier_patch   const& G,
					MyMathTypes::matrix4x4_type const& M)
    {


	// Here!
	int Nrows = M.Rows();
	int Ncols = G.Columns();
	MyMathTypes::bezier_patch result;

	for (int row = 1; row <= Nrows; ++row) {
	    for (int col = 1; col <= Ncols; ++col) {
		MyMathTypes::vector3_type Entry;
		for (int k = 1; k <= Nrows; ++k) {
		    Entry += G[row][k] * M[k][col];
		}
		result[row][col] = Entry;
	    }
	}

	return result;
    }

}// end namespace graphics

// MATH_TYPES_H
#endif
