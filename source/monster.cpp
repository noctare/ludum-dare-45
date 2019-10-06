#include "monster.hpp"
#include "world.hpp"

namespace monster_type {

bool is_melee(int type) {
	return type == skeleton;
}

bool is_magic(int type) {
	return type == dark_magician;
}

}

constexpr no::vector4f skeleton_uv_idle[2]{
	{ 0.0f, 0.0f / 7.0f, 1.0f, 1.0f / 7.0f },
	{ 0.0f, 3.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f skeleton_uv_walk[2]{
	{ 0.0f, 1.0f / 7.0f, 1.0f, 1.0f / 7.0f },
	{ 0.0f, 4.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f skeleton_uv_stab[2]{
	{ 0.0f, 2.0f / 7.0f, 1.0f, 1.0f / 7.0f },
	{ 0.0f, 5.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f skeleton_uv_hit[2]{
	{ 0.0f, 6.0f / 7.0f, 3.0f / 4.0f, 1.0f / 7.0f },
	{ 0.0f, 6.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f skeleton_uv_die[2]{
	{ 0.0f, 6.0f / 7.0f, 1.0f, 1.0f / 7.0f },
	{ 0.0f, 6.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f dark_magician_uv_idle[2]{
	{ 0.0f, 0.0f / 7.0f, 1.0f, 1.0f / 7.0f },
	{ 0.0f, 3.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f dark_magician_uv_walk[2]{
	{ 0.0f, 1.0f / 7.0f, 1.0f, 1.0f / 7.0f },
	{ 0.0f, 4.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f dark_magician_uv_cast[2]{
	{ 0.0f, 2.0f / 7.0f, 1.0f, 1.0f / 7.0f },
	{ 0.0f, 5.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f dark_magician_uv_hit[2]{
	{ 0.0f, 6.0f / 7.0f, 3.0f / 4.0f, 1.0f / 7.0f },
	{ 0.0f, 6.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

constexpr no::vector4f dark_magician_uv_die[2]{
	{ 0.0f, 6.0f / 7.0f, 1.0f, 1.0f / 7.0f },
	{ 0.0f, 6.0f / 7.0f, 1.0f, 1.0f / 7.0f }
};

monster_object::monster_object() {

}

void monster_object::update() {
	if (animation.is_done()) {
		animation.start_looping();
	}
	if (is_moving) {
		set_walk_animation();
	} else if (animation.is_looping()) {
		set_idle_animation();
	}
	const auto player_position{ world->player.transform.position + player_object::collision::offset };
	const auto position{ transform.position + collision::offset };
	if (player_position.distance_to(position + collision::size / 2.0f) < tile_size_f) {
		if (!(world->player.facing_right && facing_right)) {
			if (!(world->player.facing_down && facing_down)) {
				attack();
			}
		}
	}
	animation.update(1.0f / 60.0f);
}

void monster_object::move(bool left, bool right, bool up, bool down) {
	is_moving = false;
	direction_changed = false;
	if (is_attacking()) {
		return;
	}
	const float speed{ 2.0f };
	const auto position{ transform.position + collision::offset };
	auto delta{ world->get_allowed_movement_delta(room, left, right, up, down, speed, position, collision::size) };
	transform.position += delta;
	is_moving = (delta.x != 0.0f || delta.y != 0.0f);
	bool old_facing_right{ facing_right };
	bool old_facing_down{ facing_down };
	if (delta.x != 0.0f) {
		facing_right = (delta.x > 0.0f);
	}
	if (delta.y != 0.0f) {
		facing_down = (delta.y > 0.0f);
	}
	direction_changed = (old_facing_right != facing_right || old_facing_down != facing_down);
}

void monster_object::attack() {
	if (last_attack.milliseconds() < 700) {
		return;
	}
	switch (last_animation) {
	case animation_type::hit:
	case animation_type::stab:
	case animation_type::cast:
		return;
	default:
		break;
	}
	last_attack.start();
	if (monster_type::is_melee(type)) {
		set_stab_animation();
	} else if (monster_type::is_magic(type)) {
		set_cast_animation();
	}
}

void monster_object::on_being_hit() {
	set_hit_animation();
}

void monster_object::set_walk_animation() {
	if (last_animation == animation_type::walk && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(skeleton_uv_walk[direction_index].xy, skeleton_uv_walk[direction_index].zw);
	animation.set_frame(0);
	last_animation = animation_type::walk;
}

void monster_object::set_idle_animation() {
	if (last_animation == animation_type::idle && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(skeleton_uv_idle[direction_index].xy, skeleton_uv_idle[direction_index].zw);
	animation.set_frame(0);
	last_animation = animation_type::idle;
}

void monster_object::set_stab_animation() {
	if (last_animation == animation_type::stab && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(skeleton_uv_stab[direction_index].xy, skeleton_uv_stab[direction_index].zw);
	animation.stop_looping();
	animation.set_frame(0);
	last_animation = animation_type::stab;
}

void monster_object::set_cast_animation() {
	if (last_animation == animation_type::cast && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(dark_magician_uv_cast[direction_index].xy, dark_magician_uv_cast[direction_index].zw);
	animation.stop_looping();
	animation.set_frame(0);
	last_animation = animation_type::cast;
}

void monster_object::set_hit_animation() {
	if (last_animation == animation_type::hit && !direction_changed) {
		return;
	}
	animation.frames = 1;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(skeleton_uv_hit[direction_index].xy, skeleton_uv_hit[direction_index].zw);
	animation.stop_looping();
	animation.set_frame(0);
	last_animation = animation_type::hit;
}
