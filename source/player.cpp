#include "player.hpp"
#include "world.hpp"
#include "item.hpp"
#include "game.hpp"

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

player_object::player_object() {
	for (int i{ 0 }; i < 8; i++) {
		items[i] = -1;
	}
	stats.max_mana = 100.0f;
	stats.max_health = 50.0f;
	stats.defense = 3.0f;
	stats.strength = 4.0f;
	stats.attack_speed = 8.0f;
	stats.move_speed = 1.0f;
	stats.bonus_strength = 0.0f;
	stats.critical_strike_chance = 0.1f;
	stats.health_regeneration_rate = 0.001f;
	stats.health = stats.max_health;
	stats.mana = stats.max_mana;
	stats.mana_regeneration_rate = 0.005f;
}

void player_object::update() {
	if (!room || !room->is_position_within(transform.position)) {
		room = world->find_room(transform.position);
		world->game->set_background(room->type);
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
	const auto combined_stats{ final_stats() };
	stats.health += combined_stats.health_regeneration_rate;
	stats.health = std::min(stats.health, combined_stats.max_health);
	stats.mana += combined_stats.mana_regeneration_rate;
	stats.mana = std::min(stats.mana, combined_stats.max_mana);
}

void player_object::move(bool left, bool right, bool up, bool down) {
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
	const float speed{ 2.0f };
	const auto position{ transform.position + collision::offset };
	auto delta{ world->get_allowed_movement_delta(room, left, right, up, down, speed, position, collision::size) };
	transform.position += delta;
	is_moving = (delta.x != 0.0f || delta.y != 0.0f);
}

void player_object::attack() {
	if (equipped_weapon() == -1 || last_attack.milliseconds() < final_stats().attack_speed_to_delay_in_ms()) {
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
	last_attack.start();
	no::vector2f attack_size{ 8.0f };
	no::vector2f attack_origin{ transform.position + collision::offset + collision::size / 2.0f - attack_size / 2.0f };
	no::vector2f attack_speed{ facing_right ? 3.0f : -3.0f, facing_down ? 3.0f : -3.0f };
	if (item_type::is_sword(equipped_weapon())) {
		room->spawn_attack(true, equipped_weapon(), 1, attack_origin, attack_size, attack_speed, 100);
		set_stab_animation();
	} else if (item_type::is_staff(equipped_weapon())) {
		room->spawn_attack(true, equipped_weapon(), 1, attack_origin, attack_size, attack_speed, 1000);
		set_cast_animation();
		stats.health += item_type::get_stats(equipped_weapon()).on_item_use.health;
		stats.mana += item_type::get_stats(equipped_weapon()).on_item_use.mana;
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

no::transform2 player_object::collision_transform() const {
	no::transform2 collision;
	collision.position = transform.position + collision::offset;
	collision.scale = collision::size;
	return collision;
}

int player_object::equipped_weapon() const {
	if (weapon == -1) {
		return -1;
	} else {
		return weapons[weapon];
	}
}

void player_object::change_weapon(int scroll) {
	if (weapons.empty()) {
		return;
	}
	weapon += scroll;
	if (weapon < 0) {
		weapon = static_cast<int>(weapons.size()) - 1;
	} else if (weapon >= static_cast<int>(weapons.size())) {
		weapon = 0;
	}
}

void player_object::give_item(int type, int slot) {
	if (item_type::is_weapon(type)) {
		for (const int weapon : weapons) {
			if (weapon == type) {
				return;
			}
		}
		if (weapons.empty()) {
			weapon = 0;
		}
		weapons.push_back(type);
	} else {
		if (slot >= 0 && slot < 8) {
			items[slot] = type;
		}
	}
}

int player_object::active_power() const {
	return power;
}

int player_object::item_in_slot(int slot) const {
	return items[slot];
}

object_stats player_object::final_stats() const {
	auto result{ stats };
	for (const int item : items) {
		const auto item_stats{ item_type::get_stats(item) };
		result.max_mana += item_stats.max_mana;
		result.max_health += item_stats.max_health;
		result.defense += item_stats.defense;
		result.strength += item_stats.strength;
		result.attack_speed += item_stats.attack_speed;
		result.move_speed += item_stats.move_speed;
		result.bonus_strength += item_stats.bonus_strength;
		result.critical_strike_chance += item_stats.critical_strike_chance;
		result.health_regeneration_rate += item_stats.health_regeneration_rate;
		result.mana_regeneration_rate += item_stats.mana_regeneration_rate;
	}
	if (equipped_weapon() >= 0) {
		const auto weapon_stats{ item_type::get_stats(equipped_weapon()) };
		result.max_mana += weapon_stats.max_mana;
		result.max_health += weapon_stats.max_health;
		result.defense += weapon_stats.defense;
		result.strength += weapon_stats.strength;
		result.attack_speed += weapon_stats.attack_speed;
		result.move_speed += weapon_stats.move_speed;
		result.bonus_strength += weapon_stats.bonus_strength;
		result.critical_strike_chance += weapon_stats.critical_strike_chance;
		result.health_regeneration_rate += weapon_stats.health_regeneration_rate;
		result.mana_regeneration_rate += weapon_stats.mana_regeneration_rate;
	}
	return result;
}

void player_object::open_chest() {
	for (auto& chest : room->chests) {
		if (!chest.open && chest.collision_transform().distance_to(collision_transform()) < 20.0f) {
			chest.open = true;
			if (!chest.is_crate) {
				world->game->ui.on_chest_open(chest.item);
			}
		}
	}
}
