#include "Game/BaseEntity.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"

STATIC uint BaseEntity::NextID = 0;

BaseEntity::BaseEntity() : m_boundingRadius(0.0f), m_id(NextValidID()),
	m_entityType(DEFAULT_ENTITY_TYPE), m_boolTag(false)
{
	m_modelMatrix.SetPosition(Vec2::ZERO);
	m_modelMatrix.SetScale(Vec2::ONE);
}


BaseEntity::BaseEntity(const int entity_type) :	m_boundingRadius(0.0f),	m_id(NextValidID()),
	m_entityType(entity_type), m_boolTag(false)
{
	m_modelMatrix.SetPosition(Vec2::ZERO);
	m_modelMatrix.SetScale(Vec2::ONE);
}


BaseEntity::BaseEntity(const int entity_type, const Vec2& pos, const float bounding_radius):
	m_boundingRadius(bounding_radius), m_id(NextValidID()), m_entityType(entity_type), m_boolTag(false)
{
	m_modelMatrix.SetPosition(pos);
	m_modelMatrix.SetScale(Vec2(bounding_radius, bounding_radius));
}

BaseEntity::~BaseEntity()
{
	delete m_mesh;
	m_mesh = nullptr;
}


void BaseEntity::Update(double delta_seconds)
{
	UNUSED(delta_seconds);
}


void BaseEntity::Render() const
{
}


void BaseEntity::Init()
{
	InitVisuals();
}


void BaseEntity::InitVisuals()
{
	// Get Everything to draw the triangle
	m_material = g_theRenderer->CreateOrGetMaterial("white", false);
	m_material->SetShader("default_lit.hlsl");
	m_material->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);
	TextureView* white_texture(reinterpret_cast<TextureView*>(g_theRenderer->CreateOrGetTextureView2D("0xFFFFFFFF")));
	m_material->SetDiffuseMap(white_texture);

	CPUMesh disc_mesh;
	CpuMeshAddDisc(&disc_mesh, Rgba::WHITE, GetBoundingRadius());
	m_mesh = new GPUMesh(g_theRenderer);
	m_mesh->CreateFromCPUMesh<Vertex_Lit>(disc_mesh);
}


Vec2 BaseEntity::GetPosition() const
{
	return m_modelMatrix.GetPosition2D();
}


Vec2 BaseEntity::GetScale() const
{
	return m_modelMatrix.GetScale2D();
}


float BaseEntity::GetBoundingRadius() const
{
	return m_boundingRadius;
}


uint BaseEntity::GetID() const
{
	return m_id;
}


int BaseEntity::GetType() const
{
	return m_entityType;
}


bool BaseEntity::IsTagged() const
{
	return m_boolTag;
}


void BaseEntity::SetPos(const Vec2& new_pos)
{
	m_modelMatrix.SetPosition(new_pos);
}


void BaseEntity::SetScale(const Vec2& new_scale)
{
	const Vec2 scale = GetScale();
	m_boundingRadius *= Max(new_scale.x, new_scale.y) / Max(scale.x, scale.y);
	m_modelMatrix.SetScale(new_scale);
}


void BaseEntity::SetScale(float scalar_value)
{
	const Vec2 scale = GetScale();
	m_boundingRadius *= (scalar_value / Max(scale.x, scale.y));
	m_modelMatrix.SetScale(Vec2(scalar_value, scalar_value));
}


void BaseEntity::SetBoundingRadius(float new_radius)
{
	m_boundingRadius = new_radius;
}


void BaseEntity::SetEntityType(int new_type)
{
	m_entityType = new_type;
}


void BaseEntity::Tag()
{
	m_boolTag = true;
}


void BaseEntity::UnTag()
{
	m_boolTag = false;
}


uint BaseEntity::NextValidID()
{
	return NextID++;
}
