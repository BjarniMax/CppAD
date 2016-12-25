% $Id
% -----------------------------------------------------------------------------
% CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-16 Bradley M. Bell
%
% CppAD is distributed under multiple licenses. This distribution is under
% the terms of the
%                     Eclipse Public License Version 1.0.
%
% A copy of this license is included in the COPYING file of this distribution.
% Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
% -----------------------------------------------------------------------------
% load the module
swig_cppad
%
% initialze exit status as OK
error_count = 0;
% ---------------------------------------------
% std::vector<double>
% ---------------------------------------------
ok  = true;
vec = swig_cppad.vector_double(2);
% size
ok  = ok & vec.size() == 2;
% resize
vec.resize(4);
ok  = ok & vec.size() == 4;
% setting elements
for i = 0 : (vec.size()-1)
	vec(i) = 2.0 * i;
end
% getting elements
for i = 0 : (vec.size()-1)
	ok = ok & vec(i) == 2.0 * i;
end
if( ok )
	printf('swig_cppad.vector_double: OK\n')
else
	printf('swig_cppad.vector_double: Error\n')
	error_count = error_count + 1;
end
% --------------------------------------------
% a_double
% --------------------------------------------
ok    = true;
two   = swig_cppad.a_double(2.0);
three = swig_cppad.a_double(3.0);
five  = two + three;
ok    = ok & 4.5 < five.value() & five.value() < 5.5;
six        = two * three;
ok         = ok & 5.5 < six.value() & six.value() < 6.5;
neg_one    = two - three;
ok         = ok & -1.5 < neg_one.value() & neg_one.value() < -0.5;
two_thirds = two / three;
ok         = ok & 0.5 < two_thirds.value() & two_thirds.value() < 1.0;
if( ok )
	printf('swig_cppad.a_double: OK\n')
else
	printf('swig_cppad.a_double: Error\n')
	error_count = error_count + 1;
end
% ---------------------------------------------
% std::vector<a_double>
% ---------------------------------------------
ok    = true;
a_vec = swig_cppad.vector_ad(2);
% size
ok  = ok & a_vec.size() == 2;
% resize
a_vec.resize(4);
ok  = ok & a_vec.size() == 4;
% setting elements
for i = 0 : (a_vec.size()-1)
	a_vec(i) = a_double(2.0 * i);
end
% getting elements
for i = 0 : (a_vec.size()-1)
	ok = ok & a_vec(i).value() == 2.0 * i;
end
if( ok )
	printf('swig_cppad.vector_ad:  OK\n')
else
	printf('swig_cppad.vector_ad:  Error\n')
	error_count = error_count + 1;
end
% --------------------------------------------
% afun(ax, ay)
ok = true;
n  = 2;
m  = 1;
x  = swig_cppad.vector_double(n);
for i = 0 : (n-1)
	x(i) = i + 1;
end
ax     = swig_cppad.independent(x);
ay     = swig_cppad.vector_ad(m);
ay(0)  = ax(0) - ax(1);
ay(0) += ax(0);
af     = swig_cppad.a_fun(ax, ay);
xp     = swig_cppad.vector_double(n);
xp(0)  = 3.0;
xp(1)  = 1.0;
p      = 0;
yp     = af.forward(p, xp);
ok     = ok & yp(0) == 5.0;
xp(0)  = 0.0;
xp(1)  = 1.0;
p      = 1;
yp     = af.forward(p, xp);
ok     = ok & yp(0) == -1.0;
if( ok )
	printf('swig_cppad.a_fun:  OK\n')
else
	printf('swig_cppad.a_fun:  Error\n')
	error_count = error_count + 1;
end
% ---------------------------------------------
% return error_count
exit(error_count)