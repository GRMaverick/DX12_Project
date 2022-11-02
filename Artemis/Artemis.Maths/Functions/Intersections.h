#pragma once

#include "Geometry/Line.h"
#include "Geometry/Plane.h"

namespace Artemis::Maths
{
	//
	// Finds scalar projection of vector u in the direction of v
	//
	float ScalarProjection( const Vector3& _u, const Vector3& _v );

	//
	// Finds vector projection of vector u in the direction of v
	//
	Vector3 VectorProjection( const Vector3& _u, const Vector3& _v );

	//
	// Finds the distance between a line and a point
	//
	float DistancePointToLine( const Line& _line, const Vector3& _point );

	//
	// Determines an intersection between a line and sphere
	//
	bool LineSphereIntersect( const Line& _line, const Vector3& _sCentre, const float& _sRadius );

	//
	// Determines an intersection between a line and a plane
	//
	bool LinePlaneIntersection( const Plane& _plane, const Line& _line, Vector3& _intersectPoint );
}
