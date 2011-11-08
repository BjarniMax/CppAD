// $Id$
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-11 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/* 
$begin openmp_team.cpp$$
$spell
	openmp
$$

$index openmp, AD team$$
$index AD, openmp team$$
$index team, AD openmp$$

$section OpenMP Implementation of a Team of AD Threads$$
See $cref thread_team.hpp$$ for this routines specifications.

$code
$verbatim%multi_thread/openmp/openmp_team.cpp%0%// BEGIN PROGRAM%// END PROGRAM%1%$$
$$

$end
*/
// BEGIN PROGRAM
# include <omp.h>
# include <cppad/cppad.hpp>
# include "../thread_team.hpp"

namespace {
	// number of threads in this team
	size_t num_threads_;

	// used to inform CppAD when we are in parallel execution mode
	bool in_parallel(void)
	{	return static_cast<bool> ( omp_in_parallel() ); }

	// used to inform CppAD of the current thread number
	size_t thread_num(void)
	{	return static_cast<size_t>( omp_get_thread_num() ); } 
}

bool start_team(size_t num_threads)
{
	bool ok = ! in_parallel();
	ok     &= thread_num() == 0;;
	ok     &= num_threads > 0;

	// Turn off dynamic thread adjustment
	omp_set_dynamic(0);

	// Set the number of OpenMP threads
	omp_set_num_threads( int(num_threads) );

	// setup for using CppAD::AD<double> in parallel 
	CppAD::thread_alloc::parallel_setup(num_threads, in_parallel, thread_num);
	CppAD::parallel_ad<double>();

	// inform work_team of number of threads
	num_threads_ = num_threads;

	return ok;
}

bool work_team(void worker(void))
{	bool ok = ! in_parallel();
	ok     &= thread_num() == 0;;
	ok     &= num_threads_ > 0;

	int number_threads = int(num_threads_);
	int thread_num;
# pragma omp parallel for
	for(thread_num = 0; thread_num < number_threads; thread_num++)
		worker();
// end omp parallel for	

	return ok;
}

bool stop_team(void)
{	bool ok = ! in_parallel();
	ok     &= thread_num() == 0;;
	ok     &= num_threads_ > 0;

	// inform work_team of number of threads
	num_threads_ = 1;

	// Set the number of OpenMP threads to one
	omp_set_num_threads(num_threads_);

	// inform CppAD no longer in multi-threading mode
	using CppAD::thread_alloc;
	thread_alloc::parallel_setup(num_threads_, in_parallel, thread_num);

	return ok;
}

const char* name_team(void)
{	return "openmp"; }
// END PROGRAM
