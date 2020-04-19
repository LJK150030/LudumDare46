#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Vehicle.hpp"
#include "Game/EntityFunctionTemplates.hpp"

#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


Game::Game()
{
	m_inDevMode = false;
	m_time = 0.0f;
}

Game::~Game()
{	
}

void Game::Startup()
{	
	//Setup Camera
	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget(nullptr); // when binding, if nullptr, use the backbuffer
	m_gameCamera->SetOrthoView(Vec2(-WORLD_HEIGHT * WORLD_ASPECT, -WORLD_HEIGHT), Vec2(WORLD_HEIGHT * WORLD_ASPECT, WORLD_HEIGHT));
	m_defaultShader = g_theRenderer->CreateOrGetShader("default_unlit.hlsl");

	//Setup proto entities
	CPUMesh cube_mesh;
	CpuMeshAddQuad(&cube_mesh, AABB2(Vec2(-10.0f, -10.0f), Vec2(10.0f, 10.0f)));
	m_cube = new GPUMesh(g_theRenderer);
	m_cube->CreateFromCPUMesh<Vertex_Lit>(cube_mesh); // we won't be updated this;
	m_woodMaterial = g_theRenderer->CreateOrGetMaterial("wood.mat");
	m_woodMaterial->SetShader("default_lit.hlsl");
	m_woodMaterial->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);

	//Setup Game entities
	m_obstacles = std::vector<BaseEntity*>();
	m_obstacles.push_back(new BaseEntity(
		DEFAULT_ENTITY_TYPE,
		Vec2::ZERO,
		50.0f)
	);
	m_obstacles[0]->Init();
	
	m_vehicles = std::vector<Vehicle*>();
	m_vehicles.push_back(new Vehicle(
		this, 
		Vec2(-100.0f, 0.0f), 
		150.0f,
		Vec2(25.0f,0.0f), 
		1.0f, 
		4.0f, 
		50.0f, 
		1.0f, 
		5.0f));

	m_vehicles[0]->WanderAround(20.0f, 5.0f, 3.6f);
	m_vehicles[0]->AvoidObstacles(30.0f, 2.0f, 1.0f);
	m_vehicles[0]->Init();
	
	const int number_of_vehicles = 10;
	for(int veh_idx = 1; veh_idx < number_of_vehicles; ++veh_idx)
	{
		const float x = g_randomNumberGenerator.GetRandomFloatInRange(
			-WORLD_HEIGHT * WORLD_ASPECT,
			WORLD_HEIGHT * WORLD_ASPECT
		);

		const float y = g_randomNumberGenerator.GetRandomFloatInRange(
			-WORLD_HEIGHT,
			WORLD_HEIGHT
		);

// 		const float radius = g_randomNumberGenerator.GetRandomFloatInRange(
// 			5.0f,
// 			20.0f
// 		);
// 
// 		const float distance = g_randomNumberGenerator.GetRandomFloatInRange(
// 			5.0f,
// 			20.0f
// 		);
// 
// 		const float jitter = g_randomNumberGenerator.GetRandomFloatInRange(
// 			5.0f,
// 			20.0f
// 		);

		m_vehicles.push_back(new Vehicle(
			this,
			Vec2(x, y),
			0.0f,
			Vec2::ZERO,
			1.0f,
			4.0f,
			64.0f,
			1.0f,
			5.0f));

		m_vehicles[veh_idx]->Init();
		m_vehicles[veh_idx]->PursuitOn(m_vehicles[0]);
		//m_vehicles[veh_idx]->WanderAround(radius, distance, jitter);
	}

	

}

void Game::Shutdown()
{
	const int num_vehicles = static_cast<int>(m_vehicles.size());
	for (int vehicles_idx = 0; vehicles_idx < num_vehicles; ++vehicles_idx)
	{
		delete m_vehicles[vehicles_idx];
		m_vehicles[vehicles_idx] = nullptr;
	}

	const int num_obstacles = static_cast<int>(m_obstacles.size());
	for (int obstacle_idx = 0; obstacle_idx < num_obstacles; ++obstacle_idx)
	{
		delete m_obstacles[obstacle_idx];
		m_obstacles[obstacle_idx] = nullptr;
	}

	delete m_cube;
	m_cube = nullptr;
	
	delete m_gameCamera;
	m_gameCamera = nullptr;
}


void Game::Update(const double delta_seconds)
{
	m_time += static_cast<float>(delta_seconds);
	m_currentFrame++;

	const int num_vehicles = static_cast<int>(m_vehicles.size());
	for (int vehicles_idx = 0; vehicles_idx < num_vehicles; ++vehicles_idx)
	{
		m_vehicles[vehicles_idx]->Update(delta_seconds);
	}

	const int num_obstacles = static_cast<int>(m_obstacles.size());
	for (int obstacle_idx = 0; obstacle_idx < num_obstacles; ++obstacle_idx)
	{
		m_obstacles[obstacle_idx]->Update(delta_seconds);
	}
}

void Game::Render() const
{
	ColorTargetView* rtv = g_theRenderer->GetFrameColorTarget();
	m_gameCamera->SetColorTarget(rtv);

	// Move the camera to where it is in the scene
	// (right now, no rotation (looking forward), set 10 back (so looking at 0,0,0)
	const Matrix44 cam_model = Matrix44::ForEulerZXY(m_camEuler, m_camPosition);
	m_gameCamera->SetModelMatrix(cam_model);

	g_theRenderer->BeginCamera(m_gameCamera);
	g_theRenderer->ClearScreen(Rgba::CYAN);
	g_theRenderer->ClearDepthStencilTarget(1.0f);

 	g_theRenderer->BindModelMatrix(m_cubeTransform);
	g_theRenderer->BindMaterial(*m_woodMaterial);
	g_theRenderer->DrawMesh(*m_cube);

	const int num_obstacles = static_cast<int>(m_obstacles.size());
	for (int obstacle_idx = 0; obstacle_idx < num_obstacles; ++obstacle_idx)
	{
		m_obstacles[obstacle_idx]->Render();
	}
	
	const int num_vehicles = static_cast<int>(m_vehicles.size());
	for (int vehicles_idx = 0; vehicles_idx < num_vehicles; ++vehicles_idx)
	{
		m_vehicles[vehicles_idx]->Render();
	}
 
	g_theRenderer->EndCamera(m_gameCamera);
	g_theDebugRenderer->RenderToCamera(m_gameCamera);
}

bool Game::HandleKeyPressed(const unsigned char key_code)
{
	switch (key_code)
	{
	default:
		return false;
	}
}

bool Game::HandleKeyReleased(const unsigned char key_code)
{
	UNUSED(key_code);
	return true;
}

void Game::SetDeveloperMode(const bool on_or_off)
{
	m_inDevMode = on_or_off;
}

void Game::GarbageCollection() const
{
}


void Game::TagObstaclesWithinDisc(BaseEntity* vehicle, const float range)
{
	TagClosestNeighbors(vehicle, m_obstacles, range);
}

const std::vector<BaseEntity*>& Game::GetObstacles() const
{
	return m_obstacles;
}
