#include "world.hpp"
#include "noise.hpp"

#include <ctime>

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
}

void game_world_room::resize(int width, int height) {
	tiles.resize(width * height);
	size = { width, height };
}

void game_world_room::update() {

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

bool game_world_room::is_colliding_with_tile(no::vector2f position) const {
	no::vector2i local_position{ position.to<int>() };
	local_position -= index * tile_size;
	local_position -= local_position % tile_size;
	local_position /= tile_size;


	return false;
}

void game_world::update() {
	player.update();
	for (auto& room : rooms) {
		room.update();
	}
}

game_world_generator::game_world_generator(unsigned long long seed) : random{ seed } {
	no::set_noise_seed(seed);
}

void game_world_generator::generate(game_world& world) {
	for (int i{ 0 }; i < 100; i++) {
		make_room(world);
	}
}

void game_world_generator::make_tile(game_world_room& room, game_world_tile& tile, int x, int y) {
	no::vector2i index{ room.left() + x, room.top() + y };
	const float noise{ no::octave_noise(3.0f, 0.01f, 0.1f, index.to<float>().x, index.to<float>().y) };
	if (noise > 0.5f) {
		tile = tile_type::wall;
	} else {
		tile = tile_type::floor;
	}
}

void game_world_generator::make_room(game_world& world) {
	int direction{ next_room_direction() };
	auto& room{ world.rooms.emplace_back() };
	if (direction > 0 && horizontal_since_vertical_change > 0) {
		place_room_top(world, room);
		horizontal_since_vertical_change = 0;
	} else if (direction < 0 && horizontal_since_vertical_change > 0) {
		place_room_bottom(world, room);
		horizontal_since_vertical_change = 0;
	} else {
		place_room_right(world, room);
		horizontal_since_vertical_change++;
	}
	for (int x{ 0 }; x < room.width(); x++) {
		for (int y{ 0 }; y < room.height(); y++) {
			make_tile(room, room.get_tile(x, y), x, y);
		}
	}
	make_border(room, { tile_type::wall });
	for (int x{ 0 }; x < room.width(); x++) {
		for (int y{ 0 }; y < room.height(); y++) {
			const auto tile{ room.tile_at(x, y) };
			if (tile.is_only(tile_type::wall)) {
				bool left_neighbour{ x > 0 };
				bool top_neighbour{ y > 0 };
				bool right_neighbour{ x + 1 < room.width() };
				bool bottom_neighbour{ y + 1 < room.height() };
				if (left_neighbour) {
					if (top_neighbour) {
						room.get_tile(x - 1, y - 1).set_bottom_right(tile.get_top_left());
					}
					if (bottom_neighbour) {
						room.get_tile(x - 1, y + 1).set_top_right(tile.get_bottom_left());
					}
					room.get_tile(x - 1, y).set_top_right(tile.get_top_left());
					room.get_tile(x - 1, y).set_bottom_right(tile.get_bottom_left());
				}
				if (top_neighbour) {
					room.get_tile(x, y - 1).set_bottom_left(tile.get_top_left());
					room.get_tile(x, y - 1).set_bottom_right(tile.get_top_right());
				}
				if (right_neighbour) {
					if (top_neighbour) {
						room.get_tile(x + 1, y - 1).set_bottom_left(tile.get_top_right());
					}
					if (bottom_neighbour) {
						room.get_tile(x + 1, y + 1).set_top_left(tile.get_bottom_right());
					}
					room.get_tile(x + 1, y).set_top_left(tile.get_top_right());
					room.get_tile(x + 1, y).set_bottom_left(tile.get_bottom_right());
				}
				if (bottom_neighbour) {
					room.get_tile(x, y + 1).set_top_left(tile.get_bottom_left());
					room.get_tile(x, y + 1).set_top_right(tile.get_bottom_right());
				}
			}
		}
	}
	world_size.x = room.index.x + room.width();
	world_size.y = room.index.y + room.height();
	last_world_size_delta = { room.width(), room.height() };
}

void game_world_generator::make_border(game_world_room& room, game_world_tile tile) const {
	for (int x{ 0 }; x < room.width(); x++) {
		room.set_tile(x, 0, tile);
		room.set_tile(x, room.height() - 1, tile);
	}
	for (int y{ 0 }; y < room.height(); y++) {
		room.set_tile(0, y, tile);
		room.set_tile(room.width() - 1, y, tile);
	}
}

void game_world_generator::place_room_right(game_world& world, game_world_room& room) {
	const int width{ random.next<int>(20, 60) };
	const int height{ random.next<int>(15, 40) };
	int left{ world_size.x + 1 };
	const int top{ world_size.y - last_world_size_delta.y / 2 - height / 2 };
	while (will_room_collide(world, left, top, width, height)) {
		left++;
	}
	room.index = { left, std::max(top, 0) };
	room.resize(width, height);
}

void game_world_generator::place_room_bottom(game_world& world, game_world_room& room) {
	const int width{ random.next<int>(20, 60) };
	const int height{ random.next<int>(40, 70) };
	const int left{ world_size.x - last_world_size_delta.x };
	int top{ world_size.y + 1 };
	while (will_room_collide(world, left, top, width, height)) {
		top++;
	}
	room.index = { left, top };
	room.resize(width, height);
}

void game_world_generator::place_room_top(game_world& world, game_world_room& room) {
	const int width{ random.next<int>(20, 60) };
	const int height{ random.next<int>(40, 70) };
	const int mid_height{ std::max(0, world_size.y / 2 - height / 2) };
	const int left{ world_size.x - last_world_size_delta.x };
	int top{ world_size.y - last_world_size_delta.y - height - 1 };
	while (top > 0 && will_room_collide(world, left, top, width, height)) {
		top--;
	}
	if (left < 0 || top < 0) {
		place_room_bottom(world, room);
		return;
	}
	room.index = { left, top };
	room.resize(width, height);
}

int game_world_generator::next_room_direction() {
	if (vertical == 0 && random.chance(0.6f)) {
		vertical = random.next<int>(-15, 15);
	} else if (vertical > 0) {
		vertical--;
	} else if (vertical < 0) {
		vertical++;
	}
	return vertical;
}

bool game_world_generator::will_room_collide(game_world& world, int left, int top, int width, int height) {
	for (const auto& room : world.rooms) {
		if (room.index.y > top + height) {
			continue;
		} else if (top > room.index.y + room.height()) {
			continue;
		} else if (left + width > room.index.x + room.width()) {
			continue;
		} else if (room.index.x > left + width) {
			continue;
		} else {
			return true;
		}
	}
	return false;
}
