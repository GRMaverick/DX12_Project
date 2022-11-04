#include "pch.h"
#include "CppUnitTest.h"

#include "Functions/Intersections.h"

using namespace Artemis::Maths;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArtemisMathsUnitTests
{
	TEST_CLASS( Intersections_UnitTest )
	{
	public:
		TEST_METHOD( Intersections_VectorProjection )
		{
			// Arrange
			const Vector3 a = Vector3( 2.0f, 3.0f, 1.0f );
			const Vector3 b = Vector3( 5.0f, -2.0f, 2.0f );

			// Act
			const Vector3 q2 = VectorProjection( a, b );

			// Assert
			Assert::AreEqual( RoundToNDecimalPlaces( 0.909091f, 4 ), RoundToNDecimalPlaces( q2.x, 4 ), L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( RoundToNDecimalPlaces( -0.363636f, 4 ), RoundToNDecimalPlaces( q2.y, 4 ), L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( RoundToNDecimalPlaces( 0.363636f, 4 ), RoundToNDecimalPlaces( q2.z, 4 ), L"Z Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Intersections_ScalarProjection )
		{
			// Arrange
			const Vector3 a = Vector3( 2.0f, 3.0f, 1.0f );
			const Vector3 b = Vector3( 5.0f, -2.0f, 2.0f );

			// Act
			const float sProjection = ScalarProjection( a, b );

			Assert::AreEqual( RoundToNDecimalPlaces( 1.0445f, 4 ), RoundToNDecimalPlaces( sProjection, 4 ), L"Scalar Projection Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Intersections_DistancePointToLine )
		{
			// Arrange
			const Vector3 point = Vector3( 3.0f, 1.0f, 9.0f );
			const Line    line  = Line( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 2.0f, 4.0f, 6.0f ).Normalise() );

			// Act
			const float final = DistancePointToLine( line, point );

			// Assert
			Assert::AreEqual( RoundToNDecimalPlaces( 4.225771f, 3 ), RoundToNDecimalPlaces( final, 3 ), L"Distance Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Intersections_LineSphereIntersectTrue )
		{
			// Arrange
			constexpr float sphereRadius = 5.0f;
			const Vector3   sphereCentre = Vector3( 3.0f, 1.0f, 9.0f );
			const Line      line         = Line( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 2.0f, 4.0f, 6.0f ).Normalise() );

			// Act
			const bool intersect = LineSphereIntersect( line, sphereCentre, sphereRadius );

			// Assert
			Assert::IsTrue( intersect, L"Collision Failed", LINE_INFO() );
		}

		TEST_METHOD( Intersections_LineSphereIntersectFalse )
		{
			// Arrange
			constexpr float sphereRadius = 2.0f;
			const Vector3   sphereCentre = Vector3( 3.0f, 1.0f, 9.0f );
			const Line      line         = Line( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 2.0f, 4.0f, 6.0f ).Normalise() );

			// Act
			const bool intersect = LineSphereIntersect( line, sphereCentre, sphereRadius );

			// Assert
			Assert::IsFalse( intersect, L"Collision Occurred", LINE_INFO() );
		}

		TEST_METHOD( Intersections_LinePlaneIntersectTrue )
		{
			Vector3     iPoint;
			const Line  line  = Line( Vector3( 0.0f, 0.0f, -2.0f ), Vector3( 0.0f, 0.0f, 1.0f ) );
			const Plane plane = Plane( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 0.0f, 0.0f, -1.0f ) );

			const bool result = LinePlaneIntersection( plane, line, iPoint );

			Assert::IsTrue( result, L"Intersection Failed", LINE_INFO() );
		}

		TEST_METHOD( Intersections_LinePlaneIntersectFalse )
		{
			Vector3     iPoint;
			const Line  line  = Line( Vector3( 0.0f, 0.0f, -2.0f ), Vector3( 0.0f, 0.0f, -1.0f ) );
			const Plane plane = Plane( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 0.0f, 0.0f, -1.0f ) );

			const bool result = LinePlaneIntersection( plane, line, iPoint );

			Assert::IsFalse( result, L"Intersection Passed", LINE_INFO() );
		}

	private:
		float RoundToNDecimalPlaces( float target, int decimalPlaces ) const
		{
			return floorf( target * pow( 10, decimalPlaces ) + 0.5 ) / pow( 10, decimalPlaces );
		}
	};
}
