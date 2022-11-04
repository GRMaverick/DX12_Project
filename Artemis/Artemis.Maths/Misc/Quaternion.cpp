#include "Quaternion.h"
#include <math.h>

namespace Artemis::Maths
{
	Quaternion::Quaternion( void )
	{
		this->w      = 1.0f;
		this->vector = Vector3( 0.0f );
	}

	Quaternion::Quaternion( const Quaternion& _rhs )
	{
		this->w      = _rhs.w;
		this->vector = _rhs.vector;
	}

	Quaternion::Quaternion( const Vector3& _vector, const float& _w )
	{
		this->w      = _w;
		this->vector = _vector;
	}

	Quaternion::Quaternion( const float& _x, const float& _y, const float& _z, const float& _w )
	{
		this->w      = _w;
		this->vector = Vector3( _x, _y, _z );
	}

	Quaternion::~Quaternion( void )
	{
	}

	float Quaternion::Magnitude( void ) const
	{
		return static_cast<float>(sqrt( (vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z) + (w * w) ));
	}

	Quaternion& Quaternion::operator+=( const Quaternion& _quat )
	{
		vector += _quat.vector;
		w += _quat.w;
		return *this;
	}

	Quaternion& Quaternion::operator-=( const Quaternion& _quat )
	{
		vector -= _quat.vector;
		w -= _quat.w;
		return *this;
	}

	Quaternion& Quaternion::operator*( const Quaternion& _multiplier ) const
	{
		Quaternion q = Quaternion();

		q.w = this->w * _multiplier.w - this->vector.x * _multiplier.vector.x - this->vector.y * _multiplier.vector.y - this->vector.z * _multiplier.vector.z;

		q.vector.x = this->w * _multiplier.vector.x + this->vector.x * _multiplier.w + this->vector.y * _multiplier.vector.z - this->vector.z * _multiplier.vector.y;

		q.vector.y = this->w * _multiplier.vector.y + this->vector.y * _multiplier.w + this->vector.z * _multiplier.vector.x - this->vector.x * _multiplier.vector.z;

		q.vector.z = this->w * _multiplier.vector.z + this->vector.z * _multiplier.w + this->vector.x * _multiplier.vector.y - this->vector.y * _multiplier.vector.x;
		return q;
	}

	Quaternion& Quaternion::operator*=( const Quaternion& _multiplier )
	{
		Quaternion q = *this;

		this->w = q.w * _multiplier.w - q.vector.x * _multiplier.vector.x - q.vector.y * _multiplier.vector.y - q.vector.z * _multiplier.vector.z;

		this->vector.x = q.w * _multiplier.vector.x + q.vector.x * _multiplier.w + q.vector.y * _multiplier.vector.z - q.vector.z * _multiplier.vector.y;

		this->vector.y = q.w * _multiplier.vector.y + q.vector.y * _multiplier.w + q.vector.z * _multiplier.vector.x - q.vector.x * _multiplier.vector.z;

		this->vector.z = q.w * _multiplier.vector.z + q.vector.z * _multiplier.w + q.vector.x * _multiplier.vector.y - q.vector.y * _multiplier.vector.x;
		return *this;
	}

	Quaternion Quaternion::Conjugate( void ) const
	{
		return Quaternion( -vector.x, -vector.y, -vector.z, w );
	}

	Quaternion& Quaternion::Normalise( void ) const
	{
		Quaternion quat;
		float      magnitude = (this->vector.x * this->vector.x) + (this->vector.y * this->vector.y) + (this->vector.z * this->vector.z) + (this->w * this->w);

		if ( magnitude < 0.0f )
		{
			quat.w = 1.0f;
		}

		magnitude = 1.0f / sqrt( magnitude );
		quat.w *= magnitude;
		quat.vector = quat.vector * magnitude;

		return quat;
	}

	Quaternion& Quaternion::RotateByVector( const Vector3& vector )
	{
		const Quaternion q( vector.x, vector.y, vector.z, 0.0f );
		(*this) *= q;

		return *this;
	}

	void Quaternion::FromEuler( const Vector3& _vector )
	{
		const float xRadians = _vector.x * (3.14159 / 180);
		const float yRadians = _vector.y * (3.14159 / 180);
		const float zRadians = _vector.z * (3.14159 / 180);

		const Quaternion xQuat = Quaternion( static_cast<float>(sin( xRadians / 2 )), 0.0f, 0.0f, static_cast<float>(cos( xRadians / 2 )) );
		const Quaternion yQuat = Quaternion( 0.0f, static_cast<float>(sin( yRadians / 2 )), 0.0f, static_cast<float>(cos( yRadians / 2 )) );
		const Quaternion zQuat = Quaternion( 0.0f, 0.0f, static_cast<float>(sin( zRadians / 2 )), static_cast<float>(cos( zRadians / 2 )) );

		Quaternion finalQ = Quaternion( 0.0f, 0.0f, 0.0f, 1.0f );

		finalQ *= xQuat;
		finalQ *= yQuat;
		finalQ *= zQuat;

		*this = finalQ;
	}

	Vector3& Quaternion::ToEuler( void ) const
	{
		Vector3 vector = Vector3();

		const float xSq = this->vector.x * this->vector.x;
		const float ySq = this->vector.y * this->vector.y;
		const float zSq = this->vector.z * this->vector.z;

		const float sinRoll = 2 * (this->w * this->vector.x + this->vector.y * this->vector.z);
		const float cosRoll = 1 - 2 * (xSq + ySq);

		const float sinPitch = 2 * (this->w * this->vector.y - this->vector.z * this->vector.x);

		const float sinYaw = 2 * (this->w * this->vector.z + this->vector.x * this->vector.y);
		const float cosYaw = 1 - 2 * (ySq + zSq);

		vector.x = atan2( sinRoll, cosRoll );
		vector.y = (fabs( sinPitch ) >= 1) ? copysign( 3.14159, sinPitch ) : asin( sinPitch );
		vector.z = atan2( sinYaw, cosYaw );

		vector.x = vector.x * (180 / 3.14159);
		vector.y = vector.y * (180 / 3.14159);
		vector.z = vector.z * (180 / 3.14159);

		return vector;
	}
}
