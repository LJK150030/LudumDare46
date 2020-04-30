#include "Game/WallEntity.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"

#include "Engine/Math/MathUtils.hpp"

WallEntity::WallEntity(Game* game, const float wall_length, const Vec2& forward, const float signed_distance):
	BaseEntity(ENTITY_WALL), m_theGame(game), m_plane(forward, signed_distance),
	m_wallHalfLength(wall_length*0.5f)
{
}

WallEntity::~WallEntity()
{
}

void WallEntity::Init()
{
	Vec2 center = m_plane.PointOnPlane();
	m_modelMatrix.SetTvec(center);
	
	const Vec2 forward = m_plane.GetDirection();
	m_modelMatrix.SetIvec(forward);
	m_modelMatrix.SetJvec(forward.GetRotated90Degrees());

	InitVisuals();
}

void WallEntity::InitVisuals()
{
	// Get Everything to draw the triangle
	m_material = g_theRenderer->CreateOrGetMaterial("Yellow", false);
	m_material->SetShader("default_lit.hlsl");
	m_material->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);
	TextureView* yellow_texture(reinterpret_cast<TextureView*>(g_theRenderer->CreateOrGetTextureView2D("0xFF0000FF")));
	m_material->SetDiffuseMap(yellow_texture);

	Vec2 left(-m_wallHalfLength, 0.0f);
	Vec2 right(m_wallHalfLength, 0.0f);
	
	CPUMesh line_mesh;
	CpuMeshAddLine(&line_mesh, left, right, 2.0f, Rgba::WHITE);
	m_mesh = new GPUMesh(g_theRenderer);
	m_mesh->CreateFromCPUMesh<Vertex_Lit>(line_mesh);
}

void WallEntity::Update(double delta_seconds)
{
	UNUSED(delta_seconds);
}

void WallEntity::Render() const
{
	const Matrix44 model_matrix(m_modelMatrix);
	g_theRenderer->BindModelMatrix(model_matrix);
	g_theRenderer->BindMaterial(*m_material);
	g_theRenderer->DrawMesh(*m_mesh);
}


Plane2 WallEntity::GetPlane() const
{
	return m_plane;
}

float WallEntity::GetPlanHalfLength() const
{
	return m_wallHalfLength;
}
