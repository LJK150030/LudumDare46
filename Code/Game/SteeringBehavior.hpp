#pragma once
#include "Engine/Math/Vec2.hpp"

class Vehicle;

class SteeringBehavior
{
public:
	explicit SteeringBehavior(Vehicle* agent);
	~SteeringBehavior();

	Vec2 Calculate();
	
	Vec2 Seek(const Vec2& target_pos);

private:
	Vehicle*    m_vehicle = nullptr;
	
	Vec2		m_steeringForce;
};
