#pragma once
#include "Game/BaseEntity.hpp"
#include "Game/MovingEntity.hpp"
#include "Game/Vehicle.hpp"

// expecting to only use with Entity objects

template <class T, class S>
void TagClosestNeighbors(T* entity_ptr, S& vec_of_others, const float radius)
{
	for (typename S::iterator s_it = vec_of_others.begin(); s_it != vec_of_others.end(); ++s_it)
	{
		(*s_it)->UnTag();

		const Vec2 direction = (*s_it)->GetPosition() - entity_ptr->GetPosition();

		const float other_radius = (*s_it)->GetBoundingRadius();
		const float range = radius + other_radius;

		//if entity within range, tag for further consideration
		if (((*s_it) != entity_ptr) && (direction.GetLengthSquared() < range*range))
		{
			(*s_it)->Tag();
		}
	}
}
