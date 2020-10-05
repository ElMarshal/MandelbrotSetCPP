#pragma once
#include "real_number.h"

class Complex
{
public:
	Real r, i;

public:
	Complex();
	Complex(Real r_val, Real i_val);

	Complex operator+(const Complex& rhs);
	Complex operator*(const Complex& rhs) const;
	Complex square() const;
	Real length() const;
	Real length_squared() const;
};
