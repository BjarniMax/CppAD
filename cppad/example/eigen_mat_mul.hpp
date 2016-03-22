// $Id$
# ifndef CPPAD_EXAMPLE_EIGEN_MAT_MUL_HPP
# define CPPAD_EXAMPLE_EIGEN_MAT_MUL_HPP

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-16 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

/*
$begin atomic_eigen_mat_mul.hpp$$
$spell
	Eigen
$$

$section Atomic Eigen Matrix Multiply Class$$

$nospell

$head Start Class Definition$$
$srccode%cpp% */
# include <cppad/cppad.hpp>
# include <Eigen/Core>


/* %$$
$head Publice Types$$
$srccode%cpp% */
namespace { // BEGIN_EMPTY_NAMESPACE
//
template <class Base>
class atomic_eigen_mat_mul : public CppAD::atomic_base<Base> {
public:
	// -----------------------------------------------------------
	// type of elements during calculation of derivatives
	typedef Base              scalar;
	// type of elements during taping
	typedef CppAD::AD<scalar> ad_scalar;
	// type of matrix during calculation of derivatives
	typedef Eigen::Matrix<scalar, Eigen::Dynamic, Eigen::Dynamic>    matrix;
	// type of matrix during taping
	typedef Eigen::Matrix<ad_scalar, Eigen::Dynamic, Eigen::Dynamic> ad_matrix;
/* %$$
$head Public Constructor$$
$srccode%cpp% */
	// constructor
	atomic_eigen_mat_mul(
		// number of rows in left operand
		const size_t nrow_left  ,
		// number of rows in left and columns in right operand
		const size_t n_middle   ,
		// number of columns in right operand
		const size_t ncol_right
	) :
	CppAD::atomic_base<Base>(
		"atom_eigen_mat_mul"                             ,
		CppAD::atomic_base<Base>::set_sparsity_enum
	) ,
	nrow_left_(  nrow_left  )                   ,
	n_middle_(   n_middle   )                   ,
	ncol_right_( ncol_right )                   ,
	nx_( (nrow_left + ncol_right) * n_middle_ ) ,
	ny_( nrow_left * ncol_right )
	{ }
/* %$$
$head Public Pack$$
$srccode%cpp% */
	template <class Matrix, class Vector>
	void pack(
		Vector&        packed  ,
		const Matrix&  left    ,
		const Matrix&  right   )
	{	assert( packed.size() == nx_      );
		assert( rows( left ) == nrow_left_ );
		assert( cols( left ) == n_middle_ );
		assert( rows( right ) == n_middle_ );
		assert( cols( right ) == ncol_right_ );
		//
		size_t index = 0;
		for(size_t i = 0; i < nrow_left_; i++)
		{	for(size_t j = 0; j < n_middle_; j++)
			{	packed[ index ] = left(i, j);
				++index;
			}
		}
		for(size_t i = 0; i < n_middle_; i++)
		{	for(size_t j = 0; j < ncol_right_; j++)
			{	packed[ index ] = right(i, j);
				++index;
			}
		}
		assert( index == nx_ );
		return;
	}
/* %$$
$head Public Unpack$$
$srccode%cpp% */
	template <class Matrix, class Vector>
	void unpack(
		const Vector&   packed  ,
		Matrix&         result  )
	{	assert( packed.size() == ny_      );
		assert( rows( result ) == nrow_left_ );
		assert( cols( result ) == ncol_right_ );
		//
		size_t index = 0;
		for(size_t i = 0; i < nrow_left_; i++)
		{	for(size_t j = 0; j < ncol_right_; j++)
			{	result(i, j) = packed[ index ];
				++index;
			}
		}
		assert( index == ny_ );
		return;
	}
/* %$$
$head Private Variables$$
$srccode%cpp% */
private:
	// -------------------------------------------------------------
	// number of of rows in left operand and result
	const size_t nrow_left_;
	// number of of columns in left operand and rows in right operand
	const size_t n_middle_;
	// number of columns in right operand and result
	const size_t ncol_right_;
	// size of the domain space
	const size_t nx_;
	// size of the range space
	const size_t ny_;
	// -------------------------------------------------------------
	// one matrix for each order for left operand
	CppAD::vector<matrix> left_;
	// one matrix for each order for right operand
	CppAD::vector<matrix> right_;
	// one matrix for each for the result operand
	CppAD::vector<matrix> result_;
	// -------------------------------------------------------------
/* %$$
$head Private rows$$
$srccode%cpp% */
	// convert from int to size_t
	static size_t rows(const matrix& x)
	{	return size_t( x.rows() ); }
	static size_t rows(const ad_matrix& x)
	{	return size_t( x.rows() ); }
/* %$$
$head Private cols$$
$srccode%cpp% */
	// convert from int to size_t
	static size_t cols(const matrix& x)
	{	return size_t( x.cols() ); }
	static size_t cols(const ad_matrix& x)
	{	return size_t( x.cols() ); }
/* %$$
$head Private Unpack$$
$srccode%cpp% */
	void unpack(
		size_t                       n_order ,
		const CppAD::vector<scalar>& packed  )
	{	assert( packed.size() == n_order * nx_ );

		// check size of return values
		assert( left_.size() == right_.size() );
		assert( left_.size() == result_.size() );
		if( left_.size() < n_order )
		{	left_.resize(n_order);
			right_.resize(n_order);
			result_.resize(n_order);
			//
			for(size_t k = 0; k < n_order; k++)
			{	left_[k].resize(nrow_left_, n_middle_);
				right_[k].resize(n_middle_, ncol_right_);
				result_[k].resize(nrow_left_, ncol_right_);
			}
		}
		// set the return values
		for(size_t k = 0; k < n_order; k++)
		{	// unpack left values for this order
			assert( rows( left_[k] ) == nrow_left_ );
			assert( cols( left_[k] ) == n_middle_ );
			size_t index = 0;
			for(size_t i = 0; i < nrow_left_; i++)
			{	for(size_t j = 0; j < n_middle_; j++)
				{	left_[k](i, j) = packed[ index * n_order + k ];
					++index;
				}
			}
			// unpack right values for this order
			assert( rows( right_[k] ) == n_middle_ );
			assert( cols( right_[k] ) == ncol_right_ );
			for(size_t i = 0; i < n_middle_; i++)
			{	for(size_t j = 0; j < ncol_right_; j++)
				{	right_[k](i, j) = packed[ index * n_order + k ];
					++index;
				}
			}
			assert( index == nx_ );
		}
		return;
	}
/* %$$
$head Private Pack$$
$srccode%cpp% */
	void pack(
		size_t                  n_order ,
		CppAD::vector<scalar>&  packed  )
	{	assert( packed.size() == n_order * ny_ );
		assert( result_.size() >= n_order );
		//
		for(size_t k = 0; k < n_order; k++)
		{	assert( rows( result_[k] ) == nrow_left_ );
			assert( cols( result_[k] ) == ncol_right_ );
			size_t index = 0;
			for(size_t i = 0; i < nrow_left_; i++)
			{	for(size_t j = 0; j < ncol_right_; j++)
				{	packed[ index * n_order + k ] = result_[k](i, j);
					++index;
				}
			}
			assert( index == ny_ );
		}
		return;
	}
/* %$$
$head Private forward$$
$srccode%cpp% */
	// forward mode routine called by CppAD
	virtual bool forward(
		// lowest order Taylor coefficient we are evaluating
		size_t                          p ,
		// highest order Taylor coefficient we are evaluating
		size_t                          q ,
		// which components of x are variables
		const CppAD::vector<bool>&      vx ,
		// which components of y are variables
		CppAD::vector<bool>&            vy ,
		// tx [ j * (q+1) + k ] is x_j^k
		const CppAD::vector<scalar>&    tx ,
		// ty [ i * (q+1) + k ] is y_i^k
		CppAD::vector<scalar>&          ty
	)
	{	size_t n_order = q + 1;
		assert( vx.size() == 0 || nx_ == vx.size() );
		assert( vx.size() == 0 || ny_ == vy.size() );
		assert( nx_ * n_order == tx.size() );
		assert( ny_ * n_order == ty.size() );

		// unpack tx into left_ and right_
		unpack(n_order, tx);

		// result for each order
		for(size_t k = 0; k < n_order; k++)
		{	result_[k] = matrix::Zero(nrow_left_, ncol_right_);
			for(size_t ell = 0; ell <= k; ell++)
			{	result_[k] += left_[ell] * right_[k-ell];
			}
		}

		// pack result_ into ty
		pack(n_order, ty);

		// check if we are compute vy
		if( vx.size() == 0 )
			return true;

		// compute variable information for y; i.e., vy
		scalar zero(0.0);
		assert( n_order == 1 );
		for(size_t i = 0; i < nrow_left_; i++)
		{	for(size_t j = 0; j < ncol_right_; j++)
			{	bool var = false;
				for(size_t ell = 0; ell < n_middle_; ell++)
				{	size_t index   = i * n_middle_ + ell;
					bool var_left  = vx[index];
					bool nz_left   = var_left | (left_[0](i, ell) != zero);
					index          = nrow_left_ * n_middle_;
					index         += ell * ncol_right_ + j;
					bool var_right = vx[index];
					bool nz_right  = var_right | (right_[0](ell, j) != zero);
					var |= var_left & nz_right;
					var |= nz_left  & var_right;
				}
				size_t index = i * ncol_right_ + j;
				vy[index]    = var;
			}
		}
		return true;
	}
/* %$$
$head End Class Definition$$
$srccode%cpp% */
}; // End of atomic_eigen_mat_mul class

}  // END_EMPTY_NAMESPACE
/* %$$
$$ $comment end nospell$$
$end
*/


# endif