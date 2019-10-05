#include "game.hpp"
#include "assets.hpp"

#define INSTALLED_VERSION 0

void configure() {
#if INSTALLED_VERSION
	no::set_asset_directory("");
#else
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
