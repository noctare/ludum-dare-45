#include "world.hpp"
#include "assets.hpp"
#include "surface.hpp"
#include "game.hpp"
#include "item.hpp"

#include <filesystem>

no::transform2 chest_object::collision_transform() const {
	no::transform2 collision;
	collision.position = transform.position + collision::offset;
	collision.scale = collision::size;
	return collision;
}

game_world_tile::game_world_tile(unsigned char type) {
	corner[0] = type;
	corner[1] = type;
	corner[2] = type;
	corner[3] = type;
}

bool game_world_tile::is_only(unsigned char type) const {
	return corner[0] == type && corner[1] == type && corner[2] == type && corner[3] == type;
}

unsigned int game_world_tile::get_uv_index() const {
	return ((static_cast<unsigned int>(corner[0]) & 0xFF) << 24) + ((static_cast<unsigned int>(corner[1]) & 0xFF) << 16)
		+ ((static_cast<unsigned int>(corner[2]) & 0xFF) << 8) + ((static_cast<unsigned int>(corner[3]) & 0xFF));
}

unsigned char game_world_tile::get_top_left() const {
	return corner[0];
}

unsigned char game_world_tile::get_top_right() const {
	return corner[1];
}

unsigned char game_world_tile::get_bottom_left() const {
	return corner[2];
}

unsigned char game_world_tile::get_bottom_right() const {
	return corner[3];
}

void game_world_tile::set_top_left(unsigned char type) {
	corner[0] = type;
}

void game_world_tile::set_top_right(unsigned char type) {
	corner[1] = type;
}

void game_world_tile::set_bottom_left(unsigned char type) {
	corner[2] = type;
}

void game_world_tile::set_bottom_right(unsigned char type) {
	corner[3] = type;
}

game_world_room::game_world_room(game_world_room&& that) noexcept : tiles{ std::move(that.tiles) } {
	std::swap(index, that.index);
	std::swap(size, that.size);
	std::swap(world, that.world);
	std::swap(doors, that.doors);
	std::swap(monsters, that.monsters);
	std::swap(initial_monsters_spawned, that.initial_monsters_spawned);
	std::swap(attacks, that.attacks);
	std::swap(type, that.type);
	std::swap(chests, that.chests);
	std::swap(is_boss_room, that.is_boss_room);
}

void game_world_room::resize(int width, int height) {
	tiles.resize(width * height);
	size = { width, height };
}

int game_world_room::next_monster_type() {
	const int next_type{ world->random.next(0, monster_type::total_types - 1) };
	if (type == 'f') {
		if (is_boss_room && monsters.empty()) {
			return monster_type::fire_boss;
		}
		const float success_rate[monster_type::total_types]{
			0.9f, // skeleton
			0.4f, // life wizard
			0.9f, // dark wizard
			0.8f, // toxic wizard
			1.0f, // big fire slime
			1.0f, // small fire slime
			0.0f, // big water slime
			0.0f, // small water slime
			0.1f, // knight
			0.0f, // water fish
			1.0f, // fire imp
			0.0f, // fire boss
			0.0f, // water boss
			0.0f, // final boss
		};
		return world->random.chance(success_rate[next_type]) ? next_type : monster_type::small_fire_slime;
	} else if (type == 'w') {
		if (is_boss_room && monsters.empty()) {
			return monster_type::water_boss;
		}
		const float success_rate[monster_type::total_types]{
			0.9f, // skeleton
			0.8f, // life wizard
			0.4f, // dark wizard
			0.7f, // toxic wizard
			0.0f, // big fire slime
			0.0f, // small fire slime
			1.0f, // big water slime
			1.0f, // small water slime
			0.1f, // knight
			1.0f, // water fish
			0.0f, // fire imp
			0.0f, // fire boss
			0.0f, // water boss
			0.0f, // final boss
		};
		return world->random.chance(success_rate[next_type]) ? next_type : monster_type::small_water_slime;
	} else if (type == 'l') {
		if (is_boss_room && monsters.empty()) {
			return monster_type::final_boss;
		}
		const float success_rate[monster_type::total_types]{
			1.0f, // skeleton
			1.0f, // life wizard
			0.5f, // dark wizard
			0.5f, // toxic wizard
			0.2f, // big fire slime
			0.2f, // small fire slime
			0.2f, // big water slime
			0.2f, // small water slime
			1.0f, // knight
			0.7f, // water fish
			0.7f, // fire imp
			0.0f, // fire boss
			0.0f, // water boss
			0.0f, // final boss
		};
		return world->random.chance(success_rate[next_type]) ? next_type : monster_type::knight;
	}
	return monster_type::skeleton;
}

void game_world_room::update() {
	for (auto& monster : monsters) {
		monster.update();
	}
	std::sort(monsters.begin(), monsters.end(), [](const monster_object& a, const monster_object& b) {
		return b.transform.position.y > a.transform.position.y;
	});
	process_attacks();
}

void game_world_room::add_monsters() {
	if (!initial_monsters_spawned && !world->is_lobby) {
		int spawn_count{ world->random.next<int>(0, width() / 2) };
		if (is_boss_room) {
			spawn_count = std::max(4, spawn_count); // POST-BUGFIX: Fix boss not spawning because this was 0.
		}
		for (int i{ 0 }; i < spawn_count; i++) {
			if (auto position{ find_empty_position() }) {
				auto& monster{ monsters.emplace_back(next_monster_type()) };
				monster.id = world->next_object_id();
				monster.world = world;
				monster.room = this;
				if (monster.type == monster_type::fire_boss || monster.type == monster_type::water_boss || monster.type == monster_type::final_boss) {
					monster.transform.position = index.to<float>() * tile_size_f;
					monster.transform.position.x += static_cast<float>(width() * tile_size) / 2.0f - 48.0f;
				} else {
					monster.transform.position = position.value();
				}
			}
		}
		if (!is_boss_room && world->random.chance(0.8f)) {
			const int chest_count{ world->random.next<int>(1, 5) };
			for (int i{ 0 }; i < chest_count; i++) {
				if (auto position{ find_empty_position() }) {
					auto& chest{ chests.emplace_back() };
					chest.transform.position = position.value();
					chest.item = world->random.next<int>(0, 36);
					chest.id = world->next_object_id();
					chest.is_crate = world->random.chance(0.4f); // POST-TWEAK: Chests were too rare.
				}
			}
		}
		initial_monsters_spawned = true;
	}
}

void game_world_room::set_tile(int x, int y, game_world_tile tile) {
	tiles[make_index(x, y)] = tile;
}

game_world_tile game_world_room::tile_at(int x, int y) const {
	return tiles[make_index(x, y)];
}

game_world_tile& game_world_room::get_tile(int x, int y) {
	return tiles[make_index(x, y)];
}

int game_world_room::left() const {
	return index.x;
}

int game_world_room::top() const {
	return index.y;
}

int game_world_room::right() const {
	return index.x + width();
}

int game_world_room::bottom() const {
	return index.y + height();
}

int game_world_room::width() const {
	return size.x;
}

int game_world_room::height() const {
	return size.y;
}

int game_world_room::make_index(int x, int y) const {
	return y * width() + x;
}

void game_world_room::spawn_attack(bool by_player, int type, int attack_health, no::vector2f position, no::vector2f size, no::vector2f speed, int max_life_ms) {
	auto& attack{ attacks.emplace_back() };
	attack.type = type;
	attack.max_life_ms = max_life_ms;
	attack.origin = position;
	attack.position = position;
	attack.size = size;
	attack.speed = speed;
	attack.by_player = by_player;
	attack.health = attack_health;
	attack.life_timer.start();
	/*if (attack.by_player) {
		if (item_type::is_staff(attack.type)) {
			world->game->play_sound(world->game->magic_sound);
		} else {
			world->game->play_sound(world->game->stab_sound);
		}
	} else {
		if (monster_type::is_magic(attack.type)) {
			world->game->play_sound(world->game->magic_sound);
		} else {
			world->game->play_sound(world->game->stab_sound);
		}
	}*/
}

void game_world_room::process_attacks() {
	auto& player{ world->player };
	const auto player_stats{ player.final_stats() };
	for (auto& attack : attacks) {
		if (attack.by_player) {
			for (auto& monster : monsters) {
				if (monster.dead) {
					continue;
				}
				if (monster.collision_transform().collides_with(attack.position, attack.size)) {
					// POST-BUGFIX: Don't hit same enemy twice with same attack.
					bool skip_this_one{ false };
					for (const int hit_id : attack.hits) {
						if (hit_id == monster.id) {
							skip_this_one = true;
							break;
						}
					}
					if (skip_this_one) {
						continue;
					}
					//
					monster.on_being_hit();
					attack.hits.push_back(monster.id);
					float damage{ 0.0f };
					damage -= monster.stats.defense;
					damage += player_stats.strength;
					damage += player_stats.bonus_strength;
					if (damage <= 0.0f) {
						damage = player_stats.bonus_strength;
					}
					if (world->random.chance(player_stats.critical_strike_chance)) {
						damage *= 2.0f;
						world->game->ui.add_hit_splat(monster.id);
					}
					monster.stats.health -= damage;
					if (monster.stats.health <= 0.0f) {
#if POST_LD_FEATURE_KILL_COUNT
						world->game->kill_count++;
#endif
						monster.dead = true;
						if (monster.type == monster_type::fire_boss) {
							//player.give_item(item_type::fire_head, 0);
							//world->game->enter_lobby();
							world->is_boss_dead = true; // POST-TWEAK: Don't go to lobby immediately.
							world->boss_item_to_give = item_type::fire_head;
							return;
						} else if (monster.type == monster_type::water_boss) {
							//player.give_item(item_type::water_head, 1);
							//world->game->enter_lobby();
							world->is_boss_dead = true; // POST-TWEAK: Don't go to lobby immediately.
							world->boss_item_to_give = item_type::water_head;
							return;
						} else if (monster.type == monster_type::final_boss) {
							// POST-BUGFIX: Fixes bug where player does not go to lobby after final boss.
							//world->game->enter_lobby();
							world->is_boss_dead = true; // POST-TWEAK: Don't go to lobby immediately.
							world->boss_item_to_give = item_type::staff_of_life;
							return;
						}
					}
					attack.health--;
					if (attack.health <= 0) {
						break;
					}
				}
			}
		} else {
			if (player.collision_transform().collides_with(attack.position, attack.size)) {
				const auto monster_stats{ monster_type::get_stats(attack.type) };
				player.on_being_hit();
				float damage{ 0.0f };
				damage -= player_stats.defense;
				damage += monster_stats.strength;
				damage += monster_stats.bonus_strength;
				if (damage <= 0.0f) {
					damage = monster_stats.bonus_strength;
				}
				if (world->random.chance(monster_stats.critical_strike_chance)) {
					damage *= 2.0f;
					world->game->ui.add_hit_splat(player.id);
				}
				player.stats.health -= damage;
				//if (player.stats.health <= 0.0f) {
					//world->game->enter_lobby();
					//return;
				//}
				attack.health--;
				if (attack.health <= 0) {
					break;
				}
			}
		}
		attack.position += attack.speed;
	}
	for (int i{ 0 }; i < static_cast<int>(attacks.size()); i++) {
		if (attacks[i].health <= 0 || attacks[i].life_timer.milliseconds() >= attacks[i].max_life_ms) {
			attacks.erase(attacks.begin() + i);
			i--;
		}
	}
}

bool game_world::test_tile_mask(const game_world_room& room, no::vector2f position) const {
	no::vector2i chunk_position{ room.index * tile_size };
	no::vector2i tile_index{ position.to<int>() };
	tile_index -= chunk_position;
	tile_index -= tile_index % tile_size;
	tile_index /= tile_size;
	if (tile_index.x < 0 || tile_index.y < 0 || tile_index.x >= room.width() || tile_index.y >= room.height()) {
		return false;
	}
	no::vector2i uv{ autotiler.get_uv(room.tile_at(tile_index.x, tile_index.y)) };
	no::vector2i a_pos{ tile_index * tile_size + chunk_position };
	no::vector2i b_pos{ position.to<int>() };
	int check_x{ std::abs(a_pos.x - b_pos.x) };
	int check_y{ std::abs(a_pos.y - b_pos.y) };
	return collision.is_solid(uv.x, uv.y, check_x, check_y);
}

bool game_world::is_x_empty(game_world_room* room, no::vector2f position, no::vector2f size, float x_direction, float speed) {
	if (!room) {
		room = find_room(position);
		if (!room) {
			return false;
		}
	}
	if (x_direction > 0.0f) {
		x_direction += size.x;
	}
	position.x += x_direction + speed;
	bool c{ x_direction > 0.0f };
	if (test_tile_mask(*room, position)) {
		return false;
	}
	for (auto& monster : room->monsters) {
		if (!monster.dead && monster.collision_transform().collides_with(position)) {
			return false;
		}
	}
	for (auto& chest : room->chests) {
		if (chest.can_collide() && chest.collision_transform().collides_with(position)) {
			return false;
		}
	}
	if (player.collision_transform().collides_with(position)) {
		return true;
	}
	position.y += size.y;
	if (test_tile_mask(*room, position)) {
		return false;
	}
	for (auto& monster : room->monsters) {
		if (!monster.dead && monster.collision_transform().collides_with(position)) {
			return false;
		}
	}
	for (auto& chest : room->chests) {
		if (chest.can_collide() && chest.collision_transform().collides_with(position)) {
			return false;
		}
	}
	if (player.collision_transform().collides_with(position)) {
		return false;
	}
	return true;
}

bool game_world::is_y_empty(game_world_room* room, no::vector2f position, no::vector2f size, float y_direction, float speed) {
	if (!room) {
		room = find_room(position);
		if (!room) {
			return false;
		}
	}
	if (y_direction > 0.0f) {
		y_direction += size.y;
	}
	position.y += y_direction + speed;
	bool c{ y_direction < 0.0f };
	if (test_tile_mask(*room, position)) {
		return false;
	}
	for (auto& monster : room->monsters) {
		if (!monster.dead && monster.collision_transform().collides_with(position)) {
			return false;
		}
	}
	for (auto& chest : room->chests) {
		if (chest.can_collide() && chest.collision_transform().collides_with(position)) {
			return false;
		}
	}
	if (player.collision_transform().collides_with(position)) {
		return false;
	}
	position.x += size.x;
	if (test_tile_mask(*room, position)) {
		return false;
	}
	for (auto& monster : room->monsters) {
		if (!monster.dead && monster.collision_transform().collides_with(position)) {
			return false;
		}
	}
	for (auto& chest : room->chests) {
		if (chest.can_collide() && chest.collision_transform().collides_with(position)) {
			return false;
		}
	}
	if (player.collision_transform().collides_with(position)) {
		return false;
	}
	return true;
}

no::vector2f game_world::get_allowed_movement_delta(game_world_room* room, bool left, bool right, bool up, bool down, float speed, no::vector2f position, no::vector2f size) {
	if (left && right && up && down) {
		return {};
	}
	if (left && right) {
		if (up || down) {
			left = false;
			right = false;
		} else if (!up && !down) {
			return {};
		}
	}
	no::vector2f delta;
	if (left) {
		if (is_x_empty(room, position, size, -1.0f, -speed)) {
			position.x -= speed;
			delta.x -= speed;
		}
		speed /= 2.0f;
	} else if (right) {
		if (is_x_empty(room, position, size, 1.0f, speed)) {
			position.x += speed;
			delta.x += speed;
		}
		speed /= 2.0f;
	}
	if (up) {
		if (is_y_empty(room, position, size, -1.0f, -speed)) {
			position.y -= speed;
			delta.y -= speed;
		}
	}
	if (down) {
		if (is_y_empty(room, position, size, 1.0f, speed)) {
			position.y += speed;
			delta.y += speed;
		}
	}
	return delta;
}

game_world_room* game_world::find_room(no::vector2f position)  {
	for (auto& room : rooms) {
		if (room.is_position_within(position)) {
			return &room;
		}
	}
	return nullptr;
}

game_world_room* game_world::find_left_neighbour_room(game_world_room& room, const std::function<bool(game_world_room&)>& allow) {
	game_world_room* closest_neighbour{ nullptr };
	for (auto& potential_neighbour : rooms) {
		if (&potential_neighbour != &room) {
			if (potential_neighbour.index.x > room.index.x) {
				continue; // is too far right
			}
			if (potential_neighbour.index.y > room.index.y + room.height()) {
				continue; // is too far below
			}
			if (room.index.y > potential_neighbour.index.y + potential_neighbour.height()) {
				continue; // is too far up
			}
			if (!allow(potential_neighbour)) {
				continue;
			}
			if (closest_neighbour) {
				if (potential_neighbour.index.x > closest_neighbour->index.x) {
					closest_neighbour = &potential_neighbour;
				}
			} else {
				closest_neighbour = &potential_neighbour;
			}
		}
	}
	return closest_neighbour;
}

game_world_room* game_world::find_right_neighbour_room(game_world_room& room, const std::function<bool(game_world_room&)>& allow) {
	game_world_room* closest_neighbour{ nullptr };
	for (auto& potential_neighbour : rooms) {
		if (&potential_neighbour != &room) {
			if (room.index.x > potential_neighbour.index.x) {
				continue; // is too far left
			}
			if (potential_neighbour.index.y > room.index.y + room.height()) {
				continue; // is too far below
			}
			if (room.index.y > potential_neighbour.index.y + potential_neighbour.height()) {
				continue; // is too far up
			}
			if (!allow(potential_neighbour)) {
				continue;
			}
			if (closest_neighbour) {
				if (closest_neighbour->index.x > potential_neighbour.index.x) {
					closest_neighbour = &potential_neighbour;
				}
			} else {
				closest_neighbour = &potential_neighbour;
			}
		}
	}
	return closest_neighbour;
}

game_world_room* game_world::find_top_neighbour_room(game_world_room& room, const std::function<bool(game_world_room&)>& allow) {
	game_world_room* closest_neighbour{ nullptr };
	for (auto& potential_neighbour : rooms) {
		if (&potential_neighbour != &room) {
			if (potential_neighbour.index.x > room.index.x) {
				continue; // is too far right
			}
			if (room.index.x > potential_neighbour.index.x) {
				continue; // is too far left
			}
			if (potential_neighbour.index.y > room.index.y + room.height()) {
				continue; // is too far below
			}
			if (!allow(potential_neighbour)) {
				continue;
			}
			if (closest_neighbour) {
				if (potential_neighbour.index.y > closest_neighbour->index.y) {
					closest_neighbour = &potential_neighbour;
				}
			} else {
				closest_neighbour = &potential_neighbour;
			}
		}
	}
	return closest_neighbour;
}

game_world_room* game_world::find_bottom_neighbour_room(game_world_room& room, const std::function<bool(game_world_room&)>& allow) {
	game_world_room* closest_neighbour{ nullptr };
	for (auto& potential_neighbour : rooms) {
		if (&potential_neighbour != &room) {
			if (room.index.x > potential_neighbour.index.x) {
				continue; // is too far left
			}
			if (potential_neighbour.index.x > room.index.x + room.width()) {
				continue; // is too far right
			}
			if (room.index.y > potential_neighbour.index.y + potential_neighbour.height()) {
				continue; // is too far up
			}
			if (!allow(potential_neighbour)) {
				continue;
			}
			if (closest_neighbour) {
				if (closest_neighbour->index.y > potential_neighbour.index.y) {
					closest_neighbour = &potential_neighbour;
				}
			} else {
				closest_neighbour = &potential_neighbour;
			}
		}
	}
	return closest_neighbour;
}

bool game_world_room::is_tile_colliding_with(no::vector2f position) const {
	return world->test_tile_mask(*this, position);
}

bool game_world_room::is_position_within(no::vector2f position) const {
	no::vector2i position_index{ position.to<int>() / tile_size };
	return position_index >= index && index.x + width() > position_index.x && index.y + height() > position_index.y;
}

bool game_world_room::is_connected_to(const game_world_room& room) const {
	for (const auto& door : doors) {
		if (door.to_room == &room) {
			return true;
		}
	}
	return false;
}

game_world_room::door_connection* game_world_room::find_colliding_door(no::vector2f position, no::vector2f size) {
	for (auto& door : doors) {
		no::transform2 transform;
		transform.position = { door.from_tile.to<float>() * tile_size_f + index.to<float>() * tile_size_f };
		transform.scale = tile_size_f;
		if (transform.collides_with(position, size)) {
			return &door;
		}
	}
	return nullptr;
}

std::optional<no::vector2f> game_world_room::find_empty_position() const {
	for (int attempt{ 0 }; attempt < 100; attempt++) {
		const int x{ world->random.next<int>(2, width() - 2) };
		const int y{ world->random.next<int>(2, height() - 2) };
		if (!tile_at(x, y).is_only(tile_type::floor)) {
			continue;
		} else if (!tile_at(x + 1, y).is_only(tile_type::floor)) {
			continue;
		} else if (!tile_at(x, y + 1).is_only(tile_type::floor)) {
			continue;
		} else if (!tile_at(x - 1, y).is_only(tile_type::floor)) {
			continue;
		} else if (!tile_at(x, y - 1).is_only(tile_type::floor)) {
			continue;
		}
		const no::vector2f position{
			static_cast<float>(x * tile_size + index.x * tile_size),
			static_cast<float>(y * tile_size + index.y * tile_size)
		};
		bool next{ false };
		for (const auto& chest : chests) {
			// POST-BUGFIX: Change distance
			if (chest.collision_transform().position.distance_to(position) < chest.collision_transform().scale.x * 1.2f) {
				next = true;
				break;
			}
		}
		if (!next) {
			return position;
		}
	}
	return {};
}

game_object* game_world_room::object_with_id(int id) const {
	if (world->player.id == id) {
		return &world->player;
	}
	for (auto& monster : monsters) {
		if (monster.id == id) {
			return (game_object*)&monster;
		}
	}
	for (auto& chest : chests) {
		if (chest.id == id) {
			return (game_object*)&chest;
		}
	}
	return nullptr;
}

game_world::game_world() {
	const no::surface mask{ no::asset_path("textures/collisions.png") };
	collision.width = mask.width();
	collision.mask.reserve(mask.count());
	for (int y{ 0 }; y < mask.height(); y++) {
		for (int x{ 0 }; x < mask.width(); x++) {
			collision.mask.push_back(mask.at(x, y) != 0xFFFFFFFF);
		}
	}
	player.world = this;
	player.id = next_object_id();
}

void game_world::update() {
	// POST-TWEAK
	if (player.stats.health <= 0.0f) {
		if (!player.animation.is_done()) {
			player.animation.update(1.0f / 60.0f);
		} else if (player.last_animation != animation_type::die) {
			player.set_die_animation();
		}
		if (player.last_animation == animation_type::die) {
			if (player.animation.is_done()) {
				game->enter_lobby();
			} else {
				player.animation.update(1.0f / 60.0f);
			}
			return;
		}
	}
	//
	player.update();
	if (!player.room || game->show_all_rooms) {
		for (auto& room : rooms) {
			room.update();
		}
	} else if (player.room) {
		player.room->update();
	}
}

void game_world::add_monsters() {
	for (auto& room : rooms) {
		room.add_monsters();
	}
}

int game_world::next_object_id() {
	return object_id_counter++;
}
