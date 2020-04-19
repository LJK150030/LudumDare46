#pragma once
#include "Game/MovingEntity.hpp"
#include "Engine/Math/Matrix44.hpp"

class Game;
class SteeringBehavior;
class GPUMesh;
class Material;

class Vehicle : public MovingEntity
{
private:	
	Game*				m_theGame = nullptr;
	SteeringBehavior*	m_steering = nullptr;
	Behavior			m_behavior = CONSTANT_DIR;

	//debugging
	Matrix44	m_modelMatrix = Matrix44::IDENTITY;
	Material*	m_material = nullptr;
	GPUMesh*	m_mesh = nullptr;

	Material*	m_forwardMaterial = nullptr;
	GPUMesh*	m_forwardMesh = nullptr;

	Material*	m_steeringMaterial = nullptr;
	GPUMesh*	m_steeringMesh = nullptr;

public:
	Vehicle(Game* game, const Vec2& pos, float rotation_degrees, const Vec2& velocity, float mass,
		float max_force, float max_speed, float max_turn_speed_deg, float scale);
	
	~Vehicle();
	
	void	Update(double delta_seconds) override;
	void	Render() const override;

	
	void	SeekTarget(const Vec2& target_pos);
	void	FleeTarget(const Vec2& target_pos);
	void	ArriveAt(const Vec2& target_pos, float scalar_modifier = 1.0f);
	void	PursuitOn(const Vehicle* moving_target, float head_on_tolerance_frac = 0.97f, float turn_around_modifier = 0.25f);
	void	EvadeFrom(const Vehicle* moving_target);
	void	WanderAround(float radius, float distance, float jitter);

private:
	void InitVisuals();
	void UpdateModelMatrix();
	
	void InitDebugVisuals();
	void UpdateDebugArrows(const Vec2& steering_force);
	void RenderDebugArrows() const;
};
