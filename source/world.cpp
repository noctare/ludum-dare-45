#include "world.hpp"
#include "assets.hpp"
#include "surface.hpp"
#include "game.hpp"

#include <filesystem>

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
}

void game_world_room::resize(int width, int height) {
	tiles.resize(width * height);
	size = { width, height };
}

void game_world_room::update() {
	if (!initial_monsters_spawned) {
		const int spawn_count{ world->random.next<int>(0, 5) };
		for (int i{ 0 }; i < spawn_count; i++) {
			if (auto position{ find_empty_position() }) {
				auto& monster{ monsters.emplace_back() };
				monster.world = world;
				monster.room = this;
				monster.transform.position = position.value();
			}
		}
		initial_monsters_spawned = true;
	}
	for (auto& monster : monsters) {
		monster.update();
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
	position.y += size.y;
	if (test_tile_mask(*room, position)) {
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
	position.x += size.x;
	if (test_tile_mask(*room, position)) {
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
	return position_index >= index && index.x + width() > position_index.x&& index.y + height() > position_index.y;
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
		return no::vector2f{
			static_cast<float>(x * tile_size + index.x * tile_size),
			static_cast<float>(y * tile_size + index.y * tile_size)
		};
	}
	return {};
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
}

void game_world::update() {
	player.update();
	if (!player.room || game->show_all_rooms) {
		for (auto& room : rooms) {
			room.update();
		}
	} else if (player.room) {
		player.room->update();
	}
}
