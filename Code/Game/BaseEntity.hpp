#pragma once
#include "Game/GameCommon.hpp" 
#include "Engine/Math/Matrix44.hpp"

class GPUMesh;
class Material;

class BaseEntity
{	
protected:
	float	m_boundingRadius;

	//Render Data
	Matrix44	m_modelMatrix = Matrix44::IDENTITY;
	Material*	m_material = nullptr;
	GPUMesh*	m_mesh = nullptr;
	
private:
	static uint NextID;

	//Meta Data
	uint		m_id; // unique ID
	int			m_entityType;
	bool		m_boolTag;


public:
	// Really should not be  calling Base entity constructor, only allow children to call this
	BaseEntity();
	explicit BaseEntity(int entity_type);
	explicit BaseEntity(int entity_type, const Vec2& pos, float bounding_radius);
	virtual ~BaseEntity();

	//Initializers
	virtual void Init();
	virtual void InitVisuals();

	// Common
	virtual void Update(double delta_seconds);
	virtual void Render() const;

	
	// Accessors
	Vec2	GetPosition() const;
	Vec2	GetScale() const;
	float	GetBoundingRadius() const;
	uint	GetID() const;
	int		GetType() const;
	bool	IsTagged() const;
	
	// Mutators
	void	SetPos(const Vec2& new_pos);
	void	SetScale(const Vec2& new_scale);
	void	SetScale(float scalar_value);
	void	SetBoundingRadius(float new_radius);
	void	SetEntityType(int new_type);
	void	Tag();
	void	UnTag();


private:
	uint NextValidID();
};
