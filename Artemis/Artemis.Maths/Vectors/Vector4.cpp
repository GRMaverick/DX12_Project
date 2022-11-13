#include "Vector4.h"

namespace Artemis::Maths
{
	Vector4::Vector4( void )
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->w = 0.0f;
	}

	Vector4::Vector4( const float& _xyzw )
	{
		this->x = _xyzw;
		this->y = _xyzw;
		this->z = _xyzw;
		this->w = _xyzw;
	}

	Vector4::Vector4( const Vector4& _rhs )
	{
		this->x = _rhs.x;
		this->y = _rhs.y;
		this->z = _rhs.z;
		this->w = _rhs.w;
	}

	Vector4::Vector4( const float& _x, const float& _y, const float& _z, const float& _w )
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->w = _w;
	}

	Vector4::~Vector4( void )
	{
	}

	Vector4 Vector4::operator+( const Vector4& _rhs ) const
	{
		return Vector4( this->x + _rhs.x, this->y + _rhs.y, this->z + _rhs.z, this->w + _rhs.w );
	}

	Vector4 Vector4::operator-( const Vector4& _rhs ) const
	{
		return Vector4( this->x - _rhs.x, this->y - _rhs.y, this->z - _rhs.z, this->w - _rhs.w );
	}

	Vector4 Vector4::operator*( const float& _scalar ) const
	{
		return Vector4( this->x * _scalar, this->y * _scalar, this->z * _scalar, this->w * _scalar );
	}

	Vector4 Vector4::operator/( const float& _scalar ) const
	{
		return Vector4( this->x / _scalar, this->y / _scalar, this->z / _scalar, this->w / _scalar );
	}

	void Vector4::operator+=( const Vector4& _rhs )
	{
		this->x += _rhs.x;
		this->y += _rhs.y;
		this->z += _rhs.z;
		this->w += _rhs.w;
	}

	void Vector4::operator-=( const Vector4& _rhs )
	{
		this->x -= _rhs.x;
		this->y -= _rhs.y;
		this->z -= _rhs.z;
		this->w -= _rhs.w;
	}

	void Vector4::operator*=( const Vector4& _rhs )
	{
		this->x *= _rhs.x;
		this->y *= _rhs.y;
		this->z *= _rhs.z;
		this->w *= _rhs.w;
	}

	void Vector4::operator/=( const Vector4& _rhs )
	{
		this->x /= _rhs.x;
		this->y /= _rhs.y;
		this->z /= _rhs.z;
		this->w /= _rhs.w;
	}

	float Vector4::Dot( const Vector4& _rhs ) const
	{
		return (float)(this->x * _rhs.x + this->y * _rhs.y + this->z * _rhs.z + this->w * _rhs.w);
	}
}
