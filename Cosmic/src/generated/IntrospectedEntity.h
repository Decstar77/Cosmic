#include "F:/Telescope/Cosmic/Cosmic/src/core/entities\Camera.h"
#include "F:/Telescope/Cosmic/Cosmic/src/core/entities\Drone.h"
#include "F:/Telescope/Cosmic/Cosmic/src/core/entities\Entity.h"
#include "F:/Telescope/Cosmic/Cosmic/src/core/entities\Environment.h"
#include "F:/Telescope/Cosmic/Cosmic/src/core/entities\GameCamera.h"
#include "F:/Telescope/Cosmic/Cosmic/src/core/entities\Light.h"
#include "F:/Telescope/Cosmic/Cosmic/src/core/entities\Turret.h"
namespace cm
{
	static void IntrospectedEntityGUI(Entity *entity)
	{
		EntityType entity_type = entity->GetType();
		switch (entity_type)
		{
		case EntityType::CAMERA:
		{
			if (ImGui::TreeNode("Camera"))
			{
				Camera* camera = reinterpret_cast<Camera*>(entity);
				ImGui::DragFloat("dfovy", &camera->dfovy);
				ImGui::DragFloat("aspect", &camera->aspect);
				ImGui::DragFloat("znear", &camera->znear);
				ImGui::DragFloat("zfar", &camera->zfar);
				ImGui::TreePop();
			}
			;
		}
		break;
		case EntityType::DRONE:
		{
			if (ImGui::TreeNode("Drone"))
			{
				Drone* drone = reinterpret_cast<Drone*>(entity);
				ImGui::DragFloat3("velocity", drone->velocity.ptr, 0.25f);
				ImGui::DragFloat3("acceleration", drone->acceleration.ptr, 0.25f);
				ImGui::DragFloat("max_speed", &drone->max_speed);
				ImGui::DragFloat("banking_angle", &drone->banking_angle);
				ImGui::TreePop();
			}
			;
		}
		break;
		case EntityType::POINTLIGHT:
		{
			if (ImGui::TreeNode("PointLight"))
			{
				PointLight* pointlight = reinterpret_cast<PointLight*>(entity);
				ImGui::DragFloat("strength", &pointlight->strength);
				ImGui::DragFloat("radius", &pointlight->radius);
				ImGui::DragFloat3("colour", pointlight->colour.ptr, 0.25f);
				ImGui::TreePop();
			}
			;
		}
		break;
		case EntityType::TURRET:
		{
			if (ImGui::TreeNode("Turret"))
			{
				Turret* turret = reinterpret_cast<Turret*>(entity);
				ImGui::TreePop();
			}
			;
		}
		break;
		}
	}
}
