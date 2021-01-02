#include "Editor.h"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_win32.h"
#include "vendor/imgui/imgui_impl_dx11.h"

#include "src/core/entities/Environment.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace cm
{
	void Editor::Start()
	{
		StartImgui();

		context = {};
		context.draw_player_collider = true;
		context.camera.Init();

		auto general_window = std::make_shared<GeneralWindow>();
		auto world_window = std::make_shared<WorldWindow>();
		auto console_window = std::make_shared<ConsoleWindow>();
		auto add_window = std::make_shared<AddWindow>();
		auto gizmos = std::make_shared<WorldGizmo>();
		// 
		editor_windows.push_back((EditorWindow *)general_window.get());
		editor_windows.push_back((EditorWindow *)world_window.get());
		editor_windows.push_back((EditorWindow *)console_window.get());
		editor_windows.push_back((EditorWindow *)add_window.get());
		editor_windows.push_back((EditorWindow *)gizmos.get());
		// 
		editor_chain.push_back(general_window);
		editor_chain.push_back(world_window);
		editor_chain.push_back(console_window);
		editor_chain.push_back(add_window);
		editor_chain.push_back(gizmos);
		editor_chain.push_back(std::make_shared<WorldPicker>());
	}

	void Editor::Update(GameState *game_state)
	{
		World *world = game_state->GetActiveWorld();
		BeginFrame();

		//bool s = false;
		//ImGui::ShowDemoWindow(&s);

		MouseInput::EnableMouse();

		for (int32 i = 0; i < editor_windows.size(); i++)
		{
			editor_windows.at(i)->Render(&context, game_state);
		}

		bool world_operations = true;

		for (int32 i = 0; i < editor_chain.size(); i++)
		{
			if (editor_chain.at(i)->GetResult(&context, game_state))
			{
				world_operations = false;
				break;
			}
		}

		if (KeyInput::GetKeyHeldDown(KeyCode::LEFT_CONTROL) && KeyInput::GetKeyJustDown(KeyCode::S))
		{
			XMLTree xml_tree = world->ToXMLTree();
			Platform::WriteTextFile("../res/" + xml_tree.name + ".xml", xml_tree.ToString());
			LOG("Game saved");
		}

		if (world_operations)
		{
			context.camera.camera->SetAsActiveCamera();
			MouseInput::EnableMouse();
			if (MouseInput::GetMouseHeldDown(MouseCode::RIGHT_MOUSE_BUTTON))
			{
				MouseInput::DisableMouse();
				context.camera.Update();
			}
		}

		if (context.draw_player_collider)
		{
			Capsule collider = world->player->collider;
			collider.bot += world->player->GetPosition();
			collider.top += world->player->GetPosition();
			Debug::Push(collider);
		}

		if (context.draw_entity_colliders)
		{
			std::vector<Entity *> entities = world->CreateCollection([](Entity *entity) {
				return entity->HasCollider();
			});

			for (Entity *entity : entities)
			{
				Debug::Push(reinterpret_cast<Environment*>(entity)->GetGlobalCollider());
			}

		}

		if (context.draw_bounding_boxes)
		{
			std::vector<Entity *> entities = world->CreateCollection();

			for (Entity *entity : entities)
			{
				Debug::Push(entity->GetGlobalBoundingBox());
			}
		}

	}

	void Editor::Render()
	{
		EndFrame();
	}

	LRESULT Editor::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		{
			return true;
		}

		return false;
	}

	Editor::Editor()
	{

	}

	void Editor::StartImgui()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(1.00f, 1.00f, 1.00f, 0.31f);

		ImGui_ImplWin32_Init(DirectXState::window);
		ImGui_ImplDX11_Init(DirectXState::device, DirectXState::context);
	}

	void Editor::BeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void Editor::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}