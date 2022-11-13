#include "Matrix4x4.h"

#include <cassert>
#include <cmath>

namespace Artemis::Maths
{
	float Round3Dp( const float _value )
	{
		const int trunc = _value * 1000;
		return static_cast<float>(trunc) / 1000;
	}

	Matrix4x4::Matrix4x4( void )
	{
		row1.x = 0.0f;
		row1.y = 0.0f;
		row1.z = 0.0f;
		row1.w = 0.0f;

		row2.x = 0.0f;
		row2.y = 0.0f;
		row2.z = 0.0f;
		row2.w = 0.0f;

		row3.x = 0.0f;
		row3.y = 0.0f;
		row3.z = 0.0f;
		row3.w = 0.0f;

		row4.x = 0.0f;
		row4.y = 0.0f;
		row4.z = 0.0f;
		row4.w = 0.0f;
	}

	Matrix4x4::Matrix4x4( const float _e11, const float _e12, const float _e13, const float _e14, const float _e21, const float _e22, const float _e23, const float _e24, const float _e31, const float _e32, const float _e33, const float _e34, float _e41, float _e42, float _e43, float _e44 )
	{
		row1.x = _e11;
		row1.y = _e12;
		row1.z = _e13;
		row1.w = _e14;

		row2.x = _e21;
		row2.y = _e22;
		row2.z = _e23;
		row2.w = _e24;

		row3.x = _e31;
		row3.y = _e32;
		row3.z = _e33;
		row3.w = _e34;

		row4.x = _e41;
		row4.y = _e42;
		row4.z = _e43;
		row4.w = _e44;
	}

	Matrix4x4::Matrix4x4( const Matrix4x4& _matrix )
	{
		row1.x = _matrix.row1.x;
		row1.y = _matrix.row1.y;
		row1.z = _matrix.row1.z;
		row1.w = _matrix.row1.w;

		row2.x = _matrix.row2.x;
		row2.y = _matrix.row2.y;
		row2.z = _matrix.row2.z;
		row2.w = _matrix.row2.w;

		row3.x = _matrix.row3.x;
		row3.y = _matrix.row3.y;
		row3.z = _matrix.row3.z;
		row3.w = _matrix.row3.w;

		row4.x = _matrix.row4.x;
		row4.y = _matrix.row4.y;
		row4.z = _matrix.row4.z;
		row4.w = _matrix.row4.w;
	}

	Matrix4x4::~Matrix4x4( void )
	{
	}

	Vector4 Matrix4x4::GetColumn( const unsigned int& _index ) const
	{
		assert( (_index >= 0 && _index <= 3) && "Index Out Of Bounds" );

		if ( _index == 0 )
		{
			return Vector4( row1.x, row2.x, row3.x, row4.x );
		}
		else if ( _index == 1 )
		{
			return Vector4( row1.y, row2.y, row3.y, row4.y );
		}
		else if ( _index == 2 )
		{
			return Vector4( row1.z, row2.z, row3.z, row4.z );
		}
		else
		{
			return Vector4( row1.w, row2.w, row3.w, row4.w );
		}
	}

	Matrix4x4 Matrix4x4::operator+( const Matrix4x4& _matrix ) const
	{
		Matrix4x4 returnMat;

		returnMat.row1.x = row1.x + _matrix.row1.x;
		returnMat.row1.y = row1.y + _matrix.row1.y;
		returnMat.row1.z = row1.z + _matrix.row1.z;
		returnMat.row1.w = row1.w + _matrix.row1.w;

		returnMat.row2.x = row2.x + _matrix.row2.x;
		returnMat.row2.y = row2.y + _matrix.row2.y;
		returnMat.row2.z = row2.z + _matrix.row2.z;
		returnMat.row2.w = row2.w + _matrix.row2.w;

		returnMat.row3.x = row3.x + _matrix.row3.x;
		returnMat.row3.y = row3.y + _matrix.row3.y;
		returnMat.row3.z = row3.z + _matrix.row3.z;
		returnMat.row3.w = row3.w + _matrix.row3.w;

		returnMat.row4.x = row4.x + _matrix.row4.x;
		returnMat.row4.y = row4.y + _matrix.row4.y;
		returnMat.row4.z = row4.z + _matrix.row4.z;
		returnMat.row4.w = row4.w + _matrix.row4.w;

		return returnMat;
	}

	Matrix4x4 Matrix4x4::operator-( const Matrix4x4& _matrix ) const
	{
		Matrix4x4 returnMat;

		returnMat.row1.x = row1.x - _matrix.row1.x;
		returnMat.row1.y = row1.y - _matrix.row1.y;
		returnMat.row1.z = row1.z - _matrix.row1.z;
		returnMat.row1.w = row1.w - _matrix.row1.w;

		returnMat.row2.x = row2.x - _matrix.row2.x;
		returnMat.row2.y = row2.y - _matrix.row2.y;
		returnMat.row2.z = row2.z - _matrix.row2.z;
		returnMat.row2.w = row2.w - _matrix.row2.w;

		returnMat.row3.x = row3.x - _matrix.row3.x;
		returnMat.row3.y = row3.y - _matrix.row3.y;
		returnMat.row3.z = row3.z - _matrix.row3.z;
		returnMat.row3.w = row3.w - _matrix.row3.w;

		returnMat.row4.x = row4.x - _matrix.row4.x;
		returnMat.row4.y = row4.y - _matrix.row4.y;
		returnMat.row4.z = row4.z - _matrix.row4.z;
		returnMat.row4.w = row4.w - _matrix.row4.w;

		return returnMat;
	}

	Matrix4x4 Matrix4x4::operator*( const Matrix4x4& _matrix ) const
	{
		Matrix4x4 returnMat;

		returnMat.row1 = VectorMultiply( row1, _matrix );
		returnMat.row2 = VectorMultiply( row2, _matrix );
		returnMat.row3 = VectorMultiply( row3, _matrix );
		returnMat.row4 = VectorMultiply( row4, _matrix );

		return returnMat;
	}

	Matrix4x4 Matrix4x4::operator*( const float& _scalar ) const
	{
		Matrix4x4 returnMat;

		returnMat.row1.x = row1.x * _scalar;
		returnMat.row1.y = row1.y * _scalar;
		returnMat.row1.z = row1.z * _scalar;
		returnMat.row1.w = row1.w * _scalar;

		returnMat.row2.x = row2.x * _scalar;
		returnMat.row2.y = row2.y * _scalar;
		returnMat.row2.z = row2.z * _scalar;
		returnMat.row2.w = row2.w * _scalar;

		returnMat.row3.x = row3.x * _scalar;
		returnMat.row3.y = row3.y * _scalar;
		returnMat.row3.z = row3.z * _scalar;
		returnMat.row3.w = row3.w * _scalar;

		returnMat.row4.x = row4.x * _scalar;
		returnMat.row4.y = row4.y * _scalar;
		returnMat.row4.z = row4.z * _scalar;
		returnMat.row4.w = row4.w * _scalar;

		return returnMat;
	}

	Matrix4x4 operator*( const float& _scalar, const Matrix4x4& _matrix )
	{
		Matrix4x4 returnMat;

		returnMat.row1.x = _matrix.row1.x * _scalar;
		returnMat.row1.y = _matrix.row1.y * _scalar;
		returnMat.row1.z = _matrix.row1.z * _scalar;
		returnMat.row1.w = _matrix.row1.w * _scalar;

		returnMat.row2.x = _matrix.row2.x * _scalar;
		returnMat.row2.y = _matrix.row2.y * _scalar;
		returnMat.row2.z = _matrix.row2.z * _scalar;
		returnMat.row2.w = _matrix.row2.w * _scalar;

		returnMat.row3.x = _matrix.row3.x * _scalar;
		returnMat.row3.y = _matrix.row3.y * _scalar;
		returnMat.row3.z = _matrix.row3.z * _scalar;
		returnMat.row3.w = _matrix.row3.w * _scalar;

		returnMat.row4.x = _matrix.row4.x * _scalar;
		returnMat.row4.y = _matrix.row4.y * _scalar;
		returnMat.row4.z = _matrix.row4.z * _scalar;
		returnMat.row4.w = _matrix.row4.w * _scalar;

		return returnMat;
	}

	Matrix4x4 Matrix4x4::operator/( const float& _scalar ) const
	{
		Matrix4x4 returnMat;

		returnMat.row1.x = Round3Dp( row1.x / _scalar );
		returnMat.row1.y = Round3Dp( row1.y / _scalar );
		returnMat.row1.z = Round3Dp( row1.z / _scalar );
		returnMat.row1.w = Round3Dp( row1.w / _scalar );

		returnMat.row2.x = Round3Dp( row2.x / _scalar );
		returnMat.row2.y = Round3Dp( row2.y / _scalar );
		returnMat.row2.z = Round3Dp( row2.z / _scalar );
		returnMat.row2.w = Round3Dp( row2.w / _scalar );

		returnMat.row3.x = Round3Dp( row3.x / _scalar );
		returnMat.row3.y = Round3Dp( row3.y / _scalar );
		returnMat.row3.z = Round3Dp( row3.z / _scalar );
		returnMat.row3.w = Round3Dp( row3.w / _scalar );

		returnMat.row4.x = Round3Dp( row4.x / _scalar );
		returnMat.row4.y = Round3Dp( row4.y / _scalar );
		returnMat.row4.z = Round3Dp( row4.z / _scalar );
		returnMat.row4.w = Round3Dp( row4.w / _scalar );

		return returnMat;
	}

	Matrix4x4& Matrix4x4::operator+=( const Matrix4x4& _matrix )
	{
		row1.x += _matrix.row1.x;
		row1.y += _matrix.row1.y;
		row1.z += _matrix.row1.z;
		row1.w += _matrix.row1.w;

		row2.x += _matrix.row2.x;
		row2.y += _matrix.row2.y;
		row2.z += _matrix.row2.z;
		row2.w += _matrix.row2.w;

		row3.x += _matrix.row3.x;
		row3.y += _matrix.row3.y;
		row3.z += _matrix.row3.z;
		row3.w += _matrix.row3.w;

		row4.x += _matrix.row4.x;
		row4.y += _matrix.row4.y;
		row4.z += _matrix.row4.z;
		row4.w += _matrix.row4.w;

		return *this;
	}

	Matrix4x4& Matrix4x4::operator-=( const Matrix4x4& _matrix )
	{
		row1.x -= _matrix.row1.x;
		row1.y -= _matrix.row1.y;
		row1.z -= _matrix.row1.z;
		row1.w -= _matrix.row1.w;

		row2.x -= _matrix.row2.x;
		row2.y -= _matrix.row2.y;
		row2.z -= _matrix.row2.z;
		row2.w -= _matrix.row2.w;

		row3.x -= _matrix.row3.x;
		row3.y -= _matrix.row3.y;
		row3.z -= _matrix.row3.z;
		row3.w -= _matrix.row3.w;

		row4.x -= _matrix.row4.x;
		row4.y -= _matrix.row4.y;
		row4.z -= _matrix.row4.z;
		row4.w -= _matrix.row4.w;

		return *this;
	}

	Matrix4x4& Matrix4x4::operator*=( const float& _scalar )
	{
		row1.x *= _scalar;
		row1.y *= _scalar;
		row1.z *= _scalar;
		row1.w *= _scalar;

		row2.x *= _scalar;
		row2.y *= _scalar;
		row2.z *= _scalar;
		row2.w *= _scalar;

		row3.x *= _scalar;
		row3.y *= _scalar;
		row3.z *= _scalar;
		row3.w *= _scalar;

		row4.x *= _scalar;
		row4.y *= _scalar;
		row4.z *= _scalar;
		row4.w *= _scalar;

		return *this;
	}

	Matrix4x4& Matrix4x4::operator/=( const float& _scalar )
	{
		row1.x /= _scalar;
		row1.y /= _scalar;
		row1.z /= _scalar;
		row1.w /= _scalar;

		row2.x /= _scalar;
		row2.y /= _scalar;
		row2.z /= _scalar;
		row2.w /= _scalar;

		row3.x /= _scalar;
		row3.y /= _scalar;
		row3.z /= _scalar;
		row3.w /= _scalar;

		row4.x /= _scalar;
		row4.y /= _scalar;
		row4.z /= _scalar;
		row4.w /= _scalar;

		return *this;
	}

	float Matrix4x4::Determinant( void ) const
	{
		const float part1 = row1.x * Matrix3x3( row2.y, row2.z, row2.w, row3.y, row3.z, row3.w, row4.y, row4.z, row4.w ).Determinant();
		const float part2 = row1.y * Matrix3x3( row2.x, row2.z, row2.w, row3.x, row3.z, row3.w, row4.x, row4.z, row4.w ).Determinant();
		const float part3 = row1.z * Matrix3x3( row2.x, row2.y, row2.w, row3.x, row3.y, row3.w, row4.x, row4.y, row4.w ).Determinant();
		const float part4 = row1.w * Matrix3x3( row2.x, row2.y, row2.z, row3.x, row3.y, row3.z, row4.x, row4.y, row4.z ).Determinant();

		return part1 - part2 + part3 - part4;
	}

	Matrix4x4 Matrix4x4::Transpose( void ) const
	{
		return Matrix4x4( row1.x, row2.x, row3.x, row4.x, row1.y, row2.y, row3.y, row4.y, row1.z, row2.z, row3.z, row4.z, row1.w, row2.w, row3.w, row4.w );
	}

	void getCofactor( float A[4][4], float temp[4][4], int p, int q, int n )
	{
		int i = 0, j = 0;

		// Looping for each element of the matrix
		for ( int row = 0; row < n; row++ )
		{
			for ( int col = 0; col < n; col++ )
			{
				//  Copying into temporary matrix only those
				//  element which are not in given row and
				//  column
				if ( row != p && col != q )
				{
					temp[i][j++] = A[row][col];

					// Row is filled, so increase row index and
					// reset col index
					if ( j == n - 1 )
					{
						j = 0;
						i++;
					}
				}
			}
		}
	}

	int DetInternal( float A[4][4], int n )
	{
		int D = 0; // Initialize result

		//  Base case : if matrix contains single element
		if ( n == 1 )
			return A[0][0];

		float temp[4][4]; // To store cofactors

		int sign = 1; // To store sign multiplier

		// Iterate for each element of first row
		for ( int f = 0; f < n; f++ )
		{
			// Getting Cofactor of A[0][f]
			getCofactor( A, temp, 0, f, n );
			D += sign * A[0][f] * DetInternal( temp, n - 1 );

			// terms are to be added with alternate sign
			sign = -sign;
		}

		return D;
	}

	Matrix4x4 Matrix4x4::Adjugate() const
	{
		float A[4][4]   = {{row1.x, row1.y, row1.z, row1.w}, {row2.x, row2.y, row2.z, row2.w}, {row3.x, row3.y, row3.z, row3.w}, {row4.x, row4.y, row4.z, row4.w}};
		float adj[4][4] = {};

		int   sign       = 1;
		float temp[4][4] = {};;

		for ( int i = 0; i < 4; i++ )
		{
			for ( int j = 0; j < 4; j++ )
			{
				// Get cofactor of A[i][j]
				getCofactor( A, temp, i, j, 4 );

				// sign of adj[j][i] positive if sum of row
				// and column indexes is even.
				sign = ((i + j) % 2 == 0) ? 1 : -1;

				// Interchanging rows and columns to get the
				// transpose of the cofactor matrix
				adj[j][i] = (sign) * DetInternal( temp, 3 );
			}
		}

		return Matrix4x4( adj[0][0], adj[0][1], adj[0][2], adj[0][3], adj[1][0], adj[1][1], adj[1][2], adj[1][3], adj[2][0], adj[2][1], adj[2][2], adj[2][3], adj[3][0], adj[3][1], adj[3][2], adj[3][3] );
	}

	Matrix4x4 Matrix4x4::Inverse( void ) const
	{
		if ( const float determinant = this->Determinant(); determinant != 0 )
		{
			return Adjugate() / determinant;
		}
		else
		{
			return Matrix4x4();
		}
	}

	Vector4 Matrix4x4::VectorMultiply( const Vector4& _vector, const Matrix4x4& _matrix )
	{
		Vector4 returnVector;

		returnVector.x = _vector.Dot( _matrix.GetColumn( 0 ) );
		returnVector.y = _vector.Dot( _matrix.GetColumn( 1 ) );
		returnVector.z = _vector.Dot( _matrix.GetColumn( 2 ) );
		returnVector.w = _vector.Dot( _matrix.GetColumn( 3 ) );

		return returnVector;
	}
}
