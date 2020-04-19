#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"

class Vehicle;

class SteeringBehavior
{
private:
	Vehicle*    m_vehicle = nullptr;

	//Arriving
	float m_scalarModifier = 1.0f;

	//Pursuing
	float m_headingTowardsTolerance = 0.97;
	float m_turnaroundCoefficient = 0.25f;
	
	//Targeting
	Vec2			m_target = Vec2::ZERO;
	const Vehicle*	m_movingTarget = nullptr;

	//Wandering
	float	m_wanderRadius = 1.0f;
	float	m_wanderDistance = 0.0f;
	float	m_wanderJitter = 0.0f;
	Vec2	m_wanderTarget = Vec2::ZERO;
	
public:
	
	explicit SteeringBehavior(Vehicle* agent);
	~SteeringBehavior();

	Vec2 Calculate(Behavior behavior);
	
	Vec2 Seek(const Vec2& target_pos);
	Vec2 Flee(const Vec2& target_pos);
	Vec2 Arrive(const Vec2& target_pos);
	Vec2 Pursuit(const Vehicle* evader);
	Vec2 Evade(const Vehicle* pursuer);
	Vec2 Wander();

	// Target Setting
	void	SetTarget(const Vec2& target_pos);
	void	SetMovingTarget(const Vehicle* target_pos);

	//Arriving Settings
	void	SetArriveModifier(float scalar_modifier);

	//Pursuing Settings
	void	SetPursuitHeadTowardsTolerance(float fraction);
	void	SetPursuitTurnaround(float coefficient);

	// Random Walk Setting
	void	SetRandomWalk(float radius, float distance, float jitter);
	
private:
	float TurnaroundTime(const Vehicle* agent, const Vec2& target_pos, float coefficient) const;
};
