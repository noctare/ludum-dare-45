#pragma once

#include "event.hpp"

class game_state;

class player_controller {
public:
	
	player_controller(game_state& game);
	void register_event_listeners();
	void update();
	bool try_enter_door();

private:

	no::event_listener key_press;
	game_state& game;

};
