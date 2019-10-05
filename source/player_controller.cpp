#include "player_controller.hpp"
#include "game.hpp"

void player_controller::update(game_state& game) {
	const auto& keyboard{ game.keyboard() };
	bool up{ keyboard.is_key_down(no::key::w) };
	bool left{ keyboard.is_key_down(no::key::a) };
	bool down{ keyboard.is_key_down(no::key::s) };
	bool right{ keyboard.is_key_down(no::key::d) };
	if (left && right) {
		if (up || down) {
			left = false;
			right = false;
		} else if (!up && !down) {
			return;
		}
	}
	auto& player{ game.world.player };
	float speed{ 12.0f };
	if (left) {
		player.move_left(speed);
		speed /= 2.0f;
	} else if (right) {
		player.move_right(speed);
		speed /= 2.0f;
	}
	if (up) {
		player.move_up(speed);
	} else if (down) {
		player.move_down(speed);
	}
}
