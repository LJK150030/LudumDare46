#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Renderer/RenderContext.hpp"

class Camera;
class Shader;
class GPUMesh;
class Material;
class BaseEntity;
class Vehicle;

class Game
{
public:
	bool m_inDevMode;

private:
	Camera* m_gameCamera = nullptr;
	Shader* m_defaultShader = nullptr;
	Vec3 m_camPosition = Vec3(0.0f, 0.0f, 0.01f);
	Vec3 m_camEuler = Vec3(0.0f, 0.0f, 0.0f);

	float m_time;
	float m_cameraTime = 0.0f;
	int m_currentFrame = 0;

	std::vector<Vehicle*>			m_vehicles;
	std::vector<BaseEntity*>		m_obstacles;
	// box "entity"
	GPUMesh* m_cube = nullptr;
	Matrix44 m_cubeTransform = Matrix44::IDENTITY;
	Material* m_woodMaterial = nullptr;


public:
	Game();
	~Game();

	//boiler plate
	void Startup();
	void Update(double delta_seconds);
	void Render() const;
	void Shutdown();

	//input
	bool HandleKeyPressed(unsigned char key_code);
	bool HandleKeyReleased(unsigned char key_code);

	//helper
	void SetDeveloperMode(bool on_or_off);
	void GarbageCollection() const;
	void TagObstaclesWithinDisc(BaseEntity* vehicle, float range);

private:
	

};