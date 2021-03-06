#include "Game/Vehicle.hpp"
#include "Game/SteeringBehavior.hpp"
#include "Game/Game.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"

Vehicle::Vehicle(Game* game, const Vec2& pos, const float rotation_degrees, const Vec2& velocity,
	const float mass, const float max_force, const float max_speed, const float max_turn_speed_deg,
	const float scale, const Rgba color) :
	MovingEntity(pos, scale, velocity, max_speed,
		Vec2(CosDegrees(rotation_degrees), SinDegrees(rotation_degrees)),
		mass, Vec2(scale, scale), max_turn_speed_deg, max_force),
	m_theGame(game), m_color(color)
{
	m_steering = new SteeringBehavior(this);
	SetEntityType(ENTITY_VEHICLE);
}


Vehicle::~Vehicle()
{
	if (m_steeringMesh != nullptr)
	{
		delete m_steeringMesh;
		m_steeringMesh = nullptr;
	}

	
	if(m_forwardMesh != nullptr)
	{
		delete m_forwardMesh;
		m_forwardMesh = nullptr;
	}

	delete m_steering;
	m_steering = nullptr;
}

void Vehicle::Init()
{
	InitVisuals();
	InitDebugVisuals();
}

void Vehicle::Update(const double delta_seconds)
{
	const Vec2 steering_force = m_steering->Calculate(m_behaviors);

	// Acceleration = force/mass
	const Vec2 acceleration = steering_force * m_inverseMass;

	// Velocity = v_0 + a*t
	m_velocity += acceleration * static_cast<float>(delta_seconds);
	const float vel_length_sqrd = m_velocity.GetLengthSquared();

	// if 0.0f then we divide by zero
	// if VARY small, we might have floating point precision error
	if (vel_length_sqrd > 0.000001f)
	{
		const Vec2 vel_norm = m_velocity.GetNormalized();
		SetForward(vel_norm);

		if(vel_length_sqrd > m_maxSpeed * m_maxSpeed)
		{
			m_velocity = vel_norm * m_maxSpeed;
		}
	}

	
	Vec2 new_world_pos = GetPosition();
	new_world_pos += m_velocity * static_cast<float>(delta_seconds);
	
	new_world_pos.WrapAround(
		-1.0f * WORLD_HEIGHT * WORLD_ASPECT,
		-1.0f * WORLD_HEIGHT, 
		WORLD_HEIGHT * WORLD_ASPECT, 
		WORLD_HEIGHT_ADJUST
	);
	
	SetPos(new_world_pos);

	
	if(m_theGame->m_inDevMode)
	{
		UpdateDebugArrows(steering_force);
	}
}


void Vehicle::Render() const
{
	Matrix44 model_matrix(m_modelMatrix);
	g_theRenderer->BindModelMatrix(model_matrix);
	g_theRenderer->BindMaterial(*m_material);
	g_theRenderer->DrawMesh(*m_mesh);

	if (m_theGame->m_inDevMode)
	{
		RenderDebugArrows();
	}
}


void Vehicle::TurnOffSteering()
{
	m_behaviors.reset();
	m_steering->SetMovingTarget(nullptr);
	m_steering->SetTarget(Vec2::ZERO);
}


void Vehicle::SeekTarget(const Vec2& target_pos)
{
	m_steering->SetTarget(target_pos);
	m_behaviors[STEER_SEEK] = true;
}


void Vehicle::SeekTarget(const Vehicle* moving_target)
{
	m_steering->SetMovingTarget(moving_target);
	m_behaviors[STEER_SEEK] = true;
}


void Vehicle::FleeTarget(const Vec2& target_pos) 
{
	m_steering->SetTarget(target_pos);
	m_behaviors[STEER_FLEE] = true;
}


void Vehicle::ArriveAt(const Vec2& target_pos, const float scalar_modifier)
{
	m_steering->SetTarget(target_pos);
	m_steering->SetArriveModifier(scalar_modifier);
	m_behaviors[STEER_ARRIVE] = true;
}


void Vehicle::PursuitOn(const Vehicle* moving_target, const float head_on_tolerance_frac, 
	const float turn_around_modifier)
{
	m_steering->SetMovingTarget(moving_target);
	m_steering->SetPursuitHeadTowardsTolerance(head_on_tolerance_frac);
	m_steering->SetPursuitTurnaround(turn_around_modifier);
	m_behaviors[STEER_PURSUIT] = true;
}


void Vehicle::EvadeFrom(const Vehicle* moving_target)
{
	m_steering->SetMovingTarget(moving_target);
	m_behaviors[STEER_EVADE] = true;
}


void Vehicle::WanderAround(const float radius, const float distance, const float jitter)
{
	m_steering->SetRandomWalk(radius, distance, jitter);
	m_behaviors[STEER_WANDER] = true;
}


void Vehicle::AvoidObstacles(const float min_look_ahead, const float avoidance_mul,
	const float breaking_weight)
{
	m_steering->SetObstaclesAvoidance(min_look_ahead, avoidance_mul, breaking_weight);
	m_behaviors[STEER_OBSTACLE_AVOIDANCE] = true;
}


void Vehicle::AvoidWalls(const uint num_whiskers, const float whisker_length, const float avoidance_mul,
	const float field_of_view_degrees)
{
	m_steering->SetWallAvoidance(num_whiskers, whisker_length, avoidance_mul, field_of_view_degrees);
	m_behaviors[STEER_WALL_AVOIDANCE] = true;
}


Game* Vehicle::GetTheGame() const
{
	return m_theGame;
}


void Vehicle::InitVisuals()
{
	// Get Everything to draw the triangle
	m_material = g_theRenderer->CreateOrGetMaterial("white", false);
	m_material->SetShader("default_unlit.hlsl");
	m_material->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);
	TextureView* white_texture(reinterpret_cast<TextureView*>(g_theRenderer->CreateOrGetTextureView2D("0xFFFFFFFF")));
	m_material->SetDiffuseMap(white_texture);

	CPUMesh triangle_mesh;
	CpuMeshAddTriangle(&triangle_mesh, GetBoundingRadius(), m_color);
	m_mesh = new GPUMesh(g_theRenderer);
	m_mesh->CreateFromCPUMesh<Vertex_Lit>(triangle_mesh);
}


void Vehicle::InitDebugVisuals()
{
	// Get Everything to forward Debug
	m_forwardMaterial = g_theRenderer->CreateOrGetMaterial("black", false);
	m_forwardMaterial->SetShader("default_lit.hlsl");
	m_forwardMaterial->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);
	TextureView* black_texture(reinterpret_cast<TextureView*>(g_theRenderer->CreateOrGetTextureView2D("0x000000FF")));
	m_forwardMaterial->SetDiffuseMap(black_texture);

	CPUMesh forward_line_mesh;
	CpuMeshAddLine(&forward_line_mesh, Vec2::ZERO, Vec2(1.0f, 0.0f), 1.0f, Rgba::BLACK);
	m_forwardMesh = new GPUMesh(g_theRenderer);
	m_forwardMesh->CreateFromCPUMesh<Vertex_Lit>(forward_line_mesh);


	// Get Everything to steering Debug
	m_steeringMaterial = g_theRenderer->CreateOrGetMaterial("red", false);
	m_steeringMaterial->SetShader("default_lit.hlsl");
	m_steeringMaterial->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);
	TextureView* red_texture(reinterpret_cast<TextureView*>(g_theRenderer->CreateOrGetTextureView2D("0xFF0000FF")));
	m_steeringMaterial->SetDiffuseMap(red_texture);

	CPUMesh steering_line_mesh;
	CpuMeshAddLine(&steering_line_mesh, Vec2::ZERO, Vec2(1.0f, 0.0f), 1.0f, Rgba::BLACK);
	m_steeringMesh = new GPUMesh(g_theRenderer);
	m_steeringMesh->CreateFromCPUMesh<Vertex_Lit>(steering_line_mesh);
}


void Vehicle::UpdateDebugArrows(const Vec2& steering_force)
{
	//  Update Debug drawing for arrows
	if (m_forwardMesh != nullptr)
	{
		delete m_forwardMesh;
		m_forwardMesh = nullptr;
	}

	CPUMesh forward_line_mesh;
	CpuMeshAddLine(
		&forward_line_mesh, 
		Vec2::ZERO, 
		Vec2(1.0f, 0.0f), 
		1.0f, 
		Rgba::BLACK
	);
	
	m_forwardMesh = new GPUMesh(g_theRenderer);
	m_forwardMesh->CreateFromCPUMesh<Vertex_Lit>(forward_line_mesh);

	if (m_steeringMesh != nullptr)
	{
		delete m_steeringMesh;
		m_steeringMesh = nullptr;
	}

	CPUMesh steering_line_mesh;
	CpuMeshAddLine(
		&steering_line_mesh, 
		Vec2::ZERO, 
		Vec2(steering_force.GetLength(), 0.0f),
		1.0f, 
		Rgba::RED
	);
	
	m_steeringMesh = new GPUMesh(g_theRenderer);
	m_steeringMesh->CreateFromCPUMesh<Vertex_Lit>(steering_line_mesh);
}


void Vehicle::RenderDebugArrows() const
{
	// if Debugging
	g_theRenderer->BindMaterial(*m_forwardMaterial);
	g_theRenderer->DrawMesh(*m_forwardMesh);

	g_theRenderer->BindMaterial(*m_steeringMaterial);
	g_theRenderer->DrawMesh(*m_steeringMesh);
}
