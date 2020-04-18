#include "Game/SteeringBehavior.hpp"
#include "Game/Vehicle.hpp"

SteeringBehavior::SteeringBehavior(Vehicle* agent): m_vehicle(agent)
{
}


SteeringBehavior::~SteeringBehavior()
{
}


Vec2 SteeringBehavior::Seek(const Vec2& target_pos)
{
	Vec2 direction = target_pos - m_vehicle->GetPosition();
	direction.Normalize();

	const Vec2 desired_velocity = direction * m_vehicle->GetMaxSpeed();

	const Vec2 seek_force = desired_velocity - m_vehicle->GetVelocity();
	
	return seek_force;
}
