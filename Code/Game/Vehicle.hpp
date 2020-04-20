#pragma once
#include "Game/MovingEntity.hpp"
#include <bitset>

class Game;
class SteeringBehavior;


class Vehicle : public MovingEntity
{
private:	
	Game*				m_theGame = nullptr;
	SteeringBehavior*	m_steering = nullptr;
	
	//bit field
	std::bitset<NUM_STEER_BEHAVIORS> m_behaviors;
	
	//debugging
	Rgba m_color = Rgba::WHITE;
	Material*	m_forwardMaterial = nullptr;
	GPUMesh*	m_forwardMesh = nullptr;

	Material*	m_steeringMaterial = nullptr;
	GPUMesh*	m_steeringMesh = nullptr;

public:
	Vehicle(Game* game, const Vec2& pos, float rotation_degrees, const Vec2& velocity, float mass,
		float max_force, float max_speed, float max_turn_speed_deg, float scale, Rgba color = Rgba::WHITE);
	
	~Vehicle();

	void Init() override;
	
	void	Update(double delta_seconds) override;
	void	Render() const override;

	//Steering behaviors
	void	SeekTarget(const Vec2& target_pos);
	void	FleeTarget(const Vec2& target_pos);
	void	ArriveAt(const Vec2& target_pos, float scalar_modifier = 1.0f);
	void	PursuitOn(const Vehicle* moving_target, float head_on_tolerance_frac = 0.97f, float turn_around_modifier = 0.25f);
	void	EvadeFrom(const Vehicle* moving_target);
	void	WanderAround(float radius, float distance, float jitter);
	void	AvoidObstacles(float min_look_ahead, float avoidance_mul, float breaking_weight);
	void	AvoidWalls(uint num_whiskers, float whisker_length, float avoidance_mul, float field_of_view_degrees);
	
	//Helppers
	Game*	GetTheGame() const;

private:
	void InitVisuals() override;
	void UpdateModelMatrix(const Vec2& new_pos);
	
	void InitDebugVisuals();
	void UpdateDebugArrows(const Vec2& steering_force);
	void RenderDebugArrows() const;
};
