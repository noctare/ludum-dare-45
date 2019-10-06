#include "generator.hpp"
#include "noise.hpp"

game_world_generator::game_world_generator(unsigned long long seed) : random{ seed } {
	no::set_noise_seed(seed);
}

void game_world_generator::generate(game_world& world) {
	for (int i{ 0 }; i < 50; i++) {
		make_room(world);
	}
	place_doors(world);
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
	room.world = &world;
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
				const bool left_neighbour{ x > 0 };
				const bool top_neighbour{ y > 0 };
				const bool right_neighbour{ x + 1 < room.width() };
				const bool bottom_neighbour{ y + 1 < room.height() };
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
	const int width{ random.next<int>(10, 20) };
	const int height{ random.next<int>(10, 20) };
	int left{ world_size.x };
	const int top{ world_size.y - last_world_size_delta.y / 2 - height / 2 };
	while (will_room_collide(world, left, top, width, height)) {
		left++;
	}
	room.index = { left, std::max(top, 0) };
	room.resize(width, height);
}

void game_world_generator::place_room_bottom(game_world& world, game_world_room& room) {
	const int width{ random.next<int>(10, 20) };
	const int height{ random.next<int>(10, 20) };
	const int left{ world_size.x - last_world_size_delta.x };
	int top{ world_size.y };
	while (will_room_collide(world, left, top, width, height)) {
		top++;
	}
	room.index = { left, top };
	room.resize(width, height);
}

void game_world_generator::place_room_top(game_world& world, game_world_room& room) {
	const int width{ random.next<int>(10, 20) };
	const int height{ random.next<int>(10, 20) };
	const int mid_height{ std::max(0, world_size.y / 2 - height / 2) };
	const int left{ world_size.x - last_world_size_delta.x };
	int top{ world_size.y - last_world_size_delta.y - height };
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

std::optional<no::vector2i> game_world_generator::try_place_door_left(game_world_room& room) {
	int y{ room.height() / 6 + random.next<int>(room.height() / 2) };
	while (y < room.height() - 2 && !room.get_tile(3, y).is_only(tile_type::floor) && !room.get_tile(4, y).is_only(tile_type::floor)) {
		y++;
	}
	if (y < room.height() - 2) {
		return no::vector2i{ 1, y };
	}
	return {};
}

std::optional<no::vector2i> game_world_generator::try_place_door_right(game_world_room& room) {
	int y{ room.height() / 6 + random.next<int>(room.height() / 2) };
	while (y < room.height() - 2 && !room.get_tile(room.width() - 3, y).is_only(tile_type::floor) && !room.get_tile(room.width() - 4, y).is_only(tile_type::floor)) {
		y++;
	}
	if (y < room.height() - 2) {
		return no::vector2i{ room.width() - 2, y };
	}
	return {};
}

std::optional<no::vector2i> game_world_generator::try_place_door_top(game_world_room& room) {
	int x{ room.width() / 6 + random.next<int>(room.width() / 2) };
	while (x < room.width() - 2 && !room.get_tile(x, 3).is_only(tile_type::floor) && !room.get_tile(x, 4).is_only(tile_type::floor)) {
		x++;
	}
	if (x < room.width() - 2) {
		return no::vector2i{ x, 1 };
	}
	return {};
}

std::optional<no::vector2i> game_world_generator::try_place_door_bottom(game_world_room& room) {
	int x{ room.width() / 6 + random.next<int>(room.width() / 2) };
	while (x < room.width() - 2 && !room.get_tile(x, room.height() - 3).is_only(tile_type::floor) && !room.get_tile(x, room.height() - 4).is_only(tile_type::floor)) {
		x++;
	}
	if (x < room.width() - 2) {
		return no::vector2i{ x, room.height() - 2 };
	}
	return {};
}

void game_world_generator::place_doors(game_world& world) {
	for (auto& room : world.rooms) {
		if (auto left{ world.find_left_neighbour_room(room) }; left && !left->is_connected_to(room)) {
			if (auto from_tile{ try_place_door_left(room) }) {
				if (auto to_tile{ try_place_door_right(*left) }) {
					room.add_door(from_tile.value(), left, to_tile.value());
					left->add_door(to_tile.value(), &room, from_tile.value());
				}
			}
		}
		if (auto right{ world.find_right_neighbour_room(room) }; right && !right->is_connected_to(room)) {
			if (auto from_tile{ try_place_door_right(room) }) {
				if (auto to_tile{ try_place_door_left(*right) }) {
					room.add_door(from_tile.value(), right, to_tile.value());
					right->add_door(to_tile.value(), &room, from_tile.value());
				}
			}
		}
		if (auto top{ world.find_top_neighbour_room(room) }; top && !top->is_connected_to(room)) {
			if (auto from_tile{ try_place_door_top(room) }) {
				if (auto to_tile{ try_place_door_bottom(*top) }) {
					room.add_door(from_tile.value(), top, to_tile.value());
					top->add_door(to_tile.value(), &room, from_tile.value());
				}
			}
		}
		if (auto bottom{ world.find_bottom_neighbour_room(room) }; bottom && !bottom->is_connected_to(room)) {
			if (auto from_tile{ try_place_door_bottom(room) }) {
				if (auto to_tile{ try_place_door_top(*bottom) }) {
					room.add_door(from_tile.value(), bottom, to_tile.value());
					bottom->add_door(to_tile.value(), &room, from_tile.value());
				}
			}
		}
	}
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
		if (room.index.y >= top + height) {
			continue;
		} else if (top >= room.index.y + room.height()) {
			continue;
		} else if (room.index.x >= left + width) {
			continue;
		} else if (left >= room.index.x + room.width()) {
			continue;
		} else {
			return true;
		}
	}
	return false;
}
