#include "GeneralWindow.h"

#include "src/generated/IntrospectedEntity.h"

namespace cm
{
	struct ComboListInfo
	{
		String list;
		int32 current_item;
	};

	template<typename T>
	static ComboListInfo GetComboStringFromIntrospectedEnum(T current)
	{
		StringStream ss;
		ComboListInfo info;

		for (int32 i = 1; i < (int32)T::COUNT; i++)
		{
			T t = static_cast<T>(i);

			if (t == current)
			{
				info.current_item = i - 1;
			}

			ss << ToString(t) << '\0';
		}

		info.list = ss.str();

		return info;
	}

	void GeneralWindow::Render(EditorContext *context, GameState *game_state)
	{
		World *world = game_state->GetActiveWorld();

		ImGui::Begin("General");

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{

			}

			ImGui::EndPopup();
		}

		if (ImGui::CollapsingHeader("Entity Properties") && context->current_entity.IsValid())
		{
			Entity *entity = context->current_entity.Get();

			if (ImGui::TreeNode("Basic"))
			{
				char name[256] = {};
				strcpy_s(name, entity->name.c_str());
				if (ImGui::InputText("Name", name, sizeof(name)))
				{
					entity->name = name;
				}

				EntityType entity_type = entity->type;
				ComboListInfo entities = GetComboStringFromIntrospectedEnum(entity_type);
				ImGui::Combo("Entry", &entities.current_item, entities.list.c_str());

				ImGui::Checkbox("Active", &entity->active);
				ImGui::Checkbox("Should render", &entity->should_draw);
				ImGui::TreePop();
			}

			{
				Transform transform = entity->GetLocalTransform();
				if (ImGui::TreeNode("Transform"))
				{
					ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.0, 0.94f, 0.7f));
					ImGui::DragFloat3("Position", transform.position.ptr, 0.25f);
					ImGui::PopStyleColor(1);

					ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(140.0f / 360.0f, 0.94f, 0.7f));
					ImGui::DragFloat4("Orientation", transform.orientation.ptr, 0.25f);
					ImGui::PopStyleColor(1);

					ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(207.0f / 360.0f, 0.94f, 0.7f));
					ImGui::DragFloat3("Scale", transform.scale.ptr, 0.25f);
					ImGui::PopStyleColor(1);
					ImGui::TreePop();

					if (ImGui::Button("Reset transform"))
					{
						transform = Transform();
					}

					entity->transform = transform;
				}
			}

			if (ImGui::TreeNode("Rendering"))
			{
				MeshInstance mesh_entry = entity->GetMesh();

				StringStream ss;

				for (MeshInstance &data : game_state->asset_table->mesh_intances)
				{
					ss << data.meta_data.name << '\0';
				}

				ComboListInfo meshes;
				meshes.current_item = mesh_entry.asset_table_index;
				meshes.list = ss.str();

				if (ImGui::Combo("Mesh", &meshes.current_item, meshes.list.c_str()))
				{
					MeshInstance new_mesh_entry = game_state->asset_table->mesh_intances.at(meshes.current_item);
					entity->SetMesh(new_mesh_entry);
				}
				ImGui::TreePop();
			}


			if (ImGui::TreeNode("Collider"))
			{
				Collider collider = entity->GetCollider();

				Debug::Push(entity->GetGlobalCollider());

				ComboListInfo colliders = GetComboStringFromIntrospectedEnum(collider.type);

				if (ImGui::Combo("Collider", &colliders.current_item, colliders.list.c_str()))
				{
					collider = {};
					collider.obb.extents = Vec3f(0); // @NOTE: Cheap way to clear things to zero
					collider.obb.mat = Mat4f(0); // @NOTE: Cheap way to clear things to zero
					collider.type = static_cast<ColliderType>(colliders.current_item + 1);
				}

				switch (collider.type)
				{
				case ColliderType::PLANE: ASSERT(0, "TODO");
					break;
				case ColliderType::SPHERE: ASSERT(0, "TODO");
					break;
				case ColliderType::AABB: ASSERT(0, "TODO");
					break;
				case ColliderType::OBB: ASSERT(0, "TODO");
					break;
				case ColliderType::CAPSULE:
				{
					ImGui::InputFloat3("Bot", collider.capsule.bot.ptr);
					ImGui::InputFloat3("Top", collider.capsule.top.ptr);
					ImGui::InputFloat("Radius", &collider.capsule.radius);
				}
				break;
				case ColliderType::MESH:
				{
					collider = GameState::GetAssetTable()->GetEditableMesh(entity->GetMesh()).GetMeshCollider();
					int32 size = (int32)collider.mesh.size();
					String info1 = "Triangle count: " + std::to_string(size);
					real32 r = (3.0f / 740.0f) * size + 7.0f / 74.0f;
					real32 b = -(3.0f / 740.0f) * size + 67.0f / 74.0f;

					if (size > 250.f)
					{
						ImGui::Text("Too many triangles, colour is indication of performace!!");
					}

					ImGui::TextColored(ImVec4(r, b, 0.2f, 1.0f), info1.c_str());

				}break;
				};

				entity->SetCollider(collider);

				ImGui::TreePop();
			}

			IntrospectedEntityGUI(entity);

			if (entity->behaviour == EntityBehaviour::PHYSICS_ENTITY)
			{
				if (ImGui::TreeNode("Physics"))
				{
					if (ImGui::TreeNode("Motion state"))
					{
						PhysicsEntity *phys = reinterpret_cast<PhysicsEntity*> (entity);
						ImGui::InputFloat3("Position", phys->transform.position.ptr);
						ImGui::InputFloat3("Velocity", phys->veloctiy.ptr);
						ImGui::InputFloat3("Acceleration", phys->acceration.ptr);
						ImGui::InputFloat3("Forces", phys->forces.ptr);
						real32 mass = phys->GetMass();
						ImGui::InputFloat("Mass", &mass);
						phys->SetMass(mass);
						ImGui::DragFloat("Damping", &phys->damping, 0.01f, 0.001f, 0.999f);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Connections"))
					{

						int32 current_type = 0;
						ConnectionType type = ConnectionType::CABLE;
						int32 current_mode = 0;
						ConnectionMode mode = ConnectionMode::ENTITY_ANCHORED;

						ComboListInfo types = GetComboStringFromIntrospectedEnum(type);
						ImGui::Combo("Type", &current_type, types.list.c_str());

						ComboListInfo modes = GetComboStringFromIntrospectedEnum(mode);
						ImGui::Combo("Mode", &current_mode, modes.list.c_str());


						Connection &spring = ForceRegistry::instance->connected_springs.at(0);
						ImGui::InputFloat("Spring strength", &spring.strength_coef);
						ImGui::InputFloat("Rest length", &spring.rest_length);
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}
		}

		if (ImGui::CollapsingHeader("Operations"))
		{
			static bool test_window = true;

			if (ImGui::BeginPopupContextItem("AIDHJA")) // <-- This is using IsItemHovered()
			{
				if (ImGui::MenuItem("Close")) { test_window = false; }
				ImGui::EndPopup();
			}

			ImGui::Checkbox("Grid Mode", &context->grid_mode);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.0f / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3.0f / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3.0f / 7.0f, 0.8f, 0.8f));
			// ImGui::PushStyleVar(ImGuiStyleVasr)
			if (ImGui::Button("Bake collison geometry", ImVec2(-1, 0)))
			{
				//BakeWorldCollisionGeometry(world, &game_state->mesh_table);
			}
			ImGui::PopStyleColor(3);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(6.0f / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(6.0f / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(6.0f / 7.0f, 0.8f, 0.8f));
			// ImGui::PushStyleVar(ImGuiStyleVasr)
			if (ImGui::Button("Bake lighting - The dream boiis", ImVec2(-1, 0)))
			{
			}
			ImGui::PopStyleColor(3);


			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(199.0f / 360.0f, 0.71f, 0.86f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(199.0f / 360.0f, 0.81f, 0.9f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(199.0f / 360.0f, 0.91f, 0.96f));
			// ImGui::PushStyleVar(ImGuiStyleVasr)
			if (ImGui::Button("Load world", ImVec2(-1, 0)))
			{
				Transform t = context->camera.camera->transform;
				XMLTree tree_world(Platform::ReadTextFile("../res/World.xml"));
				GameState::GetActiveWorld()->CreateFromXMLTree(&tree_world);
				context->camera.Init(); // @NOTE: Reinject the editor camera
				context->camera.camera->SetTransform(t);
				context->camera.camera->SetAsActiveCamera();
			}
			ImGui::PopStyleColor(3);
		}
		if (ImGui::CollapsingHeader("Perfomance"))
		{

		}
		if (ImGui::CollapsingHeader("Config"))
		{

		}
		if (ImGui::CollapsingHeader("Debug"))
		{
			ImGui::Checkbox("Draw entity collider", &context->draw_entity_colliders);
			ImGui::Checkbox("Draw player collider", &context->draw_player_collider);
			ImGui::Checkbox("Draw bounding boxes", &context->draw_bounding_boxes);
			ImGui::Checkbox("Console window", &context->console_window);
			ImGui::Checkbox("World window", &context->world_window);
		}

		if (MouseInput::GetMouseJustDown(MouseCode::LEFT_MOUSE_BUTTON))
		{
			result = ImGui::IsWindowHovered() || ImGui::GetIO().MouseDownOwned[0];
		}
		else
		{
			result = ImGui::IsWindowHovered();
		}



		ImGui::End();
	}

	bool GeneralWindow::GetResult(EditorContext *context, GameState *game_state)
	{
		return result;
	}

	static inline void DoDisplay(EditorContext *context, World *world, Entity *entity)
	{
		if (entity && entity->IsValid())
		{
			ImGui::PushID(entity->GetId());
			bool has_child = entity->GetFirstChild()->IsValid();
			if (ImGui::TreeNodeEx(entity->name.c_str(), has_child ? 0 : ImGuiTreeNodeFlags_Leaf))
			{
				ImGui::SameLine();
				if (ImGui::SmallButton("select"))
				{
					context->current_entity = entity->CreateEntityReference();
				}

				if (has_child)
				{
					DoDisplay(context, world, entity->GetFirstChild());
				}

				ImGui::TreePop();
			}

			ImGui::PopID();


			DoDisplay(context, world, entity->GetSibling());
		}
	}

	void WorldWindow::Render(EditorContext *context, GameState *game_state)
	{
		World *world = game_state->GetActiveWorld();

		if (context->world_window)
		{
			ImGui::Begin("World");

			std::vector<Entity *> valid_entities = world->CreateCollection([](Entity *ent) {
				return !ent->GetParent()->IsValid();
			});

			for (Entity *entity : valid_entities)
			{
				DoDisplay(context, world, entity);
			}

			result = ImGui::IsWindowHovered();

			ImGui::End();
		}
	}

	bool WorldWindow::GetResult(EditorContext *context, GameState *game_state)
	{
		return result;
	}

	bool ConsoleWindow::GetResult(EditorContext *context, GameState *game_state)
	{
		return result;
	}

	void ConsoleWindow::Render(EditorContext *context, GameState *game_state)
	{
		if (context->console_window)
		{
			ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
			ImGui::Begin("Console");

			if (ImGui::SmallButton("Clear"))
			{
				Debug::ClearLogger();
			}

			ImGui::SameLine();
			scroll_to_bottom = ImGui::SmallButton("Scroll to bottom");
			ImGui::SameLine();
			ImGui::Checkbox("Auto scroll", &auto_scroll);

			ImGui::Separator();
			const real32 footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text

			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

			// if (warnings.length() > 0)
			// {
			//     ImGui::SetWindowFocus();
			// }

			String log_info = Debug::GetLogInfo();
			String error_info = Debug::GetErrorInfo();

			if (filter.PassFilter(log_info.c_str()))
			{
				ImGui::TextUnformatted(log_info.c_str());
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85, 0.15, 0.15, 1));
				ImGui::TextUnformatted(error_info.c_str());
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

			if (auto_scroll || scroll_to_bottom)
			{
				ImGui::SetScrollHereY(1.0f);
			}

			result = ImGui::IsWindowHovered();

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::EndChild();
			ImGui::End();
		}
		else
		{
			result = false;
		}
	}


	void AddWindow::Render(EditorContext *context, GameState *game_state)
	{
		if (KeyInput::GetKeyHeldDown(KeyCode::LEFT_SHIFT) && KeyInput::GetKeyJustDown(KeyCode::A))
		{
			running = !running;
		}

		if (running)
		{
			ImGui::Begin("General");

			const char* names[] = { "Entities", "Environment","Lights", "Voxel things" };
			const char* lights[] = { "Directional", "Spot", "Point", "Area (Baking only)" };
			const char* entities[] = { "Turret", "Enemy", "Wall", "Props" };
			const char* environment[] = { "Empty enirmonment piece" };
			static bool toggles[] = { true, false, false, false, false };
			bool used = false;

			ImGui::OpenPopup("my_toggle_popup");
			if (ImGui::BeginPopup("my_toggle_popup"))
			{
				ImGui::Text("Add New object"); ImGui::Separator();
				for (int i = 0; i < std::size(names); i++)
				{
					if (ImGui::BeginMenu(names[i]))
					{
						if (i == 0)
						{
							for (int j = 0; j < std::size(entities); j++)
							{
								if (j == 0 && ImGui::MenuItem(entities[j]))
								{
									//context->current_entity = GameState::GetActiveWorld()->AddTurrentEntity("Untitled Turret Entity", Transform());
									running = false;
									result = true;
								}
							}
							ImGui::EndMenu();
						}
						else if (i == 1)
						{
							for (int j = 0; j < std::size(environment); j++)
							{
								if (ImGui::MenuItem(environment[j]))
								{
									Environment *env = GameState::GetActiveWorld()->CreateEntity<Environment>();

									context->current_entity = env->CreateEntityReference();


									running = false;
									result = true;
								}
							}
							ImGui::EndMenu();
						}
						else if (i == 2)
						{
							for (int light = 0; light < std::size(lights); light++)
							{
								if (ImGui::MenuItem(lights[light]))
								{
									if (light == 0)
									{
										context->current_entity = GameState::GetActiveWorld()->
											CreateEntity<DirectionalLight>()->CreateEntityReference();
									}
									else if (light == 1)
									{
										context->current_entity = GameState::GetActiveWorld()->
											CreateEntity<SpotLight>()->CreateEntityReference();
									}
									else if (light == 2)
									{
										context->current_entity = GameState::GetActiveWorld()->
											CreateEntity<PointLight>()->CreateEntityReference();
									}

									running = false;
									result = true;
								}
							}
							ImGui::EndMenu();
						}
						else
						{
							ImGui::EndMenu();
						}
					}

					if (i < std::size(names) - 1)
					{
						ImGui::Separator();
					}
				}

				ImGui::EndPopup();
			}

			if (MouseInput::GetMouseJustDown(MouseCode::RIGHT_MOUSE_BUTTON))
			{
				running = false;
				result = true;
			}

			ImGui::End();
		}
		else
		{
			running = false;
			result = false;
		}
	}

	bool AddWindow::GetResult(EditorContext *context, GameState *game_state)
	{
		return result;
	}



	bool WorldGizmo::GetResult(EditorContext *context, GameState *game_state)
	{
		if (result)
		{
			EntityReference ref = context->current_entity;

			Vec3f pos;
			Vec3f euler;
			Vec3f scale;

			if (ref.IsValid())
			{
				if (MouseInput::GetMouseJustDown(MouseCode::LEFT_MOUSE_BUTTON) &&
					KeyInput::GetKeyHeldDown(KeyCode::LEFT_SHIFT) &&
					gizmo_type == GizmoType::TRANSLATION)
				{
					ImGuizmo::DecomposeMatrixToComponents(world_mat.ptr, pos.ptr, euler.ptr, scale.ptr);
					LOG("Duplicate add");
					//context->current_entity = game_state->world->AddTurrentEntity("New", entity.SpacialGetGlobalTransform());
					//context->current_entity.SpacialSetTransform(Transform(pos, euler, scale));
				}
				else
				{
					Entity *entity = ref.Get();
					Entity *parent = entity->GetParent();

					if (parent->IsValid())
					{
						Mat4f m2 = Inverse(parent->GetGlobalTransformMatrix());
						Mat4f w = world_mat * m2;

						ImGuizmo::DecomposeMatrixToComponents(w.ptr, pos.ptr, euler.ptr, scale.ptr);
						entity->transform = Transform(pos, euler, scale);
					}
					else
					{
						ImGuizmo::DecomposeMatrixToComponents(world_mat.ptr, pos.ptr, euler.ptr, scale.ptr);
						entity->transform = Transform(pos, euler, scale);
					}
				}
			}

			//if (MouseInput::GetMouseJustDown(MouseCode::LEFT_MOUSE_BUTTON))
			//{
			//	start_mat = entity.SpacialGetLocalTransformMatrix();
			//}

			//Vec3f pos;
			//Vec3f euler;
			//Vec3f scale;
			//if (KeyInput::GetKeyJustDown(KeyCode::LEFT_SHIFT) && gizmo_type == GizmoType::TRANSLATION)
			//{
			//	ImGuizmo::DecomposeMatrixToComponents(start_mat.ptr, pos.ptr, euler.ptr, scale.ptr);

			//	context->current_entity = game_state->world->AddTurrentEntity("New", entity.SpacialGetTransform());

			//	entity.SpacialSetTransform(Transform(pos, euler, scale));
			//}
			//else
			//{
			//	ImGuizmo::DecomposeMatrixToComponents(mat.ptr, pos.ptr, euler.ptr, scale.ptr);
			//	entity.SpacialSetTransform(Transform(pos, euler, scale));
			//}
		}

		return result;
	}

	void WorldGizmo::Render(EditorContext *context, GameState *game_state)
	{
		EntityReference entity = context->current_entity;
		result = false;

		if (entity.IsValid())
		{
			Mat4f view = context->camera.camera->CalculateViewMatrix();
			Mat4f proj = context->camera.camera->CalculateProjectionMatrix();
			world_mat = entity.Get()->GetGlobalTransformMatrix();

			delta = Mat4f();

			ImGuizmo::Enable(true);

			ImGuiIO &io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

			ChooseMode();
			ChooseType();
			CheckSnapping();

			ImGuizmo::OPERATION op = GetImGuizmoOperation();
			ImGuizmo::MODE md = GetImGuizmoMode();

			Vec3f snap_amount = Vec3f(0.1f, 0.1f, 0.1f);

			if (gizmo_type == GizmoType::ROTATION)
			{
				snap_amount = Vec3f(15);
			}

			ImGuizmo::Manipulate(view.ptr, proj.ptr, op, md, world_mat.ptr, delta.ptr, snapping ? (snap_amount.ptr) : nullptr);

			result = ImGuizmo::IsOver() && MouseInput::GetMouseHeldDown(MouseCode::LEFT_MOUSE_BUTTON);
		}
	}


	ImGuizmo::OPERATION WorldGizmo::GetImGuizmoOperation()
	{
		ImGuizmo::OPERATION op;

		switch (gizmo_type)
		{
		case GizmoType::TRANSLATION:
			op = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case GizmoType::ROTATION:
			op = ImGuizmo::OPERATION::ROTATE;
			break;
		case GizmoType::SCALING:
			op = ImGuizmo::OPERATION::SCALE;
			break;
		default:
			op = ImGuizmo::OPERATION::TRANSLATE;
		}

		return op;
	}

	ImGuizmo::MODE WorldGizmo::GetImGuizmoMode()
	{
		ImGuizmo::MODE md;

		switch (gizmo_mode)
		{
		case GizmoTransformationMode::LOCAL:
			md = ImGuizmo::MODE::LOCAL;
			break;
		case GizmoTransformationMode::WORLD:
			md = ImGuizmo::MODE::WORLD;
			break;
		default:
			md = ImGuizmo::MODE::WORLD;
		}

		return md;
	}

	void WorldGizmo::ChooseMode()
	{
		if (KeyInput::GetKeyJustDown(KeyCode::TILDE))
		{
			gizmo_mode = (gizmo_mode == GizmoTransformationMode::LOCAL)
				? GizmoTransformationMode::WORLD
				: GizmoTransformationMode::LOCAL;
		}
	}

	void WorldGizmo::ChooseType()
	{
		if (KeyInput::GetKeyJustDown(KeyCode::T))
		{
			gizmo_type = GizmoType::TRANSLATION;
		}
		else if (KeyInput::GetKeyJustDown(KeyCode::R))
		{
			gizmo_type = GizmoType::ROTATION;
		}
		else if (KeyInput::GetKeyJustDown(KeyCode::Y))
		{
			gizmo_type = GizmoType::SCALING;
		}

	}

	void WorldGizmo::CheckSnapping()
	{
		snapping = KeyInput::GetKeyHeldDown(KeyCode::LEFT_CONTROL);
	}


	bool WorldPicker::GetResult(EditorContext *context, GameState *game_state)
	{
		World *world = game_state->GetActiveWorld();

		if (MouseInput::GetMouseJustDown(MouseCode::LEFT_MOUSE_BUTTON))
		{
			Ray ray = Camera::GetActiveCamera()->ShootRay(MouseInput::GetCurrentPosition(), Vec2f((real32)Platform::GetClientWidth(), (real32)Platform::GetClientHeight()));

			std::vector<Entity*> entities = world->CreateCollection();

			real32 min = FLT_MAX;
			Entity* picked = nullptr;
			for (auto entity : entities)
			{
				real32 t = 0;
				AABB bounding_box = entity->GetGlobalBoundingBox();

				if (RaycastAABB(ray, bounding_box, &t))
				{
					if (!entity->HasMesh())
					{
						if (t < min)
						{
							min = t;
							picked = entity;
						}
					}
					else
					{
						MeshCollider list = game_state->GetAssetTable()->GetEditableMesh(entity->GetMesh()).GetMeshCollider();

						Mat4f entity_matrix = entity->GetGlobalTransformMatrix();
						Mat4f inv_entity_matrix = Inverse(entity_matrix);

						Ray local_ray;

						local_ray.origin = Vec3f(Vec4f(ray.origin, 1.0f) * inv_entity_matrix);
						local_ray.direction = Normalize(Vec3f(Vec4f(ray.direction, 0.0f) * Transpose(entity_matrix)));

						if (RaycastMeshColldier(local_ray, list))
						{
							if (t < min)
							{
								min = t;
								picked = entity;
							}
						}
					}
				}
			}

			if (picked && picked->IsValid())
			{
				context->current_entity = picked->CreateEntityReference();
			}
			else
			{
				context->current_entity = {};
			}
		}

		return false;
	}


	void EditorCamera::Update()
	{
		if (MouseInput::GetMouseHeldDown(MouseCode::RIGHT_MOUSE_BUTTON))
		{
			MouseCallback();
			KeyCallback();
		}
	}

	void EditorCamera::MouseCallback()
	{
		Vec2f delta = MouseInput::GetMouseDelta();

		real32 nyaw = this->yaw - delta.x * this->look_speed;
		real32 npitch = this->pitch + delta.y * this->look_speed;

		npitch = Clamp(npitch, -89.0f, 89.0f);
		this->yaw = Lerp(this->yaw, nyaw, 0.67f);
		this->pitch = Lerp(this->pitch, npitch, 0.67f);

		Vec3f direction;
		direction.x = Cos(DegToRad(this->yaw)) * Cos(DegToRad(this->pitch));
		direction.y = Sin(DegToRad(this->pitch));
		direction.z = Sin(DegToRad(this->yaw)) * Cos(DegToRad(this->pitch));

		Transform transform = camera->transform;

		Mat4f result = LookAtLH(transform.position, transform.position + direction, Vec3f(0, 1, 0));

		camera->transform.orientation = Mat4ToQuat(result);

	}

	void EditorCamera::KeyCallback()
	{
		Basisf basis = camera->transform.GetBasis();

		real32 dtime = 0.016f;

		if (KeyInput::GetKeyHeldDown(KeyCode::W))
		{
			camera->transform.position += (basis.forward * this->move_speed * dtime);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::S))
		{
			camera->transform.position += (-1.0f * basis.forward * this->move_speed * dtime);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::A))
		{
			camera->transform.position += (-1.0f * basis.right * this->move_speed * dtime);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::D))
		{
			camera->transform.position += (basis.right * this->move_speed * dtime);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::Q))
		{
			camera->transform.position += (Vec3f(0, 1, 0) * this->move_speed * dtime);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::E))
		{
			camera->transform.position += (Vec3f(0, -1, 0)* this->move_speed * dtime);
		}

	}

	void EditorCamera::Init()
	{
		camera = GameState::GetActiveWorld()->CreateEntity<Camera>();
		camera->name = "EDITOR CAMERA";
	}

	EditorCamera::EditorCamera() : yaw(90), pitch(0)
	{

	}

	EditorCamera::~EditorCamera()
	{

	}

	bool GridMode::GetResult(EditorContext *context, GameState *game_state)
	{
		if (context->grid_mode)
		{
			Grid &grid = game_state->GetActiveWorld()->grid;

			Ray ray = Camera::GetActiveCamera()->ShootRay(MouseInput::GetCurrentPosition(),
				Vec2f((real32)Platform::GetClientWidth(), (real32)Platform::GetClientHeight()));

			real32 min_dist = 100000.0f;
			GridCell *selected_cell = nullptr;

			for (GridCell &cell : grid.cells)
			{
				if (!cell.empty)
				{
					AABB aabb = CreateAABBFromCenterRadius(cell.center,
						Vec3f(grid.grid_cell_half_width, grid.grid_cell_half_height, grid.grid_cell_half_depth));

					real32 t;
					if (RaycastAABB(ray, aabb, &t))
					{
						if (t < min_dist)
						{
							selected_cell = &cell;
							min_dist = t;
						}
					}
				}
			}

			if (selected_cell)
			{
				if (KeyInput::GetKeyHeldDown(KeyCode::LEFT_SHIFT))
				{
					AABB aabb = CreateAABBFromCenterRadius(selected_cell->center,
						Vec3f(grid.grid_cell_half_width, grid.grid_cell_half_height, grid.grid_cell_half_depth));

					Debug::Push(aabb);

					if (MouseInput::GetMouseJustDown(MouseCode::LEFT_MOUSE_BUTTON))
					{
						selected_cell->empty = true;
						return true;
					}
				}
				else
				{
					std::vector<GridCell> neighbours = grid.GetNeighbours(*selected_cell);

					min_dist = 1000000.0f;
					selected_cell = nullptr;

					for (GridCell &cell : neighbours)
					{
						if (cell.empty)
						{
							AABB aabb = CreateAABBFromCenterRadius(cell.center,
								Vec3f(grid.grid_cell_half_width, grid.grid_cell_half_height, grid.grid_cell_half_depth));

							real32 t;
							if (RaycastAABB(ray, aabb, &t))
							{
								if (t < min_dist)
								{
									selected_cell = &cell;
									min_dist = t;
								}
							}
						}
					}

					if (selected_cell)
					{
						AABB aabb = CreateAABBFromCenterRadius(selected_cell->center,
							Vec3f(grid.grid_cell_half_width, grid.grid_cell_half_height, grid.grid_cell_half_depth));

						Debug::Push(aabb);

						if (MouseInput::GetMouseJustDown(MouseCode::LEFT_MOUSE_BUTTON))
						{
							int32 index = grid.GetIndex(selected_cell->xindex, selected_cell->yindex, selected_cell->zindex);
							grid.cells.at(index).empty = false;

							return true;
						}
					}
				}
			}
		}

		return false;
	}

	bool CameraFreeMode::GetResult(EditorContext *context, GameState *game_state)
	{
		context->camera.camera->SetAsActiveCamera();

		MouseInput::EnableMouse();
		if (MouseInput::GetMouseHeldDown(MouseCode::RIGHT_MOUSE_BUTTON))
		{
			MouseInput::DisableMouse();
			context->camera.Update();
			return true;
		}

		return false;
	}

}