#pragma once
#include "real_number.h"

class Complex
{
public:
	Real r, i;

public:
	Complex();
	Complex(Real r_val, Real i_val);
	Complex operator*(Complex& rhs) const;
	Complex square() const;
};
