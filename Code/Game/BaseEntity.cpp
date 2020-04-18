#include "Game/BaseEntity.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

STATIC uint BaseEntity::NextID = 0;

BaseEntity::BaseEntity() : m_position(Vec2::ZERO), m_scale(Vec2::ONE), m_boundingRadius(0.0f),
	m_id(NextValidID()), m_entityType(DEFAULT_ENTITY_TYPE), m_boolTag(false)
{
}


BaseEntity::BaseEntity(const int entity_type) : m_position(Vec2::ZERO), m_scale(Vec2::ONE),
	m_boundingRadius(0.0f),	m_id(NextValidID()), m_entityType(entity_type), m_boolTag(false)
{
}


BaseEntity::BaseEntity(const int entity_type, const Vec2& pos, const float bounding_radius): m_position(pos),
	m_scale(Vec2::ONE), m_boundingRadius(bounding_radius), m_id(NextValidID()), m_entityType(entity_type),
	m_boolTag(false)
{
}

BaseEntity::~BaseEntity()
{
}


void BaseEntity::Update(double delta_seconds)
{
	UNUSED(delta_seconds);
}


void BaseEntity::Render() const
{
}


Vec2 BaseEntity::GetPosition() const
{
	return m_position;
}


Vec2 BaseEntity::GetScale() const
{
	return m_scale;
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
	m_position = new_pos;
}


void BaseEntity::SetScale(const Vec2& new_scale)
{
	m_boundingRadius *= Max(new_scale.x, new_scale.y) / Max(m_scale.x, m_scale.y);
	m_scale = new_scale;
}


void BaseEntity::SetScale(float scalar_value)
{
	m_boundingRadius *= (scalar_value / Max(m_scale.x, m_scale.y));
	m_scale = Vec2(scalar_value, scalar_value);
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
