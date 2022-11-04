#include "Matrix3x3.h"

namespace Artemis::Maths
{
	Matrix3x3::Matrix3x3( void )
	{
		row1[0] = 0.0f;
		row1[1] = 0.0f;
		row1[2] = 0.0f;

		row2[0] = 0.0f;
		row2[1] = 0.0f;
		row2[2] = 0.0f;

		row3[0] = 0.0f;
		row3[1] = 0.0f;
		row3[2] = 0.0f;
	}

	Matrix3x3::Matrix3x3( const float _e11, const float _e12, const float _e13, const float _e21, const float _e22, const float _e23, const float _e31, const float _e32, const float _e33 )
	{
		row1[0] = _e11;
		row1[1] = _e12;
		row1[2] = _e13;

		row2[0] = _e21;
		row2[1] = _e22;
		row2[2] = _e23;

		row3[0] = _e31;
		row3[1] = _e32;
		row3[2] = _e33;
	}

	Matrix3x3::Matrix3x3( const Matrix3x3& _matrix )
	{
		row1[0] = _matrix.row1[0];
		row1[1] = _matrix.row1[1];
		row1[2] = _matrix.row1[2];

		row2[0] = _matrix.row2[0];
		row2[1] = _matrix.row2[1];
		row2[2] = _matrix.row2[2];

		row3[0] = _matrix.row3[0];
		row3[1] = _matrix.row3[1];
		row3[2] = _matrix.row3[2];
	}

	Matrix3x3::~Matrix3x3( void )
	{
	}

	Matrix3x3 Matrix3x3::operator+( const Matrix3x3& _matrix )
	{
		Matrix3x3 returnMat;

		returnMat.row1[0] = row1[0] + _matrix.row1[0];
		returnMat.row1[1] = row1[1] + _matrix.row1[1];
		returnMat.row1[2] = row1[2] + _matrix.row1[2];

		returnMat.row2[0] = row2[0] + _matrix.row2[0];
		returnMat.row2[1] = row2[1] + _matrix.row2[1];
		returnMat.row2[2] = row2[2] + _matrix.row2[2];

		returnMat.row3[0] = row3[0] + _matrix.row3[0];
		returnMat.row3[1] = row3[1] + _matrix.row3[1];
		returnMat.row3[2] = row3[2] + _matrix.row3[2];

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator-( const Matrix3x3& _matrix ) const
	{
		Matrix3x3 returnMat;

		returnMat.row1[0] = row1[0] - _matrix.row1[0];
		returnMat.row1[1] = row1[1] - _matrix.row1[1];
		returnMat.row1[2] = row1[2] - _matrix.row1[2];

		returnMat.row2[0] = row2[0] - _matrix.row2[0];
		returnMat.row2[1] = row2[1] - _matrix.row2[1];
		returnMat.row2[2] = row2[2] - _matrix.row2[2];

		returnMat.row3[0] = row3[0] - _matrix.row3[0];
		returnMat.row3[1] = row3[1] - _matrix.row3[1];
		returnMat.row3[2] = row3[2] - _matrix.row3[2];

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator*( const Matrix3x3& _matrix ) const
	{
		Matrix3x3 returnMat;

		returnMat.row1[0] = row1[0] * _matrix.row1[0] + row1[1] * _matrix.row2[0] + row1[2] * _matrix.row3[0];
		returnMat.row1[1] = row1[0] * _matrix.row1[1] + row1[1] * _matrix.row2[1] + row1[2] * _matrix.row3[1];
		returnMat.row1[2] = row1[0] * _matrix.row1[2] + row1[1] * _matrix.row2[2] + row1[2] * _matrix.row3[2];

		returnMat.row2[0] = row2[0] * _matrix.row1[0] + row2[1] * _matrix.row2[0] + row2[2] * _matrix.row3[0];
		returnMat.row2[1] = row2[0] * _matrix.row1[1] + row2[1] * _matrix.row2[1] + row2[2] * _matrix.row3[1];
		returnMat.row2[2] = row2[0] * _matrix.row1[2] + row2[1] * _matrix.row2[2] + row2[2] * _matrix.row3[2];

		returnMat.row3[0] = row3[0] * _matrix.row1[0] + row3[1] * _matrix.row2[0] + row3[2] * _matrix.row3[0];
		returnMat.row3[1] = row3[0] * _matrix.row1[1] + row3[1] * _matrix.row2[1] + row3[2] * _matrix.row3[1];
		returnMat.row3[2] = row3[0] * _matrix.row1[2] + row3[1] * _matrix.row2[2] + row3[2] * _matrix.row3[2];

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator*( const float& _scalar ) const
	{
		Matrix3x3 returnMat;

		returnMat.row1[0] = row1[0] * _scalar;
		returnMat.row1[1] = row1[1] * _scalar;
		returnMat.row1[2] = row1[2] * _scalar;

		returnMat.row2[0] = row2[0] * _scalar;
		returnMat.row2[1] = row2[1] * _scalar;
		returnMat.row2[2] = row2[2] * _scalar;

		returnMat.row3[0] = row3[0] * _scalar;
		returnMat.row3[1] = row3[1] * _scalar;
		returnMat.row3[2] = row3[2] * _scalar;

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator/( const float& _scalar ) const
	{
		Matrix3x3 returnMat;

		returnMat.row1[0] = row1[0] / _scalar;
		returnMat.row1[1] = row1[1] / _scalar;
		returnMat.row1[2] = row1[2] / _scalar;

		returnMat.row2[0] = row2[0] / _scalar;
		returnMat.row2[1] = row2[1] / _scalar;
		returnMat.row2[2] = row2[2] / _scalar;

		returnMat.row3[0] = row3[0] / _scalar;
		returnMat.row3[1] = row3[1] / _scalar;
		returnMat.row3[2] = row3[2] / _scalar;

		return returnMat;
	}

	Matrix3x3& Matrix3x3::operator+=( const Matrix3x3& _matrix )
	{
		row1[0] += _matrix.row1[0];
		row1[1] += _matrix.row1[1];
		row1[2] += _matrix.row1[2];

		row2[0] += _matrix.row2[0];
		row2[1] += _matrix.row2[1];
		row2[2] += _matrix.row2[2];

		row3[0] += _matrix.row3[0];
		row3[1] += _matrix.row3[1];
		row3[2] += _matrix.row3[2];

		return *this;
	}

	Matrix3x3& Matrix3x3::operator-=( const Matrix3x3& _matrix )
	{
		row1[0] -= _matrix.row1[0];
		row1[1] -= _matrix.row1[1];
		row1[2] -= _matrix.row1[2];

		row2[0] -= _matrix.row2[0];
		row2[1] -= _matrix.row2[1];
		row2[2] -= _matrix.row2[2];

		row3[0] -= _matrix.row3[0];
		row3[1] -= _matrix.row3[1];
		row3[2] -= _matrix.row3[2];

		return *this;
	}

	Matrix3x3& Matrix3x3::operator*=( const float& _scalar )
	{
		row1[0] *= _scalar;
		row1[1] *= _scalar;
		row1[2] *= _scalar;

		row2[0] *= _scalar;
		row2[1] *= _scalar;
		row2[2] *= _scalar;

		row3[0] *= _scalar;
		row3[1] *= _scalar;
		row3[2] *= _scalar;

		return *this;
	}

	Matrix3x3& Matrix3x3::operator/=( const float& _scalar )
	{
		row1[0] /= _scalar;
		row1[1] /= _scalar;
		row1[2] /= _scalar;

		row2[0] /= _scalar;
		row2[1] /= _scalar;
		row2[2] /= _scalar;

		row3[0] /= _scalar;
		row3[1] /= _scalar;
		row3[2] /= _scalar;

		return *this;
	}

	float Matrix3x3::Determinant( void ) const
	{
		return (row1[0] * row2[1] * row3[2]) - (row1[0] * row3[1] * row2[2]) + (row2[0] * row3[1] * row1[2]) - (row2[0] * row1[1] * row3[2]) + (row3[0] * row1[1] * row2[2]) - (row3[0] * row2[1] * row1[2]);
	}

	Matrix3x3 Matrix3x3::Transpose( void ) const
	{
		return Matrix3x3( row1[0], row2[0], row3[0], row1[1], row2[1], row3[1], row1[2], row2[2], row3[2] );
	}

	Matrix3x3 Matrix3x3::Inverse( void ) const
	{
		if ( const float determinant = this->Determinant(); determinant != 0 )
		{
			return Matrix3x3( (row2[1] * row3[2] - row2[2] * row3[1]) / determinant, -(row1[1] * row3[2] - row1[2] * row3[1]) / determinant, (row1[1] * row2[2] - row1[2] * row2[1]) / determinant, -(row2[0] * row3[2] - row2[2] * row3[0]) / determinant, (row1[0] * row3[2] - row1[2] * row3[0]) / determinant, -(row1[0] * row2[2] - row1[2] * row2[0]) / determinant, (row2[0] * row3[1] - row2[1] * row3[0]) / determinant, -(row1[0] * row3[1] - row1[1] * row3[0]) / determinant, (row1[0] * row2[1] - row1[1] * row2[0]) / determinant );
		}
		else
		{
			return Matrix3x3();
		}
	}

	Vector3& Matrix3x3::VectorMultiply( const Vector3& _vector, const Matrix3x3& _matrix )
	{
		Vector3 returnVector;

		returnVector.x = _matrix.row1[0] * _vector.x + _matrix.row1[1] * _vector.y + _matrix.row1[2] * _vector.z;
		returnVector.y = _matrix.row2[0] * _vector.x + _matrix.row2[1] * _vector.y + _matrix.row2[2] * _vector.z;
		returnVector.z = _matrix.row3[0] * _vector.x + _matrix.row3[1] * _vector.y + _matrix.row3[2] * _vector.z;

		return returnVector;
	}
}
