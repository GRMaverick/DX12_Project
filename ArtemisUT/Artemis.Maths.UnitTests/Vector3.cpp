#include "pch.h"
#include "CppUnitTest.h"

#include "Vectors/Vector3.h"

using namespace Artemis::Maths;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArtemisMathsUnitTests
{
	TEST_CLASS( Vector3_UnitTest )
	{
	public:
		TEST_METHOD( Vector3_DefaultConstructor )
		{
			const Vector3 vector;

			Assert::AreEqual( 0.0f, vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, vector.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_SingleConstructor )
		{
			Vector3 vector( 10.0f );

			Assert::AreEqual( 10.0f, vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, vector.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_CopyConstructor )
		{
			Vector3       copy;
			const Vector3 original( 10.0f );

			copy = original;

			Assert::AreEqual( 10.0f, copy.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, copy.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, copy.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_TripleConstructor )
		{
			const Vector3 vector( 1.0f, 2.0f, 3.0f );

			Assert::AreEqual( 1.0f, vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, vector.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_AdditionFunction )
		{
			// Arrange
			const Vector3 left  = Vector3( 0.0f );
			const Vector3 right = Vector3( 1.0f );

			// Act
			const Vector3 total = left + right;

			// Assert
			Assert::AreEqual( 1.0f, total.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, total.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, total.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_SubtractFunction )
		{
			// Arrange 
			const Vector3 left  = Vector3( 1.0f, 2.0f, 3.0f );
			const Vector3 right = left;

			// Act
			const Vector3 total = left - right;

			// Assert
			Assert::AreEqual( 0.0f, total.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, total.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, total.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_Increment )
		{
			// Arrange
			Vector3       left  = Vector3( 1.0f );
			const Vector3 right = Vector3( 123.0f );

			// Act
			left += right;

			// Assert
			Assert::AreEqual( 124.0f, left.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 124.0f, left.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 124.0f, left.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_Decrement )
		{
			// Arrange
			Vector3       left  = Vector3( 1.0f );
			const Vector3 right = Vector3( 123.0f );

			// Act
			left -= right;

			// Assert
			Assert::AreEqual( -122.0f, left.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -122.0f, left.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -122.0f, left.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_IncrementalMultiplication )
		{
			// Arrange
			Vector3       left  = Vector3( 2.0f );
			const Vector3 right = Vector3( 4.0f );

			// Act
			left *= right;

			// Assert
			Assert::AreEqual( 8.0f, left.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, left.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, left.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_IncrementalDivision )
		{
			// Arrange
			Vector3       left  = Vector3( 2.0f );
			const Vector3 right = Vector3( 4.0f );

			// Act
			left /= right;

			// Assert
			Assert::AreEqual( 0.5f, left.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.5f, left.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.5f, left.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_MultiplyByScalar )
		{
			// Arrange
			const Vector3 vector = Vector3( 1.0f, 2.0f, 3.0f );
			const float   scalar = 5;

			// Act
			const Vector3 newVector = vector * scalar;

			//Assert
			Assert::AreEqual( 5.0f, newVector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, newVector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 15.0f, newVector.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_Negate )
		{
			// Arrange
			const Vector3 vector = Vector3( 1.0f, 1.0f, 1.0f );

			// Act
			const Vector3 negatedVector = vector.Negate();

			// Assert
			Assert::AreEqual( -1.0f, negatedVector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -1.0f, negatedVector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -1.0f, negatedVector.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_Normalise )
		{
			// Arrange
			Vector3     vector   = Vector3( 1.0, 1.0, 1.0 );
			const float expected = RoundToNDecimalPlaces( 0.577351f, 4 );

			// Act
			const Vector3 negatedVector = vector.Normalise();

			// Assert
			Assert::AreEqual( expected, RoundToNDecimalPlaces( negatedVector.x, 4 ), L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( expected, RoundToNDecimalPlaces( negatedVector.y, 4 ), L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( expected, RoundToNDecimalPlaces( negatedVector.z, 4 ), L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_Magnitude )
		{
			// Arrange
			const Vector3 vector = Vector3( 9.0f, 2.0f, 7.0f );

			// Act
			const float length = vector.Magnitude();

			// Assert
			Assert::AreEqual( 11.5758f, RoundToNDecimalPlaces( length, 4 ), L"Magnitude A Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_Cross )
		{
			// Arrange
			Vector3 vectorA = Vector3( 1.0f, 2.0f, 3.0f );
			Vector3 vectorB = Vector3( 3.0f, 2.0f, 1.0f );

			// Act
			Vector3 orthogonal = vectorA.Cross( vectorB );

			// Assert
			Assert::AreEqual( -4.0f, orthogonal.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, orthogonal.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -4.0f, orthogonal.z, L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_Dot )
		{
			// Arrange
			const Vector3 vectorA = Vector3( 9.0f, 2.0f, 7.0f );
			const Vector3 vectorB = Vector3( 4.0f, 8.0f, 10.0f );

			// Act
			const float dotProduct = vectorA.Dot( vectorB );

			// Assert
			Assert::AreEqual( 122.0f, dotProduct, L"Dot Product Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector3_AngleInDegree )
		{
			// Arrange
			const Vector3 vectorA = Vector3( 9.0f, 2.0f, 7.0f );
			const Vector3 vectorB = Vector3( 4.0f, 8.0f, 10.0f );

			// Act
			const float angle = vectorA.AngleInDegrees( vectorB );

			// Assert
			Assert::AreEqual( 38.2f, RoundToNDecimalPlaces( angle, 1 ), L"Angle Incorrect", LINE_INFO() );
		}

	private:
		float RoundToNDecimalPlaces( float target, int decimalPlaces ) const
		{
			return floorf( target * pow( 10, decimalPlaces ) + 0.5 ) / pow( 10, decimalPlaces );
		}
	};
}
