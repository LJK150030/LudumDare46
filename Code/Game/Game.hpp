#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Plane2.hpp"

class Camera;
class Shader;
class GPUMesh;
class Material;
class BaseEntity;
class Vehicle;
class WallEntity;

class Game
{
public:
	bool m_inDevMode;

private:
	//Game objects
	uint num_enemies = 4;
	const uint MIN_NUM_ENEMIES = 1;
	const uint MAX_NUM_ENEMIES = 16'383;
	uint vehicle_head_idx = 0;
	
	std::vector<Vehicle*>		m_vehicles;
	std::vector<BaseEntity*>	m_obstacles;
	std::vector<WallEntity*>	m_worldBounds;
	
	//Camera
	Camera* m_gameCamera = nullptr;
	Shader* m_defaultShader = nullptr;
	Vec3 m_camPosition = Vec3(0.0f, 0.0f, 0.01f);
	Vec3 m_camEuler = Vec3(0.0f, 0.0f, 0.0f);

	//Game meta data
	float m_time;
	float m_cameraTime = 0.0f;
	int m_currentFrame = 0;


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
	const std::vector<BaseEntity*>& GetObstacles() const;
	const std::vector<WallEntity*>& GetWalls() const;
	
private:
	

};