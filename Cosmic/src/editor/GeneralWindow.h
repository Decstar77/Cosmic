#pragma once
#include "src/Cosmic.h"
#include "src/core/World.h"
#include "src/Debug.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_win32.h"
#include "vendor/imgui/imgui_impl_dx11.h"
#include "vendor/imguizmo/ImGuizmo.h"

namespace cm
{
	class EditorCamera
	{
	public:
		Camera *camera;

		real32 move_speed = 10.0f;
		real32 look_speed = 0.05f;
		real32 yaw;
		real32 pitch;

	public:
		inline static Vec2f DELTA = Vec2f(0);

		void Update();
		void MouseCallback();
		void KeyCallback();
		void Init();


	public:
		EditorCamera();
		~EditorCamera();

	};

	class EditorContext
	{
	public:
		bool draw_bounding_boxes;
		bool draw_entity_colliders;
		bool draw_player_collider;
		bool console_window;
		bool world_window;

		EntityReference current_entity;
		EditorCamera camera;
	};

	class EditorChain
	{
	public:
		virtual bool GetResult(EditorContext *context, GameState *game_state) = 0;
		virtual ~EditorChain() {}

	};

	class EditorWindow
	{
	public:
		virtual void Render(EditorContext *context, GameState *game_state) = 0;
		virtual ~EditorWindow() {}
	};

	class GeneralWindow : public EditorChain, EditorWindow
	{
		bool result = false;
	public:
		virtual void Render(EditorContext *context, GameState *game_state) override;
		virtual bool GetResult(EditorContext *context, GameState *game_state) override;

	};

	class WorldWindow : public EditorChain, EditorWindow
	{
		bool result = false;
	public:
		virtual void Render(EditorContext *context, GameState *game_state) override;
		virtual bool GetResult(EditorContext *context, GameState *game_state) override;

	};

	class AddWindow : public EditorChain, EditorWindow
	{
		bool result = false;
		bool running = false;
	public:
		virtual void Render(EditorContext *context, GameState *game_state) override;
		virtual bool GetResult(EditorContext *context, GameState *game_state) override;
	};

	class ConsoleWindow : public EditorChain, EditorWindow
	{
	public:
		virtual bool GetResult(EditorContext *context, GameState *game_state) override;
		virtual void Render(EditorContext *context, GameState *game_state) override;

	private:
		bool result = false;
		bool active;
		bool auto_scroll;
		bool scroll_to_bottom;
		char input_buffer[256];
		ImGuiTextFilter filter;
	};

	class WorldGizmo : public EditorChain, EditorWindow
	{
		enum class GizmoType
		{
			TRANSLATION,
			ROTATION,
			SCALING
		};

		enum class GizmoTransformationMode
		{
			LOCAL,
			WORLD
		};

	public:
		virtual bool GetResult(EditorContext *context, GameState *game_state) override;
		virtual void Render(EditorContext *context, GameState *game_state) override;

	private:
		ImGuizmo::OPERATION GetImGuizmoOperation();
		ImGuizmo::MODE GetImGuizmoMode();
		void ChooseMode();
		void ChooseType();
		void CheckSnapping();

		Mat4f world_mat;
		Mat4f delta;

		Mat4f start_mat;
		Mat4f start_delta;

		bool started = false;

		bool result = false;
		bool snapping = false;
		bool duplicated = false;
		bool duplication = false;

		GizmoType gizmo_type;
		GizmoTransformationMode gizmo_mode;
	};

	class WorldPicker : public EditorChain
	{

	public:
		virtual bool GetResult(EditorContext *context, GameState *game_state) override;

	};
}

