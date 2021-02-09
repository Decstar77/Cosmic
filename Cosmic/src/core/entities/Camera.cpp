#include "Camera.h"



namespace cm
{
	void Camera::Update(const real32 &dt)
	{
	}

	cm::Mat4f Camera::CalculateViewMatrix() const
	{
		Mat4 view_matrix = this->GetGlobalTransformMatrix();//transform.CalculateTransformMatrix();
		view_matrix = Inverse(view_matrix);
		return view_matrix;
	}

	cm::Mat4f Camera::CalculateProjectionMatrix() const
	{
		Mat4f projection_matrix;

		switch (camera_type)
		{
		case CameraType::PERSPECTIVE:
			projection_matrix = PerspectiveLH(DegToRad(dfovy), aspect, znear, zfar);
			break;
		case CameraType::ORTHOGRAPHIC:
			projection_matrix = OrthographicLH(left, right, top, bottom, near_plane, far_plane);
			break;
		default:
			break;
		}

		return projection_matrix;
	}

	cm::Frustrum Camera::GetFrustrum() const
	{
		return CreateFrustrum(CalculateProjectionMatrix(), CalculateViewMatrix());
	}

	cm::Ray Camera::ShootRay(const Vec2f &mouse_position, const Vec2f &window_dimenions)
	{
		Mat4f proj = CalculateProjectionMatrix();
		Mat4f view = CalculateViewMatrix();

		Vec4f normal_coords = GetNormalisedDeviceCoordinates(window_dimenions.x, window_dimenions.y, mouse_position.x, mouse_position.y);
		Vec4f view_coords = ToViewCoords(proj, normal_coords);

		// @NOTE: This 1 ensure we a have something pointing in to the screen
		view_coords = Vec4f(view_coords.x, view_coords.y, 1, 0);
		Vec3f world_coords = ToWorldCoords(view, view_coords);

		Ray ray;
		ray.origin = GetGlobalPosition();
		ray.direction = Normalize(Vec3f(world_coords.x, world_coords.y, world_coords.z));

		return ray;
	}

	Camera::Camera() : camera_type(CameraType::PERSPECTIVE), dfovy(45.0f), aspect(1.77f), znear(0.3f), zfar(1000.0f)
	{
		type = EntityType::CAMERA;
		if (!active_camera)
		{
			active_camera = this;
		}
	}

	Camera::~Camera()
	{
		if (active_camera == this)
		{
			active_camera = nullptr;
		}
	}

}