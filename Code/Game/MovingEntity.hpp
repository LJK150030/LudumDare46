#pragma once
#include "Game/BaseEntity.hpp"

class MovingEntity : public BaseEntity
{
public:

protected:
	Vec2	m_velocity;

	// local coord
	Vec2	m_forward;
	Vec2	m_tangent;

	float m_mass;
	float m_maxSpeed;			// meters per second
	float m_maxAcceleration;	// meters per second^2
	float m_maxTurnSpeedDeg;	// degrees per second

private:


public:
	explicit MovingEntity(const Vec2& position, float bound_radius, const Vec2& velocity, float max_speed,
		const Vec2& forward_norm, float mass, const Vec2& scale, float max_turn_speed_deg,
		float max_acceleration);
	virtual ~MovingEntity();

	// Accessors
	Vec2	GetVelocity() const;
	float	GetSpeed() const;
	float	GetSpeedSq() const;
	bool	IsSpeedMaxedOut() const;
	Vec2	GetForward() const;
	Vec2	GetTangent() const;
	float	GetMass() const;
	float	GetMaxSpeed() const;
	float	GetMaxAcceleration() const;
	float	GetMaxTurnSpeedDeg() const;
	
	// Mutators
	void SetVelocity(const Vec2& new_vel);
	void SetMaxSpeed(float new_max_speed);
	void SetMaxAcceleration(float new_max_acc);
	void SetForward(const Vec2& new_forward_normal);
	bool RotateForwardToFaceTarget(const Vec2& target_pos);
	void SetMaxTurnSpeed(float new_turn_speed_deg);
};