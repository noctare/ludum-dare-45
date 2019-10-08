#include "game.hpp"
#include "item.hpp"
#include "window.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_platform.h"
#include "assets.hpp"
#include <ctime>

#define WITH_DEBUG_MENU 0

game_state::game_state() : ui{ *this }, renderer{ *this }, controller{ *this }, intro_text{ *this, ui.camera }
#if POST_LD_FEATURE_KILL_COUNT
,kill_count_text{ *this, ui.camera }
#endif
, instructions{ *this, ui.camera }
{
#if WITH_DEBUG_MENU
	no::imgui::create(window());
#endif
	set_synchronization(no::draw_synchronization::if_updated);
	window().set_swap_interval(no::swap_interval::immediate);
	window().set_icon_from_resource(102);
	bg_music = no::require_sound("bg");
	cover_texture = no::require_texture("cover");
	if (ui.font) {
		intro_text.render(*ui.font, "Press any key to start playing!");
		intro_text.transform.scale *= 2.0f;
		// POST-TWEAK: Show instructions!
		instructions.render(*ui.font,
			"Movement: WASD\n\n"
			"Open chests: Space\n\n"
			"Attack: Space (Must have weapon equipped)\n\n"
			"Change weapon: Q and E\n\n"
			"Open door: Space / Enter / Left shift\n\n"
			"Use item: 3, 4, 5, 6, 7, and 8\n(must be consumable item, such as potion)"
		);
		instructions.transform.scale *= 2.0f;
		//
	}
	intro_listen_key = keyboard().press.listen([this](no::key key) {
		if (show_intro) {
			start_playing();
		} else if (key == no::key::m) {
			// POST-TWEAK: Let players toggle audio, instead of having to delete files.
			play_audio = !play_audio;
			if (play_audio) {
				play_sound(bg_music);
				bg_loop.start();
			} else {
				for (auto& audio_player : audio_players) {
					audio_player->stop();
				}
			}
			//
		} else if (key == no::key::l) {
			// POST-TWEAK: Let players toggle synchronization
			limit_fps = !limit_fps;
			set_synchronization(limit_fps ? no::draw_synchronization::if_updated : no::draw_synchronization::always);
			//
		}
	});
	random_intro_dist_timer.start();
	play_sound(bg_music);
	bg_loop.start();
}

void game_state::start_playing() {
	show_intro = false;
	set_background('l');
	world.game = this;
	enter_lobby();
	ui.register_event_listeners(); // POST-BUGFIX: Moved to here instead of constructor
	controller.register_event_listeners();
}

game_state::~game_state() {
#if WITH_DEBUG_MENU
	no::imgui::destroy();
#endif
	no::release_sound("bg");
	no::release_texture("cover");
}

void game_state::play_sound(no::audio_source* sound) {
	for (auto& audio_player : audio_players) {
		if (!audio_player->is_playing()) {
			audio_player->play(sound);
			return;
		}
	}
	audio_players.emplace_back(audio().add_player())->play(sound);
}

void game_state::enter_lobby() {
	world.rooms.clear();
	world.player.room = nullptr;
	world.is_boss_dead = false;
	renderer.clear_rendered();
	generator.generate_lobby(world);
	set_background('l'); // POST-BUGFIX: Background wasn't set until going to next room.
	for (auto& room : world.rooms) {
		if (const auto position{ room.find_empty_position() }) {
			world.player.transform.position = position.value();
			break;
		}
	}
	world.player.stats.health = world.player.final_stats().max_health;
	world.player.stats.mana = world.player.final_stats().max_mana;
#if POST_LD_FEATURE_KILL_COUNT
	in_lobby = true;
#endif
}

void game_state::enter_dungeon(char type) {
	world.rooms.clear();
	world.player.room = nullptr;
	world.is_boss_dead = false;
	renderer.clear_rendered();
	generator.generate_dungeon(world, type);
	set_background(type); // POST-BUGFIX: Background wasn't set until going to next room.
	for (auto& room : world.rooms) {
		if (const auto position{ room.find_empty_position() }) {
			world.player.transform.position = position.value();
			break;
		}
	}
	world.add_monsters();
#if POST_LD_FEATURE_KILL_COUNT
	monster_count = 0;
	kill_count = 0;
	for (auto& room : world.rooms) {
		monster_count += static_cast<int>(room.monsters.size());
	}
	in_lobby = false;
#endif
}

void game_state::update() {
	if (bg_loop.milliseconds() > 42000) {
		play_sound(bg_music);
		bg_loop.start();
	}
	if (show_intro) {
		return;
	}
#if WITH_DEBUG_MENU
	no::imgui::start_frame();
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Options")) {
		ImGui::PushItemWidth(360.0f);
		if (ImGui::MenuItem("Limit FPS", nullptr, &limit_fps)) {
			set_synchronization(limit_fps ? no::draw_synchronization::if_updated : no::draw_synchronization::always);
		}
		if (ImGui::MenuItem("God mode", nullptr, &god_mode)) {
			if (god_mode) {
				show_all_rooms = true;
			}
		}
		if (ImGui::MenuItem("Show all rooms", nullptr, &show_all_rooms)) {

		}
		if (ImGui::MenuItem("Show collisions", nullptr, &show_collisions)) {

		}
		ImGui::PopItemWidth();
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View")) {
		ImGui::PushItemWidth(360.0f);
		if (ImGui::MenuItem("Reset camera position")) {
			renderer.camera.transform.position = 0.0f;
		}
		if (ImGui::MenuItem("Zoom (400%)")) {
			zoom = 4.0f;
		}
		if (ImGui::MenuItem("Zoom (300%)")) {
			zoom = 3.0f;
		}
		if (ImGui::MenuItem("Zoom (200%)")) {
			zoom = 2.0f;
		}
		if (ImGui::MenuItem("Zoom (100%)")) {
			zoom = 1.0f;
		}
		if (ImGui::MenuItem("Zoom (50%)")) {
			zoom = 0.5f;
		}
		if (ImGui::MenuItem("Zoom (25%)")) {
			zoom = 0.25f;
		}
		if (ImGui::MenuItem("Zoom (5%)")) {
			zoom = 0.05f;
		}
		ImGui::PopItemWidth();
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Player")) {
		ImGui::PushItemWidth(360.0f);
		if (ImGui::MenuItem("Give weapons")) {
			world.player.give_item(item_type::sword, 0);
			world.player.give_item(item_type::scimitar, 0);
			world.player.give_item(item_type::fire_staff, 0);
			world.player.give_item(item_type::water_staff, 0);
		}
		if (ImGui::MenuItem("Give boss stuff")) {
			world.player.give_item(item_type::water_head, 0);
			world.player.give_item(item_type::fire_head, 1);
		}
		if (ImGui::MenuItem("Give some stuff")) {
			world.player.give_item(item_type::pendant_of_thunder, 2);
			world.player.give_item(item_type::band_of_patience, 3);
			world.player.give_item(item_type::moon_tiara, 4);
			world.player.give_item(item_type::armor_of_neglect, 5);
			world.player.give_item(item_type::life_potion, 6);
			world.player.give_item(item_type::moon_tiara, 7);
		}
		// POST-DEBUG: Just to test the boss bugfixes/tweaks. Not part of release build.
		if (ImGui::MenuItem("Go to boss room")) {
			for (auto& room : world.rooms) {
				if (room.is_boss_room) {
					world.player.transform.position = room.index.to<float>() * 32.0f + 96.0f;
					world.player.transform.position.y += 64.0f;
					break;
				}
			}
		}
		//
		ImGui::PopItemWidth();
		ImGui::EndMenu();
	}
	ImGui::PushStyleColor(ImGuiCol_Text, 0xFF11EEEE);
	ImGui::Text("\tFPS: %i", frame_counter().current_fps());
	ImGui::PopStyleColor();
	ImGui::PushStyleColor(ImGuiCol_Text, 0xFFEEFF77);
	ImGui::Text("\tZoom: %i%%", static_cast<int>(zoom * 100.0f));
	ImGui::PopStyleColor();
	ImGui::Text("\tPlayer Position: %s", CSTRING(world.player.transform.position));
	if (world.player.room) {
		ImGui::Text("\tActive Attacks: %i", static_cast<int>(world.player.room->attacks.size()));
		ImGui::Text("\tHit Splats: %i", static_cast<int>(ui.splats.size()));
	}
	ImGui::EndMainMenuBar();
	no::imgui::end_frame();
#endif
	if (god_mode) {
		renderer.camera.transform.position.y -= keyboard().is_key_down(no::key::w) * 15.0f;
		renderer.camera.transform.position.x -= keyboard().is_key_down(no::key::a) * 15.0f;
		renderer.camera.transform.position.y += keyboard().is_key_down(no::key::s) * 15.0f;
		renderer.camera.transform.position.x += keyboard().is_key_down(no::key::d) * 15.0f;
	} else {
		controller.update();
	}
	if (!world.player.locked_by_ui) {
		world.update();
	}
	renderer.update();
	ui.update();
}

void game_state::draw() {
	if (show_intro) {
		renderer.camera.transform.scale = window().size().to<float>();
		no::bind_shader(renderer.shader);
		no::set_shader_view_projection(renderer.camera);
		no::bind_texture(cover_texture);
		no::transform2 transform;
		transform.scale = no::texture_size(cover_texture).to<float>();
		float aspect_ratio{ transform.scale.x / transform.scale.y };
		transform.scale.y = renderer.camera.transform.scale.y;
		transform.scale.x = transform.scale.x * aspect_ratio;
		transform.position.x = renderer.camera.transform.scale.x / 2.0f - transform.scale.x / 2.0f;
		no::get_shader_variable("color").set(no::vector4f{ 1.0f });
		no::draw_shape(renderer.rectangle, transform);

		if (random_intro_dist_timer.milliseconds() > 100) {
			random_intro_dist_1 = world.random.next<float>(-4.0f, 4.0f);
			random_intro_dist_2 = world.random.next<float>(-4.0f, 4.0f);
			random_intro_dist_timer.start();
		}

		// shadow
		no::get_shader_variable("color").set(no::vector4f{ 0.2f, 0.2f, 0.2f, 1.0f });
		intro_text.transform.position.x = renderer.camera.transform.scale.x / 2.0f - intro_text.transform.scale.x / 2.0f;
		intro_text.transform.position.y = 256.0f;
		intro_text.transform.position -= 4.0f + random_intro_dist_1;
		intro_text.draw(renderer.rectangle);

		// shadow
		no::get_shader_variable("color").set(no::vector4f{ 0.2f, 0.2f, 0.2f, 1.0f });
		intro_text.transform.position.x = renderer.camera.transform.scale.x / 2.0f - intro_text.transform.scale.x / 2.0f;
		intro_text.transform.position.y = 256.0f;
		intro_text.transform.position += 4.0f + random_intro_dist_2;
		intro_text.draw(renderer.rectangle);

		// white
		no::get_shader_variable("color").set(no::vector4f{ 1.0f });
		intro_text.transform.position.x = renderer.camera.transform.scale.x / 2.0f - intro_text.transform.scale.x / 2.0f;
		intro_text.transform.position.y = 256.0f;
		intro_text.transform.position += random_intro_dist_1 / 2.0f;
		intro_text.draw(renderer.rectangle);

		// POST-TWEAK: Show instructions!
		// shadow
		no::get_shader_variable("color").set(no::vector4f{ 0.2f, 0.2f, 0.2f, 1.0f });
		instructions.transform.position.x = renderer.camera.transform.scale.x / 2.5f - instructions.transform.scale.x / 2.0f;
		instructions.transform.position.y = renderer.camera.transform.scale.y / 1.3f - instructions.transform.scale.y;
		instructions.transform.position -= 4.0f;
		instructions.draw(renderer.rectangle);
		// shadow
		no::get_shader_variable("color").set(no::vector4f{ 0.2f, 0.2f, 0.2f, 1.0f });
		instructions.transform.position.x = renderer.camera.transform.scale.x / 2.5f - instructions.transform.scale.x / 2.0f;
		instructions.transform.position.y = renderer.camera.transform.scale.y / 1.3f - instructions.transform.scale.y;
		instructions.transform.position += 4.0f;
		instructions.draw(renderer.rectangle);
		// white
		no::get_shader_variable("color").set(no::vector4f{ 1.0f });
		instructions.transform.position.x = renderer.camera.transform.scale.x / 2.5f - instructions.transform.scale.x / 2.0f;
		instructions.transform.position.y = renderer.camera.transform.scale.y / 1.3f - instructions.transform.scale.y;
		instructions.draw(renderer.rectangle);
		//
		return;
	}
	renderer.draw();
	ui.draw();
#if WITH_DEBUG_MENU
	no::imgui::draw();
#endif
}

void game_state::set_background(char type) {
	if (type == 'f') {
		window().set_clear_color({ 71.0f / 256.0f, 27.0f / 256.0f, 0.0f });
	} else if (type == 'w') {
		window().set_clear_color({ 0.0f, 36.0f / 256.0f, 71.0f / 256.0f });
	} else if (type == 'l') {
		window().set_clear_color({ 27.0f / 256.0f, 42.0f / 256.0f, 39.0f / 256.0f });
	}
}
