#pragma once
#include "real_number.h"

class Vector2
{
public:
	Real x, y;

public:
	Vector2();
	Vector2(Real x_val, Real y_val);
	Vector2 operator+(const Vector2& rhs);
};
