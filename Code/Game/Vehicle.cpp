#include "Game/Vehicle.hpp"
#include "Game/SteeringBehavior.hpp"
#include "Game/Game.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"

Vehicle::Vehicle(Game* game, const Vec2& pos, const float rotation_degrees, const Vec2& velocity, 
	const float mass, const float max_force, const float max_speed, const float max_turn_speed_deg,
	const float scale, Behavior behavior):
		MovingEntity(pos, scale, velocity, max_speed, 
			Vec2(CosDegrees(rotation_degrees),	SinDegrees(rotation_degrees)), 
			mass, Vec2(scale, scale), max_turn_speed_deg, max_force),
	m_theGame(game), m_behavior(behavior)
{
	m_steering = new SteeringBehavior(this);
	InitVisuals();
	InitDebugVisuals();
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
	
	delete m_mesh;
	m_mesh = nullptr;

	delete m_steering;
	m_steering = nullptr;
}

void Vehicle::Update(const double delta_seconds)
{
	//debugging
	const Vec2 steering_force = m_steering->Calculate(m_behavior, m_target, 1.0f);

	// Acceleration = force/mass
	const Vec2 acceleration = steering_force * m_inverseMass;
	
	// Velocity = v_0 + a*t
	m_velocity += acceleration * static_cast<float>(delta_seconds);
	m_velocity.ClampLength(m_maxSpeed);
	m_position += m_velocity * static_cast<float>(delta_seconds);

	// always update the local coordinate unless we are not moving
		// if 0.0f then we divide by zero
		// if VARY small, we might have floating point precision error
	if(m_velocity.GetLengthSquared() > 0.000001f)
	{
		m_forward = m_velocity.GetNormalized();
		m_tangent = m_forward.GetRotated90Degrees();
	}

	// for debugging, if the point goes off screen, wrap it around
	m_position.WrapAround(-1.0f * WORLD_HEIGHT * WORLD_ASPECT,
		-1.0f * WORLD_HEIGHT, WORLD_HEIGHT * WORLD_ASPECT, WORLD_HEIGHT);


	UpdateModelMatrix();
	UpdateDebugArrows(steering_force);
}


void Vehicle::Render() const
{
	g_theRenderer->BindModelMatrix(m_modelMatrix);
	g_theRenderer->BindMaterial(*m_material);
	g_theRenderer->DrawMesh(*m_mesh);


	// if Debugging
	g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);
	g_theRenderer->BindMaterial(*m_forwardMaterial);
	g_theRenderer->DrawMesh(*m_forwardMesh);

	g_theRenderer->BindMaterial(*m_steeringMaterial);
	g_theRenderer->DrawMesh(*m_steeringMesh);
}


void Vehicle::SetTarget(const Vec2& target_pos)
{
	m_target = target_pos;
}


void Vehicle::InitVisuals()
{
	// Get Everything to draw the triangle
	m_material = g_theRenderer->CreateOrGetMaterial("white", false);
	m_material->SetShader("default_lit.hlsl");
	m_material->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);
	TextureView* white_texture(reinterpret_cast<TextureView*>(g_theRenderer->CreateOrGetTextureView2D("0xFFFFFFFF")));
	m_material->SetDiffuseMap(white_texture);

	CPUMesh triangle_mesh;
	CpuMeshAddTriangle(&triangle_mesh, GetBoundingRadius(), Rgba::RED);
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


void Vehicle::UpdateModelMatrix()
{
	m_modelMatrix.SetTvec3(Vec3(m_position.x, m_position.y, 0.0f));
	const float cur_rot_deg = GetRotationDegrees();
	const Matrix44 rotation_matrix = Matrix44::MakeZRotationDegrees(cur_rot_deg);
	m_modelMatrix.SetRotationMatrix(rotation_matrix);

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
	CpuMeshAddLine(&forward_line_mesh, m_position, m_position + m_forward, 1.0f, Rgba::BLACK);
	m_forwardMesh = new GPUMesh(g_theRenderer);
	m_forwardMesh->CreateFromCPUMesh<Vertex_Lit>(forward_line_mesh);

	if (m_steeringMesh != nullptr)
	{
		delete m_steeringMesh;
		m_steeringMesh = nullptr;
	}

	CPUMesh steering_line_mesh;
	CpuMeshAddLine(&steering_line_mesh, m_position, m_position + steering_force, 1.0f, Rgba::RED);
	m_steeringMesh = new GPUMesh(g_theRenderer);
	m_steeringMesh->CreateFromCPUMesh<Vertex_Lit>(steering_line_mesh);
}
