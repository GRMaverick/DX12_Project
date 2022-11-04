#pragma once

#include <vector>

#include "Vectors/Vector3.h"

namespace Artemis::Maths
{
	class NarrowPhaseCollisionDetection
	{
	public:
		static bool GjkAlgorithm( const std::vector<Vector3>& _aVertices, const std::vector<Vector3>& _bVertices );

	private:
		static Vector3      v;
		static Vector3      b, c, d;
		static unsigned int n; //simplex size

		static bool Update( const Vector3& _a );
		static bool CheckOneFaceAb( const Vector3& a, const Vector3& b, const Vector3& c );
		static bool CheckOneFaceAc( const Vector3& a, const Vector3& b, const Vector3& c );
		static bool CheckTwoFaces( Vector3& _abc, const Vector3& acd, Vector3& _ac, Vector3& _ab, const Vector3& ad, const Vector3& _ao );

		static Vector3 AveragePoint( const std::vector<Vector3>& _vertices );
		static Vector3 SupportPoint( const std::vector<Vector3>& _aVertices, const std::vector<Vector3>& _bVertices, const Vector3& _supportingDirection );
		static Vector3 TripleProduct( const Vector3& _ab, const Vector3& _c );
		static int     IndexOfFurthestPoint( const std::vector<Vector3>& _vertices, const Vector3& _supportingDirection );
	};
}
