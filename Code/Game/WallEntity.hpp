#pragma once
#include "Game/BaseEntity.hpp"
#include "Engine/Math/Plane2.hpp"


class Game;

class WallEntity : public BaseEntity
{
private:
	Game*	m_theGame = nullptr;
	Plane2	m_plane;
	float	m_wallHalfLength;

public:
	explicit WallEntity(Game* game, float wall_length, const Vec2& forward, float signed_distance);
	~WallEntity();

	//Initializers
	void Init() override;
	void InitVisuals() override;

	// Common
	void Update(double delta_seconds) override;
	void Render() const override;

	Plane2 GetPlane() const;
	float GetPlanHalfLength() const;
};