#include "monster.hpp"
#include "world.hpp"
#include "game.hpp"
#include "item.hpp"

namespace monster_type {

bool is_melee(int type) {
	return type == skeleton || type == big_fire_slime || type == small_fire_slime || type == big_water_slime
		|| type == small_water_slime || type == knight || type == water_fish || type == fire_imp || type == fire_boss
		|| type == water_boss;
}

bool is_magic(int type) {
	return type == life_wizard || type == dark_wizard || type == toxic_wizard || type == fire_imp || type == water_fish
		|| type == fire_boss || type == water_boss || type == final_boss;
}

no::vector2f sheet_frames(int type) {
	switch (type) {
	case skeleton: return { 4.0f, 7.0f };
	case life_wizard: return { 4.0f, 7.0f };
	case dark_wizard: return { 4.0f, 7.0f };
	case toxic_wizard: return { 4.0f, 7.0f };
	case big_fire_slime: return { 4.0f, 4.0f };
	case small_fire_slime: return { 4.0f, 4.0f };
	case big_water_slime: return { 4.0f, 4.0f };
	case small_water_slime: return { 4.0f, 4.0f };
	case knight: return { 4.0f, 7.0f };
	case water_fish: return { 4.0f, 9.0f };
	case fire_imp: return { 5.0f, 7.0f };
	case fire_boss: return { 5.0f, 4.0f };
	case water_boss: return { 5.0f, 4.0f };
	case final_boss: return { 5.0f, 4.0f };
	default: return { 1.0f };
	}
}

object_stats get_stats(int type) {
	object_stats stats;
	stats.max_health = 1.0f;
	switch (type) {
	case skeleton:
		stats.max_health = 50.0f;
		stats.defense = 5.0f;
		stats.strength = 20.0f; // POST-TWEAK: 17 -> 20
		stats.attack_speed = 8.0f;
		stats.move_speed = 1.0f;
		stats.bonus_strength = 0.0f;
		stats.critical_strike_chance = 0.05f;
		stats.health_regeneration_rate = 0.001f;
		stats.bonus_strength = 3.0f; // POST-TWEAK: Added
		break;

	case life_wizard:
		stats.defense = 2.0f;
		stats.strength = 18.0f; // POST-TWEAK: 14 -> 18
		stats.max_health = 100.0f;
		stats.attack_speed = 4.0f;
		stats.bonus_strength = 3.0f; // POST-TWEAK: Added
		break;

	case dark_wizard:
		stats.defense = 10.0f;
		stats.strength = 18.0f; // POST-TWEAK: 16 - 18
		stats.max_health = 80.0f;
		stats.attack_speed = 6.0f;
		stats.bonus_strength = 5.0f; // POST-TWEAK: Added
		break;

	case toxic_wizard:
		stats.defense = 7.0f;
		stats.strength = 22.0f; // POST-TWEAK: 18 -> 22
		stats.max_health = 60.0f;
		stats.attack_speed = 10.0f;
		stats.bonus_strength = 3.0f; // POST-TWEAK: Added
		break;

	case big_fire_slime:
		stats.defense = 5.0f;
		stats.strength = 15.0f;
		stats.max_health = 35.0f; // POST-TWEAK: 20 -> 35
		stats.attack_speed = 8.0f;
		stats.bonus_strength = 7.0f; // POST-TWEAK: Added
		break;

	case small_fire_slime:
		stats.defense = 2.0f;
		stats.strength = 10.0f; // POST-TWEAK: 13 -> 20
		stats.max_health = 10.0f;
		stats.attack_speed = 10.0f;
		stats.bonus_strength = 3.0f; // POST-TWEAK: Added
		break;

	case big_water_slime:
		stats.defense = 5.0f;
		stats.strength = 15.0f; // POST-TWEAK: 13 -> 15
		stats.max_health = 35.0f; // POST-TWEAK: 20 -> 35
		stats.attack_speed = 10.0f;
		stats.bonus_strength = 7.0f; // POST-TWEAK: Added
		break;

	case small_water_slime:
		stats.defense = 2.0f;
		stats.strength = 15.0f; // POST-TWEAK: 11 -> 15
		stats.max_health = 10.0f;
		stats.attack_speed = 12.0f;
		stats.bonus_strength = 3.0f; // POST-TWEAK: Added
		break;

	case knight:
		stats.defense = 15.0f;
		stats.strength = 30.0f; // POST-TWEAK: 20 -> 30
		stats.max_health = 140.0f;
		stats.attack_speed = 2.0f;
		stats.bonus_strength = 10.0f; // POST-TWEAK: Added
		break;

	case water_fish:
		stats.defense = 12.0f;
		stats.strength = 25.0f; // POST-TWEAK: 20 -> 25
		stats.max_health = 90.0f;
		stats.attack_speed = 4.0f;
		stats.bonus_strength = 5.0f; // POST-TWEAK: Added
		break;

	case fire_imp:
		stats.defense = 11.0f;
		stats.strength = 25.0f; // POST-TWEAK: 21 -> 25
		stats.max_health = 110.0f;
		stats.attack_speed = 6.0f;
		stats.bonus_strength = 5.0f; // POST-TWEAK: Added
		break;

	case fire_boss:
	case water_boss:
	case final_boss:
		stats.defense = 20.0f;
		stats.strength = 25.0f;
		stats.max_health = 300.0f;
		stats.attack_speed = 6.0f; // POST-TWEAK: 9 -> 6
		stats.critical_strike_chance = 0.4; // POST-TWEAK: 0.2 -> 0.4
		stats.bonus_strength = 5.0f;
		break;
	}
	stats.health = stats.max_health;
	stats.mana = stats.max_mana;
	return stats;
}

no::transform2 get_collision_transform(int type) {
	no::transform2 transform;
	switch (type) {

	case skeleton:
		transform.position = { 10.0f, 16.0f };
		transform.scale = { 11.0f, 13.0f };
		break;

	case life_wizard:
	case dark_wizard:
	case toxic_wizard:
		transform.position = { 8.0f, 10.0f };
		transform.scale = { 17.0f, 20.0f };
		break;

	case big_fire_slime:
	case big_water_slime:
		transform.position = { 2.0f, 10.0f };
		transform.scale = { 28.0f, 20.0f };
		break;

	case small_fire_slime:
	case small_water_slime:
		transform.position = { 8.0f, 11.0f };
		transform.scale = { 16.0f, 12.0f };
		break;

	case knight:
		transform.position = { 15.0f, 16.0f };
		transform.scale = { 20.0f, 25.0f };
		break;

	case water_fish:
		transform.position = { 6.0f, 8.0f };
		transform.scale = { 18.0f, 23.0f };
		break;

	case fire_imp:
		transform.position = { 4.0f, 8.0f };
		transform.scale = { 20.0f, 23.0f };
		break;

	case fire_boss:
	case water_boss:
	case final_boss:
		transform.position = { 22.0f, 31.0f };
		transform.scale = { 50.0f, 40.0f };
		break;

	}
	return transform;
}

int animation_frames(int type, int animation) {
	switch (type) {
	case skeleton:
	case life_wizard:
	case dark_wizard:
	case toxic_wizard:
	case knight:
		switch (animation) {
		case animation_type::walk: return 4;
		case animation_type::idle: return 4;
		case animation_type::stab: return 4;
		case animation_type::cast: return 4;
		case animation_type::hit: return 1;
		case animation_type::die: return 3;
		case animation_type::hit_flash: return 1;
		default: return 1;
		}
	case big_fire_slime:
	case small_fire_slime:
	case big_water_slime:
	case small_water_slime:
		switch (animation) {
		case animation_type::walk: return 4;
		case animation_type::idle: return 4;
		case animation_type::stab: return 4;
		case animation_type::hit: return 1;
		case animation_type::die: return 4;
		case animation_type::hit_flash: return 1;
		default: return 1;
		}
	case water_fish:
		switch (animation) {
		case animation_type::walk: return 4;
		case animation_type::idle: return 4;
		case animation_type::stab: return 3;
		case animation_type::cast: return 2;
		case animation_type::hit: return 1;
		case animation_type::die: return 3;
		case animation_type::hit_flash: return 1;
		default: return 1;
		}
	case fire_imp:
		switch (animation) {
		case animation_type::walk: return 4;
		case animation_type::idle: return 4;
		case animation_type::stab: return 4;
		case animation_type::cast: return 2;
		case animation_type::hit: return 1;
		case animation_type::die: return 5;
		case animation_type::hit_flash: return 1;
		default: return 1;
		}
	case fire_boss:
	case water_boss:
		switch (animation) {
		case animation_type::idle: return 4;
		case animation_type::stab: return 4;
		case animation_type::cast: return 4;
		case animation_type::hit: return 1;
		case animation_type::die: return 5;
		case animation_type::hit_flash: return 1;
		default: return 1;
		}
	case final_boss:
		switch (animation) {
		case animation_type::idle: return 4;
		case animation_type::stab: return 4;
		case animation_type::cast: return 4;
		case animation_type::hit: return 1;
		case animation_type::die: return 3;
		case animation_type::hit_flash: return 1;
		default: return 1;
		}
	}
	return 1;
}

no::vector4f get_uv(int type, int animation, int direction) {
	switch (type) {
	case skeleton:
	case knight:
	case life_wizard:
	case dark_wizard:
	case toxic_wizard:
		switch (animation) {
		case animation_type::walk:
			if (direction == 0) {
				return { 0.0f, 1.0f / 7.0f, 1.0f, 1.0f / 7.0f };
			} else {
				return { 0.0f, 4.0f / 7.0f, 1.0f, 1.0f / 7.0f };
			}
		case animation_type::idle:
			if (direction == 0) {
				return { 0.0f, 0.0f / 7.0f, 1.0f, 1.0f / 7.0f };
			} else {
				return { 0.0f, 3.0f / 7.0f, 1.0f, 1.0f / 7.0f };
			}
		case animation_type::stab:
		case animation_type::cast:
			if (direction == 0) {
				return { 0.0f, 2.0f / 7.0f, 1.0f, 1.0f / 7.0f };
			} else {
				return { 0.0f, 5.0f / 7.0f, 1.0f, 1.0f / 7.0f };
			}
		case animation_type::hit:
			return { 0.0f, 6.0f / 7.0f, 1.0f / 4.0f, 1.0f / 7.0f };
		case animation_type::die:
			return { 0.0f, 6.0f / 7.0f, 3.0f / 4.0f, 1.0f / 7.0f };
		case animation_type::hit_flash:
			return { 3.0f / 4.0f, 6.0f / 7.0f, 1.0f / 4.0f, 1.0f / 7.0f };
		}
		break;
	case big_fire_slime:
	case small_fire_slime:
	case big_water_slime:
	case small_water_slime:
		switch (animation) {
		case animation_type::walk:
		case animation_type::idle:
		case animation_type::stab:
			if (direction == 0) {
				return { 0.0f, 0.0f / 4.0f, 1.0f, 1.0f / 4.0f };
			} else {
				return { 0.0f, 1.0f / 4.0f, 1.0f, 1.0f / 4.0f };
			}
		case animation_type::hit:
			return { 0.0f, 2.0f / 4.0f, 1.0f / 4.0f, 1.0f / 4.0f };
		case animation_type::die:
			return { 0.0f, 2.0f / 4.0f, 1.0f, 1.0f / 4.0f };
		case animation_type::hit_flash:
			return { 0.0f, 3.0f / 4.0f, 1.0f / 4.0f, 1.0f / 4.0f };
		}
		break;
	case water_fish:
		switch (animation) {
		case animation_type::walk:
			if (direction == 0) {
				return { 0.0f, 1.0f / 9.0f, 1.0f, 1.0f / 9.0f };
			} else {
				return { 0.0f, 5.0f / 9.0f, 1.0f, 1.0f / 9.0f };
			}
		case animation_type::idle:
			if (direction == 0) {
				return { 0.0f, 0.0f / 9.0f, 1.0f, 1.0f / 9.0f };
			} else {
				return { 0.0f, 4.0f / 9.0f, 1.0f, 1.0f / 9.0f };
			}
		case animation_type::stab:
			if (direction == 0) {
				return { 0.0f, 2.0f / 9.0f, 3.0f / 4.0f, 1.0f / 9.0f };
			} else {
				return { 0.0f, 6.0f / 9.0f, 3.0f / 4.0f, 1.0f / 9.0f };
			}
		case animation_type::cast:
			if (direction == 0) {
				return { 0.0f, 3.0f / 9.0f, 2.0f / 4.0f, 1.0f / 9.0f };
			} else {
				return { 0.0f, 7.0f / 9.0f, 2.0f / 4.0f, 1.0f / 9.0f };
			}
		case animation_type::hit:
			return { 0.0f, 8.0f / 9.0f, 1.0f / 4.0f, 1.0f / 9.0f };
		case animation_type::die:
			return { 0.0f, 8.0f / 9.0f, 3.0f / 4.0f, 1.0f / 9.0f };
		case animation_type::hit_flash:
			return { 3.0f / 4.0f, 8.0f / 9.0f, 1.0f / 4.0f, 1.0f / 9.0f };
		}
		break;
	case fire_imp:
		switch (animation) {
		case animation_type::walk:
		case animation_type::idle:
			if (direction == 0) {
				return { 0.0f, 0.0f / 7.0f, 4.0f / 5.0f, 1.0f / 7.0f };
			} else {
				return { 0.0f, 3.0f / 7.0f, 4.0f / 5.0f, 1.0f / 7.0f };
			}
		case animation_type::stab:
			if (direction == 0) {
				return { 0.0f, 1.0f / 7.0f, 4.0f / 5.0f, 1.0f / 7.0f };
			} else {
				return { 0.0f, 4.0f / 7.0f, 4.0f / 5.0f, 1.0f / 7.0f };
			}
		case animation_type::cast:
			if (direction == 0) {
				return { 0.0f, 2.0f / 7.0f, 2.0f / 5.0f, 1.0f / 7.0f };
			} else {
				return { 0.0f, 5.0f / 7.0f, 2.0f / 5.0f, 1.0f / 7.0f };
			}
		case animation_type::hit:
			return { 0.0f, 6.0f / 7.0f, 1.0f / 5.0f, 1.0f / 7.0f };
		case animation_type::die:
			return { 0.0f, 6.0f / 7.0f, 1.0f, 1.0f / 7.0f };
		case animation_type::hit_flash:
			return { 2.0f / 5.0f, 5.0f / 7.0f, 1.0f / 5.0f, 1.0f / 7.0f };
		}
		break;
	case fire_boss:
	case water_boss:
		switch (animation) {
		case animation_type::idle:
			return { 0.0f, 0.0f / 4.0f, 4.0f / 5.0f, 1.0f / 4.0f };
		case animation_type::stab:
			return { 0.0f, 1.0f / 4.0f, 4.0f / 5.0f, 1.0f / 4.0f };
		case animation_type::cast:
			return { 0.0f, 2.0f / 4.0f, 4.0f / 5.0f, 1.0f / 4.0f };
		case animation_type::hit:
			return { 0.0f, 3.0f / 4.0f, 1.0f / 5.0f, 1.0f / 4.0f };
		case animation_type::die:
			return { 0.0f, 3.0f / 4.0f, 1.0f, 1.0f / 4.0f };
		case animation_type::hit_flash:
			return { 4.0f / 5.0f, 2.0f / 4.0f, 1.0f / 5.0f, 1.0f / 4.0f };
		}
		break;
	case final_boss:
		switch (animation) {
		case animation_type::idle:
			return { 0.0f, 0.0f, 4.0f / 5.0f, 1.0f / 4.0f };
		case animation_type::stab:
		case animation_type::cast:
			return { 0.0f, 1.0f / 4.0f, 4.0f / 5.0f, 1.0f / 4.0f };
		case animation_type::hit:
			return { 0.0f, 3.0f / 4.0f, 1.0f / 5.0f, 1.0f / 4.0f };
		case animation_type::die:
			return { 0.0f, 3.0f / 4.0f, 3.0f / 5.0f, 1.0f / 4.0f };
		case animation_type::hit_flash:
			return { 4.0f / 5.0f, 2.0f / 4.0f, 1.0f / 5.0f, 1.0f / 4.0f };
		}
		break;
	default:
		break;
	}
	return 1.0f;
}

}

monster_object::monster_object(int type) : type{ type } {
	x_direction_change_timer.start();
	y_direction_change_timer.start();
	last_attack.start();
	stats = monster_type::get_stats(type);
	become_angry_timer.start();
	input_left = std::rand() % 10 > 5;
	if (!input_left && std::rand() % 10 > 5) {
		input_right = !input_left;
	}
	input_up = std::rand() % 10 > 5;
	if (!input_up && std::rand() % 10 > 5) {
		input_down = !input_up;
	}
}

void monster_object::update() {
	if (dead) {
		if (!animation.is_done()) {
			animation.update(1.0f / 60.0f);
		} else if (last_animation != animation_type::die) {
			set_die_animation(); // POST-BUGFIX: Delay die animation until hit-flash has shown.
		} else if (animation.is_done() && last_animation == animation_type::die) {
			if (type == monster_type::fire_boss) {
				world->game->ui.on_chest_open(item_type::fire_head, true);
			} else if (type == monster_type::water_boss) {
				world->game->ui.on_chest_open(item_type::water_head, true);
			} else if (type == monster_type::final_boss) {
				world->game->ui.on_chest_open(item_type::staff_of_life, true);
			}
		}
		return;
	}
	if (type == monster_type::fire_boss || type == monster_type::water_boss || type == monster_type::final_boss) {
		facing_down = true;
		facing_right = true; // POST-BUGFIX: Boss animations should not flip. Set to true explicitly.
	}
	if (animation.is_done()) {
		if (last_animation == animation_type::hit_flash) {
			set_hit_animation();
			return;
		} else {
			animation.start_looping();
		}
	}
	if (animation.is_looping()) {
		if (is_moving) {
			set_walk_animation();
		} else {
			set_idle_animation();
		}
	}
	if (type != monster_type::fire_boss && type != monster_type::water_boss && type != monster_type::final_boss) {
		const auto player_collision{ world->player.collision_transform() };
		const auto monster_collision{ collision_transform() };
		distance_to_player = player_collision.position.distance_to(monster_collision.position + monster_collision.scale / 2.0f);
		if (distance_to_player < tile_size_f * 5.0f && distance_to_player > tile_size_f * 0.75f && become_angry_timer.seconds() > 1) {
			if (x_direction_change_timer.milliseconds() > 200) {
				if (world->random.chance(0.05f) || distance_to_player > tile_size_f) {
					input_right = player_collision.position.x > monster_collision.position.x;
					input_left = !input_right;
					x_direction_change_timer.start();
				}
			}
			if (y_direction_change_timer.milliseconds() > 200) {
				if (world->random.chance(0.05f) || distance_to_player > tile_size_f) {
					input_down = player_collision.position.y > monster_collision.position.y;
					input_up = !input_down;
					y_direction_change_timer.start();
				}
			}
		}
	}
	if (monster_type::is_melee(type) && distance_to_player < tile_size_f * 0.5f && become_angry_timer.seconds() > 2) {
		attack();
	} else if (monster_type::is_magic(type) && distance_to_player < tile_size_f * 3.0f && become_angry_timer.seconds() > 2) {
		attack();
	} else if (animation.is_looping() && type != monster_type::fire_boss && type != monster_type::water_boss && type != monster_type::final_boss) {
		move(input_left, input_right, input_up, input_down); // POST-BUGFIX: Final boss should not move.
	}
	animation.update(1.0f / 60.0f);
	const auto combined_stats{ monster_type::get_stats(type) };
	stats.health += combined_stats.health_regeneration_rate;
	stats.health = std::min(stats.health, combined_stats.max_health);
	stats.mana += combined_stats.mana_regeneration_rate;
	stats.mana = std::min(stats.mana, combined_stats.max_mana);
}

void monster_object::move(bool left, bool right, bool up, bool down) {
	is_moving = false;
	direction_changed = false;
	if (is_attacking()) {
		return;
	}
	const bool old_facing_right{ facing_right };
	const bool old_facing_down{ facing_down };
	if (right || left) {
		facing_right = right;
	}
	if (down || up) {
		facing_down = down;
	}
	direction_changed = (facing_right != old_facing_right || facing_down != old_facing_down);
	const float speed{ 1.0f };
	const auto collision{ collision_transform() };
	auto delta{ world->get_allowed_movement_delta(room, left, right, up, down, speed, collision.position, collision.scale) };
	transform.position += delta;
	is_moving = (delta.x != 0.0f || delta.y != 0.0f);
}

void monster_object::attack() {
	if (last_attack.milliseconds() < monster_type::get_stats(type).attack_speed_to_delay_in_ms()) {
		return;
	}
	switch (last_animation) {
	case animation_type::stab:
	case animation_type::cast:
	case animation_type::hit_flash:
		return;
	default:
		break;
	}
	no::vector2f to_player{ collision_transform().position + collision_transform().scale / 2.0f };
	to_player -= world->player.collision_transform().position + world->player.collision_transform().scale / 2.0f;
	last_attack.start();
	no::vector2f attack_size{ 8.0f };
	const auto collision{ collision_transform() };
	no::vector2f attack_origin{ collision.position + collision.scale / 2.0f - attack_size / 2.0f };
	no::vector2f attack_speed{ facing_right ? 3.0f : -3.0f, facing_down ? 3.0f : -3.0f };
	if (monster_type::is_melee(type) && monster_type::is_magic(type)) {
		if (type == monster_type::fire_boss || type == monster_type::water_boss || type == monster_type::final_boss) {
			if (world->random.chance(0.1f)) {
				room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 100);
				set_stab_animation();
			} else {
				room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 1500);
				attack_speed.x = -attack_speed.x;
				room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 1500);
				attack_speed.x = 0.0f;
				room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 1500);
				attack_speed = { to_player.x > 1.0f ? -3.0f : 3.0f, to_player.y > 1.0f ? -3.0f : 3.0f };
				room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 1500);
				set_cast_animation();
			}
		} else {
			if (world->random.chance(0.5f)) {
				room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 100);
				set_stab_animation();
			} else {
				attack_speed = { to_player.x > 1.0f ? -3.0f : 3.0f, to_player.y > 1.0f ? -3.0f : 3.0f };
				room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 1000);
				set_cast_animation();
			}
		}
	} else {
		if (monster_type::is_melee(type)) {
			room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 100);
			set_stab_animation();
		}
		if (monster_type::is_magic(type)) {
			attack_speed = { to_player.x > 1.0f ? -3.0f : 3.0f, to_player.y > 1.0f ? -3.0f : 3.0f };
			room->spawn_attack(false, type, 1, attack_origin, attack_size, attack_speed, 1000);
			set_cast_animation();
		}
	}
}

void monster_object::on_being_hit() {
	set_hit_flash_animation();
}

void monster_object::set_walk_animation() {
	if (last_animation == animation_type::walk && !direction_changed) {
		return;
	}
	last_animation = animation_type::walk;
	animation.frames = monster_type::animation_frames(type, last_animation);
	const auto uv{ monster_type::get_uv(type, last_animation, facing_down ? 1 : 0) };
	animation.set_tex_coords(uv.xy, uv.zw);
	animation.set_frame(0);
	animation.fps = 10.0f;
}

void monster_object::set_idle_animation() {
	if (last_animation == animation_type::idle && !direction_changed) {
		return;
	}
	last_animation = animation_type::idle;
	animation.frames = monster_type::animation_frames(type, last_animation);
	const int direction_index{ facing_down ? 1 : 0 };
	const auto uv{ monster_type::get_uv(type, last_animation, facing_down ? 1 : 0) };
	animation.set_tex_coords(uv.xy, uv.zw);
	animation.set_frame(0);
	animation.fps = 10.0f;
}

void monster_object::set_stab_animation() {
	if (last_animation == animation_type::stab && !direction_changed) {
		return;
	}
	last_animation = animation_type::stab;
	animation.frames = monster_type::animation_frames(type, last_animation);
	const auto uv{ monster_type::get_uv(type, last_animation, facing_down ? 1 : 0) };
	animation.set_tex_coords(uv.xy, uv.zw);
	animation.stop_looping();
	animation.set_frame(0);
	animation.fps = 10.0f;
}

void monster_object::set_cast_animation() {
	if (last_animation == animation_type::cast && !direction_changed) {
		return;
	}
	last_animation = animation_type::cast;
	animation.frames = monster_type::animation_frames(type, last_animation);
	const auto uv{ monster_type::get_uv(type, last_animation, facing_down ? 1 : 0) };
	animation.set_tex_coords(uv.xy, uv.zw);
	animation.stop_looping();
	animation.set_frame(0);
	animation.fps = 10.0f;
}

void monster_object::set_hit_animation() {
	if (last_animation == animation_type::hit && !direction_changed) {
		return;
	}
	last_animation = animation_type::hit;
	animation.frames = monster_type::animation_frames(type, last_animation);
	const auto uv{ monster_type::get_uv(type, last_animation, facing_down ? 1 : 0) };
	animation.set_tex_coords(uv.xy, uv.zw);
	animation.stop_looping();
	animation.set_frame(0);
	animation.fps = 5.0f;
}

void monster_object::set_hit_flash_animation() {
	if (last_animation == animation_type::hit_flash && !direction_changed) {
		return;
	}
	last_animation = animation_type::hit_flash;
	animation.frames = monster_type::animation_frames(type, last_animation);
	const auto uv{ monster_type::get_uv(type, last_animation, facing_down ? 1 : 0) };
	animation.set_tex_coords(uv.xy, uv.zw);
	animation.stop_looping();
	animation.set_frame(0);
	animation.fps = 10.0f;
}

void monster_object::set_die_animation() {
	if (last_animation == animation_type::die && !direction_changed) {
		return;
	}
	last_animation = animation_type::die;
	animation.frames = monster_type::animation_frames(type, last_animation);
	const auto uv{ monster_type::get_uv(type, last_animation, facing_down ? 1 : 0) };
	animation.set_tex_coords(uv.xy, uv.zw);
	animation.stop_looping();
	animation.set_frame(0);
	animation.fps = 5.0f;
}
