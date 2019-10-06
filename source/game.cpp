#include "game.hpp"
#include "window.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_platform.h"
#include "assets.hpp"
#include <ctime>

game_state::game_state() : renderer{ *this }, generator{ static_cast<unsigned long long>(std::time(nullptr)) }, controller{ *this } {
	no::imgui::create(window());
	set_synchronization(no::draw_synchronization::if_updated);
	window().set_swap_interval(no::swap_interval::immediate);
	window().set_clear_color({ 71.0f / 256.0f, 27.0f / 256.0f, 0.0f });
	generator.generate(world);
	world.game = this;
	controller.register_event_listeners();
	for (auto& room : world.rooms) {
		if (const auto position{ room.find_empty_position() }) {
			world.player.transform.position = position.value();
			break;
		}
	}
}

game_state::~game_state() {
	no::imgui::destroy();
}

void game_state::update() {
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
		if (ImGui::MenuItem("Show attack collisions", nullptr, &show_attacks)) {

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
		if (ImGui::MenuItem("Set weapon: None")) {
			world.player.weapon = weapon_type::none;
		}
		if (ImGui::MenuItem("Set weapon: Normal sword")) {
			world.player.weapon = weapon_type::normal_sword;
		}
		if (ImGui::MenuItem("Set weapon: Normal staff")) {
			world.player.weapon = weapon_type::normal_staff;
		}
		if (ImGui::MenuItem("Set weapon: Fire sword")) {
			world.player.weapon = weapon_type::fire_sword;
		}
		if (ImGui::MenuItem("Set weapon: Fire staff")) {
			world.player.weapon = weapon_type::fire_staff;
		}
		if (ImGui::MenuItem("Set weapon: Water sword")) {
			world.player.weapon = weapon_type::water_sword;
		}
		if (ImGui::MenuItem("Set weapon: Water staff")) {
			world.player.weapon = weapon_type::water_staff;
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
	}
	ImGui::EndMainMenuBar();
	no::imgui::end_frame();
	if (god_mode) {
		renderer.camera.transform.position.y -= keyboard().is_key_down(no::key::w) * 15.0f;
		renderer.camera.transform.position.x -= keyboard().is_key_down(no::key::a) * 15.0f;
		renderer.camera.transform.position.y += keyboard().is_key_down(no::key::s) * 15.0f;
		renderer.camera.transform.position.x += keyboard().is_key_down(no::key::d) * 15.0f;
	} else {
		controller.update();
	}
	world.update();
	renderer.update();
}

void game_state::draw() {
	renderer.draw();
	no::imgui::draw();
}
