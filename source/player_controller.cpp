#include "player_controller.hpp"
#include "game.hpp"

void player_controller::update(game_state& game) {
	const auto& keyboard{ game.keyboard() };
	bool up{ keyboard.is_key_down(no::key::w) };
	bool left{ keyboard.is_key_down(no::key::a) };
	bool down{ keyboard.is_key_down(no::key::s) };
	bool right{ keyboard.is_key_down(no::key::d) };
	game.world.player.move(left, right, up, down);
}
