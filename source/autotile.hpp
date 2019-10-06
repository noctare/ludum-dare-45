#pragma once

#include "math.hpp"

#include <unordered_map>

class game_world_tile;

class world_autotiler {
public:

	std::unordered_map<unsigned int, no::vector2i> uv;

	world_autotiler();

	no::vector2i get_uv(const game_world_tile& tile) const;
	void load_main_tiles();
	void load_tile(unsigned char top_left, unsigned char top_right, unsigned char bottom_left, unsigned char bottom_right, int x, int y);
	void load_group(int primary, int sub, int x, int y);

};
