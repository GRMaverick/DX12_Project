#include "pch.h"
#include "CppUnitTest.h"

#include "Matrices/Matrix4x4.h"

using namespace Artemis::Maths;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArtemisMathsUnitTests
{
	TEST_CLASS( Matrix4x4_UnitTest )
	{
	public:
		TEST_METHOD( Matrix4x4_DefaultConstructor )
		{
			// Arrange / Act
			const Matrix4x4 matrix;

			// Assert
			Assert::AreEqual( 0.0f, matrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, matrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, matrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, matrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, matrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_ElementalConstructor )
		{
			// Arrange / Act
			const Matrix4x4 matrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Assert
			Assert::AreEqual( 1.0f, matrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, matrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, matrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, matrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 5.0f, matrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 6.0f, matrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 7.0f, matrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, matrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 9.0f, matrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, matrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 11.0f, matrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 12.0f, matrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 13.0f, matrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 14.0f, matrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 15.0f, matrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 16.0f, matrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_CopyConstructor )
		{
			// Arrange
			Matrix4x4       copy;
			const Matrix4x4 original( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			copy = original;

			// Assert
			Assert::AreEqual( 1.0f, copy.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, copy.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, copy.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, copy.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 5.0f, copy.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 6.0f, copy.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 7.0f, copy.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, copy.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 9.0f, copy.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, copy.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 11.0f, copy.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 12.0f, copy.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 13.0f, copy.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 14.0f, copy.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 15.0f, copy.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 16.0f, copy.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_TransposeIsGood )
		{
			// Arrange
			const Matrix4x4 matrix( 5.0f, -2.0f, 2.0f, 7.0f, 1.0f, 0.0f, 0.0f, 3.0f, -3.0f, 1.0f, 5.0f, 0.0f, 3.0f, -1.0f, -9.0f, 4.0f );

			// Act
			const Matrix4x4 adj = matrix.Transpose();

			// Assert
			Assert::AreEqual( 5.0f, adj.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, adj.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -3.0f, adj.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, adj.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( -2.0f, adj.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, adj.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, adj.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -1.0f, adj.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 2.0f, adj.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, adj.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 5.0f, adj.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -9.0f, adj.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 7.0f, adj.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, adj.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, adj.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, adj.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_DeterminantIsZero )
		{
			// Arrange
			const Matrix4x4 matrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			const float determinant = matrix.Determinant();

			// Assert
			Assert::AreEqual( 0.0f, determinant, L"Determinant is not ZERO", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_DeterminantIsNotZero )
		{
			// Arrange
			const Matrix4x4 matrix( 5.0f, -2.0f, 2.0f, 7.0f, 1.0f, 0.0f, 0.0f, 3.0f, -3.0f, 1.0f, 5.0f, 0.0f, 3.0f, -1.0f, -9.0f, 4.0f );

			// Act
			const float determinant = matrix.Determinant();

			//Assert
			Assert::AreEqual( 88.0f, determinant, L"Determinant is ZERO", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_InverseIsTrue )
		{
			// Arrange
			const Matrix4x4 matrix( 5.0f, -2.0f, 2.0f, 7.0f, 1.0f, 0.0f, 0.0f, 3.0f, -3.0f, 1.0f, 5.0f, 0.0f, 3.0f, -1.0f, -9.0f, 4.0f );

			// Act
			const Matrix4x4 inverse = matrix.Inverse();

			// Assert
			Assert::AreEqual( -0.136f, inverse.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.863f, inverse.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -0.681f, inverse.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -0.409f, inverse.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( -0.636f, inverse.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.363f, inverse.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -0.931f, inverse.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -0.659f, inverse.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.045f, inverse.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.045f, inverse.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -0.022f, inverse.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -0.113f, inverse.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.045f, inverse.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.045f, inverse.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.227f, inverse.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.136f, inverse.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_InverseIsFalse )
		{
			// Arrange
			const Matrix4x4 matrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			const Matrix4x4 inverse = matrix.Inverse();

			// Assert
			Assert::AreEqual( 0.0f, inverse.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, inverse.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, inverse.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, inverse.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, inverse.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_ScalarMultiplication )
		{
			// Arrange
			constexpr float scalar = 2.0f;
			const Matrix4x4 matrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			const Matrix4x4 finalMatrix = matrix * scalar;

			// Assert
			Assert::AreEqual( 2.0f, finalMatrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalMatrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 6.0f, finalMatrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, finalMatrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 10.0f, finalMatrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 12.0f, finalMatrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 14.0f, finalMatrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 16.0f, finalMatrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 18.0f, finalMatrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 20.0f, finalMatrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 22.0f, finalMatrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 24.0f, finalMatrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 26.0f, finalMatrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 28.0f, finalMatrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 30.0f, finalMatrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 32.0f, finalMatrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_ScalarDivision )
		{
			// Arrange
			constexpr float scalar = 2.0f;
			const Matrix4x4 matrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			const Matrix4x4 finalMatrix = matrix / scalar;

			// Assert
			Assert::AreEqual( 0.5f, finalMatrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, finalMatrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.5f, finalMatrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, finalMatrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 2.5f, finalMatrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, finalMatrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.5f, finalMatrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalMatrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 4.5f, finalMatrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 5.0f, finalMatrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 5.5f, finalMatrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 6.0f, finalMatrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 6.5f, finalMatrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 7.0f, finalMatrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 7.5f, finalMatrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, finalMatrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_IncrementalScalarMultiplication )
		{
			// Arrange
			constexpr float scalar = 2.0f;
			Matrix4x4       matrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			matrix *= scalar;

			// Assert
			Assert::AreEqual( 2.0f, matrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, matrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 6.0f, matrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, matrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 10.0f, matrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 12.0f, matrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 14.0f, matrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 16.0f, matrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 18.0f, matrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 20.0f, matrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 22.0f, matrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 24.0f, matrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 26.0f, matrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 28.0f, matrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 30.0f, matrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 32.0f, matrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_IncrementalScalarDivision )
		{
			// Arrange
			constexpr float scalar = 2.0f;
			Matrix4x4       matrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			matrix /= scalar;

			// Assert
			Assert::AreEqual( 0.5f, matrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, matrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.5f, matrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, matrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 2.5f, matrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, matrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.5f, matrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, matrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 4.5f, matrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 5.0f, matrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 5.5f, matrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 6.0f, matrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 6.5f, matrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 7.0f, matrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 7.5f, matrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, matrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_MatrixAddition )
		{
			// Arrange
			const Matrix4x4 leftMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );
			const Matrix4x4 rightMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			const Matrix4x4 finalMatrix = leftMatrix + rightMatrix;

			// Assert
			Assert::AreEqual( 2.0f, finalMatrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalMatrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 6.0f, finalMatrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, finalMatrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 10.0f, finalMatrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 12.0f, finalMatrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 14.0f, finalMatrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 16.0f, finalMatrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 18.0f, finalMatrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 20.0f, finalMatrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 22.0f, finalMatrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 24.0f, finalMatrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 26.0f, finalMatrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 28.0f, finalMatrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 30.0f, finalMatrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 32.0f, finalMatrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_MatrixSubtraction )
		{
			const Matrix4x4 leftMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );
			const Matrix4x4 rightMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			const Matrix4x4 finalMatrix = leftMatrix - rightMatrix;

			// Assert
			Assert::AreEqual( 0.0f, finalMatrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, finalMatrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, finalMatrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, finalMatrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_MatrixMultiplication )
		{
			const Matrix4x4 leftMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );
			const Matrix4x4 rightMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			const Matrix4x4 finalMatrix = leftMatrix * rightMatrix;

			// Assert
			Assert::AreEqual( 90.0f, finalMatrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 100.0f, finalMatrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 110.0f, finalMatrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 120.0f, finalMatrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 202.0f, finalMatrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 228.0f, finalMatrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 254.0f, finalMatrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 280.0f, finalMatrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 314.0f, finalMatrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 356.0f, finalMatrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 398.0f, finalMatrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 440.0f, finalMatrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 426.0f, finalMatrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 484.0f, finalMatrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 542.0f, finalMatrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 600.0f, finalMatrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_IncrementalMatrixAddition )
		{
			// Arrange
			Matrix4x4       finalMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );
			const Matrix4x4 rightMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			finalMatrix += rightMatrix;

			// Assert
			Assert::AreEqual( 2.0f, finalMatrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalMatrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 6.0f, finalMatrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, finalMatrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 10.0f, finalMatrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 12.0f, finalMatrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 14.0f, finalMatrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 16.0f, finalMatrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 18.0f, finalMatrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 20.0f, finalMatrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 22.0f, finalMatrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 24.0f, finalMatrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 26.0f, finalMatrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 28.0f, finalMatrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 30.0f, finalMatrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 32.0f, finalMatrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_IncrementalMatrixSubtraction )
		{
			// Arrange
			Matrix4x4       finalMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );
			const Matrix4x4 rightMatrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			// Act
			finalMatrix -= rightMatrix;

			// Assert
			Assert::AreEqual( 0.0f, finalMatrix.row1.x, L"Element 1,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row1.y, L"Element 1,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row1.z, L"Element 1,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row1.w, L"Element 1,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, finalMatrix.row2.x, L"Element 2,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row2.y, L"Element 2,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row2.z, L"Element 2,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row2.w, L"Element 2,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, finalMatrix.row3.x, L"Element 3,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row3.y, L"Element 3,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row3.z, L"Element 3,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row3.w, L"Element 3,4 Component Incorrect", LINE_INFO() );

			Assert::AreEqual( 0.0f, finalMatrix.row4.x, L"Element 4,1 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row4.y, L"Element 4,2 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row4.z, L"Element 4,3 Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, finalMatrix.row4.w, L"Element 4,4 Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Matrix4x4_VectorMultiplication )
		{
			const Vector4   vectorOperand( 1.0f, 2.0f, 3.0f, 4.0f );
			const Matrix4x4 matrixOperand( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f );

			const Vector4 finalVector = Matrix4x4::VectorMultiply( vectorOperand, matrixOperand );

			// Assert
			Assert::AreEqual( 90.0f, finalVector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 100.0f, finalVector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 110.0f, finalVector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 120.0f, finalVector.w, L"Z Component Incorrect", LINE_INFO() );
		}
	};
}
