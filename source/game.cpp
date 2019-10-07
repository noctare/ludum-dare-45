#include "game.hpp"
#include "item.hpp"
#include "window.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_platform.h"
#include "assets.hpp"
#include <ctime>

#define WITH_DEBUG_MENU 0

game_state::game_state() : ui{ *this }, renderer { *this }, controller{ *this } {
#if WITH_DEBUG_MENU
	no::imgui::create(window());
#endif
	set_synchronization(no::draw_synchronization::if_updated);
	window().set_swap_interval(no::swap_interval::immediate);
	set_background('f');
	world.game = this;
	controller.register_event_listeners();
	enter_lobby();
	bg_music = no::require_sound("bg");
	magic_sound = no::require_sound("magic");
	stab_sound = no::require_sound("stab");
	play_sound(bg_music);
	bg_loop.start();
}

game_state::~game_state() {
#if WITH_DEBUG_MENU
	no::imgui::destroy();
#endif
	no::release_sound("bg");
	no::release_sound("magic");
	no::release_sound("stab");
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
	renderer.clear_rendered();
	generator.generate_lobby(world);
	for (auto& room : world.rooms) {
		if (const auto position{ room.find_empty_position() }) {
			world.player.transform.position = position.value();
			break;
		}
	}
	world.player.stats.health = world.player.final_stats().max_health;
	world.player.stats.mana = world.player.final_stats().max_mana;
}

void game_state::enter_dungeon(char type) {
	world.rooms.clear();
	renderer.clear_rendered();
	generator.generate_dungeon(world, type);
	for (auto& room : world.rooms) {
		if (const auto position{ room.find_empty_position() }) {
			world.player.transform.position = position.value();
			break;
		}
	}
}

void game_state::update() {
	if (bg_loop.milliseconds() > 42000) {
		play_sound(bg_music);
		bg_loop.start();
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
