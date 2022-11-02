#include "Vector3.h"
#include <algorithm>

namespace Artemis::Maths
{
	Vector3::Vector3( void )
	{
		this->m_x            = 0.0f;
		this->m_y            = 0.0f;
		this->m_z            = 0.0f;
		this->m_IsNormalised = false;
	}

	Vector3::Vector3( const float& _xyz )
	{
		this->m_x            = _xyz;
		this->m_y            = _xyz;
		this->m_z            = _xyz;
		this->m_IsNormalised = false;
	}

	Vector3::Vector3( const Vector3& _rhs )
	{
		this->m_x            = _rhs.m_x;
		this->m_y            = _rhs.m_y;
		this->m_z            = _rhs.m_z;
		this->m_IsNormalised = _rhs.m_IsNormalised;
	}

	Vector3::Vector3( const float& _x, const float& _y, const float& _z )
	{
		this->m_x            = _x;
		this->m_y            = _y;
		this->m_z            = _z;
		this->m_IsNormalised = false;
	}

	Vector3::~Vector3( void )
	{
	}

	bool Vector3::IsNormalised( void ) const
	{
		return m_IsNormalised;
	}

	Vector3 Vector3::operator+( const Vector3& _rhs ) const
	{
		return Vector3( this->m_x + _rhs.m_x, this->m_y + _rhs.m_y, this->m_z + _rhs.m_z );
	}

	Vector3 Vector3::operator-( const Vector3& _rhs ) const
	{
		return Vector3( this->m_x - _rhs.m_x, this->m_y - _rhs.m_y, this->m_z - _rhs.m_z );
	}

	Vector3 Vector3::operator*( const float& _scalar ) const
	{
		return Vector3( this->m_x * _scalar, this->m_y * _scalar, this->m_z * _scalar );
	}

	Vector3 Vector3::operator/( const float& _scalar ) const
	{
		return Vector3( this->m_x / _scalar, this->m_y / _scalar, this->m_z / _scalar );
	}

	void Vector3::operator+=( const Vector3& _rhs )
	{
		this->m_x += _rhs.m_x;
		this->m_y += _rhs.m_y;
		this->m_z += _rhs.m_z;
	}

	void Vector3::operator-=( const Vector3& _rhs )
	{
		this->m_x -= _rhs.m_x;
		this->m_y -= _rhs.m_y;
		this->m_z -= _rhs.m_z;
	}

	void Vector3::operator*=( const Vector3& _rhs )
	{
		this->m_x *= _rhs.m_x;
		this->m_y *= _rhs.m_y;
		this->m_z *= _rhs.m_z;
	}

	void Vector3::operator/=( const Vector3& _rhs )
	{
		this->m_x /= _rhs.m_x;
		this->m_y /= _rhs.m_y;
		this->m_z /= _rhs.m_z;
	}

	float Vector3::Dot( const Vector3& _rhs ) const
	{
		return (float)(this->m_x * _rhs.m_x + this->m_y * _rhs.m_y + this->m_z * _rhs.m_z);
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
		return floorf( sqrt( this->m_x * this->m_x + this->m_y * this->m_y + this->m_z * this->m_z ) * 100000 + 0.5 ) / 100000;
	}

	Vector3 Vector3::Negate( void ) const
	{
		return Vector3( -this->m_x, -this->m_y, -this->m_z );
	}

	Vector3 Vector3::Normalise( void )
	{
		this->m_IsNormalised = true;
		return Vector3( this->m_x / Magnitude(), this->m_y / Magnitude(), this->m_z / Magnitude() );
	}

	Vector3 Vector3::Cross( const Vector3& _rhs ) const
	{
		return Vector3( this->m_y * _rhs.m_z - this->m_z * _rhs.m_y, this->m_z * _rhs.m_x - this->m_x * _rhs.m_z, this->m_x * _rhs.m_y - this->m_y * _rhs.m_x );
	}
}
