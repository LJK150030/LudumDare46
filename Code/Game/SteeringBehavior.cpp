#include "Game/SteeringBehavior.hpp"
#include "Game/Vehicle.hpp"

SteeringBehavior::SteeringBehavior(Vehicle* agent): m_vehicle(agent), m_steeringForce(Vec2::ZERO)
{
}


SteeringBehavior::~SteeringBehavior()
{
}

Vec2 SteeringBehavior::Calculate()
{
	m_steeringForce = Vec2::ZERO;
	return m_steeringForce;
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
