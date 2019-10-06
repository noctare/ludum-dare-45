#pragma once

#include "object.hpp"
#include "draw.hpp"

namespace monster_type {

constexpr int skeleton{ 0 };
constexpr int dark_magician{ 1 };
constexpr int slime{ 2 };

bool is_melee(int type);
bool is_magic(int type);

}

class monster_object : public game_object {
public:

	struct collision {
		static constexpr no::vector2f offset{ 10.0f, 16.0f };
		static constexpr no::vector2f size{ 11.0f, 13.0f };
	};

	int type{ 0 };

	monster_object();

	void update();
	void attack();
	void on_being_hit();

	no::transform2 collision_transform() const {
		no::transform2 collision;
		collision.position = transform.position + collision::offset;
		collision.scale = collision::size;
		return collision;
	}

private:

	void monster_object::move(bool left, bool right, bool up, bool down);

	void set_walk_animation();
	void set_idle_animation();
	void set_stab_animation();
	void set_cast_animation();
	void set_hit_animation();

};
