#pragma once

#include "object.hpp"
#include "draw.hpp"

namespace monster_type {

constexpr int skeleton{ 0 };
constexpr int life_wizard{ 1 };
constexpr int dark_wizard{ 2 };
constexpr int toxic_wizard{ 3 };
constexpr int big_fire_slime{ 4 };
constexpr int small_fire_slime{ 5 };
constexpr int big_water_slime{ 6 };
constexpr int small_water_slime{ 7 };
constexpr int knight{ 8 };
constexpr int water_fish{ 9 };
constexpr int fire_imp{ 10 };
constexpr int fire_boss{ 11 };
constexpr int water_boss{ 12 };
constexpr int final_boss{ 13 };
constexpr int total_types{ 14 };

bool is_melee(int type);
bool is_magic(int type);
no::vector2f sheet_frames(int type);
object_stats get_stats(int type);
no::transform2 get_collision_transform(int type);

}

class monster_object : public game_object {
public:

	int type{ 0 };
	float distance_to_player{ 0.0f };
	bool dead{ false };

	monster_object(int type);

	void update();
	void attack();
	void on_being_hit();

	no::transform2 collision_transform() const override {
		no::transform2 collision{ monster_type::get_collision_transform(type) };
		collision.position += transform.position;
		return collision;
	}

	void set_die_animation();

	int class_type() const override {
		return 2;
	}

private:

	void monster_object::move(bool left, bool right, bool up, bool down);

	void set_walk_animation();
	void set_idle_animation();
	void set_stab_animation();
	void set_cast_animation();
	void set_hit_animation();
	void set_hit_flash_animation();

	bool input_left{ false };
	bool input_right{ false };
	bool input_up{ false };
	bool input_down{ false };
	no::timer x_direction_change_timer;
	no::timer y_direction_change_timer;
	no::timer become_angry_timer;

};
