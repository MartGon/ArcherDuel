#pragma once
#include <math.h>

template <typename T>
class Vector2
{
public:
	Vector2()
	{
		x = 0;
		y = 0;
	}

	Vector2(T x, T y)
	{
		this->x = x;
		this->y = y;
	}

	~Vector2()
	{

	}

	// Coordinates
	T x;
	T y;

	// Methods
	void normalize()
	{
		float mod = getModule();
		x /= mod;
		y /= mod;
	}

	float getModule()
	{
		return sqrt(x * x + y * y);
	}

	friend Vector2<T>& operator+(const Vector2<T> &v1, const Vector2<T> &v2)
	{
		T fX = v1.x + v2.x;
		T fY = v1.y + v2.y;

		return Vector2(fX, fY);
	}

	template <typename L>
	friend Vector2<L>& operator*(const Vector2<L> &vector, const T &value)
	{
		L sX = vector.x * value;
		L sY = vector.y * value;

		return Vector2<L>(sX, sY);
	}
};

// Operators