#include "Player.h"
#include "src/core/World.h"

#include "src/core/Clock.h" // @TODO: Remove

namespace cm
{
	void Player::Update(const real32 &dt)
	{
		PlayerLook(dt);
		PlayerMove(dt);
	}

	void Player::Start()
	{
		this->type = EntityType::PLAYER;
		this->name = "Player";

		this->character_length = 1.8f;
		this->character_eye_height = 1.6f;

		this->max_slope_angle = 45.0f;
		this->walk_speed = 140.f;
		this->run_speed = 14.0f;
		this->resistance = 14.0f;

		this->grounded = true;
		this->jump_power = 10.0f;
		this->gravity_amount = 25.0f;

		this->look_speed = 0.05f;

		this->pitch = 0.0f;
		this->yaw = 90.0f;

		// 		this->collider.center = Vec3f(0, character_length / 2.0f, 0);
		// 		this->collider.radius = Vec3f(0.2f, character_length / 2.0f, 0.2f);

		this->collider = CreateCapsuleFromBaseTip(Vec3f(0), Vec3f(0, this->character_length, 0), 0.54f);

		camera = world->CreateEntity<Camera>();
		camera->SetParent(this);
		camera->transform = Transform(Vec3f(0, character_eye_height, 0));

		weapon = world->CreateEntity<Entity>();
		weapon->SetType(EntityType::ENVIRONMENT);
		weapon->SetName("Player weapn");
		weapon->SetPosition(Vec3f(0.296f, -0.311f, 0.643f));
		Vec3f dir = camera->transform.GetBasis().forward;
		weapon->transform.LookAtLH(camera->transform.position + -100.0f * dir);
		weapon->SetMesh(GameState::GetAssetTable()->FindMeshEntry("SM_Wep_Rifle_Small_01"));
		weapon->SetParent(camera);
	}

	void Player::PlayerLook(const real32 &dt)
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

		Transform transform = camera->GetLocalTransform();

		Mat4f result = LookAtLH(transform.position, transform.position + direction, Vec3f(0, 1, 0));

		camera->transform.orientation = (Mat4ToQuat(result));

		if (MouseInput::GetMouseJustDown(MouseCode::LEFT_MOUSE_BUTTON))
		{
			speaker.PlaySound("gunsoundpack/gun_pistol_shot_01.wav");
		}
	}

	static bool CollideAndSlide(const std::vector<Entity *> &terrian)
	{

	}

	void Player::PlayerMove(const real32 &dt)
	{
		Basisf basis = camera->transform.GetBasis();

		Vec3f starting_position = transform.position;

		basis.forward.y = 0.0f;
		basis.right.y = 0.0f;
		basis.upward = Vec3f(0, 1, 0);

		basis.forward = Normalize(basis.forward);
		basis.right = Normalize(basis.right);

		acceleration = Vec3f(0);

		real32 speed = walk_speed;
		if (KeyInput::GetKeyHeldDown(KeyCode::W))
		{
			acceleration += (basis.forward * speed);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::S))
		{
			acceleration += (-1.0f * basis.forward * speed);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::A))
		{
			acceleration += (-1.0f * basis.right * speed);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::D))
		{
			acceleration += (basis.right * speed);
		}

		acceleration = ClampMag(acceleration, speed);

		acceleration.x += -resistance * velocity.x;
		acceleration.z += -resistance * velocity.z;

		acceleration.y -= gravity_amount;

		if (KeyInput::GetKeyJustDown(KeyCode::SPACE) && grounded)
		{
			grounded = false;
			velocity.y += jump_power;
		}

		velocity = acceleration * dt + velocity;

		transform.position = velocity * dt + transform.position;

		Clock clock;
		clock.Start();

		std::vector<Entity *> terrian = world->CreateCollection([](Entity *entity) {
			return entity->HasCollider();
		});

		for (Entity *entity : terrian)
		{
			Capsule capcol = collider;

			capcol.bot += transform.position;
			capcol.top += transform.position;

			Collider other_col = entity->GetGlobalCollider();

			if (other_col.type == ColliderType::MESH)
			{
				IntersectionInfo final_info = {};
				for (Triangle tri : other_col.mesh)
				{
					IntersectionInfo info = {};
					bool r = CheckIntersectionCapsuleTriangle(capcol, tri, &info);
					if (r)
					{
						if (info.penetration > final_info.penetration)
						{
							final_info = info;
						}
					}
				}

				if (final_info.penetration > 0)
				{

					real32 theta = Dot(final_info.normal, Vec3f(0, 1, 0));
					if (theta > DegToRad(90.0f - max_slope_angle))
					{
						// @HACK: Just negating all sliding motion if we think we are on ground
						Vec3f undesired_motion = final_info.normal * (final_info.penetration + 0.0001f);

						transform.position.y += undesired_motion.y;
						velocity.y = 0.0f;						// @NOTE: Remove any vertical movement
						acceleration.y = 0.0f;					// @NOTE: Remove any vertical movement

						grounded = true;
					}
					else
					{
						//LOG("Resolving collision");
						//LOG(final_info.penetration);
						//LOGTS(final_info.normal);
						Vec3f undesired_motion = final_info.normal * (final_info.penetration + 0.0001f);
						undesired_motion.y = 0.0f; // @TODO: Remove if jumping ?

						velocity += undesired_motion;
						transform.position += undesired_motion;
					}
				}
			}

#if 0
			Vec3f inv = Vec3f(1.0f) / collider.radius;
			Vec3f pos = transform.position * inv;
			Vec3f vel = acceleration * dtime * dtime + velocity * dtime * inv;

			for (Triangle tri : col)
			{
				Triangle local_tri = CreateTriangle(tri.v0 * inv, tri.v1 * inv, tri.v2 * inv);
				Plane plane = CreatePlane(local_tri);

				real32 cp = GetPlaneScalar(plane);

				real32 nume = 1.0f - Dot(plane.normal, pos) + cp;
				real32 demo = Dot(plane.normal, vel);

				if (Equal(demo, 0.0f))
				{

				}
				else
				{
					real32 t = nume / demo;
					if (t > 0 && t < 1)
					{
						//@NOTE: We hit the plane now test if we're in the triangle
						Vec3f npos = SweepForward(pos, vel, t);
						Vec3f world_npos = npos * collider.radius;



						if (IsPointInsideTriangle(local_tri, npos))
						{
							Vec3f r = vel;
							real32 d = Dot(r, plane.normal);
							Vec3f reflect = r - d * plane.normal;

							vel = vel * collider.radius;
							velocity = reflect * collider.radius;

							Vec3f back = Normalize(-1.0f * vel) * 0.01f;

							move = false;
							transform.position = world_npos + velocity + back;
						}
						else
						{
							{
								real32 a = Dot(vel, vel);
								real32 b = 2.0f * (Dot(vel, pos - local_tri.v0));
								real32 c = MagSqrd(local_tri.v0 - pos) - 1;

								Vec2f roots = QuadracticSolve(a, b, c);
								t = Min(roots.x, roots.y);
							}

							{
								real32 a = Dot(vel, vel);
								real32 b = 2.0f * (Dot(vel, pos - local_tri.v1));
								real32 c = MagSqrd(local_tri.v1 - pos) - 1;

								Vec2f roots = QuadracticSolve(a, b, c);
								t = Min(roots.x, roots.y, t);
							}

							{
								real32 a = Dot(vel, vel);
								real32 b = 2.0f * (Dot(vel, pos - local_tri.v2));
								real32 c = MagSqrd(local_tri.v2 - pos) - 1;

								Vec2f roots = QuadracticSolve(a, b, c);
								t = Min(roots.x, roots.y, t);
							}

							{
								Vec3f edge = local_tri.v1 - local_tri.v0;
								Vec3f base = local_tri.v0 - pos;

								real32 e2 = MagSqrd(edge);
								real32 edv = Dot(edge, velocity);
								real32 vdb = Dot(vel, base);
								real32 edb = Dot(edge, base);

								real32 a = e2 - MagSqrd(vel) + edv * edv;
								real32 b = e2 * 2.0f * vdb - 2.0f * (edv * edb);
								real32 c = e2 * (1 - MagSqrd(base)) + (edb * edb);

								Vec2f roots = QuadracticSolve(a, b, c);
								real32 min = Min(roots.x, roots.y);

								real32 tt = (edv * min - edb) / e2;
								if (tt >= 0.0f && tt <= 1.0f)
									t = Min(tt, t);

								LOG(t);
							}

							{
								Vec3f edge = local_tri.v2 - local_tri.v1;
								Vec3f base = local_tri.v1 - pos;

								real32 e2 = MagSqrd(edge);
								real32 edv = Dot(edge, velocity);
								real32 vdb = Dot(vel, base);
								real32 edb = Dot(edge, base);

								real32 a = e2 - MagSqrd(vel) + edv * edv;
								real32 b = e2 * 2.0f * vdb - 2.0f * (edv * edb);
								real32 c = e2 * (1 - MagSqrd(base)) + (edb * edb);

								Vec2f roots = QuadracticSolve(a, b, c);
								real32 min = Min(roots.x, roots.y);

								real32 tt = (edv * min - edb) / e2;
								if (tt >= 0.0f && tt <= 1.0f)
									t = Min(tt, t);

								LOG(t);
							}

							{
								Vec3f edge = local_tri.v0 - local_tri.v2;
								Vec3f base = local_tri.v2 - pos;

								real32 e2 = MagSqrd(edge);
								real32 edv = Dot(edge, velocity);
								real32 vdb = Dot(vel, base);
								real32 edb = Dot(edge, base);

								real32 a = e2 - MagSqrd(vel) + edv * edv;
								real32 b = e2 * 2.0f * vdb - 2.0f * (edv * edb);
								real32 c = e2 * (1 - MagSqrd(base)) + (edb * edb);

								Vec2f roots = QuadracticSolve(a, b, c);
								real32 min = Min(roots.x, roots.y);

								real32 tt = (edv * min - edb) / e2;

								if (tt >= 0.0f && tt <= 1.0f)
									t = Min(tt, t);

								LOG(t);
							}

							npos = SweepForward(pos, vel, t);
							world_npos = npos * collider.radius;

							Vec3f r = vel;
							real32 d = Dot(r, plane.normal);
							Vec3f reflect = r - d * plane.normal;

							vel = vel * collider.radius;
							velocity = reflect * collider.radius;

							Vec3f back = Normalize(-1.0f * vel) * 0.01f;

							move = false;
							transform.position = world_npos + velocity + back;
						}

					}
				}
			}
#endif
		}

		Vec3f ending_position = transform.position;
		if (grounded)
		{
			distance_to_make_walk_sound = 0.3f;
			real32 distance_traveled_this_frame = DistanceSqrd(starting_position, ending_position);
			distance_since_last_walk_sound += distance_traveled_this_frame;
			if (distance_since_last_walk_sound >= distance_to_make_walk_sound)
			{
				distance_since_last_walk_sound = 0.0f;
				speaker.PlaySound("footstepssounds/Footsteps_Casual_Metal_04.wav");
			}
		}
		clock.End();
		//std::cout << clock.Get().delta_milliseconds << std::endl;
	}

	Vec3f Player::SweepForward(const Vec3f &pos, const Vec3f &velo, real32 t)
	{
		return pos + velo * t;
	}

	Vec2f Player::QuadracticSolve(const real32 &a, const real32 &b, const real32 &c)
	{
		Vec2f result(2.0f, 2.0f);

		real32 discm = b * b - 4.0f * a * c;
		real32 aa = 2.0f * a;

		if (discm >= 0)
		{
			real32 d = Sqrt(discm);

			result.x = (-b + d) / aa;
			result.y = (-b - d) / aa;
		}

		return result;
	}

}
