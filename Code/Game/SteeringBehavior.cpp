#include "Game/SteeringBehavior.hpp"
#include "Game/Vehicle.hpp"
#include "Engine/Math/MathUtils.hpp"

SteeringBehavior::SteeringBehavior(Vehicle* agent): m_vehicle(agent)
{
}


SteeringBehavior::~SteeringBehavior()
{
}


Vec2 SteeringBehavior::Calculate(const Behavior behavior, const Vec2& target_pos, const Vehicle* evader, 
	const float scalar_modifier)
{
	switch(behavior)
	{
		case CONSTANT_DIR: 
		{
			return Vec2::ZERO;
		}
		case STEER_SEEK:
		{
			return Seek(target_pos);
		}
		case STEER_FLEE:
		{
			return Flee(target_pos);
		}
		case STEER_ARRIVE:
		{
			return Arrive(target_pos, scalar_modifier);
		}
		case STEER_PURSUIT:
		{
			if(evader != nullptr)
			{
				return Pursuit(evader);
			}
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


Vec2 SteeringBehavior::Arrive(const Vec2& target_pos, const float scalar_modifier)
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


Vec2 SteeringBehavior::Pursuit(const Vehicle* evader, const float heading_towards_tolerance_fraction, 
	const float turnaround_coefficient)
{
	// if we are in front of the evader and heading towards them, then just seek
	const Vec2 to_evader = evader->GetPosition() - m_vehicle->GetPosition();
	const float relative_heading_v_2_e = DotProduct(m_vehicle->GetForward(), evader->GetForward());
	const float relative_direction = DotProduct(to_evader, m_vehicle->GetForward());

	if(relative_direction > 0 && relative_heading_v_2_e < -1.0f * heading_towards_tolerance_fraction)
	{
		return Seek(evader->GetPosition());
	}
	
	const float sum_of_vehicles_velocity = m_vehicle->GetMaxSpeed() + evader->GetSpeed();
	float look_ahead_time = to_evader.GetLength() / sum_of_vehicles_velocity;
	look_ahead_time += TurnaroundTime(m_vehicle, evader->GetPosition(), turnaround_coefficient);
	
	const Vec2 predicted_position = evader->GetPosition() + evader->GetVelocity() * look_ahead_time;
	return Seek(predicted_position);
}


float SteeringBehavior::TurnaroundTime(const Vehicle* agent, const Vec2& target_pos, const float coefficient) const
{
	Vec2 to_target = target_pos - m_vehicle->GetPosition();
	to_target.Normalize();

	const float relative_direction = DotProduct(agent->GetForward(), to_target);
	const float offset_rel_dir = relative_direction - 1.0f;
	const float flip_with_coefficient = offset_rel_dir * coefficient;
	return flip_with_coefficient;
}
