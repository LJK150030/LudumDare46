#pragma once
#include "Game/GameCommon.hpp" 
#include "Engine/Math/Matrix44.hpp"

class GPUMesh;
class Material;

class BaseEntity
{
public:
	enum
	{
		DEFAULT_ENTITY_TYPE = -1
	};

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
	virtual ~BaseEntity();

	// Common
	virtual void Update(double delta_seconds);
	virtual void Render() const;
	//virtual bool HandleMessage(const Telegram& msg);
	//virtual void WriteBlackboard();
	//virtual void ReadBlackboard();
	
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
	
protected:
	BaseEntity();
	explicit BaseEntity(int entity_type);
	explicit BaseEntity(int entity_type, const Vec2& pos, float bounding_radius);

	//Initializers
	virtual void Init();
	virtual void InitVisuals();

private:
	// Helper 
	uint NextValidID();
};
