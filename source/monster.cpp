#include "monster.hpp"

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

monster_object::monster_object() {
	animation.frames = 4;
}

void monster_object::update() {
	const int direction_index{ facing_down ? 1 : 0 };
	if (is_moving) {
		animation.set_tex_coords(skeleton_uv_walk[direction_index].xy, skeleton_uv_walk[direction_index].zw);
	} else {
		animation.set_tex_coords(skeleton_uv_idle[direction_index].xy, skeleton_uv_idle[direction_index].zw);
	}
	if (is_moving) {
		set_walk_animation();
	} else if (animation.is_looping()) {
		set_idle_animation();
	} else if (animation.is_done()) {
		animation.start_looping();
	}
	animation.update(1.0f / 60.0f);
}

void monster_object::attack() {
	if (animation.is_looping() && last_attack.milliseconds() > 500) {
		last_attack.start();
		set_stab_animation();
	}
}

void monster_object::set_walk_animation() {
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(skeleton_uv_walk[direction_index].xy, skeleton_uv_walk[direction_index].zw);
	if (last_animation != 0) {
		animation.set_frame(0);
	}
	last_animation = 0;
}

void monster_object::set_idle_animation() {
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(skeleton_uv_idle[direction_index].xy, skeleton_uv_idle[direction_index].zw);
	if (last_animation != 1) {
		animation.set_frame(0);
	}
	last_animation = 1;
}

void monster_object::set_stab_animation() {
	const int direction_index{ facing_down ? 1 : 0 };
	animation.set_tex_coords(skeleton_uv_stab[direction_index].xy, skeleton_uv_stab[direction_index].zw);
	animation.stop_looping();
	if (last_animation != 2) {
		animation.set_frame(0);
	}
	last_animation = 2;
}

