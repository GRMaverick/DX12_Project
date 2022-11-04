#include "Vector3.h"
#include <algorithm>

namespace Artemis::Maths
{
	Vector3::Vector3( void )
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	Vector3::Vector3( const float& _xyz )
	{
		this->x = _xyz;
		this->y = _xyz;
		this->z = _xyz;
	}

	Vector3::Vector3( const Vector3& _rhs )
	{
		this->x = _rhs.x;
		this->y = _rhs.y;
		this->z = _rhs.z;
	}

	Vector3::Vector3( const float& _x, const float& _y, const float& _z )
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}

	Vector3::~Vector3( void )
	{
	}

	bool Vector3::IsNormalised( void ) const
	{
		return Magnitude() == 1.0f;
	}

	Vector3 Vector3::operator+( const Vector3& _rhs ) const
	{
		return Vector3( this->x + _rhs.x, this->y + _rhs.y, this->z + _rhs.z );
	}

	Vector3 Vector3::operator-( const Vector3& _rhs ) const
	{
		return Vector3( this->x - _rhs.x, this->y - _rhs.y, this->z - _rhs.z );
	}

	Vector3 Vector3::operator*( const float& _scalar ) const
	{
		return Vector3( this->x * _scalar, this->y * _scalar, this->z * _scalar );
	}

	Vector3 Vector3::operator/( const float& _scalar ) const
	{
		return Vector3( this->x / _scalar, this->y / _scalar, this->z / _scalar );
	}

	void Vector3::operator+=( const Vector3& _rhs )
	{
		this->x += _rhs.x;
		this->y += _rhs.y;
		this->z += _rhs.z;
	}

	void Vector3::operator-=( const Vector3& _rhs )
#
	{
		this->x -= _rhs.x;
		this->y -= _rhs.y;
		this->z -= _rhs.z;
	}

	void Vector3::operator*=( const Vector3& _rhs )
	{
		this->x *= _rhs.x;
		this->y *= _rhs.y;
		this->z *= _rhs.z;
	}

	void Vector3::operator/=( const Vector3& _rhs )
	{
		this->x /= _rhs.x;
		this->y /= _rhs.y;
		this->z /= _rhs.z;
	}

	float Vector3::Dot( const Vector3& _rhs ) const
	{
		return (float)(this->x * _rhs.x + this->y * _rhs.y + this->z * _rhs.z);
	}

	float Vector3::AngleInRadians( const Vector3& _rhs ) const
	{
		return acos( this->Dot( _rhs ) / (this->Magnitude() * _rhs.Magnitude()) );
	}

	float Vector3::AngleInDegrees( const Vector3& _rhs ) const
	{
		return acos( this->Dot( _rhs ) / (this->Magnitude() * _rhs.Magnitude()) ) * (180 / 3.14159265359);
	}

	float Vector3::Magnitude( void ) const
	{
		return floorf( sqrt( this->x * this->x + this->y * this->y + this->z * this->z ) * 100000 + 0.5 ) / 100000;
	}

	Vector3 Vector3::Negate( void ) const
	{
		return Vector3( -this->x, -this->y, -this->z );
	}

	Vector3 Vector3::Normalise( void ) const
	{
		return Vector3( this->x / Magnitude(), this->y / Magnitude(), this->z / Magnitude() );
	}

	Vector3 Vector3::Cross( const Vector3& _rhs ) const
	{
		return Vector3( this->y * _rhs.z - this->z * _rhs.y, this->z * _rhs.x - this->x * _rhs.z, this->x * _rhs.y - this->y * _rhs.x );
	}
}
