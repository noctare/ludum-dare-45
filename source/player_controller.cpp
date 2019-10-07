#include "player_controller.hpp"
#include "game.hpp"

player_controller::player_controller(game_state& game) : game{ game } {

}

void player_controller::register_event_listeners() {
	key_press = game.keyboard().press.listen([this](no::key key) {
		if (game.world.player.locked_by_ui) {
			return;
		}
		if (key == no::key::space) {
			try_open_chest();
			try_attack();
			try_enter_door();
		} else if (key == no::key::q) {
			game.world.player.change_weapon(-1);
		} else if (key == no::key::e) {
			game.world.player.change_weapon(1);
		} else if (key == no::key::num_1) {

		} else if (key == no::key::num_2) {

		}
	});
}

void player_controller::update() {
	if (game.world.player.locked_by_ui) {
		return;
	}
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
		if (door->flag != 0) {
			if (door->flag == 1) {
				game.enter_dungeon('f');
			} else if (door->flag == 2) {
				game.enter_dungeon('l');
			} else if (door->flag == 3) {
				game.enter_dungeon('w');
			}
			return true;
		}
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

bool player_controller::try_open_chest() {
	auto& player{ game.world.player };
	player.open_chest();
	return true;
}
