#pragma once

#include "world.hpp"

#include <optional>

class game_world_generator {
public:

	game_world_generator();

	void generate_dungeon(game_world& world, char type);
	void generate_lobby(game_world& world);

private:

	void make_tile(game_world_room& room, game_world_tile& tile, int x, int y);
	void make_room(game_world& world, char room_type);
	void make_border(game_world_room& room, game_world_tile tile) const;
	void place_room_right(game_world& world, game_world_room& room);
	void place_room_bottom(game_world& world, game_world_room& room);
	void place_room_top(game_world& world, game_world_room& room);

	std::optional<no::vector2i> try_place_door_left(game_world_room& room);
	std::optional<no::vector2i> try_place_door_right(game_world_room& room);
	std::optional<no::vector2i> try_place_door_top(game_world_room& room);
	std::optional<no::vector2i> try_place_door_bottom(game_world_room& room);

	void place_doors(game_world& world);

	int next_room_direction();
	bool will_room_collide(game_world& world, int left, int top, int width, int height);

	no::random_number_generator random;
	no::vector2i world_size; // Tiles per row and column of the world.
	no::vector2i last_world_size_delta;
	int vertical{ 0 };
	int horizontal_since_vertical_change{ 0 };

	std::vector<char> last_room_door_directions;

	int next_room_width();
	int next_room_height();

	bool generating_lobby{ false };
	bool generating_boss_room{ false };

};
