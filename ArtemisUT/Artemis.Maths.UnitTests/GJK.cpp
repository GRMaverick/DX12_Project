#include "pch.h"
#include "CppUnitTest.h"

#include <vector>

#include "Vectors/Vector3.h"
#include "Functions/CollisionDetections.h"

using namespace Artemis::Maths;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArtemisMathsUnitTests
{
	TEST_CLASS( GJK_UnitTest )
	{
	public:
		TEST_METHOD( GJK_Failed )
		{
			// Arrange
			const Vector3 position( 3.0f, 3.0f, 3.0f );

			std::vector<Vector3> aObjectVertices;
			aObjectVertices.push_back( Vector3( -1, -1, -1 ) ); // Front
			aObjectVertices.push_back( Vector3( 1, -1, -1 ) );
			aObjectVertices.push_back( Vector3( -1, 1, -1 ) );
			aObjectVertices.push_back( Vector3( 1, 1, -1 ) );
			aObjectVertices.push_back( Vector3( 1, 1, 1 ) ); // Back
			aObjectVertices.push_back( Vector3( 1, -1, 1 ) );
			aObjectVertices.push_back( Vector3( -1, 1, 1 ) );
			aObjectVertices.push_back( Vector3( -1, -1, 1 ) );

			std::vector<Vector3> bObjectVertices;
			bObjectVertices.push_back( position + Vector3( -1, -1, -1 ) ); // Front
			bObjectVertices.push_back( position + Vector3( 1, -1, -1 ) );
			bObjectVertices.push_back( position + Vector3( -1, 1, -1 ) );
			bObjectVertices.push_back( position + Vector3( 1, 1, -1 ) );
			bObjectVertices.push_back( position + Vector3( 1, 1, 1 ) ); // Back
			bObjectVertices.push_back( position + Vector3( 1, -1, 1 ) );
			bObjectVertices.push_back( position + Vector3( -1, 1, 1 ) );
			bObjectVertices.push_back( position + Vector3( -1, -1, 1 ) );

			// Act
			const bool isCollisionDetected = NarrowPhaseCollisionDetection::GjkAlgorithm( aObjectVertices, bObjectVertices );

			// Assert
			Assert::IsFalse( isCollisionDetected );
		}

		TEST_METHOD( GJK_Succeeded )
		{
			// Arrange
			const Vector3 position( 2.0f, 2.0f, 2.0f );

			std::vector<Vector3> aObjectVertices;
			aObjectVertices.push_back( Vector3( -1, -1, -1 ) ); // Front
			aObjectVertices.push_back( Vector3( 1, -1, -1 ) );
			aObjectVertices.push_back( Vector3( -1, 1, -1 ) );
			aObjectVertices.push_back( Vector3( 1, 1, -1 ) );
			aObjectVertices.push_back( Vector3( 1, 1, 1 ) ); // Back
			aObjectVertices.push_back( Vector3( 1, -1, 1 ) );
			aObjectVertices.push_back( Vector3( -1, 1, 1 ) );
			aObjectVertices.push_back( Vector3( -1, -1, 1 ) );

			std::vector<Vector3> bObjectVertices;
			bObjectVertices.push_back( position + Vector3( -1, -1, -1 ) ); // Front
			bObjectVertices.push_back( position + Vector3( 1, -1, -1 ) );
			bObjectVertices.push_back( position + Vector3( -1, 1, -1 ) );
			bObjectVertices.push_back( position + Vector3( 1, 1, -1 ) );
			bObjectVertices.push_back( position + Vector3( 1, 1, 1 ) ); // Back
			bObjectVertices.push_back( position + Vector3( 1, -1, 1 ) );
			bObjectVertices.push_back( position + Vector3( -1, 1, 1 ) );
			bObjectVertices.push_back( position + Vector3( -1, -1, 1 ) );

			// Act
			const bool isCollisionDetected = NarrowPhaseCollisionDetection::GjkAlgorithm( aObjectVertices, bObjectVertices );

			// Assert
			Assert::IsTrue( isCollisionDetected );
		}
	};
}
