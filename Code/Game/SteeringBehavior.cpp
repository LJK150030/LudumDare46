#include "Game/SteeringBehavior.hpp"
#include "Game/Vehicle.hpp"
#include "Engine/Math/MathUtils.hpp"

SteeringBehavior::SteeringBehavior(Vehicle* agent): m_vehicle(agent), m_steeringForce(Vec2::ZERO)
{
}


SteeringBehavior::~SteeringBehavior()
{
}

Vec2 SteeringBehavior::Calculate(const Behavior behavior, const Vec2& target_pos, float scalar_modifier)
{
	switch(behavior)
	{
		case CONSTANT_DIR: 
		{
			return Vec2::ZERO;
		}
		case SEEK:
		{
			return Seek(target_pos);
		}
		case FLEE: 
		{
			return Flee(target_pos);
		}
		case ARRIVE: 
		{
			return Arrive(target_pos, scalar_modifier);
		}
		case PURSUIT:
		{
			return Vec2::ZERO;
		}
		default:
		{
			return Vec2::ZERO;
		}
	}
}


Vec2 SteeringBehavior::Seek(const Vec2& target_pos)
{
	Vec2 direction = target_pos - m_vehicle->GetPosition();
	direction.Normalize();

	const Vec2 desired_velocity = direction * m_vehicle->GetMaxSpeed();

	const Vec2 seek_force = desired_velocity - m_vehicle->GetVelocity();
	
	return seek_force;
}


Vec2 SteeringBehavior::Flee(const Vec2& target_pos)
{
	Vec2 direction = m_vehicle->GetPosition() - target_pos;
	direction.Normalize();

	const Vec2 desired_velocity = direction * m_vehicle->GetMaxSpeed();

	const Vec2 seek_force = desired_velocity - m_vehicle->GetVelocity();

	return seek_force;
}


Vec2 SteeringBehavior::Arrive(const Vec2& target_pos, float scalar_modifier)
{
	const Vec2 direction = target_pos - m_vehicle->GetPosition();

	const float dist = direction.GetLength();

	Vec2 seek_force = Vec2::ZERO;
	
	if(dist > 0.0f)
	{
		float speed = dist * scalar_modifier;
		speed = Min(speed, m_vehicle->GetMaxSpeed());
		const Vec2 desired_velocity = direction * speed / dist;
		seek_force = desired_velocity - m_vehicle->GetVelocity();
	}

	return seek_force;
}
