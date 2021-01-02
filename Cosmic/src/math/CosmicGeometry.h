//===============================================================//
/*
	____                        _
  / ____|                      (_)
 | |      ___   ___  _ __ ___   _   ___
 | |     / _ \ / __|| '_ ` _ \ | | / __|
 | |____| (_) |\__ \| | | | | || || (__
  \_____|\___/ |___/|_| |_| |_||_| \___|

*/
//===============================================================//

#pragma once
#include "CosmicMath.h"
namespace cm
{
	/*
		Intersection testing
		-------------------------------------------------------------------------------------------
		|         | Tringle | AABB    | Plane   | Sphere  | Ray     | OBB     | LineSeg | Capsule |
		| Tringle | HALF    | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    |         |
		| Plane   | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    |
		| Sphere  | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    |         |
		| AABB    | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    |         |
		| LineSeg | DONE    | DONE    | DONE    | DONE    |         | DONE    |         |         |
		| Ray     | DONE    | DONE    | DONE    | DONE    |         | DONE    |         |         |
		| OBB     | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    | DONE    |         |
		| Capsule |         |         | DONE    |         |         |         |         |         |
		| Mesh    |         |         |		    |         | DONE    |         |         |         |
		-------------------------------------------------------------------------------------------

		Intersection finding
		--------------------------------------------------------------------------
		|         | AABB    | Plane   | Sphere  | Ray     | OBB     | Capsule |
		| AABB    | SUB     |         | SUB     |         |         |         |
		| Plane   |         |         |         |         |         |         |
		| Sphere  | SUB     |         | SUB     |         |         |         |
		| Ray     | DONE    | DONE    | DONE    |         |         |         |
		| OBB     |         |         |         |         |         |         |
		| Capsule |         |         |         |         |         |         |
		--------------------------------------------------------------------------

		// @TODO: Order .cpp file as appears in .h
		// @TODO: Ray casts take an optional paramter dist, rather reutnr rayCastResult struct ??, overload operator
	*/

#define Intersection_ERROR_TOLLERANCE FLOATING_POINT_ERROR_PRESCION

	struct IntersectionInfo
	{
		Vec3f normal;
		Vec3f close_point;
		Vec3f far_point;
		real32 penetration;
	};

	//************************************
	// Triangle 
	//************************************

	struct Triangle
	{
		Vec3f v0;
		Vec3f v1;
		Vec3f v2;
	};

	inline Triangle CreateTriangle(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
	{
		Triangle tri;

		tri.v0 = v0;
		tri.v1 = v1;
		tri.v2 = v2;

		return tri;
	}

	//************************************
	// Plane
	//************************************

	struct Plane
	{
		Vec3f center;
		Vec3f normal;
	};

	inline Plane CreatePlane(const Vec3f &center, const Vec3f &normal)
	{
		Plane plane;

		plane.center = center;
		plane.normal = Normalize(normal);

		return plane;
	}

	inline Plane CreatePlane(const Triangle &triangle)
	{
		Plane plane;

		plane.normal = Normalize(Cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0));
		plane.center = triangle.v0;

		return plane;
	}

	inline real32 GetPlaneScalar(const Plane &plane)
	{
		real32 distance = Dot(plane.normal, plane.center); // @NOTE: The distance from center to origin ie. Center - Vec3f(0)

		return distance;
	}

	inline bool IsPointInsidePlane(const Plane &plane, const Vec3f &point)
	{
		real32 d = Dot(plane.normal, point);

		bool result = Equal(d, 0.0f, Intersection_ERROR_TOLLERANCE);

		return result;
	}

	inline Vec3f ClosestPointOnPlane(const Plane &plane, const Vec3f &point)
	{
		real32 dot = Dot(plane.normal, point);
		real32 d = GetPlaneScalar(plane);

		real32 dist = (dot - d);

		Vec3f result = point - dist * plane.normal;

		return result;
	}

	//************************************
	// Sphere
	//************************************

	struct Sphere
	{
		Vec4f data; // @NOTE: We pack the radius into the W component
	};

	inline Sphere CreateSphere(const Vec3f &center, const real32 &radius)
	{
		Sphere sphere;

		sphere.data = Vec4f(center, radius);

		return sphere;
	}

	inline Vec3f GetSphereCenter(const Sphere &sphere)
	{
		Vec3f center = Vec3f(sphere.data);

		return center;
	}

	inline real32 GetSphereRadius(const Sphere &sphere)
	{
		real32 radius = sphere.data.w;

		return radius;
	}

	inline Sphere TranslateSphere(Sphere sphere, const Vec3f &translation)
	{
		sphere.data.x += translation.x;
		sphere.data.y += translation.y;
		sphere.data.z += translation.z;

		return sphere;
	}

	inline bool IsPointInsideSphere(const Sphere &sphere, const Vec3f &point)
	{
		real32 r2 = sphere.data.w * sphere.data.w;
		Vec3f sc = GetSphereCenter(sphere);
		real32 d2 = DistanceSqrd(sc, point);

		bool result = d2 <= r2;

		return result;
	}

	inline Vec3f ClosestPointOnSphere(const Sphere &sphere, const Vec3f &point)
	{
		Vec3f sc = GetSphereCenter(sphere);
		Vec3f d = point - sc;

		Vec3f result = sc + (Normalize(d) * sphere.data.w);

		return result;
	}

	//************************************
	// Ellipsoid
	//************************************

	struct Ellipsoid
	{
		Vec3f center;
		Vec3f radius;
	};

	//************************************
	// AABB
	//************************************

	struct AABB
	{
		Vec3f min;
		Vec3f max;
	};

	inline AABB CreateAABBFromCenterRadius(const Vec3f &center, const Vec3f &radius)
	{
		AABB result;

		result.min = center - radius;
		result.max = center + radius;

		return result;
	}

	inline AABB CreateAABBFromMinMax(const Vec3f &min, const Vec3f &max)
	{
		AABB result;

		result.min = min;
		result.max = max;

		return result;
	}

	inline Vec3f GetAABBCenter(const AABB &aabb)
	{
		Vec3f center = (aabb.min + aabb.max) / 2.0f;

		return center;
	}

	inline Vec3f GetAABBRadius(const AABB &aabb)
	{
		Vec3f radius = (aabb.max - aabb.min) / 2.0f;

		return radius;
	}

	inline real32 CalculateAABBVolume(const AABB &aabb)
	{
		real32 x = Abs(aabb.max.x - aabb.min.x);
		real32 y = Abs(aabb.max.y - aabb.min.y);
		real32 z = Abs(aabb.max.z - aabb.min.z);

		real32 result = x * y * z;

		return result;
	}

	inline AABB UpdateAABB(const AABB &aabb, const Quatf &rotation)
	{
		Mat3f rot = QuatToMat3(rotation);
		Vec3f old_extents = GetAABBRadius(aabb);
		Vec3f extents = Vec3f(0);

		for (int32 i = 0; i < 3; i++)
		{
			for (int32 j = 0; j < 3; j++)
			{
				extents[i] += Abs(rot[j][i]) * old_extents[j];
			}
		}

		Vec3f center = GetAABBCenter(aabb);
		AABB result = CreateAABBFromCenterRadius(center, extents);

		return result;
	}

	inline AABB UpdateAABB(const AABB &aabb, const Vec3f &translation, const Quatf &rotation)
	{
		Mat3f rot = QuatToMat3(rotation);

		Vec3f old_center = GetAABBCenter(aabb);
		Vec3f old_extents = GetAABBRadius(aabb);

		Vec3f center = translation;
		Vec3f extents = Vec3f(0);

		for (int32 i = 0; i < 3; i++)
		{
			for (int32 j = 0; j < 3; j++)
			{
				center[i] += rot[j][i] * old_center[j];
				extents[i] += Abs(rot[j][i]) * old_extents[j];
			}
		}

		AABB result = CreateAABBFromCenterRadius(center, extents);

		return result;
	}

	inline AABB GrowAABB(const AABB &aabb, const Vec3f &scale)
	{
		Vec3f center = GetAABBCenter(aabb);
		Vec3f extents = GetAABBRadius(aabb);
		Vec3f new_extents = scale * extents;

		AABB result = CreateAABBFromCenterRadius(center, new_extents);

		return result;
	}

	inline AABB UpdateAABB(const AABB &aabb, const Vec3f &translation, const Quatf &rotation, const Vec3f &scale)
	{
		AABB result = UpdateAABB(aabb, translation, rotation);
		result = GrowAABB(result, scale);

		return result;
	}

	inline void AABBTranslate(AABB *aabb, const Vec3f &translation)
	{
		aabb->min = aabb->min + translation;
		aabb->max = aabb->max + translation;
	}

	inline AABB AABBSetPosition(const AABB &aabb, const Vec3f &position)
	{
		Vec3f extents = GetAABBRadius(aabb);

		AABB result = CreateAABBFromCenterRadius(position, extents);

		return result;
	}

	inline AABB TranslateAABB(const AABB &aabb, const Vec3f &translation)
	{
		AABB result;

		result.min = aabb.min + translation;
		result.max = aabb.max + translation;

		return result;
	}

	inline bool IsPointInsideAABB(const AABB &aabb, const Vec3f &point)
	{
		bool result = point.x >= aabb.min.x &&
			point.x >= aabb.min.z &&
			point.z >= aabb.min.z &&
			point.x <= aabb.max.x &&
			point.y <= aabb.max.y &&
			point.z <= aabb.max.z;

		return result;
	}

	inline Vec3f ClosestPointOnAABB(const AABB &aabb, const Vec3f &point)
	{
		Vec3f result = point;

		result.x = (result.x < aabb.min.x) ? aabb.min.x : result.x;
		result.y = (result.y < aabb.min.y) ? aabb.min.y : result.y;
		result.z = (result.z < aabb.min.z) ? aabb.min.z : result.z;

		result.x = (result.x > aabb.max.x) ? aabb.max.x : result.x;
		result.y = (result.y > aabb.max.y) ? aabb.max.y : result.y;
		result.z = (result.z > aabb.max.z) ? aabb.max.z : result.z;

		return result;
	}

	//************************************
	// Object Orientented box
	//************************************

	struct OBB
	{
		Vec3f extents;
		union // @NOTE: This only works because the Vec3f is padded with an extra 4 bytes
		{
			Mat4f mat; // @NOTE: Usefull for translation/rotations,
					   // @NOTE: Remmeber to set mat[3][3] = 1 BEFORE using it
			struct
			{
				Basisf basis;
				Vec3f center;
			};
		};

		OBB()
		{
		}
	};

	inline OBB CreateOBB(const Vec3f &center, const Vec3f extents)
	{
		OBB obb;
		obb.center = center;
		obb.extents = extents;
		obb.basis = Basisf();

		return obb;
	}

	inline OBB CreateOBB(const Vec3f &center, const Vec3f extents, const Quatf &orientation)
	{
		OBB obb;
		obb.center = center;
		obb.extents = extents;
		obb.basis = Basisf(QuatToMat3(orientation));

		return obb;
	}

	inline OBB CreateOBB(const Vec3f &center, const Vec3f extents, const Vec3f &euler_angles)
	{
		OBB obb;
		obb.center = center;
		obb.extents = extents;
		obb.basis = Basisf(QuatToMat3(EulerToQuat(euler_angles)));

		return obb;
	}

	inline bool IsPointInsideOBB(const OBB &obb, const Vec3f &point)
	{
		Vec3f dir = point - obb.center;

		real32 xp = Dot(dir, obb.basis.right);
		real32 yp = Dot(dir, obb.basis.upward);
		real32 zp = Dot(dir, obb.basis.forward);

		bool result = xp < obb.extents.x && xp > -obb.extents.x &&
			yp < obb.extents.y && yp > -obb.extents.y &&
			zp < obb.extents.z && zp > -obb.extents.z;

		return result;
	}

	inline Vec3f ClosestPointOnOBB(const OBB &obb, const Vec3f &point)
	{
		Vec3f dir = point - obb.center;

		Vec3f p;
		p.x = Dot(dir, obb.basis.right);
		p.y = Dot(dir, obb.basis.upward);
		p.z = Dot(dir, obb.basis.forward);

		p = Clamp(p, -1.0f * obb.extents, obb.extents);

		Vec3f result = obb.center +
			(obb.basis.right * p.x) +
			(obb.basis.upward * p.y) +
			(obb.basis.forward * p.z);

		return result;
	}

	//************************************
	// Line Segement
	//************************************

	// @TODO: Should we pack the length in here?
	struct LineSegment
	{
		Vec3f start;
		Vec3f end;
	};

	inline LineSegment CreateLineSegment(const Vec3f &start, const Vec3f &end)
	{
		LineSegment line;

		line.start = start;
		line.end = end;

		return line;
	}

	inline real32 GetLineSegmentLength(const LineSegment &line)
	{
		real32 result = Distance(line.start, line.end);

		return result;
	}

	inline real32 GetLineSegmentLengthSqrd(const LineSegment &line)
	{
		real32 result = DistanceSqrd(line.start, line.start);

		return result;
	}

	inline Vec3f ClosestPointOnLineSegment(const LineSegment &line, const Vec3f &point)
	{
		Vec3f l = line.end - line.start;

		real32 nume = Dot(point - line.start, l);
		real32 demon = Dot(l, l);

		real32 t = nume / demon;

		t = Clamp(t, 0.0f, 1.0f);

		Vec3f result = line.start + l * t;

		return result;
	}

	inline bool IsPointInsideLineSegment(const LineSegment &line, const Vec3f &point)
	{
		Vec3f close = ClosestPointOnLineSegment(line, point);

		real32 d2 = DistanceSqrd(close, point);

		bool result = Equal(d2, 0.0f, Intersection_ERROR_TOLLERANCE);

		return result;
	}

	inline LineSegment CreateShorestLineBetweenTwoLineSegments(const LineSegment &a, const LineSegment &b)
	{
		// @NOTE: https://math.stackexchange.com/questions/1414285/location-of-shortest-distance-between-two-skew-lines-in-3d
		//		: This is will be a optimatizion. Also not that is only works for Skewed, non-co-planar lines

		Vec3f e1 = ClosestPointOnLineSegment(a, b.start);
		Vec3f e2 = ClosestPointOnLineSegment(a, b.end);

		Vec3f f1 = ClosestPointOnLineSegment(b, e1);
		Vec3f f2 = ClosestPointOnLineSegment(b, e2);

		real32 d1 = DistanceSqrd(e1, f1);
		real32 d2 = DistanceSqrd(e2, f2);

		LineSegment result;

		if (d1 < d2)
		{
			result.start = e1;
			result.end = f1;
		}
		else
		{
			result.start = e2;
			result.end = f2;
		}

		return result;
	}

	//************************************
	// Ray
	//************************************

	struct Ray
	{
		Vec3f origin;
		Vec3f direction;
	};

	inline Ray CreateRay(const Vec3f &origin, const Vec3f &direction)
	{
		Ray ray;

		ray.origin = origin;
		ray.direction = direction;

		return ray;
	}

	inline Vec3f TravelDownRay(const Ray &ray, const real32 &distance)
	{
		Vec3f point = ray.origin + distance * ray.direction;

		return point;
	}

	inline bool IsPointInsideRay(const Ray &ray, const Vec3f &point)
	{
		if (point == ray.origin)
		{
			return true;
		}

		Vec3f dir = Normalize(point - ray.origin);

		real32 dot = Dot(dir, ray.direction);

		bool result = Equal(dot, 1.0f, Intersection_ERROR_TOLLERANCE);

		return result;
	}

	inline Vec3f ClosestPointOnRay(const Ray &ray, const Vec3f &point)
	{
		Vec3f dir = point - ray.origin;
		real32 t = Dot(dir, ray.direction);

		t = (t < 0.0f) ? 0.0f : t;

		Vec3f result = TravelDownRay(ray, t);

		return result;
	}

	//************************************
	// Capsule 
	//************************************

	struct Capsule // @TODO: Should we pack this ?
	{
		Vec3f bot;
		Vec3f top;
		real32 radius;
		real32 length;
	};


	inline Capsule CreateCapsuleFromBotTop(const Vec3f &bot, const Vec3f &top, const real32 &radius)
	{
		Capsule cap = {};

		cap.bot = bot;
		cap.top = top;
		cap.radius = radius;
		cap.length = Distance(bot, top);

		return cap;
	}

	inline Capsule CreateCapsuleFromBaseTip(const Vec3f &base, const Vec3f &tip, const real32 &radius)
	{
		Vec3f dir = Normalize(tip - base);
		Vec3f offset = dir * radius;

		Vec3f bot = base + offset;
		Vec3f top = tip - offset;

		Capsule cap = {};

		cap.bot = bot;
		cap.top = top;
		cap.radius = radius;
		cap.length = Distance(bot, top);

		return cap;
	}

	inline Capsule UpdateCapsule(const Capsule &capsule, const Vec3f &translation)
	{
		Capsule result = capsule;

		result.bot += translation;
		result.top += translation;

		return result;
	}

	struct Frustrum
	{
		Mat4f inv_view_matrix;
		Mat4f inv_projection_matrix;
	};

	struct FrustrumCorners
	{
		union
		{
			struct
			{
				Vec3f far_top_left;
				Vec3f far_top_right;
				Vec3f far_bottom_right;
				Vec3f far_bottom_left;

				Vec3f near_top_left;
				Vec3f near_top_right;
				Vec3f near_bottom_right;
				Vec3f near_bottom_left;
			};

			Vec3f points[8];
		};

		FrustrumCorners() {};
	};

	inline Frustrum CreateFrustrum(const Mat4f &project_matrix, const Mat4f &view_matrix)
	{
		Frustrum f;

		f.inv_projection_matrix = Inverse(project_matrix);
		f.inv_view_matrix = Inverse(view_matrix);

		return f;
	}

	inline FrustrumCorners CalculateFrustrumCorners(const Frustrum &frustrum)
	{
		Vec4f corners[] =
		{
			// @NOTE: Far
			Vec4f(-1, 1, 1, 1),
			Vec4f(1, 1, 1, 1),
			Vec4f(1, -1, 1, 1),
			Vec4f(-1, -1, 1, 1),
			// @NOTE: Near
			Vec4f(-1, 1, -1, 1),
			Vec4f(1, 1, -1, 1),
			Vec4f(1, -1, -1, 1),
			Vec4f(-1, -1, -1, 1)
		};

		FrustrumCorners result;

		for (int32 i = 0; i < 8; i++)
		{
			Vec4f p = corners[i] * frustrum.inv_projection_matrix;
			Vec4f v = p / p.w;
			Vec4f w = v * frustrum.inv_view_matrix;
			result.points[i] = Vec3f(w);
		}

		return result;
	}

	//************************************
	// Triangle functions
	//************************************

	inline Vec3f GetBarycentricTriangle(const Triangle &triangle, const Vec3f &point)
	{
		Vec3f ap = point - triangle.v0;
		Vec3f bp = point - triangle.v1;
		Vec3f cp = point - triangle.v2;

		Vec3f ab = triangle.v1 - triangle.v0;
		Vec3f ac = triangle.v2 - triangle.v0;
		Vec3f bc = triangle.v2 - triangle.v1;
		Vec3f cb = triangle.v1 - triangle.v2;
		Vec3f ca = triangle.v0 - triangle.v2;

		Vec3f v;

		v = ab - Project(ab, cb);
		real32 a = 1.0f - (Dot(v, ap) / Dot(v, ab));

		v = bc - Project(bc, ac);
		real32 b = 1.0f - (Dot(v, bp) / Dot(v, bc));

		v = ca - Project(ca, ab);
		real32 c = 1.0f - (Dot(v, cp) / Dot(v, ca));

		Vec3f result = Vec3f(a, b, c);

		return result;
	}

	inline bool IsPointInsideTriangle(const Triangle &triangle, const Vec3f &point)
	{
		Vec3f a = triangle.v0 - point;
		Vec3f b = triangle.v1 - point;
		Vec3f c = triangle.v2 - point;

		Vec3f u = Cross(b, c);
		Vec3f v = Cross(c, a);
		Vec3f w = Cross(a, b);

		real32 d = Dot(u, v);
		real32 e = Dot(u, w);

		bool result = (d >= 0.0f && e >= 0.0f);

		return result;
	}

	inline Vec3f ClosestPointOnTriangle(const Triangle &triangle, const Vec3f &point)
	{
		Plane plane = CreatePlane(triangle);

		Vec3f result = ClosestPointOnPlane(plane, point);

		if (!IsPointInsideTriangle(triangle, result))
		{
			Vec3f a = ClosestPointOnLineSegment(CreateLineSegment(triangle.v0, triangle.v1), point);
			Vec3f b = ClosestPointOnLineSegment(CreateLineSegment(triangle.v1, triangle.v2), point);
			Vec3f c = ClosestPointOnLineSegment(CreateLineSegment(triangle.v2, triangle.v0), point);

			real32 aa = MagSqrd(point - a);
			real32 bb = MagSqrd(point - b);
			real32 cc = MagSqrd(point - c);

			if (aa <= bb && aa <= cc)
			{
				result = a;
			}
			else if (bb <= aa && bb <= cc)
			{
				result = b;
			}
			else // @NOTE: (cc <= aa && cc <= bb)
			{
				result = c;
			}
		}

		return result;
	}

	typedef std::vector<Triangle> MeshCollider;

	//************************************
	// Basic Intersection tests
	//************************************

	//************************************
	// Line segment tests
	//************************************

	bool CheckIntersectionLineTriangle(const LineSegment &line, const Triangle &triangle);

#define CheckIntersectionTriangleLine(triangle, line) CheckIntersectionLineTriangle(line, triangle)

	bool CheckIntersectionLinePlane(const LineSegment &line, const Plane &plane);

#define CheckIntersectionPlaneLine(plane, line) CheckIntersectionLinePlane(line, plane)

	bool CheckIntersectionLineSphere(const LineSegment &line, const Sphere &sphere);

#define CheckIntersectionSphereLine(sphere, line) CheckIntersectionLineSphere(line, sphere)

	bool CheckIntersectionLineAABB(const LineSegment &line, const AABB &aabb);

#define CheckIntersectionAABBLine(aabb, line) CheckIntersectionLineAABB(line, aabb)

	bool CheckIntersectionLineOBB(const LineSegment &line, const OBB &obb);

#define CheckIntersectionOBBLine(obb, line) CheckIntersectionLineOBB(line, obb)

	//************************************
	// Ray tests
	//************************************
	bool RaycastTriangle(const Ray &ray, const Triangle &triangle, real32 *dist = nullptr);

	bool RaycastMeshColldier(const Ray &ray, const MeshCollider &mesh_collider, real32 *dist = nullptr);

	bool RaycastPlane(const Ray &ray, const Plane &plane, real32 *dist = nullptr);

	bool RaycastSphere(const Ray &ray, const Sphere &sphere, real32 *dist = nullptr);

	bool RaycastAABB(const Ray &ray, const AABB &aabb, real32 *dist = nullptr);

	bool RaycastOBB(const Ray &ray, const OBB &obb, real32 *dist = nullptr);

	//************************************
	// Primitive tests
	//************************************

	bool CheckIntersectionCapsulePlane(const Capsule &capsule, const Plane &plane);// TODO: Complete collision tests

	bool CheckIntersectionCapsuleTriangle(const Capsule &capsule, const Triangle &triangle, IntersectionInfo *info);// TODO: Complete collision tests

	bool CheckIntersectionPlane(const Plane &a, const Plane &b);

	bool CheckIntersectionTriangle(const Triangle &a, const Triangle b);

	bool CheckIntersectionSphere(const Sphere &a, const Sphere &b);

	bool CheckIntersectionAABB(const AABB &a, const AABB &b);

	bool CheckIntersectionOBB(const OBB &a, const OBB &b);

	bool CheckIntersectionAABBSphere(const AABB &aabb, const Sphere &sphere);

#define CheckIntersectionSphereAABB(sphere, aabb) CheckIntersectionAABBSphere(aabb, sphere)

	bool CheckIntersectionOBBSphere(const OBB &obb, const Sphere &sphere);

#define CheckIntersectionSphereOBB(sphere, obb) CheckIntersectionOBBSphere(obb, sphere)

	bool CheckIntersectionPlaneSphere(const Plane &plane, const Sphere &sphere);

#define CheckIntersectionSpherePlane(sphere, plane) CheckIntersectionPlaneSphere(plane, sphere)

	bool CheckIntersectionAABBOBB(const AABB &aabb, const OBB &obb);

#define CheckIntersectionOBBAABB(obb, aabb) CheckIntersectionAABBOBB(aabb, obb)

	bool CheckIntersectionAABBPlane(const AABB &aabb, const Plane &plane);

#define CheckIntersectionPlaneAABB(plane, aabb) CheckIntersectionAABBPlane(aabb, plane)

	bool CheckIntersectionOBBPlane(const OBB &obb, const Plane &plane);

#define CheckIntersectionPlaneOBB(plane, obb) CheckIntersectionOBBPlane(obb, plane)

	bool CheckIntersectionTrianglePlane(const Triangle &triangle, const Plane &plane);

#define CheckIntersectionPlaneTriangle(plane, triangle) CheckIntersectionTrianglePlane(triangle, plane)

	bool CheckIntersectionTriangleSphere(const Triangle &triangle, const Sphere &sphere);

#define CheckIntersectionSphereTriangle(sphere, triangle) CheckIntersectionTriangleSphere(triangle, sphere)

	bool CheckIntersectionTriangleAABB(const Triangle &triangle, const AABB &aabb);

#define CheckIntersectionAABBTriangle(aabb, triangle) CheckIntersectionTriangleAABB(triangle, aabb)

	bool CheckIntersectionTriangleOBB(const Triangle &triangle, const OBB &obb);

#define CheckIntersectionOBBTriangle(obb, triangle) CheckIntersectionTriangleOBB(triangle, obb)

	//************************************
	// Manifold tests
	//************************************

	bool CheckIntersection(const Ray &ray, const Plane &plane, IntersectionInfo *info);

	bool CheckIntersection(const Ray &ray, const AABB &aabb, IntersectionInfo *info);

	bool CheckIntersection(const Ray &ray, const Sphere &sphere, IntersectionInfo *info);

	bool CheckIntersection(const Ray &ray, const OBB &obb, IntersectionInfo *info);

	bool CheckIntersection(const Sphere &sphere, const Triangle &t, IntersectionInfo *info);

	bool CheckIntersection(const Sphere &sphere, const MeshCollider &mesh, IntersectionInfo *info);

	bool CheckIntersection(const Sphere &a, const Sphere &b, IntersectionInfo *info); // @TODO: Doesn't not fill out close/far point. Will it ever ??

	bool CheckIntersection(const AABB &aabb, const Sphere &sphere, IntersectionInfo *info); // @TODO: Doesn't not fill out close/far point. Will it ever ??

	bool CheckIntersection(const AABB &a, const AABB &b, IntersectionInfo *info); // @TODO: Doesn't not fill out close/far point. Will it ever ??

	inline Vec3f RandomPointOnUnitSphere()
	{
		// @HELP: http://corysimon.github.io/articles/uniformdistn-on-sphere/
		real32 r1 = RandomUnillateral<real32>();
		real32 r2 = RandomUnillateral<real32>();

		real32 theta = 2.0f * PI * r1;
		real32 phi = Acos(1.0f - 2.0f * r2);

		real32 sin_phi = Sin(phi);
		real32 cos_phi = Cos(phi);
		real32 cos_theta = Cos(theta);
		real32 sin_theta = Sin(theta);

		real32 x = sin_phi * cos_theta;
		real32 y = sin_phi * sin_theta;
		real32 z = cos_phi;

		Vec3f result = Vec3f(x, y, z);

		return result;
	}

	inline Vec3f RandomPointOnUnitHemisphere(const Vec3f &normal = Vec3f(0, 1, 0))
	{
		Vec3f s = RandomPointOnUnitSphere();

		real32 d = Dot(s, normal);
		Vec3f result = d > 0 ? s : (-1.0f * s);

		return result;
	}


	INTROSPECTED_ENUM(ColliderType) {
		INVALID = 0,
			PLANE,
			SPHERE,
			AABB,
			OBB,
			CAPSULE,
			MESH,
			COUNT
	};

	inline String ToString(const ColliderType &entry)
	{
		switch (entry)
		{
		case ColliderType::INVALID:
			return "INVALID";
		case ColliderType::PLANE:
			return "PLANE";
		case ColliderType::SPHERE:
			return "SPHERE";
		case ColliderType::AABB:
			return "AABB";
		case ColliderType::OBB:
			return "OBB";
		case ColliderType::CAPSULE:
			return "CAPSULE";
		case ColliderType::MESH:
			return "MESH";
		}
		return "error";
	}

	inline ColliderType StringToColliderType(const String &entry)
	{
		if (entry == "INVALID")
			return ColliderType::INVALID;
		if (entry == "PLANE")
			return ColliderType::PLANE;
		if (entry == "SPHERE")
			return ColliderType::SPHERE;
		if (entry == "AABB")
			return ColliderType::AABB;
		if (entry == "OBB")
			return ColliderType::OBB;
		if (entry == "CAPSULE")
			return ColliderType::CAPSULE;
		if (entry == "MESH")
			return ColliderType::MESH;
		return ColliderType::INVALID;
	}

	struct Collider
	{
		ColliderType type;
		MeshCollider mesh; // @NOTE: Required to be outside of union because of std::vector

		union
		{
			Plane plane;
			Sphere sphere;
			AABB aabb;
			OBB obb;
			Capsule capsule;
		};

		bool pad1; // @NOTE: Use when needed
		bool pad2; // @NOTE: Use when needed

		Collider()
		{
			this->type = ColliderType::INVALID;
		}

		Collider(const Plane &plane)
		{
			this->type = ColliderType::PLANE;
			this->plane = plane;
		}

		Collider(const Sphere &sphere)
		{
			this->type = ColliderType::SPHERE;
			this->sphere = sphere;
		}

		Collider(const AABB &aabb)
		{
			this->type = ColliderType::AABB;
			this->aabb = aabb;
		}

		Collider(const OBB &obb)
		{
			this->type = ColliderType::OBB;
			this->obb = obb;
		}

		Collider(const Capsule &capsule)
		{
			this->type = ColliderType::CAPSULE;
			this->capsule = capsule;
		}

		Collider(const MeshCollider &mesh)
		{
			this->type = ColliderType::MESH;
			this->mesh = mesh;
		}

		~Collider()
		{
		}
	};

	bool RayCastColldier(const Ray &ray, const Collider &collider, real32 *dist = nullptr);

	bool CheckIntersectionCollider(const Collider &a, const Collider &b);

	// struct ColliderList
	// {
	// 	int32 count;
	// 	Collider colliders[10];
	// };

}; // namespace cm