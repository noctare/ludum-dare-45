#include "autotile.hpp"
#include "world.hpp"

world_autotiler::world_autotiler() {
	load_main_tiles();
	load_group(tile_type::wall, tile_type::floor, 0, 1);
}

no::vector2i world_autotiler::get_uv(const game_world_tile& tile) const {
	if (const auto it{ uv.find(tile.get_uv_index()) }; it != uv.end()) {
		return it->second * tile_size;
	} else {
		return 0;
	}
}

void world_autotiler::load_main_tiles() {
	for (unsigned char i{ 0 }; i < tile_type::total_types; i++) {
		const game_world_tile tile{ i };
		uv[tile.get_uv_index()] = { i, 0 };
	}
}

void world_autotiler::load_tile(unsigned char top_left, unsigned char top_right, unsigned char bottom_left, unsigned char bottom_right, int x, int y) {
	game_world_tile tile;
	tile.corner[0] = top_left;
	tile.corner[1] = top_right;
	tile.corner[2] = bottom_left;
	tile.corner[3] = bottom_right;
	uv[tile.get_uv_index()] = { x, y };
}

void world_autotiler::load_group(int primary, int sub, int x, int y) {
	load_tile(primary, primary, primary, sub, x, y);
	load_tile(primary, primary, sub, sub, x + 1, y);
	load_tile(primary, primary, sub, primary, x + 2, y);
	load_tile(primary, sub, primary, primary, x, y + 1);
	load_tile(sub, sub, primary, primary, x + 1, y + 1);
	load_tile(sub, primary, primary, primary, x + 2, y + 1);
	y += 2;
	load_tile(sub, sub, sub, primary, x, y);
	load_tile(sub, sub, primary, primary, x + 1, y);
	load_tile(sub, sub, primary, sub, x + 2, y);
	load_tile(sub, primary, sub, sub, x, y + 1);
	load_tile(primary, primary, sub, sub, x + 1, y + 1);
	load_tile(primary, sub, sub, sub, x + 2, y + 1);
	y += 2;
	load_tile(sub, primary, primary, sub, x, y);
	load_tile(primary, sub, primary, sub, x + 2, y);
	load_tile(primary, sub, sub, primary, x, y + 1);
	load_tile(sub, primary, sub, primary, x + 2, y + 1);
}
