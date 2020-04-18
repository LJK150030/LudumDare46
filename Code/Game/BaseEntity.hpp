#pragma once
#include "Game/GameCommon.hpp" 


class BaseEntity
{
public:
	enum
	{
		DEFAULT_ENTITY_TYPE = -1
	};


protected:
	Vec2	m_position;
	Vec2	m_scale;
	float	m_boundingRadius;

	
private:
	static uint NextID;

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
	

private:
	// Helper 
	uint NextValidID();

};
