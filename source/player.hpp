#pragma once

#include "transform.hpp"
#include "event.hpp"

class game_world;
class game_world_room;

class player_object {
public:

	struct collision {
		static constexpr no::vector2f offset{ 10.0f, 16.0f };
		static constexpr no::vector2f size{ 11.0f, 13.0f };
	};

	no::transform2 transform;
	bool facing_down{ false };
	bool facing_right{ false };
	bool is_moving{ false };
	game_world* world{ nullptr };
	game_world_room* room{ nullptr };

	void update();
	void move(bool left, bool right, bool up, bool down);

};
