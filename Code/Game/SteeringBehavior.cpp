#include "Game/SteeringBehavior.hpp"
#include "Game/Vehicle.hpp"
#include "Game/Game.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Ray2.hpp"

SteeringBehavior::SteeringBehavior(Vehicle* agent) : m_vehicle(agent), m_wanderTarget(agent->GetPosition())
{
}


SteeringBehavior::~SteeringBehavior()
{
}


Vec2 SteeringBehavior::Calculate(const Behavior behavior)
{
	switch(behavior)
	{
		case CONSTANT_DIR: 
		{
			return Vec2::ZERO;
		}
		case STEER_SEEK:
		{
			return Seek(m_target);
		}
		case STEER_FLEE:
		{
			return Flee(m_target);
		}
		case STEER_ARRIVE:
		{
			return Arrive(m_target);
		}
		case STEER_PURSUIT:
		{
			if(m_movingTarget != nullptr)
			{
				return Pursuit(m_movingTarget);
			}
		}
		case STEER_EVADE:
		{
			if (m_movingTarget != nullptr)
			{
				return Evade(m_movingTarget);
			}
		}
		case STEER_WANDER:
		{
			return Wander();
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


Vec2 SteeringBehavior::Arrive(const Vec2& target_pos)
{
	const Vec2 direction = target_pos - m_vehicle->GetPosition();

	const float dist = direction.GetLength();

	Vec2 seek_force = Vec2::ZERO;
	
	if(dist > 0.0f)
	{
		float speed = dist * m_scalarModifier;
		speed = Min(speed, m_vehicle->GetMaxSpeed());
		const Vec2 desired_velocity = direction * speed / dist;
		seek_force = desired_velocity - m_vehicle->GetVelocity();
	}

	return seek_force;
}


Vec2 SteeringBehavior::Pursuit(const Vehicle* evader)
{
	// if we are in front of the evader and heading towards them, then just seek
	const Vec2 to_evader = evader->GetPosition() - m_vehicle->GetPosition();
	const float relative_heading_v_2_e = DotProduct(m_vehicle->GetForward(), evader->GetForward());
	const float relative_direction = DotProduct(to_evader, m_vehicle->GetForward());

	if(relative_direction > 0 && relative_heading_v_2_e < -1.0f * m_headingTowardsTolerance)
	{
		return Seek(evader->GetPosition());
	}
	
	const float sum_of_vehicles_velocity = m_vehicle->GetMaxSpeed() + evader->GetSpeed();
	float look_ahead_time = to_evader.GetLength() / sum_of_vehicles_velocity;
	look_ahead_time += TurnaroundTime(m_vehicle, evader->GetPosition(), m_turnaroundCoefficient);
	
	const Vec2 predicted_position = evader->GetPosition() + evader->GetVelocity() * look_ahead_time;
	return Seek(predicted_position);
}


Vec2 SteeringBehavior::Evade(const Vehicle* pursuer)
{
	const Vec2 to_pursuer = pursuer->GetPosition() - m_vehicle->GetPosition();

	const float sum_of_vehicles_velocity = m_vehicle->GetMaxSpeed() + pursuer->GetSpeed();
	float look_ahead_time = to_pursuer.GetLength() / sum_of_vehicles_velocity;

	const Vec2 predicted_position = pursuer->GetPosition() + pursuer->GetVelocity() * look_ahead_time;
	return Flee(predicted_position);
}


Vec2 SteeringBehavior::Wander()
{
	float random_x = g_randomNumberGenerator.GetRandomFloatInRange(-1.0f, 1.0f);
	float random_y = g_randomNumberGenerator.GetRandomFloatInRange(-1.0f, 1.0f);
	m_wanderTarget += Vec2(random_x * m_wanderJitter, random_y * m_wanderJitter);

	m_wanderTarget.Normalize();
	m_wanderTarget *= m_wanderRadius;
	const Vec2 target_local = m_wanderTarget + Vec2(m_wanderDistance, 0.0f);

	Vec2 target_world = PointToWorldSpace(target_local, m_vehicle->GetForward(), m_vehicle->GetTangent(),
		m_vehicle->GetPosition());

	Vec2 steering_force = target_world - m_vehicle->GetPosition();
	return steering_force;
}


Vec2 SteeringBehavior::ObstacleAvoidance(const std::vector<BaseEntity*>& obstacles)
{
	const float frac_of_speed = m_vehicle->GetSpeed() / m_vehicle->GetMaxSpeed();
	const float detection_box_length = m_minLookAhead + m_minLookAhead * frac_of_speed;

	// Loop through all of the obstacles and tag those that are within the objects bounding radius
	Game* the_game = m_vehicle->GetTheGame();
	the_game->TagObstaclesWithinDisc(m_vehicle, detection_box_length);

	BaseEntity* closest_intersecting_obstacle = nullptr;
	float distance_to_closest_intersecting_point = INFINITY;
	Vec2 local_position_of_closest_obstacle = Vec2(INFINITY, INFINITY);

	int num_obstacles = static_cast<int>(obstacles.size());
	for(int ob_idx = 0; ob_idx < num_obstacles; ++ob_idx)
	{
		if(obstacles[ob_idx]->IsTagged())
		{
			//Transform the tagged obstacle into the vehicle's local space.
			const Vec2 local_pos = PointToLocalSpace(obstacles[ob_idx]->GetPosition(), m_vehicle->GetForward(),
				m_vehicle->GetTangent(), m_vehicle->GetPosition());

			//Early out if the local_pos.x is negative. This means the obstacle is behind us
			if(local_pos.x >= 0.0f)
			{
				const float expanded_radius = obstacles[ob_idx]->GetBoundingRadius() + m_vehicle->GetBoundingRadius();

				if(Abs(local_pos.y) < expanded_radius)
				{
					Ray2 vehicle_ray(m_vehicle->GetPosition(), m_vehicle->GetForward());
					float out_t[2] = { INFINITY, INFINITY };
					const uint impact = Raycast(out_t, vehicle_ray, local_pos, expanded_radius);

					switch (impact)
					{
						case 0: // no impact
						{
							break;
						}
						case 1: // only entering
						{
							if(out_t[0] > 0.0f && out_t[0] < distance_to_closest_intersecting_point)
							{
								distance_to_closest_intersecting_point = out_t[0];
								closest_intersecting_obstacle = obstacles[ob_idx];
								local_position_of_closest_obstacle = local_pos;
							}
							break;
						}
						case 2: // and entrance and exit
						{
							if (out_t[0] < distance_to_closest_intersecting_point)
							{
								distance_to_closest_intersecting_point = out_t[0];
								closest_intersecting_obstacle = obstacles[ob_idx];
								local_position_of_closest_obstacle = local_pos;
							}
							else if(out_t[1] < distance_to_closest_intersecting_point)
							{
								distance_to_closest_intersecting_point = out_t[1];
								closest_intersecting_obstacle = obstacles[ob_idx];
								local_position_of_closest_obstacle = local_pos;
							}
							break;
						}
					}
				}
			}
		}
	}

	return Vec2::ZERO;
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


void SteeringBehavior::SetTarget(const Vec2& target_pos)
{
	m_target = target_pos;
	m_movingTarget = nullptr;
}

void SteeringBehavior::SetArriveModifier(const float scalar_modifier)
{
	m_scalarModifier = scalar_modifier;
}

void SteeringBehavior::SetPursuitHeadTowardsTolerance(const float fraction)
{
	m_headingTowardsTolerance = fraction;
}

void SteeringBehavior::SetPursuitTurnaround(const float coefficient)
{
	m_turnaroundCoefficient = coefficient;
}


void SteeringBehavior::SetMovingTarget(const Vehicle* target_pos)
{
	m_movingTarget = target_pos;
}

void SteeringBehavior::SetRandomWalk(const float radius, const float distance, const float jitter)
{
	m_wanderRadius = radius;
	m_wanderDistance = distance;
	m_wanderJitter = jitter;
}
