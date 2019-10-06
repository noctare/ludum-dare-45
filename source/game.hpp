#pragma once

#include "loop.hpp"
#include "world.hpp"
#include "player_controller.hpp"
#include "renderer.hpp"
#include "generator.hpp"

class game_state;

class game_state : public no::program_state {
public:

	float zoom{ 2.0f };
	//float zoom{ 1.0f };
	//float zoom{ 0.5f };
	bool god_mode{ false };
	bool show_all_rooms{ false };
	bool show_attacks{ false };

	game_world world;

	game_state();
	~game_state() override;

	void update() override;
	void draw() override;

private:
	
	bool limit_fps{ true };

	player_controller controller;
	game_renderer renderer;
	game_world_generator generator;

};
