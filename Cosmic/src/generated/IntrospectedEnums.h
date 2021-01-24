namespace cm
{
inline String ToString(const PhysicsEntityType&entry)
{
switch (entry)
{
case PhysicsEntityType::INVALID: return "INVALID";
case PhysicsEntityType::POINTMASS: return "POINTMASS";
case PhysicsEntityType::RIGIDBODY: return "RIGIDBODY";
case PhysicsEntityType::COUNT: return "COUNT";
}
return "error";
}

inline PhysicsEntityType StringToPhysicsEntityType(const String &entry)
{
if (entry == "INVALID") return PhysicsEntityType::INVALID;
if (entry == "POINTMASS") return PhysicsEntityType::POINTMASS;
if (entry == "RIGIDBODY") return PhysicsEntityType::RIGIDBODY;
if (entry == "COUNT") return PhysicsEntityType::COUNT;
return PhysicsEntityType::INVALID;}

inline String ToString(const ConnectionType&entry)
{
switch (entry)
{
case ConnectionType::INVALID: return "INVALID";
case ConnectionType::SPRING: return "SPRING";
case ConnectionType::STIFF_SPRING: return "STIFF_SPRING";
case ConnectionType::ROD: return "ROD";
case ConnectionType::CABLE: return "CABLE";
case ConnectionType::ELASTIC: return "ELASTIC";
case ConnectionType::COUNT: return "COUNT";
}
return "error";
}

inline ConnectionType StringToConnectionType(const String &entry)
{
if (entry == "INVALID") return ConnectionType::INVALID;
if (entry == "SPRING") return ConnectionType::SPRING;
if (entry == "STIFF_SPRING") return ConnectionType::STIFF_SPRING;
if (entry == "ROD") return ConnectionType::ROD;
if (entry == "CABLE") return ConnectionType::CABLE;
if (entry == "ELASTIC") return ConnectionType::ELASTIC;
if (entry == "COUNT") return ConnectionType::COUNT;
return ConnectionType::INVALID;}

inline String ToString(const ConnectionMode&entry)
{
switch (entry)
{
case ConnectionMode::INVALID: return "INVALID";
case ConnectionMode::ENTITY_CONNECTED: return "ENTITY_CONNECTED";
case ConnectionMode::ENTITY_ANCHORED: return "ENTITY_ANCHORED";
case ConnectionMode::FIXED_ANCHORED: return "FIXED_ANCHORED";
case ConnectionMode::COUNT: return "COUNT";
}
return "error";
}

inline ConnectionMode StringToConnectionMode(const String &entry)
{
if (entry == "INVALID") return ConnectionMode::INVALID;
if (entry == "ENTITY_CONNECTED") return ConnectionMode::ENTITY_CONNECTED;
if (entry == "ENTITY_ANCHORED") return ConnectionMode::ENTITY_ANCHORED;
if (entry == "FIXED_ANCHORED") return ConnectionMode::FIXED_ANCHORED;
if (entry == "COUNT") return ConnectionMode::COUNT;
return ConnectionMode::INVALID;}

inline String ToString(const CameraType&entry)
{
switch (entry)
{
case CameraType::INVALID: return "INVALID";
case CameraType::PERSPECTIVE: return "PERSPECTIVE";
case CameraType::ORTHOGRAPHIC: return "ORTHOGRAPHIC";
case CameraType::COUNT: return "COUNT";
}
return "error";
}

inline CameraType StringToCameraType(const String &entry)
{
if (entry == "INVALID") return CameraType::INVALID;
if (entry == "PERSPECTIVE") return CameraType::PERSPECTIVE;
if (entry == "ORTHOGRAPHIC") return CameraType::ORTHOGRAPHIC;
if (entry == "COUNT") return CameraType::COUNT;
return CameraType::INVALID;}

inline String ToString(const EntityBehaviour&entry)
{
switch (entry)
{
case EntityBehaviour::INVALID: return "INVALID";
case EntityBehaviour::BASE_ENTITY: return "BASE_ENTITY";
case EntityBehaviour::TRANSIENT_ENTITY: return "TRANSIENT_ENTITY";
case EntityBehaviour::UPDATABLE_ENTITY: return "UPDATABLE_ENTITY";
case EntityBehaviour::PHYSICS_ENTITY: return "PHYSICS_ENTITY";
case EntityBehaviour::COUNT: return "COUNT";
}
return "error";
}

inline EntityBehaviour StringToEntityBehaviour(const String &entry)
{
if (entry == "INVALID") return EntityBehaviour::INVALID;
if (entry == "BASE_ENTITY") return EntityBehaviour::BASE_ENTITY;
if (entry == "TRANSIENT_ENTITY") return EntityBehaviour::TRANSIENT_ENTITY;
if (entry == "UPDATABLE_ENTITY") return EntityBehaviour::UPDATABLE_ENTITY;
if (entry == "PHYSICS_ENTITY") return EntityBehaviour::PHYSICS_ENTITY;
if (entry == "COUNT") return EntityBehaviour::COUNT;
return EntityBehaviour::INVALID;}

inline String ToString(const EntityType&entry)
{
switch (entry)
{
case EntityType::INVALID: return "INVALID";
case EntityType::PARTICEL_EMITTER: return "PARTICEL_EMITTER";
case EntityType::ENVIRONMENT: return "ENVIRONMENT";
case EntityType::CAMERA: return "CAMERA";
case EntityType::PLAYER: return "PLAYER";
case EntityType::ENEMY_DUDE: return "ENEMY_DUDE";
case EntityType::TURRET: return "TURRET";
case EntityType::DRONE: return "DRONE";
case EntityType::COUNT: return "COUNT";
}
return "error";
}

inline EntityType StringToEntityType(const String &entry)
{
if (entry == "INVALID") return EntityType::INVALID;
if (entry == "PARTICEL_EMITTER") return EntityType::PARTICEL_EMITTER;
if (entry == "ENVIRONMENT") return EntityType::ENVIRONMENT;
if (entry == "CAMERA") return EntityType::CAMERA;
if (entry == "PLAYER") return EntityType::PLAYER;
if (entry == "ENEMY_DUDE") return EntityType::ENEMY_DUDE;
if (entry == "TURRET") return EntityType::TURRET;
if (entry == "DRONE") return EntityType::DRONE;
if (entry == "COUNT") return EntityType::COUNT;
return EntityType::INVALID;}

}
