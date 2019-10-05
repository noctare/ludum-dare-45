#pragma once

#include "player.hpp"
#include "math.hpp"

constexpr int tile_size{ 32 };

namespace tile_type {
constexpr unsigned char floor{ 0 };
constexpr unsigned char wall{ 1 };
constexpr unsigned char total_types{ 2 };
}

struct game_world_tile {

	unsigned char corner[4]{};

	game_world_tile() = default;
	game_world_tile(unsigned char type);

	bool is_only(unsigned char type) const;
	unsigned int get_uv_index() const;

	unsigned char get_top_left() const;
	unsigned char get_top_right() const;
	unsigned char get_bottom_left() const;
	unsigned char get_bottom_right() const;

	void set_top_left(unsigned char type);
	void set_top_right(unsigned char type);
	void set_bottom_left(unsigned char type);
	void set_bottom_right(unsigned char type);

};

class game_world_room {
public:

	no::vector2i index;
	
	game_world_room() = default;
	game_world_room(const game_world_room&) = delete;
	game_world_room(game_world_room&&) noexcept;

	game_world_room& operator=(const game_world_room&) = delete;
	game_world_room& operator=(game_world_room&&) = delete;

	void resize(int width, int height);

	void update();
	void set_tile(int x, int y, game_world_tile tile);
	game_world_tile tile_at(int x, int y) const;
	game_world_tile& get_tile(int x, int y);

	int left() const;
	int top() const;
	int right() const;
	int bottom() const;

	int width() const;
	int height() const;

	int make_index(int x, int y) const;

	bool is_colliding_with_tile(no::vector2f position) const;

private:

	std::vector<game_world_tile> tiles;
	no::vector2i size;

};

class game_world {
public:

	player_object player;
	std::vector<game_world_room> rooms;

	void update();

};

class game_world_generator {
public:

	game_world_generator(unsigned long long seed);

	void generate(game_world& world);

private:

	void make_tile(game_world_room& room, game_world_tile& tile, int x, int y);
	void make_room(game_world& world);
	void make_border(game_world_room& room, game_world_tile tile) const;
	void place_room_right(game_world& world, game_world_room& room);
	void place_room_bottom(game_world& world, game_world_room& room);
	void place_room_top(game_world& world, game_world_room& room);

	int next_room_direction();
	bool will_room_collide(game_world& world, int left, int top, int width, int height);

	no::random_number_generator random;
	no::vector2i world_size; // Tiles per row and column of the world.
	no::vector2i last_world_size_delta;
	int vertical{ 0 };
	int horizontal_since_vertical_change{ 0 };

};
