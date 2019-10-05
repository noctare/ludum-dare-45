#include "world.hpp"
#include "noise.hpp"

#include <ctime>

game_world_room::game_world_room(game_world_room&& that) noexcept : tiles{ std::move(that.tiles) } {
	std::swap(index, that.index);
	std::swap(size, that.size);
}

void game_world_room::resize(int width, int height) {
	tiles.resize(width * height, tile_type::floor);
	size = { width, height };
}

void game_world_room::update() {

}

void game_world_room::set_tile(int x, int y, tile_type tile) {
	tiles[make_index(x, y)] = tile;
}

tile_type game_world_room::tile_at(int x, int y) const {
	return tiles[make_index(x, y)];
}

int game_world_room::make_index(int x, int y) const {
	return y * width() + x;
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
	for (int i = 0; i < 50; i++) {
		make_room(world);
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
	for (int x{ room.left() }; x < room.right(); x++) {
		for (int y{ room.top() }; y < room.bottom(); y++) {
			const float noise{ no::octave_noise(6.0f, 0.1f, 0.1f, static_cast<float>(x), static_cast<float>(y)) };
			const float wall_chance{ random.chance(0.5f) ? 0.5f : 0.2f };
			if (noise > wall_chance) {
				room.set_tile(x - room.index.x, y - room.index.y, tile_type::wall);
			} else {
				room.set_tile(x - room.index.x, y - room.index.y, tile_type::floor);
			}
		}
	}
	make_border(room, tile_type::wall);
	world_size.x = room.index.x + room.width();
	world_size.y = room.index.y + room.height();
	last_world_size_delta = { room.width(), room.height() };
}

void game_world_generator::make_border(game_world_room& room, tile_type tile) const {
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
		// If new room is above, it's not colliding:
		if (room.index.y > top + height) {
			continue;
		}
		// If new room is below, it's not colliding:
		if (top > room.index.y + room.height()) {
			continue;
		}
		// If new room is to the right, it's not colliding:
		if (left + width > room.index.x + room.width()) {
			continue;
		}
		// If new room is to the left, it's not colliding:
		if (room.index.x > left + width) {
			continue;
		}
		return true;
	}
	return false;
}
