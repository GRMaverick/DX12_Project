#include "Intersections.h"

#include <math.h>

namespace Artemis::Maths
{
	//
	// Finds scalar projection of vector u in the direction of v
	//
	float ScalarProjection( const Vector3& _u, const Vector3& _v )
	{
		if ( _v.IsNormalised() )
		{
			return _u.Dot( _v );
		}
		else
		{
			return _u.Dot( _v ) / _v.Magnitude();
		}
	}

	//
	// Finds vector projection of vector u in the direction of v
	//
	Vector3 VectorProjection( const Vector3& _u, const Vector3& _v )
	{
		const float puvMag = ScalarProjection( _u, _v );
		if ( _v.IsNormalised() )
		{
			return _v * puvMag;
		}
		else
		{
			return (_v / _v.Magnitude()) * puvMag;
		}
	}

	//
	// Finds the distance between a line and a point
	//
	float DistancePointToLine( const Line& _line, const Vector3& _point )
	{
		const Vector3 vectorToProject = _line.GetPoint() - _point;
		const Vector3 toProjectOn     = _line.GetDirection();

		const Vector3 projection = VectorProjection( vectorToProject, toProjectOn );

		return (vectorToProject - projection).Magnitude();
	}

	//
	// Detemines an intersection between a line and sphere
	//
	bool LineSphereIntersect( const Line& _line, const Vector3& _sCentre, const float& _sRadius )
	{
		return (DistancePointToLine( _line, _sCentre ) < _sRadius);
	}

	//
	// Determines an intersection between a line and a plane
	//
	bool LinePlaneIntersection( const Plane& _plane, const Line& _line, Vector3& _intersectPoint )
	{
		const Vector3 lineVector        = _line.GetDirection() - _line.GetPoint();
		const Vector3 lineToPlaneCentre = _line.GetPoint() - _plane.GetPoint();

		const float dotlinePlaneNormal = _plane.GetNormal().Dot( lineVector );
		const float dotNormCentre      = -_plane.GetNormal().Dot( lineToPlaneCentre );

		if ( fabs( dotlinePlaneNormal ) < 1e-06 )
		{
			return false; // segment is parallel to plane
		}

		const float sI = dotNormCentre / dotlinePlaneNormal;
		if ( sI < 0 || sI > 1 )
		{
			return 0;
		}

		_intersectPoint = _line.GetPoint() + (lineVector * sI);

		if ( _plane.GetExtents().x != -1.0f && _plane.GetExtents().y != -1.0f && _plane.GetExtents().z != -1.0f )
		{
			const float xMin = _plane.GetPoint().x - _plane.GetExtents().x;
			const float xMax = _plane.GetPoint().x + _plane.GetExtents().x;
			const float yMin = _plane.GetPoint().y - _plane.GetExtents().y;
			const float yMax = _plane.GetPoint().y + _plane.GetExtents().y;

			if ( xMin < _intersectPoint.x && _intersectPoint.x < xMax && yMin < _intersectPoint.y && _intersectPoint.y < yMax )
			{
				return true; // In area of collision
			}
			else
			{
				return false; // Outside area of collision
			}
		}
		return true;
	}
}
