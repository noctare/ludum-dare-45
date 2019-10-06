#pragma once

#include "draw.hpp"
#include "timer.hpp"

class game_world;
class game_world_room;

namespace animation_type {
constexpr int walk{ 0 };
constexpr int idle{ 1 };
constexpr int stab{ 2 };
constexpr int cast{ 3 };
constexpr int hit{ 4 };
constexpr int die{ 5 };
}

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
		return last_animation == 2 || last_animation == 3;
	}

};
