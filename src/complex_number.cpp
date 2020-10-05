#include "complex_number.h"

Complex::Complex() :
	r(0.0), i(0.0)
{
}

Complex::Complex(Real r_val, Real i_val) :
	r(r_val), i(i_val)
{
}

Complex Complex::operator*(Complex& rhs) const
{
	return Complex{(r*rhs.r - i*rhs.i), (r*rhs.i + i*rhs.r)};
}

Complex Complex::square() const
{
	return Complex{ (r*r - i*i), (2.0*i*r) };
}
