#pragma once

#include "draw.hpp"
#include "timer.hpp"

class game_world;
class game_world_room;

class game_object {
public:

	no::transform2 transform;
	game_world* world{ nullptr };
	game_world_room* room{ nullptr };
	bool facing_down{ false };
	bool facing_right{ false };
	bool direction_changed{ false };
	bool is_moving{ false };
	no::timer last_attack;
	no::sprite_animation animation;
	int last_animation{ -1 };

	game_object() {
		last_attack.start();
	}

	bool is_attacking() const {
		return last_animation == 2;
	}

};
