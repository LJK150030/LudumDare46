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
};
