#include "Matrix3x3.h"

#include <cassert>

namespace Artemis::Maths
{
	Matrix3x3::Matrix3x3( void )
	{
		row1 = Vector3( 0.0f );
		row2 = Vector3( 0.0f );
		row3 = Vector3( 0.0f );
	}

	Matrix3x3::Matrix3x3( const float _e11, const float _e12, const float _e13, const float _e21, const float _e22, const float _e23, const float _e31, const float _e32, const float _e33 )
	{
		row1 = Vector3( _e11, _e12, _e13 );
		row2 = Vector3( _e21, _e22, _e23 );
		row3 = Vector3( _e31, _e32, _e33 );
	}

	Matrix3x3::Matrix3x3( const Matrix3x3& _matrix )
	{
		row1 = Vector3( _matrix.row1.x, _matrix.row1.y, _matrix.row1.z );
		row2 = Vector3( _matrix.row2.x, _matrix.row2.y, _matrix.row2.z );
		row3 = Vector3( _matrix.row3.x, _matrix.row3.y, _matrix.row3.z );
	}

	Matrix3x3::~Matrix3x3( void )
	{
	}

	Vector3 Matrix3x3::GetColumn( const unsigned _index ) const
	{
		assert( (_index >= 0 && _index <=2) && "Index Out Of Bounds" );

		if ( _index == 0 )
		{
			return Vector3( row1.x, row2.x, row3.x );
		}
		else if ( _index == 1 )
		{
			return Vector3( row1.y, row2.y, row3.y );
		}
		else
		{
			return Vector3( row1.z, row2.z, row3.z );
		}
	}

	Matrix3x3 Matrix3x3::operator+( const Matrix3x3& _matrix ) const
	{
		Matrix3x3 returnMat;

		returnMat.row1 = row1 + _matrix.row1;
		returnMat.row2 = row2 + _matrix.row2;
		returnMat.row3 = row3 + _matrix.row3;

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator-( const Matrix3x3& _matrix ) const
	{
		Matrix3x3 returnMat;

		returnMat.row1 = row1 - _matrix.row1;
		returnMat.row2 = row2 - _matrix.row2;
		returnMat.row3 = row3 - _matrix.row3;

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator*( const Matrix3x3& _matrix ) const
	{
		//const Vector3 rightColumn0 = _matrix.GetColumn( 0 );
		//const Vector3 rightColumn1 = _matrix.GetColumn( 1 );
		//const Vector3 rightColumn2 = _matrix.GetColumn( 2 );

		Matrix3x3 returnMat;

		returnMat.row1 = VectorMultiply( row1, _matrix );
		returnMat.row2 = VectorMultiply( row2, _matrix );
		returnMat.row3 = VectorMultiply( row3, _matrix );

		//returnMat.row1 = Vector3( row1.Dot( rightColumn0 ), row1.Dot( rightColumn1 ), row1.Dot( rightColumn2 ) );
		//returnMat.row2 = Vector3( row2.Dot( rightColumn0 ), row2.Dot( rightColumn1 ), row2.Dot( rightColumn2 ) );
		//returnMat.row3 = Vector3( row3.Dot( rightColumn0 ), row3.Dot( rightColumn1 ), row3.Dot( rightColumn2 ) );

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator*( const float& _scalar ) const
	{
		Matrix3x3 returnMat;

		returnMat.row1 = row1 * _scalar;
		returnMat.row2 = row2 * _scalar;
		returnMat.row3 = row3 * _scalar;

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator/( const float& _scalar ) const
	{
		Matrix3x3 returnMat;

		returnMat.row1 = row1 / _scalar;
		returnMat.row2 = row2 / _scalar;
		returnMat.row3 = row3 / _scalar;

		return returnMat;
	}

	Matrix3x3& Matrix3x3::operator+=( const Matrix3x3& _matrix )
	{
		row1 += _matrix.row1;
		row2 += _matrix.row2;
		row3 += _matrix.row3;

		return *this;
	}

	Matrix3x3& Matrix3x3::operator-=( const Matrix3x3& _matrix )
	{
		row1 -= _matrix.row1;
		row2 -= _matrix.row2;
		row3 -= _matrix.row3;

		return *this;
	}

	Matrix3x3& Matrix3x3::operator*=( const float& _scalar )
	{
		row1 *= _scalar;
		row2 *= _scalar;
		row3 *= _scalar;

		return *this;
	}

	Matrix3x3& Matrix3x3::operator/=( const float& _scalar )
	{
		row1 /= _scalar;
		row2 /= _scalar;
		row3 /= _scalar;

		return *this;
	}

	float Matrix3x3::Determinant( void ) const
	{
		return (row1.x * row2.y * row3.z) - (row1.x * row3.y * row2.z) + (row2.x * row3.y * row1.z) - (row2.x * row1.y * row3.z) + (row3.x * row1.y * row2.z) - (row3.x * row2.y * row1.z);
	}

	Matrix3x3 Matrix3x3::Transpose( void ) const
	{
		return Matrix3x3( row1.x, row2.x, row3.x, row1.y, row2.y, row3.y, row1.z, row2.z, row3.z );
	}

	Matrix3x3 Matrix3x3::Inverse( void ) const
	{
		if ( const float determinant = this->Determinant(); determinant != 0 )
		{
			return Matrix3x3( (row2.y * row3.z - row2.z * row3.y) / determinant, -(row1.y * row3.z - row1.z * row3.y) / determinant, (row1.y * row2.z - row1.z * row2.y) / determinant, -(row2.x * row3.z - row2.z * row3.x) / determinant, (row1.x * row3.z - row1.z * row3.x) / determinant, -(row1.x * row2.z - row1.z * row2.x) / determinant, (row2.x * row3.y - row2.y * row3.x) / determinant, -(row1.x * row3.y - row1.y * row3.x) / determinant, (row1.x * row2.y - row1.y * row2.x) / determinant );
		}
		else
		{
			return Matrix3x3();
		}
	}

	Vector3& Matrix3x3::VectorMultiply( const Vector3& _vector, const Matrix3x3& _matrix )
	{
		Vector3 returnVector;

		returnVector.x = _vector.Dot( _matrix.GetColumn( 0 ) );
		returnVector.y = _vector.Dot( _matrix.GetColumn( 1 ) );
		returnVector.z = _vector.Dot( _matrix.GetColumn( 2 ) );

		return returnVector;
	}
}
