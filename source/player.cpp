#include "player.hpp"
#include "world.hpp"

namespace weapon_type {

bool is_sword(int type) {
	return type == normal_sword || type == fire_sword || type == water_sword;
}

bool is_staff(int type) {
	return type == normal_staff || type == fire_staff || type == water_staff;
}

}

constexpr no::vector4f player_uv_idle[2]{
	{ 0.0f, 0.0f / player_animation_rows, 1.0f, 1.0f / player_animation_rows },
	{ 0.0f, 4.0f / player_animation_rows, 1.0f, 1.0f / player_animation_rows }
};

constexpr no::vector4f player_uv_walk[2]{
	{ 0.0f, 1.0f / player_animation_rows, 1.0f, 1.0f / player_animation_rows },
	{ 0.0f, 5.0f / player_animation_rows, 1.0f, 1.0f / player_animation_rows }
};

constexpr no::vector4f player_uv_stab[2]{
	{ 0.0f, 2.0f / player_animation_rows, 1.0f, 1.0f / player_animation_rows },
	{ 0.0f, 6.0f / player_animation_rows, 1.0f, 1.0f / player_animation_rows }
};

constexpr no::vector4f player_uv_cast[2]{
	{ 0.0f, 3.0f / player_animation_rows, 1.0f, 1.0f / player_animation_rows },
	{ 0.0f, 7.0f / player_animation_rows, 1.0f, 1.0f / player_animation_rows }
};

void player_object::update() {
	if (!room || !room->is_position_within(transform.position)) {
		room = world->find_room(transform.position);
	}
	if (animation.is_done()) {
		animation.start_looping();
	}
	if (is_moving) {
		set_walk_animation();
	} else if (animation.is_looping()) {
		set_idle_animation();
	}
	animation.update(1.0f / 60.0f);
}

void player_object::move(bool left, bool right, bool up, bool down) {
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

void player_object::attack() {
	if (weapon == weapon_type::none || last_attack.milliseconds() < 500) {
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
	no::vector2f attack_size{ 4.0f };
	no::vector2f attack_origin{ transform.position + collision::offset + collision::size / 2.0f - attack_size / 2.0f };
	no::vector2f attack_speed{ facing_right ? 3.0f : -3.0f, facing_down ? 3.0f : -3.0f };
	if (weapon_type::is_sword(weapon)) {
		room->spawn_attack(true, weapon, 1, attack_origin, attack_size, attack_speed, 100);
		set_stab_animation();
	} else if (weapon_type::is_staff(weapon)) {
		room->spawn_attack(true, weapon, 1, attack_origin, attack_size, attack_speed, 1000);
		set_cast_animation();
	}
}

void player_object::on_being_hit() {
	//set_hit_animation();
}

void player_object::set_walk_animation() {
	if (last_animation == animation_type::walk && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(player_uv_walk[direction_index].xy, player_uv_walk[direction_index].zw);
	animation.set_frame(0);
	last_animation = animation_type::walk;
}

void player_object::set_idle_animation() {
	if (last_animation == animation_type::idle && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(player_uv_idle[direction_index].xy, player_uv_idle[direction_index].zw);
	animation.set_frame(0);
	last_animation = animation_type::idle;
}

void player_object::set_stab_animation() {
	if (last_animation == animation_type::stab && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(player_uv_stab[direction_index].xy, player_uv_stab[direction_index].zw);
	animation.stop_looping();
	animation.set_frame(0);
	last_animation = animation_type::stab;
}

void player_object::set_cast_animation() {
	if (last_animation == animation_type::cast && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(player_uv_cast[direction_index].xy, player_uv_cast[direction_index].zw);
	animation.stop_looping();
	animation.set_frame(0);
	last_animation = animation_type::cast;
}
