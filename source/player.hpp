#pragma once

#include "object.hpp"

constexpr float player_animation_rows{ 8.0f };

namespace weapon_type {
constexpr int none{ 0 };
constexpr int normal_sword{ 1 };
constexpr int normal_staff{ 2 };
constexpr int fire_sword{ 3 };
constexpr int fire_staff{ 4 };
constexpr int water_sword{ 5 };
constexpr int water_staff{ 6 };

bool is_sword(int type);
bool is_staff(int type);

}

class player_object : public game_object {
public:

	struct collision {
		static constexpr no::vector2f offset{ 10.0f, 16.0f };
		static constexpr no::vector2f size{ 11.0f, 13.0f };
	};

	int weapon{ weapon_type::none };

	void update();
	void move(bool left, bool right, bool up, bool down);
	void attack();
	void on_being_hit();

	no::transform2 collision_transform() const {
		no::transform2 collision;
		collision.position = transform.position + collision::offset;
		collision.scale = collision::size;
		return collision;
	}

private:

	void set_walk_animation();
	void set_idle_animation();
	void set_stab_animation();
	void set_cast_animation();
	//void set_hit_animation();

};
