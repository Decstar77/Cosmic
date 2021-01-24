#pragma once
#include "Entity.h"

namespace cm
{
	INTROSPECTED_ENUM(CameraType) {
		INVALID = 0,
			PERSPECTIVE,
			ORTHOGRAPHIC,
			COUNT
	};

	extern inline String ToString(const CameraType &entry);
	extern inline CameraType StringToCameraType(const String &entry);

	class Camera : public Entity // @Display
	{
	public:
		CameraType camera_type;

		union
		{
			struct
			{
				real32 dfovy;	// @Display
				real32 aspect;	// @Display
				real32 znear;	// @Display
				real32 zfar;	// @Display
			};
			struct
			{
				real32 left;
				real32 right;
				real32 top;
				real32 bottom;
				real32 near_plane;
				real32 far_plane;
			};
		};

		virtual void Update(const real32 &dt) override;

	public:
		Mat4f CalculateViewMatrix() const;
		Mat4f CalculateProjectionMatrix() const;

		Frustrum GetFrustrum() const;
		Ray ShootRay(const Vec2f &mouse_position, const Vec2f &window_dimenions);

		inline static Camera *GetActiveCamera() { return active_camera; }
		inline static Camera *GetMainCamera() { return main_camera; }

		inline void SetAsActiveCamera() { active_camera = this; }
		inline void SetAsMainCamera() { main_camera = this; }

	public:
		Camera();
		~Camera();

	private:
		inline static Camera *main_camera = nullptr;
		inline static Camera *active_camera = nullptr;
	};


}