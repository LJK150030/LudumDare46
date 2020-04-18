#include "Game/MovingEntity.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"

MovingEntity::MovingEntity(const Vec2& position, float bound_radius, const Vec2& velocity, 
	float max_speed, const Vec2& forward_norm, float mass, const Vec2& scale, 
	float max_turn_speed_deg, float max_acceleration): BaseEntity(0, position, bound_radius),
	m_velocity(velocity), m_forward(forward_norm), m_tangent(forward_norm.GetRotated90Degrees()),
	m_mass(mass), m_maxSpeed(max_speed), m_maxAcceleration(max_acceleration),
	m_maxTurnSpeedDeg(max_turn_speed_deg)
{
	m_scale = scale;
}


MovingEntity::~MovingEntity()
{
}


Vec2 MovingEntity::GetVelocity() const
{
	return m_velocity;
}


float MovingEntity::GetSpeed() const
{
	return m_velocity.GetLength();
}


float MovingEntity::GetSpeedSq() const
{
	return m_velocity.GetLengthSquared();
}


bool MovingEntity::IsSpeedMaxedOut() const
{
	const float max_speed_sqrt = m_maxSpeed * m_maxSpeed;
	const float cur_speed_sqrt = GetSpeedSq();
	
	return max_speed_sqrt >= cur_speed_sqrt;
}


Vec2 MovingEntity::GetForward() const
{
	return m_forward;
}


Vec2 MovingEntity::GetTangent() const
{
	return m_tangent;
}


float MovingEntity::GetMass() const
{
	return m_mass;
}


float MovingEntity::GetMaxSpeed() const
{
	return m_maxSpeed;
}


float MovingEntity::GetMaxAcceleration() const
{
	return m_maxAcceleration;
}


float MovingEntity::GetMaxTurnSpeedDeg() const
{
	return m_maxTurnSpeedDeg;
}


void MovingEntity::SetVelocity(const Vec2& new_vel)
{
	m_velocity = new_vel;
}

void MovingEntity::SetMaxSpeed(float new_max_speed)
{
	m_maxSpeed = new_max_speed;
}


void MovingEntity::SetMaxAcceleration(float new_max_acc)
{
	m_maxAcceleration = new_max_acc;
}


void MovingEntity::SetForward(const Vec2& new_forward_normal)
{
	float new_forward_length_sq = new_forward_normal.GetLengthSquared();
	float zero_test = new_forward_length_sq - 1.0f;
	ASSERT_OR_DIE(IsZero(zero_test), "Forward vector is not normalized.")

	m_forward = new_forward_normal;
	m_tangent = m_forward.GetRotated90Degrees();
}


bool MovingEntity::RotateForwardToFaceTarget(const Vec2& target_pos)
{
	Vec2 pos_to_target = target_pos - m_position;
	pos_to_target.Normalize();

	// A dot B = |A|*|B|*cos(theta)
	const float dot_product = DotProduct(m_forward, pos_to_target);
	const float angle_radians = ArcCosRadians(dot_product);

	//If we are facing it, then we don't need to rotate
	if(IsZero(angle_radians))
	{
		return true;
	}

	//otherwise, rotate by the maximum amount
	float angle_degrees = ConvertRadiansToDegrees(angle_radians);
	float rotate_amount = GetTurnedToward()
}





