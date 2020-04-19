#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"

class Vehicle;

class SteeringBehavior
{
private:
	Vehicle*    m_vehicle = nullptr;
	
public:
	
	explicit SteeringBehavior(Vehicle* agent);
	~SteeringBehavior();

	Vec2 Calculate(Behavior behavior, const Vec2& target_pos, const Vehicle* evader = nullptr, 
		float scalar_modifier = 1.0f);
	
	Vec2 Seek(const Vec2& target_pos);
	Vec2 Flee(const Vec2& target_pos);
	Vec2 Arrive(const Vec2& target_pos, float scalar_modifier = 1.0f);
	Vec2 Pursuit(const Vehicle* evader, float heading_towards_tolerance_fraction = 0.97f, 
		float turnaround_coefficient = 0.25f );

private:
	float TurnaroundTime(const Vehicle* agent, const Vec2& target_pos, float coefficient) const;
};
