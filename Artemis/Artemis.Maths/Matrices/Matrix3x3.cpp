#include "Matrix3x3.h"

namespace Artemis::Maths
{
	Matrix3x3::Matrix3x3( void )
	{
		m_row1[0] = 0.0f;
		m_row1[1] = 0.0f;
		m_row1[2] = 0.0f;

		m_row2[0] = 0.0f;
		m_row2[1] = 0.0f;
		m_row2[2] = 0.0f;

		m_row3[0] = 0.0f;
		m_row3[1] = 0.0f;
		m_row3[2] = 0.0f;
	}

	Matrix3x3::Matrix3x3( const float _e11, const float _e12, const float _e13, const float _e21, const float _e22, const float _e23, const float _e31, const float _e32, const float _e33 )
	{
		m_row1[0] = _e11;
		m_row1[1] = _e12;
		m_row1[2] = _e13;

		m_row2[0] = _e21;
		m_row2[1] = _e22;
		m_row2[2] = _e23;

		m_row3[0] = _e31;
		m_row3[1] = _e32;
		m_row3[2] = _e33;
	}

	Matrix3x3::Matrix3x3( const Matrix3x3& _matrix )
	{
		m_row1[0] = _matrix.m_row1[0];
		m_row1[1] = _matrix.m_row1[1];
		m_row1[2] = _matrix.m_row1[2];

		m_row2[0] = _matrix.m_row2[0];
		m_row2[1] = _matrix.m_row2[1];
		m_row2[2] = _matrix.m_row2[2];

		m_row3[0] = _matrix.m_row3[0];
		m_row3[1] = _matrix.m_row3[1];
		m_row3[2] = _matrix.m_row3[2];
	}

	Matrix3x3::~Matrix3x3( void )
	{
	}

	Matrix3x3 Matrix3x3::operator+( const Matrix3x3& _matrix )
	{
		Matrix3x3 returnMat;

		returnMat.m_row1[0] = m_row1[0] + _matrix.m_row1[0];
		returnMat.m_row1[1] = m_row1[1] + _matrix.m_row1[1];
		returnMat.m_row1[2] = m_row1[2] + _matrix.m_row1[2];

		returnMat.m_row2[0] = m_row2[0] + _matrix.m_row2[0];
		returnMat.m_row2[1] = m_row2[1] + _matrix.m_row2[1];
		returnMat.m_row2[2] = m_row2[2] + _matrix.m_row2[2];

		returnMat.m_row3[0] = m_row3[0] + _matrix.m_row3[0];
		returnMat.m_row3[1] = m_row3[1] + _matrix.m_row3[1];
		returnMat.m_row3[2] = m_row3[2] + _matrix.m_row3[2];

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator-( const Matrix3x3& _matrix ) const
	{
		Matrix3x3 returnMat;

		returnMat.m_row1[0] = m_row1[0] - _matrix.m_row1[0];
		returnMat.m_row1[1] = m_row1[1] - _matrix.m_row1[1];
		returnMat.m_row1[2] = m_row1[2] - _matrix.m_row1[2];

		returnMat.m_row2[0] = m_row2[0] - _matrix.m_row2[0];
		returnMat.m_row2[1] = m_row2[1] - _matrix.m_row2[1];
		returnMat.m_row2[2] = m_row2[2] - _matrix.m_row2[2];

		returnMat.m_row3[0] = m_row3[0] - _matrix.m_row3[0];
		returnMat.m_row3[1] = m_row3[1] - _matrix.m_row3[1];
		returnMat.m_row3[2] = m_row3[2] - _matrix.m_row3[2];

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator*( const Matrix3x3& _matrix ) const
	{
		Matrix3x3 returnMat;

		returnMat.m_row1[0] = m_row1[0] * _matrix.m_row1[0] + m_row1[1] * _matrix.m_row2[0] + m_row1[2] * _matrix.m_row3[0];
		returnMat.m_row1[1] = m_row1[0] * _matrix.m_row1[1] + m_row1[1] * _matrix.m_row2[1] + m_row1[2] * _matrix.m_row3[1];
		returnMat.m_row1[2] = m_row1[0] * _matrix.m_row1[2] + m_row1[1] * _matrix.m_row2[2] + m_row1[2] * _matrix.m_row3[2];

		returnMat.m_row2[0] = m_row2[0] * _matrix.m_row1[0] + m_row2[1] * _matrix.m_row2[0] + m_row2[2] * _matrix.m_row3[0];
		returnMat.m_row2[1] = m_row2[0] * _matrix.m_row1[1] + m_row2[1] * _matrix.m_row2[1] + m_row2[2] * _matrix.m_row3[1];
		returnMat.m_row2[2] = m_row2[0] * _matrix.m_row1[2] + m_row2[1] * _matrix.m_row2[2] + m_row2[2] * _matrix.m_row3[2];

		returnMat.m_row3[0] = m_row3[0] * _matrix.m_row1[0] + m_row3[1] * _matrix.m_row2[0] + m_row3[2] * _matrix.m_row3[0];
		returnMat.m_row3[1] = m_row3[0] * _matrix.m_row1[1] + m_row3[1] * _matrix.m_row2[1] + m_row3[2] * _matrix.m_row3[1];
		returnMat.m_row3[2] = m_row3[0] * _matrix.m_row1[2] + m_row3[1] * _matrix.m_row2[2] + m_row3[2] * _matrix.m_row3[2];

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator*( const float& _scalar ) const
	{
		Matrix3x3 returnMat;

		returnMat.m_row1[0] = m_row1[0] * _scalar;
		returnMat.m_row1[1] = m_row1[1] * _scalar;
		returnMat.m_row1[2] = m_row1[2] * _scalar;

		returnMat.m_row2[0] = m_row2[0] * _scalar;
		returnMat.m_row2[1] = m_row2[1] * _scalar;
		returnMat.m_row2[2] = m_row2[2] * _scalar;

		returnMat.m_row3[0] = m_row3[0] * _scalar;
		returnMat.m_row3[1] = m_row3[1] * _scalar;
		returnMat.m_row3[2] = m_row3[2] * _scalar;

		return returnMat;
	}

	Matrix3x3 Matrix3x3::operator/( const float& _scalar ) const
	{
		Matrix3x3 returnMat;

		returnMat.m_row1[0] = m_row1[0] / _scalar;
		returnMat.m_row1[1] = m_row1[1] / _scalar;
		returnMat.m_row1[2] = m_row1[2] / _scalar;

		returnMat.m_row2[0] = m_row2[0] / _scalar;
		returnMat.m_row2[1] = m_row2[1] / _scalar;
		returnMat.m_row2[2] = m_row2[2] / _scalar;

		returnMat.m_row3[0] = m_row3[0] / _scalar;
		returnMat.m_row3[1] = m_row3[1] / _scalar;
		returnMat.m_row3[2] = m_row3[2] / _scalar;

		return returnMat;
	}

	Matrix3x3& Matrix3x3::operator+=( const Matrix3x3& _matrix )
	{
		m_row1[0] += _matrix.m_row1[0];
		m_row1[1] += _matrix.m_row1[1];
		m_row1[2] += _matrix.m_row1[2];

		m_row2[0] += _matrix.m_row2[0];
		m_row2[1] += _matrix.m_row2[1];
		m_row2[2] += _matrix.m_row2[2];

		m_row3[0] += _matrix.m_row3[0];
		m_row3[1] += _matrix.m_row3[1];
		m_row3[2] += _matrix.m_row3[2];

		return *this;
	}

	Matrix3x3& Matrix3x3::operator-=( const Matrix3x3& _matrix )
	{
		m_row1[0] -= _matrix.m_row1[0];
		m_row1[1] -= _matrix.m_row1[1];
		m_row1[2] -= _matrix.m_row1[2];

		m_row2[0] -= _matrix.m_row2[0];
		m_row2[1] -= _matrix.m_row2[1];
		m_row2[2] -= _matrix.m_row2[2];

		m_row3[0] -= _matrix.m_row3[0];
		m_row3[1] -= _matrix.m_row3[1];
		m_row3[2] -= _matrix.m_row3[2];

		return *this;
	}

	Matrix3x3& Matrix3x3::operator*=( const float& _scalar )
	{
		m_row1[0] *= _scalar;
		m_row1[1] *= _scalar;
		m_row1[2] *= _scalar;

		m_row2[0] *= _scalar;
		m_row2[1] *= _scalar;
		m_row2[2] *= _scalar;

		m_row3[0] *= _scalar;
		m_row3[1] *= _scalar;
		m_row3[2] *= _scalar;

		return *this;
	}

	Matrix3x3& Matrix3x3::operator/=( const float& _scalar )
	{
		m_row1[0] /= _scalar;
		m_row1[1] /= _scalar;
		m_row1[2] /= _scalar;

		m_row2[0] /= _scalar;
		m_row2[1] /= _scalar;
		m_row2[2] /= _scalar;

		m_row3[0] /= _scalar;
		m_row3[1] /= _scalar;
		m_row3[2] /= _scalar;

		return *this;
	}

	float Matrix3x3::Determinant( void ) const
	{
		return (m_row1[0] * m_row2[1] * m_row3[2]) - (m_row1[0] * m_row3[1] * m_row2[2]) + (m_row2[0] * m_row3[1] * m_row1[2]) - (m_row2[0] * m_row1[1] * m_row3[2]) + (m_row3[0] * m_row1[1] * m_row2[2]) - (m_row3[0] * m_row2[1] * m_row1[2]);
	}

	Matrix3x3 Matrix3x3::Transpose( void ) const
	{
		return Matrix3x3( m_row1[0], m_row2[0], m_row3[0], m_row1[1], m_row2[1], m_row3[1], m_row1[2], m_row2[2], m_row3[2] );
	}

	Matrix3x3 Matrix3x3::Inverse( void ) const
	{
		if ( const float determinant = this->Determinant(); determinant != 0 )
		{
			return Matrix3x3( (m_row2[1] * m_row3[2] - m_row2[2] * m_row3[1]) / determinant, -(m_row1[1] * m_row3[2] - m_row1[2] * m_row3[1]) / determinant, (m_row1[1] * m_row2[2] - m_row1[2] * m_row2[1]) / determinant, -(m_row2[0] * m_row3[2] - m_row2[2] * m_row3[0]) / determinant, (m_row1[0] * m_row3[2] - m_row1[2] * m_row3[0]) / determinant, -(m_row1[0] * m_row2[2] - m_row1[2] * m_row2[0]) / determinant, (m_row2[0] * m_row3[1] - m_row2[1] * m_row3[0]) / determinant, -(m_row1[0] * m_row3[1] - m_row1[1] * m_row3[0]) / determinant, (m_row1[0] * m_row2[1] - m_row1[1] * m_row2[0]) / determinant );
		}
		else
		{
			return Matrix3x3();
		}
	}

	Vector3& Matrix3x3::VectorMultiply( const Vector3& _vector, const Matrix3x3& _matrix )
	{
		Vector3 returnVector;

		returnVector.m_x = _matrix.m_row1[0] * _vector.m_x + _matrix.m_row1[1] * _vector.m_y + _matrix.m_row1[2] * _vector.m_z;
		returnVector.m_y = _matrix.m_row2[0] * _vector.m_x + _matrix.m_row2[1] * _vector.m_y + _matrix.m_row2[2] * _vector.m_z;
		returnVector.m_z = _matrix.m_row3[0] * _vector.m_x + _matrix.m_row3[1] * _vector.m_y + _matrix.m_row3[2] * _vector.m_z;

		return returnVector;
	}
}
