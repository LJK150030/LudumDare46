#include "Game/MovingEntity.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix33.hpp"

MovingEntity::MovingEntity(const Vec2& position, float bound_radius, const Vec2& velocity, 
	float max_speed, const Vec2& forward_norm, float mass, const Vec2& scale, 
	float max_turn_speed_deg, float max_force): BaseEntity(0, position, bound_radius),
	m_velocity(velocity), m_mass(mass), m_maxSpeed(max_speed), m_maxForce(max_force),
	m_maxTurnSpeedDeg(max_turn_speed_deg)
{
	ASSERT_OR_DIE(!IsZero(mass), "Cannot have a movingf entity with 0 mass.");

	m_inverseMass = 1.0f / mass;
	SetScale(scale);
	SetForward(forward_norm);
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
	return m_modelMatrix.GetIvec2();
}


Vec2 MovingEntity::GetTangent() const
{
	return m_modelMatrix.GetJvec2();
}


float MovingEntity::GetMass() const
{
	return m_mass;
}


float MovingEntity::GetMaxSpeed() const
{
	return m_maxSpeed;
}


float MovingEntity::GetMaxForce() const
{
	return m_maxForce;
}


float MovingEntity::GetMaxTurnSpeedDeg() const
{
	return m_maxTurnSpeedDeg;
}

float MovingEntity::GetRotationDegrees() const
{
	const Vec2 forward = GetForward();
	return Atan2Degrees(forward.y, forward.x);
}


void MovingEntity::SetVelocity(const Vec2& new_vel)
{
	m_velocity = new_vel;
}

void MovingEntity::SetMaxSpeed(float new_max_speed)
{
	m_maxSpeed = new_max_speed;
}


void MovingEntity::SetMaxForce(float new_max_force)
{
	m_maxForce = new_max_force;
}


void MovingEntity::SetForward(const Vec2& new_forward_normal)
{
	const float new_forward_length_sq = new_forward_normal.GetLengthSquared();
	const float zero_test = new_forward_length_sq - 1.0f;
	ASSERT_OR_DIE(IsZero(zero_test), "Forward vector is not normalized.");

	m_modelMatrix.SetIvec(new_forward_normal);
	m_modelMatrix.SetJvec(new_forward_normal.GetRotated90Degrees());
}


bool MovingEntity::RotateForwardToFaceTarget(const Vec2& target_pos)
{
	const Vec2 pos = GetPosition();
	Vec2 pos_to_target = target_pos - pos;
	pos_to_target.Normalize();

	// A dot B = |A|*|B|*cos(theta)
	const Vec2 forward = GetForward();
	const float dot_product = DotProduct(forward, pos_to_target);
	const float angle_radians = ArcCosRadians(dot_product);

	// If we are facing it, then we don't need to rotate
	if(IsZero(angle_radians))
	{
		return true;
	}

	// otherwise clamp to rotating speed
	float angle_degrees = ConvertRadiansToDegrees(angle_radians);
	if(angle_degrees > m_maxTurnSpeedDeg)
	{
		angle_degrees = m_maxTurnSpeedDeg;
	}

	//using matrix to rotate the forward vector
	const float sign = forward.GetRotationSignFromDir(pos_to_target);
	const float rotate_deg = angle_degrees * sign;
	const Matrix44 rotation_matrix = Matrix44::MakeZRotationDegrees(rotate_deg);
	m_modelMatrix.SetRotationMatrix(rotation_matrix);

	m_velocity = m_modelMatrix.GetTransformVector2D(m_velocity);
	
	return false;
}


void MovingEntity::SetMaxTurnSpeed(float new_turn_speed_deg)
{
	m_maxTurnSpeedDeg = new_turn_speed_deg;
}





