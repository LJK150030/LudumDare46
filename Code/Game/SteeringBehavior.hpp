#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"

class Vehicle;

class SteeringBehavior
{
public:
	
	explicit SteeringBehavior(Vehicle* agent);
	~SteeringBehavior();

	Vec2 Calculate(Behavior behavior, const Vec2& target_pos, float scalar_modifier = 1.0f);
	
	Vec2 Seek(const Vec2& target_pos);
	Vec2 Flee(const Vec2& target_pos);

	Vec2 Arrive(const Vec2& target_pos, float scalar_modifier = 1.0f);

private:
	Vehicle*    m_vehicle = nullptr;
	
	Vec2		m_steeringForce;
};
