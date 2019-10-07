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
constexpr int hit_flash{ 6 };
}

struct object_stats {
	float mana{ 0.0f };
	float max_mana{ 0.0f };
	float health{ 0.0f };
	float max_health{ 0.0f };
	float defense{ 0.0f };
	float strength{ 0.0f };
	float attack_speed{ 0.0f };
	float move_speed{ 0.0f };
	float bonus_strength{ 0.0f };
	float critical_strike_chance{ 0.0f };
	float health_regeneration_rate{ 0.0f };
	float mana_regeneration_rate{ 0.0f };
	struct {
		float health{ 0.0f };
		float mana{ 0.0f };
	} on_item_use;

	int attack_speed_to_delay_in_ms() const {
		// 1.0 -> 1.9s
		// 2.0 -> 1.8s
		// 3.0 -> 1.7s
		// 4.0 -> 1.6s
		// 5.0 -> 1.5s
		// 6.0 -> 1.4s
		// 7.0 -> 1.3s
		// 8.0 -> 1.2s
		// 9.0 -> 1.1s
		//10.0 -> 1.0s
		//11.0 -> 0.9s
		//12.0 -> 0.8s
		//13.0 -> 0.7s
		//14.0 -> 0.6s
		//15.0 -> 0.5s
		//16.0 -> 0.4s
		const float less_ms{ attack_speed * 100.0f };
		const float max_ms{ 2000.0f };
		const float min_ms{ 400.0f };
		const float ms{ max_ms - less_ms  };
		return static_cast<int>(std::max(std::min(ms, max_ms), min_ms));
	}

};

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
	object_stats stats;
	int id{ -1 };

	game_object() {
		last_attack.start();
	}

	bool is_attacking() const {
		return last_animation == animation_type::stab || last_animation == animation_type::cast;
	}

	virtual int class_type() const = 0;
	virtual no::transform2 collision_transform() const = 0;

};
