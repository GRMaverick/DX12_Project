#include "pch.h"
#include "CppUnitTest.h"
#include "Misc/Quaternion.h"

using namespace Artemis::Maths;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArtemisMathsUnitTests
{
	TEST_CLASS( Quaternion_UnitTest )
	{
	public:
		TEST_METHOD( Quaternion_DefaultConstructor )
		{
			// Arrange
			// Act
			const Quaternion quaternion;

			// Assert
			Assert::AreEqual( 0.0f, quaternion.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, quaternion.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.0f, quaternion.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 1.0f, quaternion.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_CopyConstructor )
		{
			// Arrange 
			const Quaternion quaternionA( 2.0f, 2.0f, 2.0f, 2.0f );
			Quaternion       finalQuaternion( 1.0f, 1.0f, 1.0f, 1.0f );

			// Act
			finalQuaternion = quaternionA;

			// Assert
			Assert::AreEqual( 2.0f, finalQuaternion.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, finalQuaternion.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, finalQuaternion.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, finalQuaternion.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_VectorScalarConstructor )
		{
			// Arrange 
			// Act
			const Quaternion quaternion( Vector3( 1.0f, 2.0f, 3.0f ), 4.0f );

			// Assert
			Assert::AreEqual( 1.0f, quaternion.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, quaternion.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, quaternion.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, quaternion.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_ComponentWiseConstructor )
		{
			// Arrange
			// Act	
			const Quaternion quaternion( 1.0f, 2.0f, 3.0f, 4.0f );

			// Assert
			Assert::AreEqual( 1.0f, quaternion.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 2.0f, quaternion.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 3.0f, quaternion.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, quaternion.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_QuaternionMultiplication )
		{
			// Arrange
			const Quaternion aQuaternion( 1.0f, 1.0f, 1.0f, 1.0f );
			const Quaternion bQuaternion( 2.0f, 2.0f, 2.0f, 2.0f );
			Quaternion       finalQuaternion;

			// Act
			finalQuaternion = aQuaternion * bQuaternion;

			// Asssert
			Assert::AreEqual( 4.0f, finalQuaternion.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalQuaternion.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalQuaternion.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -4.0f, finalQuaternion.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_IncrementalAddition )
		{
			// Arrange
			const Quaternion quaternion( 1.0f, 2.0f, 3.0f, 4.0f );
			Quaternion       finalQuaternion( 5.0f, 6.0f, 7.0f, 8.0f );

			// Act
			finalQuaternion += quaternion;

			// Assert
			Assert::AreEqual( 6.0f, finalQuaternion.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, finalQuaternion.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 10.0f, finalQuaternion.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 12.0f, finalQuaternion.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_IncrementalSubtraction )
		{
			// Arrange
			const Quaternion quaternion( 1.0f, 2.0f, 3.0f, 4.0f );
			Quaternion       finalQuaternion( 5.0f, 6.0f, 7.0f, 8.0f );

			// Act
			finalQuaternion -= quaternion;

			// Assert
			Assert::AreEqual( 4.0f, finalQuaternion.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalQuaternion.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalQuaternion.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalQuaternion.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_IncrementalQuaternionMultiplication )
		{
			// Arrange
			Quaternion       finalQuaternion( 1.0f, 1.0f, 1.0f, 1.0f );
			const Quaternion aQuaternion( 2.0f, 2.0f, 2.0f, 2.0f );

			// Act
			finalQuaternion *= aQuaternion;

			// Asssert
			Assert::AreEqual( 4.0f, finalQuaternion.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalQuaternion.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 4.0f, finalQuaternion.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -4.0f, finalQuaternion.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_Conjugate )
		{
			const Quaternion quaternion( 5.0f, 6.0f, 7.0f, 8.0f );

			// Act
			const Quaternion conjugate = quaternion.Conjugate();

			// Assert
			Assert::AreEqual( -5.0f, conjugate.vector.x, L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -6.0f, conjugate.vector.y, L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( -7.0f, conjugate.vector.z, L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 8.0f, conjugate.w, L"W Component Incorrect", LINE_INFO() );
		}

		TEST_METHOD( Quaternion_FromEuler )
		{
			// Arrange
			Quaternion    quat        = Quaternion();
			const Vector3 eulerAngles = Vector3( 30.0f, 60.0f, 45.0f );

			// Act
			quat.FromEuler( eulerAngles );

			Assert::AreEqual( 0.391903f, RoundToNDecimalPlaces( quat.vector.x, 6 ), L"X Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.360423f, RoundToNDecimalPlaces( quat.vector.y, 6 ), L"Y Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.439679f, RoundToNDecimalPlaces( quat.vector.z, 6 ), L"Z Component Incorrect", LINE_INFO() );
			Assert::AreEqual( 0.723318f, RoundToNDecimalPlaces( quat.w, 6 ), L"W Component Incorrect", LINE_INFO() );
		}

	private:
		float RoundToNDecimalPlaces( float target, int decimalPlaces ) const
		{
			return floorf( target * pow( 10, decimalPlaces ) + 0.5 ) / pow( 10, decimalPlaces );
		}
	};
}
