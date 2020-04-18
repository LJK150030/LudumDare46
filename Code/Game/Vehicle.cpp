#include "Game/Vehicle.hpp"
#include "Game/SteeringBehavior.hpp"

#include "Engine/Math/MathUtils.hpp"

Vehicle::Vehicle(Game* game, const Vec2& pos, const float rotation_degrees, const Vec2& velocity, 
	const float mass, const float max_force, const float max_speed, const float max_turn_speed_deg,
	const float scale):
		MovingEntity(pos, scale, velocity, max_speed, 
			Vec2(CosDegrees(rotation_degrees),	SinDegrees(rotation_degrees)), 
			mass, Vec2(scale, scale), max_turn_speed_deg, max_force),
	m_theGame(game)
{
	m_steering = new SteeringBehavior(this);
}

Vehicle::~Vehicle()
{
	delete m_steering;
	m_steering = nullptr;
}

void Vehicle::Update(double delta_seconds)
{
	// calculate the combined force
	//Vec2 steering_force = m_steering->Calculate();

	// Acceleration = force/mass
	//Vec2 acceleration = steering_force * m_inverseMass;
	
	// Velocity = v_0 + a*t
	// m_velocity += acceleration * delta_seconds
	m_velocity.ClampLength(m_maxSpeed);
	m_position += m_velocity * delta_seconds;

	// always update the local coordinate unless we are not moving
		// if 0.0f then we divide by zero
		// if VARY small, we might have floating point precision error
	if(m_velocity.GetLengthSquared() > 0.000001f)
	{
		m_forward = m_velocity.GetNormalized();
		m_tangent = m_forward.GetRotated90Degrees();
	}

	// for debugging, if the point goes off screen, wrap it around
	m_position.WrapAround(-1.0f * WORLD_HEIGHT * WORLD_ASPECT,
		-1.0f * WORLD_HEIGHT, WORLD_HEIGHT * WORLD_ASPECT, WORLD_HEIGHT);
	 
}

void Vehicle::Render() const
{
}
