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
#include "src/core/entities/Camera.h"
#include "src/core/entities/Environment.h"
#include "src/core/entities/Turret.h"
#include "src/core/entities/EnemyGrunt.h"

#include "src/core/Physics.h"
using namespace cm;

struct ContactInfo
{
	Vec3f normal;
	Vec3f point;
	real32 penetration;
};

bool CheckContact(const OBB &obb, const Plane &plane, ContactInfo *info)
{
	if (CheckIntersectionOBBPlane(obb, plane))
	{
		Mat4f mat(obb.basis.mat, obb.center);
		Vec3f extents = obb.extents;

		Vec3f vertices[8] = {
			Vec3f(Vec4f(extents, 1) * mat),
			Vec3f(Vec4f(extents * -1.0f, 1) * mat),
			Vec3f((Vec4f(-extents.x, extents.y, extents.z, 1)) * mat),
			Vec3f((Vec4f(extents.x, -extents.y, extents.z, 1)) * mat),
			Vec3f((Vec4f(extents.x, extents.y, -extents.z, 1)) * mat),
			Vec3f((Vec4f(-extents.x, -extents.y, extents.z, 1)) * mat),
			Vec3f((Vec4f(extents.x, -extents.y, -extents.z, 1)) * mat),
			Vec3f((Vec4f(-extents.x, extents.y, -extents.z, 1)) * mat)
		};

		real32 d = GetPlaneScalar(plane);

		for (int32 i = 0; i < 8; i++)
		{
			real32 dist = Dot(plane.normal, vertices[i]);

			if (dist < d)
			{
				info->point = vertices[i] + (plane.normal * (d - dist));
				info->normal = plane.normal;
				info->penetration = d - dist;

				LOG(dist);
				LOG(d);

				return true;
			}
		}
	}

	return false;
}

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
		ground->transform.scale = Vec3f(20.0f);
		ground->transform.orientation = EulerToQuat(Vec3f(0, 0, 0));
		ground->SetMesh(GameState::GetAssetTable()->FindMeshEntry("CM_Bld_Floor5x5"));
		ground->SetCollider(asset_table->GetRawMesh(ground->GetMesh()).GetMeshCollider());

		PhysicsEntity *pm1 = world->CreateEntity<PhysicsEntity>();
		pm1->name = "pm1";
		pm1->transform.position = Vec3f(1.0f, 2.0f, 0.0f);
		pm1->transform.scale = Vec3f(0.1f);
		pm1->SetMesh(GameState::GetAssetTable()->FindMeshEntry("cube"));
		pm1->should_draw = false;
		pm1->SetMass(0);
		pm1->damping = 0.95f;
		//pm1->acceration.y = -15.0f;

		PhysicsEntity *pm2 = world->CreateEntity<PhysicsEntity>();
		pm2->name = "pm2";
		pm2->transform.position = Vec3f(1.1f, 5.9f, 0.0f);
		pm2->transform.scale = Vec3f(0.1f);
		pm2->SetMesh(GameState::GetAssetTable()->FindMeshEntry("cube"));
		pm2->should_draw = false;
		pm2->SetMass(10.0f);
		pm2->damping = 0.95f;
		pm2->acceration.y = -15.0f;

		PhysicsEntity *pm3 = world->CreateEntity<PhysicsEntity>();
		pm3->name = "pm3";
		pm3->transform.position = Vec3f(0.9f, 6.9f, 0.0f);
		pm3->transform.scale = Vec3f(0.1f);
		pm3->SetMesh(GameState::GetAssetTable()->FindMeshEntry("cube"));
		pm3->should_draw = false;
		pm3->SetMass(10.0f);
		pm3->damping = 0.95f;
		pm3->acceration.y = -15.0f;

		PhysicsEntity *pm4 = world->CreateEntity<PhysicsEntity>();
		pm4->name = "pm4";
		pm4->transform.position = Vec3f(1.01f, 0.9f, 0.0f);
		pm4->transform.scale = Vec3f(0.1f);
		pm4->SetMesh(GameState::GetAssetTable()->FindMeshEntry("cube"));
		pm4->should_draw = false;
		pm4->SetMass(10.0f);
		pm4->damping = 0.95f;
		pm4->acceration.y = -15.0f;


		//PhysicsEntity *ri = world->CreateEntity<PhysicsEntity>();
		//ri->name = "ri";
		//ri->transform.position = Vec3f(1.0f, 2.0f, 0.0f);
		//ri->transform.scale = Vec3f(1.0f);
		//ri->SetMesh(GameState::GetAssetTable()->FindMeshEntry("cube"));
		//ri->SetCollider(CreateAABBFromCenterRadius(0, 1));
		//ri->SetCollider(CreateOBB(0, 1));
		//ri->should_draw = true;
		//ri->SetMass(10.0f);
		//ri->damping = 0.95f;
		//ri->torque = Vec3f(100, 100, 100);
		//ri->AddForceAtPoint(Vec3f(0, 0, -100), Vec3f(1, 0, -1));
		//ri->physics_type = PhysicsEntityType::RIGIDBODY;


		//for (int i = 0; i < 100; i++)
		//{
		//	Vec3f dir = RandomPointOnUnitHemisphere();
		//	PhysicsEntity *pm1 = world->CreateEntity<PhysicsEntity>();
		//	pm1->name = "pm";
		//	pm1->transform.position = dir * 2.0f + Vec3f(0, 5.0f, 0);
		//	pm1->veloctiy = dir * RandomReal(0.1f, 3.0f);
		//	pm1->transform.scale = Vec3f(0.1f);
		//	pm1->SetMesh(GameState::GetAssetTable()->FindMeshEntry("cube"));
		//	pm1->should_draw = false;
		//	pm1->SetMass(1);
		//	pm1->damping = 0.95f;
		//	pm1->acceration.y = -15.0f;
		//}

		//{
		//	Connection con;
		//	con.type = ConnectionType::CABLE;
		//	con.mode = ConnectionMode::ENTITY_CONNECTED;
		//	con.rest_length = 1.0f;
		//	con.rod_restitution = 0.0f;
		//	con.ref_self = pm2->CreateEntityReference();
		//	con.ref_other = pm1->CreateEntityReference();
		//	force_registry->connected_springs.push_back(con);
		//}

		//{
		//	Connection con;
		//	con.type = ConnectionType::CABLE;
		//	con.mode = ConnectionMode::ENTITY_CONNECTED;
		//	con.rest_length = 1.0f;
		//	con.rod_restitution = 0.0f;
		//	con.ref_self = pm3->CreateEntityReference();
		//	con.ref_other = pm2->CreateEntityReference();
		//	force_registry->connected_springs.push_back(con);
		//}

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

			Clock real_clock;
			real_clock.Start();
			if (KeyInput::GetKeyJustDown(KeyCode::F6) || KeyInput::GetKeyHeldDown(KeyCode::F7))
			{
				force_registry->ApplyForces(dt);
				world->UpdatePhysicsEntities(dt, physics_solver.get());

				int32 iterration_count = 1;
				for (int32 i = 0; i < iterration_count; i++)
				{
					collision_detector->FindCollisions(world.get(), collision_resgisty.get(), dt);
					collision_resgisty->Resolve(dt);
				}


				if (MouseInput::GetMouseJustDown(MouseCode::LEFT_MOUSE_BUTTON))
				{
					//OBB col = ri->GetGlobalCollider().obb;
					//Ray ray = Camera::GetActiveCamera()->ShootRay(MouseInput::GetCurrentPosition(),
					//	Vec2f((real32)Platform::GetClientWidth(), (real32)Platform::GetClientHeight()));

					//real32 t;
					//if (RaycastOBB(ray, col, &t))
					//{
					//	Vec3f point = TravelDownRay(ray, t);

					//	ri->AddForceAtPoint(ray.direction * 50.0f, point);
					//}
				}
			}
			real_clock.End();


			//Plane ground_plane = CreatePlane(0, Vec3f(0, 1, 0));
			//OBB col = ri->GetGlobalCollider().obb;

			//ContactInfo cinfo;
			//if (CheckContact(col, ground_plane, &cinfo))
			//{
			//	Debug::Push(cinfo.point);
			//}

			//std::cout << real_clock.Get().delta_milliseconds << std::endl;

			if (!in_editor)
			{
				MouseInput::DisableMouse();
				world->UpdateEntities(dt);

				//int32 count = 80;
				//real32 ddt = dt / (real32)count;
				//for (int i = 0; i < count; i++)
				//{

				//}
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