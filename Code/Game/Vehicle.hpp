#pragma once
#include "Game/MovingEntity.hpp"

class Game;
class SteeringBehavior;

class Vehicle : public MovingEntity
{
private:
	Game*				m_theGame = nullptr;
	SteeringBehavior*	m_steering = nullptr;


public:
	Vehicle(Game* game, const Vec2& pos, float rotation_degrees, const Vec2& velocity, float mass,
		float max_force, float max_speed, float max_turn_speed_deg, float scale);
	
	~Vehicle();
	
	void	Update(double delta_seconds) override;
	void	Render() const override;
};
