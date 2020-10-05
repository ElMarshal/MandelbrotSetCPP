#include "vector2.h"

Vector2::Vector2() :
	x(0.0), y(0.0)
{
}

Vector2::Vector2(Real x_val, Real y_val) :
	x(x_val), y(y_val)
{
}

Vector2 Vector2::operator+(Vector2& rhs)
{
	return Vector2{ x+rhs.x, y+rhs.y };
}
