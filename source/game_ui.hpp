#pragma once

#include "draw.hpp"
#include "camera.hpp"
#include "font.hpp"
#include "ui.hpp"
#include "event.hpp"

class game_state;

struct critical_hit_splat {

	no::transform2 transform;
	int target_id{ -1 };
	float fade_in{ 0.0f };
	float stay{ 0.0f };
	float fade_out{ 0.0f };
	float alpha{ 0.0f };

	bool is_visible() const {
		return fade_out < 1.0f;
	}

};

class game_ui {
public:
	no::ortho_camera camera;
	no::ortho_camera text_camera;

	std::vector<critical_hit_splat> splats;
	no::font* font;

	game_ui(game_state& game);
	~game_ui();

	void update();
	void draw();

	void on_chest_open(int item, bool force = false);
	void add_hit_splat(int target_id);

	void register_event_listeners();

private:
	
	struct {
		bool open{ false };
		int item{ -1 };
	} chest_ui;
	no::text_view chest_item_name;
	no::text_view chest_message;

	no::transform2 overlay_transform() const;
	void update_hit_splats();
	void draw_hit_splats();

	game_state& game;

	int ui_texture{ -1 };
	no::rectangle rectangle;
	int critical_texture{ -1 };
	
	no::rectangle static_rectangle;
	no::text_view stat_strength;
	no::text_view stat_attack_speed;
	no::text_view stat_critical;
	no::text_view stat_defense;
	no::text_view stat_speed;
	no::text_view stat_regen_health;

	no::text_view weapon_text;

	no::event_listener key_press;

};
