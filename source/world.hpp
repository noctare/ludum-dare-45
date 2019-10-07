#pragma once

#include "player.hpp"
#include "monster.hpp"
#include "autotile.hpp"
#include "math.hpp"

#include <optional>

class game_world;
class game_state;

constexpr int tile_size{ 32 };
constexpr float tile_size_f{ 32.0f };

namespace tile_type {
constexpr unsigned char floor{ 0 };
constexpr unsigned char wall{ 1 };
constexpr unsigned char total_types{ 2 };
}

class game_world_tile {
public:

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

class chest_object : public game_object {
public:

	struct collision {
		static constexpr no::vector2f offset{ 4.0f, 12.0f };
		static constexpr no::vector2f size{ 26.0f, 20.0f };
	};

	int item{ -1 };
	bool open{ false };

	// This is how you know it's final day on ludum dare:
	bool is_crate{ false };

	no::transform2 collision_transform() const override;

	int class_type() const override {
		return 3;
	}

};

class game_world_room {
public:

	struct door_connection {
		no::vector2i from_tile;
		game_world_room* to_room{ nullptr };
		no::vector2i to_tile;
		int flag{ 0 };
	};

	struct active_attack {
		int type{ 0 }; // weapon_type if player, and monster_type if not
		int max_life_ms{ 0 };
		no::timer life_timer;
		no::vector2f origin;
		no::vector2f position;
		no::vector2f size;
		no::vector2f speed;
		bool by_player{ false };
		int health{ 0 };
	};

	game_world* world{ nullptr };
	no::vector2i index;
	std::vector<door_connection> doors;
	std::vector<monster_object> monsters;
	std::vector<active_attack> attacks;
	std::vector<chest_object> chests;
	bool initial_monsters_spawned{ false };
	char type{ 'f' }; // f = fire, w = water, l = light
	bool is_boss_room{ false };

	void add_door(no::vector2i from, game_world_room* room, no::vector2i to) {
		auto& door{ doors.emplace_back() };
		door.from_tile = from;
		door.to_room = room;
		door.to_tile = to;
	}
	
	game_world_room() = default;
	game_world_room(const game_world_room&) = delete;
	game_world_room(game_world_room&&) noexcept;

	game_world_room& operator=(const game_world_room&) = delete;
	game_world_room& operator=(game_world_room&&) = delete;

	void resize(int width, int height);

	int next_monster_type();

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

	void spawn_attack(bool by_player, int type, int attack_health, no::vector2f position, no::vector2f size, no::vector2f speed, int max_life_ms);
	void process_attacks();

	bool is_tile_colliding_with(no::vector2f position) const;
	bool is_position_within(no::vector2f position) const;
	bool is_connected_to(const game_world_room& room) const;
	door_connection* find_colliding_door(no::vector2f position, no::vector2f size);

	std::optional<no::vector2f> find_empty_position() const;

	game_object* object_with_id(int id) const;

private:

	std::vector<game_world_tile> tiles;
	no::vector2i size;

};

class tileset_collision_mask {
public:

	int width{ 0 };
	std::vector<bool> mask;

	bool is_solid(int tile_x, int tile_y, int x, int y) const {
		const int offset{ tile_y * width + tile_x };
		const int index{ offset + y * width + x };
		if (index < 0 || index >= static_cast<int>(mask.size())) {
			return false;
		} else {
			return mask[index];
		}
	}

};

class game_world {
public:

	world_autotiler autotiler;
	player_object player;
	std::vector<game_world_room> rooms;
	game_state* game{ nullptr };
	no::random_number_generator random;
	bool is_lobby{ false };

	game_world();

	void update();

	bool test_tile_mask(const game_world_room& room, no::vector2f position) const;

	bool is_x_empty(game_world_room* room, no::vector2f position, no::vector2f size, float x_direction, float speed);
	bool is_y_empty(game_world_room* room, no::vector2f position, no::vector2f size, float y_direction, float speed);
	no::vector2f get_allowed_movement_delta(game_world_room* room, bool left, bool right, bool up, bool down, float speed, no::vector2f position, no::vector2f size);

	game_world_room* find_room(no::vector2f position);

	game_world_room* find_left_neighbour_room(game_world_room& room, const std::function<bool(game_world_room&)>& allow);
	game_world_room* find_right_neighbour_room(game_world_room& room, const std::function<bool(game_world_room&)>& allow);
	game_world_room* find_top_neighbour_room(game_world_room& room, const std::function<bool(game_world_room&)>& allow);
	game_world_room* find_bottom_neighbour_room(game_world_room& room, const std::function<bool(game_world_room&)>& allow);

	int next_object_id();

private:
	
	tileset_collision_mask collision;
	int object_id_counter{ 0 };

};
