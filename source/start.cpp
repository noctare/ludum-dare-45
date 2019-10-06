#include "game.hpp"
#include "assets.hpp"

#define DEV_VERSION 1

void configure() {
#if DEV_VERSION
	no::set_asset_directory("../..");
#endif
	no::register_all_textures();
	no::register_font("leo", 32);
	no::register_font("leo", 16);
	no::register_shader("sprite");
}

void start() {
	no::create_state<game_state>("Ludum Dare 45", 800, 600, 0, true);
}
