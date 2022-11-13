#include "pch.h"
#include "CppUnitTest.h"

#include "Vectors/Vector4.h"

using namespace Artemis::Maths;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArtemisMathsUnitTests
{
	TEST_CLASS( Vector4_UnitTest )
	{
	public:
		TEST_METHOD( Vector4_DefaultConstructor )
		{
			const Vector4 vector;

			Assert::AreEqual( 0.0f, vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, vector.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_SingleConstructor )
		{
			Vector4 vector( 10.0f );

			Assert::AreEqual( 10.0f, vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, vector.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_CopyConstructor )
		{
			Vector4       copy;
			const Vector4 original( 10.0f );

			copy = original;

			Assert::AreEqual( 10.0f, copy.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, copy.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, copy.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, copy.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_TripleConstructor )
		{
			const Vector4 vector( 1.0f, 2.0f, 3.0f, 4.0f );

			Assert::AreEqual( 1.0f, vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, vector.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_AdditionFunction )
		{
			// Arrange
			const Vector4 left  = Vector4( 0.0f );
			const Vector4 right = Vector4( 1.0f );

			// Act
			const Vector4 total = left + right;

			// Assert
			Assert::AreEqual( 1.0f, total.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, total.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, total.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, total.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_SubtractFunction )
		{
			// Arrange 
			const Vector4 left  = Vector4( 1.0f, 2.0f, 3.0f, 4.0f );
			const Vector4 right = left;

			// Act
			const Vector4 total = left - right;

			// Assert
			Assert::AreEqual( 0.0f, total.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, total.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, total.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, total.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_Increment )
		{
			// Arrange
			Vector4       left  = Vector4( 1.0f );
			const Vector4 right = Vector4( 123.0f );

			// Act
			left += right;

			// Assert
			Assert::AreEqual( 124.0f, left.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 124.0f, left.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 124.0f, left.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 124.0f, left.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_Decrement )
		{
			// Arrange
			Vector4       left  = Vector4( 1.0f );
			const Vector4 right = Vector4( 123.0f );

			// Act
			left -= right;

			// Assert
			Assert::AreEqual( -122.0f, left.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -122.0f, left.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -122.0f, left.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -122.0f, left.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_IncrementalMultiplication )
		{
			// Arrange
			Vector4       left  = Vector4( 2.0f );
			const Vector4 right = Vector4( 4.0f );

			// Act
			left *= right;

			// Assert
			Assert::AreEqual( 8.0f, left.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, left.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, left.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, left.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_IncrementalDivision )
		{
			// Arrange
			Vector4       left  = Vector4( 2.0f );
			const Vector4 right = Vector4( 4.0f );

			// Act
			left /= right;

			// Assert
			Assert::AreEqual( 0.5f, left.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.5f, left.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.5f, left.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.5f, left.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Vector4_MultiplyByScalar )
		{
			// Arrange
			const Vector4 vector = Vector4( 1.0f, 2.0f, 3.0f, 4.0f );
			const float   scalar = 5;

			// Act
			const Vector4 newVector = vector * scalar;

			//Assert
			Assert::AreEqual( 5.0f, newVector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, newVector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 15.0f, newVector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 20.0f, newVector.w, L"W Component Incorrect", LINE_INFO() );
		}

		//TEST_METHOD( Vector4_Negate )
		//{
		//	// Arrange
		//	const Vector4 vector = Vector4( 1.0f, 2.0f, 3.0f, 4.0f );

		//	// Act
		//	const Vector4 negatedVector = vector.Negate();

		//	// Assert
		//	Assert::AreEqual( -1.0f, negatedVector.x, L"X Component Incorrect", LINE_INFO() );
		//	Assert::AreEqual( -1.0f, negatedVector.y, L"Y Component Incorrect", LINE_INFO() );
		//	Assert::AreEqual( -1.0f, negatedVector.z, L"Z Component Incorrect", LINE_INFO() );
		//}

		//TEST_METHOD( Vector4_Normalise )
		//{
		//	// Arrange
		//	Vector4     vector   = Vector4( 1.0, 1.0, 1.0 );
		//	const float expected = RoundToNDecimalPlaces( 0.577351f, 4 );

		//	// Act
		//	const Vector4 negatedVector = vector.Normalise();

		//	// Assert
		//	Assert::AreEqual( expected, RoundToNDecimalPlaces( negatedVector.x, 4 ), L"X Component Incorrect", LINE_INFO() );
		//	Assert::AreEqual( expected, RoundToNDecimalPlaces( negatedVector.y, 4 ), L"Y Component Incorrect", LINE_INFO() );
		//	Assert::AreEqual( expected, RoundToNDecimalPlaces( negatedVector.z, 4 ), L"Z Component Incorrect", LINE_INFO() );
		//}

		//TEST_METHOD( Vector4_Magnitude )
		//{
		//	// Arrange
		//	const Vector4 vector = Vector4( 9.0f, 2.0f, 7.0f );

		//	// Act
		//	const float length = vector.Magnitude();

		//	// Assert
		//	Assert::AreEqual( 11.5758f, RoundToNDecimalPlaces( length, 4 ), L"Magnitude A Incorrect", LINE_INFO() );
		//}

		//TEST_METHOD( Vector4_Cross )
		//{
		//	// Arrange
		//	Vector4 vectorA = Vector4( 1.0f, 2.0f, 3.0f );
		//	Vector4 vectorB = Vector4( 3.0f, 2.0f, 1.0f );

		//	// Act
		//	Vector4 orthogonal = vectorA.Cross( vectorB );

		//	// Assert
		//	Assert::AreEqual( -4.0f, orthogonal.x, L"X Component Incorrect", LINE_INFO() );
		//	Assert::AreEqual( 8.0f, orthogonal.y, L"Y Component Incorrect", LINE_INFO() );
		//	Assert::AreEqual( -4.0f, orthogonal.z, L"Z Component Incorrect", LINE_INFO() );
		//}

		TEST_METHOD( Vector4_Dot )
		{
			// Arrange
			const Vector4 vectorA = Vector4( 1.0f, 2.0f, 3.0f, 4.0f );
			const Vector4 vectorB = Vector4( 1.0f, 5.0f, 9.0f, 13.0f );

			// Act
			const float dotProduct = vectorA.Dot( vectorB );

			// Assert
			Assert::AreEqual( 90.0f, dotProduct, L"Dot Product Incorrect", LINE_INFO() );
		}

		//TEST_METHOD( Vector4_AngleInDegree )
		//{
		//	// Arrange
		//	const Vector4 vectorA = Vector4( 9.0f, 2.0f, 7.0f );
		//	const Vector4 vectorB = Vector4( 4.0f, 8.0f, 10.0f );

		//	// Act
		//	const float angle = vectorA.AngleInDegrees( vectorB );

		//	// Assert
		//	Assert::AreEqual( 38.2f, RoundToNDecimalPlaces( angle, 1 ), L"Angle Incorrect", LINE_INFO() );
		//}

	private:
		float RoundToNDecimalPlaces( float target, int decimalPlaces ) const
		{
			return floorf( target * pow( 10, decimalPlaces ) + 0.5 ) / pow( 10, decimalPlaces );
		}
	};
}
