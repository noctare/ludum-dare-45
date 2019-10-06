#include "player.hpp"
#include "world.hpp"

constexpr no::vector4f player_uv_idle[2]{
	{ 0.0f, 0.0f / 8.0f, 1.0f, 1.0f / 8.0f },
	{ 0.0f, 4.0f / 8.0f, 1.0f, 1.0f / 8.0f }
};

constexpr no::vector4f player_uv_walk[2]{
	{ 0.0f, 1.0f / 8.0f, 1.0f, 1.0f / 8.0f },
	{ 0.0f, 5.0f / 8.0f, 1.0f, 1.0f / 8.0f }
};

constexpr no::vector4f player_uv_stab[2]{
	{ 0.0f, 2.0f / 8.0f, 1.0f, 1.0f / 8.0f },
	{ 0.0f, 6.0f / 8.0f, 1.0f, 1.0f / 8.0f }
};

constexpr no::vector4f player_uv_shoot[2]{
	{ 0.0f, 3.0f / 8.0f, 1.0f, 1.0f / 8.0f },
	{ 0.0f, 7.0f / 8.0f, 1.0f, 1.0f / 8.0f }
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
	if (last_animation != 2 && last_attack.milliseconds() > 500) {
		last_attack.start();
		set_stab_animation();
	}
}

void player_object::set_walk_animation() {
	if (last_animation == 0 && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(player_uv_walk[direction_index].xy, player_uv_walk[direction_index].zw);
	animation.set_frame(0);
	last_animation = 0;
}

void player_object::set_idle_animation() {
	if (last_animation == 1 && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(player_uv_idle[direction_index].xy, player_uv_idle[direction_index].zw);
	animation.set_frame(0);
	last_animation = 1;
}

void player_object::set_stab_animation() {
	if (last_animation == 2 && !direction_changed) {
		return;
	}
	animation.frames = 4;
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(player_uv_stab[direction_index].xy, player_uv_stab[direction_index].zw);
	animation.stop_looping();
	animation.set_frame(0);
	last_animation = 2;
}
