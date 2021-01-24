#pragma once
#include "Cosmic.h"
#include "src/core/entities/components/NavMesh.h"
#include "src/rendering/DirectX11Renderer.h"
namespace cm
{
	class Debug
	{
	public:
		StringStream log_info;
		StringStream error_info;

		static void ClearLogger()
		{
			instance->log_info.clear();
			instance->log_info.str("");
			instance->error_info.clear();
			instance->error_info.str("");
		}

		static String GetLogInfo()
		{
			return instance->log_info.str();
		}

		static String GetErrorInfo()
		{
			return instance->error_info.str();
		}

		template<typename T>
		static void Log(const T &msg)
		{
			instance->log_info << msg << std::endl;
		}

		static void Log(const Vec3f &msg)
		{
			instance->log_info << ToString(msg) << std::endl;
		}

		template<typename T>
		static void LogError(const T &msg)
		{
			instance->error_info << msg << std::endl;
		}

	public:
		static void Push(const Vec3f &center, const real32 &distance = 1.0f)
		{
			ASSERT(renderer, "Debug renderer does not exist");
			renderer->PushNewLine(center - Vec3f(distance, 0, 0), center + Vec3f(distance, 0, 0));
			renderer->PushNewLine(center - Vec3f(0, distance, 0), center + Vec3f(0, distance, 0));
			renderer->PushNewLine(center - Vec3f(0, 0, distance), center + Vec3f(0, 0, distance));
		}

		static void Push(const Vec4f &center, const real32 &distance = 1.0f)
		{
			ASSERT(renderer, "Debug renderer does not exist");
			renderer->PushNewLine(center - Vec4f(distance, 0, 0, 0), center + Vec4f(distance, 0, 0, 0));
			renderer->PushNewLine(center - Vec4f(0, distance, 0, 0), center + Vec4f(0, distance, 0, 0));
			renderer->PushNewLine(center - Vec4f(0, 0, distance, 0), center + Vec4f(0, 0, distance, 0));
		}

		static void Push(const Vec3f &p0, const Vec3f &p1)
		{
			ASSERT(renderer, "Debug renderer does not exist");
			renderer->PushNewLine(p0, p1);
		}

		static void Push(const Vec4f &p0, const Vec4f &p1)
		{
			ASSERT(renderer, "Debug renderer does not exist");
			renderer->PushNewLine(p0, p1);
		}

		static void Push(const LineSegment &line)
		{
			ASSERT(renderer, "Debug renderer does not exist");
			renderer->PushNewLine(line.start, line.end);
		}

		static void Push(const Ray &ray, const real32 &distance = 10.f)
		{
			ASSERT(renderer, "Debug renderer does not exist");
			renderer->PushNewLine(ray.origin, ray.origin + ray.direction * distance);
		}

		static void Push(const Triangle &triangle)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			renderer->PushNewLine(triangle.v0, triangle.v1);
			renderer->PushNewLine(triangle.v1, triangle.v2);
			renderer->PushNewLine(triangle.v2, triangle.v0);
		}

		static void Push(const Plane &plane)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			renderer->PushNewLine(plane.center, plane.center + plane.normal);
		}

		static void Push(const MeshCollider &triangle_list)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			for (int32 i = 0; i < (int32)triangle_list.size(); i++)
			{
				Triangle triangle = triangle_list.at(i);

				renderer->PushNewLine(triangle.v0, triangle.v1);
				renderer->PushNewLine(triangle.v1, triangle.v2);
				renderer->PushNewLine(triangle.v2, triangle.v0);
			}
		}

		static void Push(const Basisf &basis, const Vec3f &position)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			renderer->PushNewLine(position, position + basis.right);
			renderer->PushNewLine(position, position + basis.upward);
			renderer->PushNewLine(position, position + basis.forward);
		}

		static void Push(const Sphere &sphere)
		{
			ASSERT(renderer, "Debug renderer does not exist");
			Vec3f center = GetSphereCenter(sphere);
			real32 radius = GetSphereRadius(sphere);

			real32 angle_inc = 25.5;
			Vec3f last = Vec3f(center.x, center.y, center.z);

			for (real32 angle = 0; angle <= 360; angle += angle_inc)
			{
				Vec3f next;

				next.x = Cos(DegToRad(angle)) * radius;
				next.z = Sin(DegToRad(angle)) * radius;

				next += center;

				renderer->PushNewLine(last, next);
				last = next;
			}
			last = Vec3f(center.x, center.y, center.z);

			for (real32 angle = 0; angle <= 360; angle += angle_inc)
			{
				Vec3f next;

				next.x = Cos(DegToRad(angle)) * radius;
				next.y = Sin(DegToRad(angle)) * radius;

				next += center;

				renderer->PushNewLine(last, next);
				last = next;
			}
			last = Vec3f(center.x, center.y, center.z);

			for (real32 angle = 0; angle <= 360; angle += angle_inc)
			{
				Vec3f next;

				next.z = Cos(DegToRad(angle)) * radius;
				next.y = Sin(DegToRad(angle)) * radius;

				next += center;

				renderer->PushNewLine(last, next);
				last = next;
			}
		}

		static void Push(const Ellipsoid &ellipsoid)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			Vec3f right = ellipsoid.center + Vec3f(ellipsoid.radius.x, 0, 0);
			Vec3f left = ellipsoid.center - Vec3f(ellipsoid.radius.x, 0, 0);

			Vec3f top = ellipsoid.center + Vec3f(0, ellipsoid.radius.y, 0);
			Vec3f bottom = ellipsoid.center - Vec3f(0, ellipsoid.radius.y, 0);

			Vec3f front = ellipsoid.center + Vec3f(0, 0, ellipsoid.radius.z);
			Vec3f back = ellipsoid.center - Vec3f(0, 0, ellipsoid.radius.z);

			Debug::Push(right, front);
			Debug::Push(right, back);
			Debug::Push(right, top);
			Debug::Push(right, bottom);

			Debug::Push(left, front);
			Debug::Push(left, back);
			Debug::Push(left, top);
			Debug::Push(left, bottom);
		}

		static void Push(const AABB &aabb)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			Vec3f min = aabb.min;
			Vec3f max = aabb.max;

			Vec3f v2 = Vec3f(max.x, min.y, min.z);
			Vec3f v3 = Vec3f(max.x, max.y, min.z);
			Vec3f v4 = Vec3f(min.x, max.y, min.z);

			Vec3f v6 = Vec3f(max.x, min.y, max.z);
			Vec3f v7 = Vec3f(min.x, min.y, max.z);
			Vec3f v8 = Vec3f(min.x, max.y, max.z);

			renderer->PushNewLine(min, v2);
			renderer->PushNewLine(min, v4);
			renderer->PushNewLine(min, v7);
			renderer->PushNewLine(max, v6);
			renderer->PushNewLine(max, v8);
			renderer->PushNewLine(max, v3);
			renderer->PushNewLine(v3, v2);
			renderer->PushNewLine(v3, v4);
			renderer->PushNewLine(v2, v6);
			renderer->PushNewLine(v6, v7);
			renderer->PushNewLine(v8, v7);
			renderer->PushNewLine(v8, v4);
		}

		static void Push(const OBB &obb)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			Mat4f mat(obb.basis.mat, obb.center);
			Vec3f extents = obb.extents;

			Vec4f v0 = Vec4f(extents, 1) * mat;
			Vec4f v1 = Vec4f(extents * -1.0f, 1) * mat;

			Vec4f v2 = (Vec4f(-extents.x, extents.y, extents.z, 1)) * mat;
			Vec4f v3 = (Vec4f(extents.x, -extents.y, extents.z, 1)) * mat;
			Vec4f v4 = (Vec4f(extents.x, extents.y, -extents.z, 1)) * mat;

			Vec4f v5 = (Vec4f(-extents.x, -extents.y, extents.z, 1)) * mat;
			Vec4f v6 = (Vec4f(extents.x, -extents.y, -extents.z, 1)) * mat;
			Vec4f v7 = (Vec4f(-extents.x, extents.y, -extents.z, 1)) * mat;

			renderer->PushNewLine(v0, v2);
			renderer->PushNewLine(v0, v4);
			renderer->PushNewLine(v0, v3);
			renderer->PushNewLine(v1, v5);
			renderer->PushNewLine(v1, v7);
			renderer->PushNewLine(v1, v6);
			renderer->PushNewLine(v3, v6);
			renderer->PushNewLine(v3, v5);
			renderer->PushNewLine(v2, v5);
			renderer->PushNewLine(v2, v7);
			renderer->PushNewLine(v4, v7);
			renderer->PushNewLine(v4, v6);
		}

		static void Push(const Capsule &capsule)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			Vec3f bot = capsule.bot;
			Vec3f top = capsule.top;

			Vec3f dir = Normalize(capsule.top - capsule.bot);

			real32 angle_inc = 25.5;
			real32 radius = capsule.radius;

			Sphere s1 = CreateSphere(bot, radius);
			Sphere s2 = CreateSphere(top, radius);

			Push(s1);
			Push(s2);

			Vec3f up = Vec3f(0, 1, 0);
			Vec3f forward = Normalize(dir);
			Vec3f right = Normalize(Cross(up, forward));

			if (Equal(right, Vec3f(0.0f)))
			{
				up = Vec3f(-1, 0, 0);
				right = Normalize(Cross(up, forward));
			}

			up = (Cross(forward, right));

			Push(bot + right * radius, top + right * radius);
			Push(bot - right * radius, top - right * radius);
			Push(bot + up * radius, top + up * radius);
			Push(bot - up * radius, top - up * radius);
		}

		static void Push(const Collider	&collider)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			switch (collider.type)
			{
			case ColliderType::PLANE:
				Push(collider.plane);
				break;
			case ColliderType::SPHERE:
				Push(collider.sphere);
				break;
			case ColliderType::AABB:
				Push(collider.aabb);
				break;
			case ColliderType::OBB:
				Push(collider.obb);
				break;
			case ColliderType::CAPSULE:
				Push(collider.capsule);
			case ColliderType::MESH:
				Push(collider.mesh);
			}
		}

		static void Push(const Frustrum &frustrum, const real32 &distance = 50.0f)
		{
			ASSERT(renderer, "Debug renderer does not exist");

			FrustrumCorners corners = CalculateFrustrumCorners(frustrum);

			Debug::Push(corners.far_top_left, corners.far_top_right);
			Debug::Push(corners.far_top_right, corners.far_bottom_right);
			Debug::Push(corners.far_bottom_right, corners.far_bottom_left);
			Debug::Push(corners.far_bottom_left, corners.far_top_left);

			Debug::Push(corners.near_top_left, corners.near_top_right);
			Debug::Push(corners.near_top_right, corners.near_bottom_right);
			Debug::Push(corners.near_bottom_right, corners.near_bottom_left);
			Debug::Push(corners.near_bottom_left, corners.near_top_left);

			Vec3f top_left_dir = -1.0f * Normalize(corners.near_top_left - corners.far_top_left) * distance;
			Vec3f top_right = -1.0f * Normalize(corners.near_top_right - corners.far_top_right) * distance;
			Vec3f bottom_right = -1.0f * Normalize(corners.near_bottom_right - corners.far_bottom_right) * distance;
			Vec3f bottom_left = -1.0f * Normalize(corners.near_bottom_left - corners.far_bottom_left) * distance;

			Debug::Push(corners.near_top_left, corners.near_top_left + top_left_dir);
			Debug::Push(corners.near_top_right, corners.near_top_right + top_right);
			Debug::Push(corners.near_bottom_right, corners.near_bottom_right + bottom_right);
			Debug::Push(corners.near_bottom_left, corners.near_bottom_left + bottom_left);

			//Debug::Push(corners.near_top_left, corners.far_top_left);
			//Debug::Push(corners.near_top_right, corners.far_top_right);
			//Debug::Push(corners.near_bottom_right, corners.far_bottom_right);
			//Debug::Push(corners.near_bottom_left, corners.far_bottom_left);
		}

		static void Push(const NavMesh &mesh)
		{
			ASSERT(renderer, "Debug renderer does not exist");
			for (const NavMesh::IndexedTriangle &tri : mesh.triangles)
			{
				Push(mesh.IndexedTriangleToTriangle(tri));
			}
		}

		static void Push(const std::vector<Vec3f> &path, const Vec3f &offset = Vec3f(0))
		{
			ASSERT(renderer, "Debug renderer does not exist");
			if (path.size() > 0)
			{
				for (int32 i = 0; i < (path.size() - 1); i++)
				{
					Push(path.at(i) + offset, path.at(i + 1) + offset);
				}
			}
		}

	public:
		Debug(DirectXDebugRenderer *renderer) { Debug::renderer = renderer; instance = this; }
		Debug(const Debug &) = delete;
		Debug &operator=(const Debug &) = delete;

	private:
		inline static Debug *instance = nullptr;
		inline static DirectXDebugRenderer *renderer = nullptr;
	};
}