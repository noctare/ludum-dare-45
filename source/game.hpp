#pragma once

#include "loop.hpp"
#include "world.hpp"
#include "player_controller.hpp"
#include "renderer.hpp"
#include "generator.hpp"
#include "game_ui.hpp"

class game_state;

#define POST_LD_FEATURE_KILL_COUNT 1

class game_state : public no::program_state {
public:

#if POST_LD_FEATURE_KILL_COUNT
	// POST-FEATURE: Kill count. On the edge of being too much of a post-submission edit.
	// However, it doesn't affect gameplay itself, and makes it  more enjoyable to play.
	int kill_count{ 0 };
	int monster_count{ 0 };
	no::text_view kill_count_text;
	bool in_lobby{ false };
	//
#endif

	// POST-TWEAK: Show instructions!
	no::text_view instructions;
	//

	// POST-TWEAK: Let players toggle audio, instead of having to delete file.
	bool play_audio{ true };
	//

	bool show_intro{ true };
	int cover_texture{ -1 };

	void start_playing();

	float zoom{ 3.0f };
	//float zoom{ 1.0f };
	//float zoom{ 0.5f };
	bool god_mode{ false };
	bool show_all_rooms{ false };
	bool show_collisions{ false };
	no::text_view intro_text;
	no::event_listener intro_listen_key;
	float random_intro_dist_1{ 0.0f };
	float random_intro_dist_2{ 0.0f };
	no::timer random_intro_dist_timer;

	game_world world;
	game_ui ui;
	game_renderer renderer;

	game_state();
	~game_state() override;

	void update() override;
	void draw() override;

	void set_background(char type);

	void enter_lobby();
	void enter_dungeon(char type);

	void play_sound(no::audio_source* sound);

	no::audio_source* bg_music{ nullptr };
	std::vector<no::audio_player*> audio_players;

private:
	
	bool limit_fps{ true };

	player_controller controller;
	game_world_generator generator;
	no::timer bg_loop;

};
