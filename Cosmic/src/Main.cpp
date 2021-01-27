// Cosmic.cpp : Defines the entry point for the application.
//
#include "Cosmic.h"
#include "math/CosmicMath.h"
#include "math/CosmicGeometry.h"
#include "core/Clock.h"
#include "src/serialization/Assets.h"
#include "platforms/Win32Platform.h"
#include "rendering/DirectX11Renderer.h"
#include "src/editor/Editor.h"
#include "Debug.h"
#include "src/generated/IntrospectedEnums.h"
#include "src/core/Grid.h"
#include "src/core/entities/Particles.h"
#include "src/core/entities/Camera.h"
#include "src/core/entities/Environment.h"
#include "src/core/entities/Turret.h"
#include "src/core/entities/GameCamera.h"
#include "src/core/entities/Light.h"
#include "src/core/entities/Drone.h"
#include "src/core/Physics.h"
#include "src/core/SoundSystem.h"
#include "src/core/JobSystem.h"

using namespace cm;


#if 1
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	std::unique_ptr<Window> window = std::make_unique<Window>("Cosmic", Vec2f(1920, 1080), hInstance);

	if (window->IsOpen())
	{
		std::unique_ptr<GraphicsContext> graphics_context = std::make_unique<GraphicsContext>();
		graphics_context->InitializeDirectX(window->GetHandle());

		std::unique_ptr<DirectXImmediateRenderer> renderer = std::make_unique<DirectXImmediateRenderer>(window->GetHandle());

		// @TODO: Debug removal
		std::unique_ptr<DirectXDebugRenderer> debug_renderer = std::make_unique<DirectXDebugRenderer>();
		std::unique_ptr<Debug> debug = std::make_unique<Debug>(debug_renderer.get());
		std::unique_ptr<Editor> editor = std::make_unique<Editor>();

		std::unique_ptr<JobSystem> job_system = std::make_unique<JobSystem>(); // @TODO: Remove singloton

		std::unique_ptr<AssetTable> asset_table = std::make_unique<AssetTable>();
		std::unique_ptr<AssetLoader> asset_loader = std::make_unique<AssetLoader>(asset_table.get(), "../res"); // @TODO: Remove singloton

		std::unique_ptr<SoundSystem> sound_system = std::make_unique<SoundSystem>(); // @TODO: Remove singloton

		//MouseInput::DisableMouse();
		MouseInput::EnableRawInput();

		GameState game_state;// @TODO: Unique ptr
		game_state.asset_table = asset_table.get();
		asset_loader->LoadEntireFolderOfModels("../res/meshes");

		while (job_system->HasThreadsWorking())
		{
			//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}

		for (MeshInstance &mesh : asset_table->mesh_intances)
		{
			EditableMesh *editable_mesh = &asset_table->editable_meshes.at(mesh.asset_table_index);

			graphics_context->CreateMesh(&mesh, editable_mesh);
		}

		std::unique_ptr<World> world = std::make_unique<World>();
		//std::unique_ptr<WorldRenderer> world_renderer = std::make_unique<WorldRenderer>();

		game_state.SetActiveWorld(world.get());

		renderer->shader = renderer->CreatePipline("t.vert.cso", "t.pixl.cso");
		debug_renderer->pipeline = renderer->CreatePipline("debug_line.vert.cso", "debug_line.pixl.cso");

		FileResult text_file = Platform::LoadFile("../res/PolygonScifi_01_A.png");
		EditableTexture texture = EditableTexture(text_file);
		Texture t = renderer->CreateTexture(texture.width, texture.height, texture.channel_count, texture.pixel_data.data());

		renderer->SetTexture(t);

		bool in_editor = false;
		editor->Start();

		Environment *ground = world->CreateEntity<Environment>();
		ground->name = "ground";
		ground->transform.position = Vec3f(0.0f, 0.0f, 0.0f);
		ground->transform.scale = Vec3f(20.0f);
		ground->transform.orientation = EulerToQuat(Vec3f(0, 0, 0));
		ground->SetMesh(GameState::GetAssetTable()->FindMeshInstance("CM_Bld_Floor5x5"));
		ground->SetCollider(asset_table->GetEditableMesh(ground->GetMesh()).GetMeshCollider());

		GameCamera *game_cam = world->CreateEntity<GameCamera>();
		Drone *drone = world->CreateEntity<Drone>();

		world->CreateEntity<PointLight>();


		Clock clock;
		clock.Start();
		real32 dt = 0.016f;
		while (window->PollMessages())
		{
			if (KeyInput::GetKeyJustDown(KeyCode::F5))
			{
				in_editor = !in_editor;
			}

			if (KeyInput::GetKeyJustDown(KeyCode::F6) || KeyInput::GetKeyHeldDown(KeyCode::F7))
			{
				//SweepInfo info;
				//Vec3f v1 = Vec3f(0, -1, 0);
				//Vec3f v2 = Vec3f(0, 0, 0);
				//if (SweepSphere(s1, v1, s2, v2, &info))
				//{
				//	s1 = TranslateSphere(s1, v1 * info.t);
				//	s2 = TranslateSphere(s2, v2 * info.t);
				//	LOG(info.t);
				//	LOGTS(info.normal);
				//}
				//else
				//{
				//	//s1 = TranslateSphere(s1, v1);
				//	s2 = TranslateSphere(s2, v2);
				//}
				//if (SweepPlane(s1, v1, p1, &info))
				//{
				//	s1 = TranslateSphere(s1, v1 * info.t);
				//	LOG(info.t);
				//	LOGTS(info.normal);
				//}
			}

			//std::cout << real_clock.Get().delta_milliseconds << std::endl;

			if (!in_editor)
			{
				world->UpdateEntities(dt);

			}
			else
			{
				//editor->context.camera.camera.CameraSetActiveCamera();
				editor->Update(&game_state);
			}
			//std::cout << "Update " << real_clock.Get().delta_milliseconds << std::endl;

			sound_system->SetListenerTransform(Camera::GetActiveCamera()->GetGlobalTransform());

			debug_renderer->SetMatrices(Camera::GetActiveCamera()->CalculateViewMatrix(), Camera::GetActiveCamera()->CalculateProjectionMatrix());
			renderer->SetMatrices(Camera::GetActiveCamera()->CalculateViewMatrix(), Camera::GetActiveCamera()->CalculateProjectionMatrix());
			renderer->ClearBuffer(Vec4f(0.45f, 0.65f, 0.85f, 1.0f));


			std::vector<Entity *> renderable_entities = world->CreateCollection([](Entity *entity) {
				return entity->active && entity->should_draw && entity->GetMesh().IsOnGPU();
			});

			for (GridCell &cell : world->grid.cells)
			{
				if (!cell.empty)
					renderer->RenderMesh(asset_table->FindMeshInstance("cube"), Transform(cell.center));
			}

			for (Entity *entity : renderable_entities)
			{
				renderer->RenderMesh(entity->GetMesh(), entity->GetGlobalTransform());
			}

			//world_renderer->Render(world.get());

			debug_renderer->RenderAndFlush();

			if (in_editor)
			{
				editor->Render();
			}

			renderer->EndFrame();

			clock.End();
			//dt = clock.Get().delta_seconds;
			dt = 0.016f;
			clock.Start();
		}
	}
	else
	{
		Platform::ErrorBox("Could not open window");
	}

	int a = 2;

}
#endif