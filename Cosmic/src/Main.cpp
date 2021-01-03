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
#include "src/core/IntrospectedEnums.h"
#include "src/core/NavMesh.h"
#include "src/core/entities/Camera.h"
#include "src/core/entities/Environment.h"
#include "src/core/entities/Turret.h"
#include "src/core/entities/EnemyGrunt.h"

#include "src/core/Physics.h"
using namespace cm;

#if 1
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	std::unique_ptr<Window> window = std::make_unique<Window>("Cosmic", Vec2f(1920, 1080), hInstance);


	if (window->IsOpen())
	{
		DirectXState::InitializeDirectX(window->GetHandle());
		std::unique_ptr<DirectXImmediateRenderer> renderer = std::make_unique<DirectXImmediateRenderer>(window->GetHandle());
		std::unique_ptr<DirectXDebugRenderer> debug_renderer = std::make_unique<DirectXDebugRenderer>();

		std::unique_ptr<Debug> debug = std::make_unique<Debug>(debug_renderer.get());
		std::unique_ptr<Editor> editor = std::make_unique<Editor>();

		std::unique_ptr<AssetTable> asset_table = std::make_unique<AssetTable>();
		std::unique_ptr<AssetLoader> asset_loader = std::make_unique<AssetLoader>(asset_table.get(), "../res");

		std::unique_ptr<PhysicsSolver> physics_solver = std::make_unique<PhysicsSolver>();
		std::unique_ptr<ForceRegistry> force_registry = std::make_unique<ForceRegistry>();
		std::unique_ptr<CollisionRegistry> collision_resgisty = std::make_unique<CollisionRegistry>();
		std::unique_ptr<CollisionDetector> collision_detector = std::make_unique<CollisionDetector>();

		GameState game_state;// @TODO: Unique ptr
		game_state.asset_table = asset_table.get();

		asset_loader->LoadEntireFolderOfModels("../res/meshes");

		//for (MeshEntry mesh_entry = 0; mesh_entry < asset_table->mesh_raw_data.size(); mesh_entry++)
		//{
		//	asset_table->mesh_intances.at(mesh_entry) = renderer->CreateMesh(asset_table->mesh_raw_data.at(mesh_entry));
		//}

		std::unique_ptr<World> world = std::make_unique<World>();
		//std::unique_ptr<WorldRenderer> world_renderer = std::make_unique<WorldRenderer>();

		game_state.SetActiveWorld(world.get());

		MouseInput::DisableMouse();
		MouseInput::EnableRawInput();

		renderer->shader = renderer->CreatePipline("t.vert.cso", "t.pixl.cso");
		debug_renderer->pipeline = renderer->CreatePipline("debug_line.vert.cso", "debug_line.pixl.cso");

		FileResult text_file = Platform::LoadFile("../res/PolygonScifi_01_A.png");
		EditableTexture texture = EditableTexture(text_file);
		Texture t = renderer->CreateTexture(texture.width, texture.height, texture.channel_count, texture.pixel_data.data());

		renderer->SetTexture(t);

		bool in_editor = false;

		editor->Start();

		world->CreatePlayer();

		while (!asset_table->GetRawMesh(GameState::GetAssetTable()->FindMeshEntry("TL_Proto_ConcaveGrid")).IsValid())
		{

		}

		Environment *c1 = world->CreateEntity<Environment>();
		c1->name = "c1";
		c1->transform.position = Vec3f(0.0f, 0.0f, 5.0f);
		c1->transform.scale = Vec3f(1.0f);
		c1->transform.orientation = EulerToQuat(Vec3f(-0, 0, 0));
		c1->SetMesh(GameState::GetAssetTable()->FindMeshEntry("COL_Ramp"));
		c1->SetCollider(asset_table->GetRawMesh(c1->GetMesh()).GetMeshCollider());

		Environment *c2 = world->CreateEntity<Environment>();
		c2->name = "c2";
		c2->transform.position = Vec3f(-4.0f, 0.0f, 5.0f);
		c2->transform.scale = Vec3f(1.0f);
		c2->transform.orientation = EulerToQuat(Vec3f(-0, 0, 0));
		c2->SetMesh(GameState::GetAssetTable()->FindMeshEntry("COL_Ramp"));
		c2->SetCollider(asset_table->GetRawMesh(c1->GetMesh()).GetMeshCollider());

		Environment *ground = world->CreateEntity<Environment>();
		ground->name = "ground";
		ground->transform.position = Vec3f(0.0f, 0.0f, 0.0f);
		ground->transform.scale = Vec3f(2.0f);
		ground->transform.orientation = EulerToQuat(Vec3f(0, 0, 0));
		ground->SetMesh(GameState::GetAssetTable()->FindMeshEntry("TL_Proto_ConcaveGrid"));
		ground->SetCollider(asset_table->GetRawMesh(ground->GetMesh()).GetMeshCollider());

		PhysicsEntity *ri = world->CreateEntity<PhysicsEntity>();
		ri->name = "ri";
		ri->transform.position = Vec3f(1.0f, 2.0f, 0.0f);
		ri->transform.scale = Vec3f(1.0f);
		ri->SetMesh(GameState::GetAssetTable()->FindMeshEntry("cube"));
		ri->SetCollider(CreateAABBFromCenterRadius(0, 1));
		ri->SetCollider(CreateOBB(0, 1));
		ri->should_draw = true;
		ri->SetMass(10.0f);
		ri->damping = 0.95f;
		//ri->torque = Vec3f(100, 100, 100);
		//ri->AddForceAtPoint(Vec3f(0, 0, -100), Vec3f(1, 0, -1));
		ri->physics_type = PhysicsEntityType::RIGIDBODY;

		NavMesh nav_mesh;
		nav_mesh.CreateNavMesh(ground->GetGlobalCollider().mesh);

		NavAgent *nav_agent = world->CreateEntity<NavAgent>();
		nav_agent->nav_mesh = &nav_mesh;
		nav_agent->transform.position = Vec3f(10, 0, -10);

		Entity *grunt = world->CreateEntity<EnemyGrunt>();
		grunt->transform.position = Vec3f(3, 0, 3);
		grunt->LookAt(0);

		Turret *turret = world->CreateEntity<Turret>();
		turret->SetName("Turret boii");

		Clock clock;
		clock.Start();
		real32 dt = 0.016f;
		while (window->PollMessages())
		{
			if (!asset_table->models_lock.IsLocked())
			{
				asset_table->models_lock.Lock();

				while (asset_table->mesh_load_requests.size() != 0)
				{
					MeshEntry mesh_entry = asset_table->mesh_load_requests.front();
					asset_table->mesh_load_requests.pop();
					asset_table->mesh_intances.at(mesh_entry) = renderer->CreateMesh(asset_table->mesh_raw_data.at(mesh_entry));
					LOG("Populated: " << (mesh_entry));
				}

				asset_table->models_lock.Unlock();
			}

			if (KeyInput::GetKeyJustDown(KeyCode::F5))
			{
				in_editor = !in_editor;
			}

			static int32 waypoint_index = 1;

			if (KeyInput::GetKeyJustDown(KeyCode::W))
			{
				nav_agent->transform.position = grunt->transform.position;
				nav_agent->FindPathTo(world->player->transform.position);
				waypoint_index = 1;
			}
			if (nav_agent->vertex_path.size() > 1)
			{
				Vec3f waypoint = nav_agent->vertex_path.at(waypoint_index);
				Vec3f dir = Normalize(waypoint - grunt->transform.position);
				grunt->transform.position += dir * 6.0f * dt;
				grunt->LookAt(waypoint);
				grunt->transform.orientation = grunt->transform.orientation * EulerToQuat(Vec3f(0, 180, 0));
				if (Equal(grunt->transform.position, waypoint, 0.5f))
				{
					waypoint_index++;
					if (waypoint_index == nav_agent->vertex_path.size())
					{
						nav_agent->vertex_path.clear();
					}
				}
			}

			Debug::Push(nav_mesh);
			Debug::Push(nav_agent->vertex_path, Vec3f(0, 1, 0));
			//std::cout << real_clock.Get().delta_milliseconds << std::endl;

			if (!in_editor)
			{
				MouseInput::DisableMouse();
				world->UpdateEntities(dt);

			}
			else
			{
				//editor->context.camera.camera.CameraSetActiveCamera();
				editor->Update(&game_state);
			}
			//std::cout << "Update " << real_clock.Get().delta_milliseconds << std::endl;

			debug_renderer->SetMatrices(Camera::GetActiveCamera()->CalculateViewMatrix(), Camera::GetActiveCamera()->CalculateProjectionMatrix());
			renderer->SetMatrices(Camera::GetActiveCamera()->CalculateViewMatrix(), Camera::GetActiveCamera()->CalculateProjectionMatrix());
			renderer->ClearBuffer(Vec4f(0.45f, 0.65f, 0.85f, 1.0f));


			std::vector<Entity *> renderable_entities = world->CreateCollection([](Entity *entity) {
				return entity->active && entity->should_draw && entity->GetMesh() != INVALID_MESH_ENTRY;
			});


			for (Entity *entity : renderable_entities)
			{
				renderer->RenderMesh(asset_table->mesh_intances.at((int32)entity->GetMesh()), entity->GetGlobalTransform());
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
}
#endif