/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef BASE_VMATH_H
#define BASE_VMATH_H

#include "math.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

typedef glm::vec2 vec2;
// ------------------------------------

template<typename T>
class vector2_base
{
public:
	union
	{
		T x, u;
	};
	union
	{
		T y, v;
	};

	vector2_base() {}
	vector2_base(T nx, T ny)
	{
		x = nx;
		y = ny;
	}

	vector2_base operator-() const { return vector2_base(-x, -y); }
	vector2_base operator-(const vector2_base &v) const { return vector2_base(x - v.x, y - v.y); }
	vector2_base operator+(const vector2_base &v) const { return vector2_base(x + v.x, y + v.y); }
	vector2_base operator*(const T v) const { return vector2_base(x * v, y * v); }
	vector2_base operator*(const vector2_base &v) const { return vector2_base(x * v.x, y * v.y); }
	vector2_base operator/(const T v) const { return vector2_base(x / v, y / v); }
	vector2_base operator/(const vector2_base &v) const { return vector2_base(x / v.x, y / v.y); }

	const vector2_base &operator+=(const vector2_base &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	const vector2_base &operator-=(const vector2_base &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	const vector2_base &operator*=(const T v)
	{
		x *= v;
		y *= v;
		return *this;
	}
	const vector2_base &operator*=(const vector2_base &v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
	const vector2_base &operator/=(const T v)
	{
		x /= v;
		y /= v;
		return *this;
	}
	const vector2_base &operator/=(const vector2_base &v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	bool operator==(const vector2_base &v) const { return x == v.x && y == v.y; } //TODO: do this with an eps instead
	bool operator!=(const vector2_base &v) const { return x != v.x || y != v.y; }

	operator const T *() { return &x; }

	T &operator[](const int index) { return index ? y : x; }
};

typedef vector2_base<bool> bvec2;
typedef vector2_base<int> ivec2;

inline float length(const vec2 &a)
{
	return glm::sqrt(a.x * a.x + a.y * a.y);
}

inline float distance(const vec2 &a, const vec2 &b)
{
	return glm::length(a - b);
}

inline vec2 normalize(const vec2 &v)
{
	vec2 vtmp = glm::normalize(v);
	return vtmp;
}

template<typename T>
inline T dot(const vec2 a, const vec2 &b)
{
	return a.x * b.x + a.y * b.y;
}

template<typename T>
inline vec2 normalize_pre_length(const vec2 &v, T len)
{
	return vec2(v.x / len, v.y / len);
}

template<typename T>
inline bool closest_point_on_line(vec2 line_point0, vec2 line_point1, vec2 target_point, vec2 &out_pos)
{
	vec2 c = target_point - line_point0;
	vec2 v = (line_point1 - line_point0);
	T d = length(line_point0 - line_point1);
	if(d > 0)
	{
		v = normalize_pre_length<T>(v, d);
		T t = dot(v, c) / d;
		out_pos = lerp(line_point0, line_point1, clamp(t, (T)0, (T)1));
		return true;
	}
	else
		return false;
}

// ------------------------------------
template<typename T>
class vector3_base
{
public:
	union
	{
		T x, r, h;
	};
	union
	{
		T y, g, s;
	};
	union
	{
		T z, b, v, l;
	};

	vector3_base() {}
	vector3_base(T nx, T ny, T nz)
	{
		x = nx;
		y = ny;
		z = nz;
	}

	vector3_base operator-(const vector3_base &v) const { return vector3_base(x - v.x, y - v.y, z - v.z); }
	vector3_base operator-() const { return vector3_base(-x, -y, -z); }
	vector3_base operator+(const vector3_base &v) const { return vector3_base(x + v.x, y + v.y, z + v.z); }
	vector3_base operator*(const T v) const { return vector3_base(x * v, y * v, z * v); }
	vector3_base operator*(const vector3_base &v) const { return vector3_base(x * v.x, y * v.y, z * v.z); }
	vector3_base operator/(const T v) const { return vector3_base(x / v, y / v, z / v); }
	vector3_base operator/(const vector3_base &v) const { return vector3_base(x / v.x, y / v.y, z / v.z); }

	const vector3_base &operator+=(const vector3_base &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	const vector3_base &operator-=(const vector3_base &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	const vector3_base &operator*=(const T v)
	{
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}
	const vector3_base &operator*=(const vector3_base &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	const vector3_base &operator/=(const T v)
	{
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}
	const vector3_base &operator/=(const vector3_base &v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	bool operator==(const vector3_base &v) const { return x == v.x && y == v.y && z == v.z; } //TODO: do this with an eps instead

	operator const T *() { return &x; }
};

template<typename T>
inline T length(const vector3_base<T> &a)
{
	return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

template<typename T>
inline vec2 rotate(const vec2 &a, T angle)
{
	angle = angle * pi / 180.0f;
	T s = sinf(angle);
	T c = cosf(angle);
	return vec2((T)(c * a.x - s * a.y), (T)(s * a.x + c * a.y));
}

template<typename T>
inline T distance(const vector3_base<T> &a, const vector3_base<T> &b)
{
	return length(a - b);
}

template<typename T>
inline T dot(const vector3_base<T> &a, const vector3_base<T> &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
inline vector3_base<T> normalize(const vector3_base<T> &v)
{
	T l = (T)(1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z));
	return vector3_base<T>(v.x * l, v.y * l, v.z * l);
}

template<typename T>
inline vector3_base<T> cross(const vector3_base<T> &a, const vector3_base<T> &b)
{
	return vector3_base<T>(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

typedef vector3_base<float> vec3;
typedef vector3_base<bool> bvec3;
typedef vector3_base<int> ivec3;

// ------------------------------------

template<typename T>
class vector4_base
{
public:
	union
	{
		T x, r, h;
	};
	union
	{
		T y, g, s;
	};
	union
	{
		T z, b, l;
	};
	union
	{
		T w, a;
	};

	vector4_base() {}
	vector4_base(T nx, T ny, T nz, T nw)
	{
		x = nx;
		y = ny;
		z = nz;
		w = nw;
	}

	vector4_base operator+(const vector4_base &v) const { return vector4_base(x + v.x, y + v.y, z + v.z, w + v.w); }
	vector4_base operator-(const vector4_base &v) const { return vector4_base(x - v.x, y - v.y, z - v.z, w - v.w); }
	vector4_base operator-() const { return vector4_base(-x, -y, -z, -w); }
	vector4_base operator*(const vector4_base &v) const { return vector4_base(x * v.x, y * v.y, z * v.z, w * v.w); }
	vector4_base operator*(const T v) const { return vector4_base(x * v, y * v, z * v, w * v); }
	vector4_base operator/(const vector4_base &v) const { return vector4_base(x / v.x, y / v.y, z / v.z, w / v.w); }
	vector4_base operator/(const T v) const { return vector4_base(x / v, y / v, z / v, w / v); }

	const vector4_base &operator+=(const vector4_base &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	const vector4_base &operator-=(const vector4_base &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	const vector4_base &operator*=(const T v)
	{
		x *= v;
		y *= v;
		z *= v;
		w *= v;
		return *this;
	}
	const vector4_base &operator*=(const vector4_base &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}
	const vector4_base &operator/=(const T v)
	{
		x /= v;
		y /= v;
		z /= v;
		w /= v;
		return *this;
	}
	const vector4_base &operator/=(const vector4_base &v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	bool operator==(const vector4_base &v) const { return x == v.x && y == v.y && z == v.z && w == v.w; } //TODO: do this with an eps instead

	operator const T *() { return &x; }
};

typedef vector4_base<float> vec4;
typedef vector4_base<bool> bvec4;
typedef vector4_base<int> ivec4;

#endif
