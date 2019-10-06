#include "player_controller.hpp"
#include "game.hpp"

player_controller::player_controller(game_state& game) : game{ game } {

}

void player_controller::register_event_listeners() {
	key_press = game.keyboard().press.listen([this](no::key key) {
		if (key == no::key::space) {
			try_attack();
			try_enter_door();
		}
	});
}

void player_controller::update() {
	const auto& keyboard{ game.keyboard() };
	bool up{ keyboard.is_key_down(no::key::w) };
	bool left{ keyboard.is_key_down(no::key::a) };
	bool down{ keyboard.is_key_down(no::key::s) };
	bool right{ keyboard.is_key_down(no::key::d) };
	game.world.player.move(left, right, up, down);
}

bool player_controller::try_enter_door() {
	auto& player{ game.world.player };
	auto room{ player.room };
	if (!room) {
		return false;
	}
	const auto position{ player.transform.position + player_object::collision::offset };
	if (auto door{ room->find_colliding_door(position, player_object::collision::size) }) {
		player.transform.position = door->to_tile.to<float>() * tile_size_f + door->to_room->index.to<float>() * tile_size_f;
		if (door->to_tile.x == 1) {
			player.transform.position.x += tile_size / 2.0f;
		}
		if (door->to_tile.y == 1) {
			player.transform.position.y += tile_size_f / 2.0f;
		}
		if (door->to_tile.x == room->width() - 2) {
			player.transform.position.x -= tile_size_f * 2.0f;
		}
		if (door->to_tile.y == room->height() - 2) {
			player.transform.position.y -= tile_size_f * 2.0f;
		}
	}
	return true;
}

bool player_controller::try_attack() {
	auto& player{ game.world.player };
	player.attack();
	return true;
}
