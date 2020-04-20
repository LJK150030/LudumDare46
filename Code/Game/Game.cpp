#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Vehicle.hpp"
#include "Game/WallEntity.hpp"
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

	//Setup Game entities
	m_obstacles = std::vector<BaseEntity*>();
	m_obstacles.push_back(new BaseEntity(
		DEFAULT_ENTITY_TYPE,
		Vec2::ZERO,
		32.0f)
	);
	m_obstacles[0]->Init();


	m_worldBounds = std::vector<WallEntity*>();

	//East
	m_worldBounds.push_back(new WallEntity(
		this, 
		2.0f * WORLD_HEIGHT, 
		Vec2(-1.0f, 0.0f),
		-WORLD_HEIGHT * WORLD_ASPECT
	));
	m_worldBounds[0]->Init();

	// North
	m_worldBounds.push_back(new WallEntity(
		this,
		2.0f * WORLD_HEIGHT * WORLD_ASPECT,
		Vec2(0.0f, -1.0f),
		-WORLD_HEIGHT
	));
	m_worldBounds[1]->Init();

	// West
	m_worldBounds.push_back(new WallEntity(
		this,
		2.0f * WORLD_HEIGHT,
		Vec2(1.0f, 0.0f),
		-WORLD_HEIGHT * WORLD_ASPECT
	));
	m_worldBounds[2]->Init();

	// South
	m_worldBounds.push_back(new WallEntity(
		this,
		2.0f * WORLD_HEIGHT * WORLD_ASPECT,
		Vec2(0.0f, 1.0f),
		-WORLD_HEIGHT
	));
	m_worldBounds[3]->Init();

	
	
	m_vehicles = std::vector<Vehicle*>();
	m_vehicles.reserve(MAX_NUM_ENEMIES);
	m_vehicles.push_back(new Vehicle(
		this, 
		Vec2(-100.0f, 0.0f), 
		150.0f,
		Vec2(50.0f,0.0f), 
		1.0f, 
		4.0f, 
		50.0f, 
		1.0f, 
		5.0f));
	++vehicle_head_idx;

	m_vehicles[0]->WanderAround(7.0f, 20.0f, 1.32f);
	m_vehicles[0]->AvoidObstacles(30.0f, 3.0f, 0.25f);
	m_vehicles[0]->AvoidWalls(3, 30.0f, 3.0f, 45.0f);
	m_vehicles[0]->Init();
	
	for(uint veh_idx = 1; veh_idx < MAX_NUM_ENEMIES; ++veh_idx)
	{
		const float x = g_randomNumberGenerator.GetRandomFloatInRange(
			-WORLD_HEIGHT * WORLD_ASPECT,
			WORLD_HEIGHT * WORLD_ASPECT
		);

		const float y = g_randomNumberGenerator.GetRandomFloatInRange(
			-WORLD_HEIGHT,
			WORLD_HEIGHT
		);

		m_vehicles.push_back(new Vehicle(
			this,
			Vec2(x, y),
			0.0f,
			Vec2::ZERO,
			1.0f,
			4.0f,
			75.0f,
			1.0f,
			5.0f,
			Rgba::GRAY));

		m_vehicles[veh_idx]->Init();
		m_vehicles[veh_idx]->TurnOffSteering();
		++vehicle_head_idx;
	}

	

}

void Game::Shutdown()
{
	const uint num_vehicles = static_cast<uint>(m_vehicles.size());
	for (uint vehicles_idx = 0; vehicles_idx < num_vehicles; ++vehicles_idx)
	{
		delete m_vehicles[vehicles_idx];
		m_vehicles[vehicles_idx] = nullptr;
	}


	const uint num_walls = static_cast<uint>(m_worldBounds.size());
	for (uint wall_idx = 0; wall_idx < num_walls; ++wall_idx)
	{
		delete m_worldBounds[wall_idx];
		m_worldBounds[wall_idx] = nullptr;
	}

	
	const uint num_obstacles = static_cast<uint>(m_obstacles.size());
	for (uint obstacle_idx = 0; obstacle_idx < num_obstacles; ++obstacle_idx)
	{
		delete m_obstacles[obstacle_idx];
		m_obstacles[obstacle_idx] = nullptr;
	}

	delete m_gameCamera;
	m_gameCamera = nullptr;
}


void Game::Update(const double delta_seconds)
{
	m_time += static_cast<float>(delta_seconds);
	m_currentFrame++;

	for (uint vehicles_idx = 0; vehicles_idx < num_enemies; ++vehicles_idx)
	{
		m_vehicles[vehicles_idx]->Update(delta_seconds);
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

	const int num_obstacles = static_cast<int>(m_obstacles.size());
	for (int obstacle_idx = 0; obstacle_idx < num_obstacles; ++obstacle_idx)
	{
		m_obstacles[obstacle_idx]->Render();
	}

	const int num_walls = static_cast<int>(m_worldBounds.size());
	for (int wall_idx = 0; wall_idx < num_walls; ++wall_idx)
	{
		m_worldBounds[wall_idx]->Render();
	}
	
	for (uint vehicles_idx = 0; vehicles_idx < num_enemies; ++vehicles_idx)
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
		case NUM_1_KEY: // Reset steering
		{
			for (uint veh_idx = 1; veh_idx < num_enemies; ++veh_idx)
			{
				m_vehicles[veh_idx]->TurnOffSteering();
			}
			return true;
		}
		case NUM_2_KEY: // Seek steering
		{
			for (uint veh_idx = 1; veh_idx < num_enemies; ++veh_idx)
			{
				m_vehicles[veh_idx]->TurnOffSteering();
				m_vehicles[veh_idx]->SeekTarget(Vec2::ZERO);
			}
			return true;
		}
		case NUM_3_KEY: // Flee steering
		{
			for (uint veh_idx = 1; veh_idx < num_enemies; ++veh_idx)
			{
				m_vehicles[veh_idx]->TurnOffSteering();
				m_vehicles[veh_idx]->FleeTarget(Vec2::ZERO);
			}
			return true;
		}
		case NUM_4_KEY:  // Pursuit steering
		{
			for (uint veh_idx = 1; veh_idx < num_enemies; ++veh_idx)
			{
				const float arrive_at = g_randomNumberGenerator.GetRandomFloatInRange(
					0.1f,
					20.0f
				);

				
				m_vehicles[veh_idx]->TurnOffSteering();
				m_vehicles[veh_idx]->ArriveAt(m_vehicles[0]->GetPosition(), arrive_at);
			}
			return true;
		}
		case NUM_5_KEY:  // Pursuit steering
		{
			for (uint veh_idx = 1; veh_idx < num_enemies; ++veh_idx)
			{
			
				m_vehicles[veh_idx]->TurnOffSteering();
				m_vehicles[veh_idx]->PursuitOn(m_vehicles[0]);
			}
			return true;
		}
		case NUM_6_KEY: // Evade steering
		{
			for (uint veh_idx = 1; veh_idx < num_enemies; ++veh_idx)
			{
				m_vehicles[veh_idx]->TurnOffSteering();
				m_vehicles[veh_idx]->EvadeFrom(m_vehicles[0]);
			}
			return true;
		}
		case NUM_7_KEY: // random Walk steering
		{
			for (uint veh_idx = 1; veh_idx < num_enemies; ++veh_idx)
			{
				const float radius = g_randomNumberGenerator.GetRandomFloatInRange(
					1.0f,
					100.0f
				);

				const float distance = g_randomNumberGenerator.GetRandomFloatInRange(
					70.0f,
					100.0f
				);

				const float jitter = g_randomNumberGenerator.GetRandomFloatInRange(
					1.0f,
					50.0f
				);

				m_vehicles[veh_idx]->TurnOffSteering();
				m_vehicles[veh_idx]->WanderAround(radius, distance, jitter);
			}
			return true;
		}
		case Q_KEY:
		{
			num_enemies *= 0.5f;
			if(num_enemies < MIN_NUM_ENEMIES)
			{
				num_enemies = MIN_NUM_ENEMIES;
			}
			return true;
		}
		case W_KEY:
		{
			num_enemies *= 2.0f;
			if (num_enemies > MAX_NUM_ENEMIES)
			{
				num_enemies = MAX_NUM_ENEMIES;
			}
			return true;
		}
			
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


const std::vector<WallEntity*>& Game::GetWalls() const
{
	return m_worldBounds;
}
