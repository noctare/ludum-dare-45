#include "game.hpp"
#include "window.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_platform.h"
#include "assets.hpp"
#include <ctime>

game_state::game_state() : renderer{ *this } {
	no::imgui::create(window());
	set_synchronization(no::draw_synchronization::if_updated);
	window().set_swap_interval(no::swap_interval::immediate);
	game_world_generator generator{ static_cast<unsigned long long>(std::time(nullptr)) };
	generator.generate(world);
	world.player.transform.position = 128.0f;
	world.player.transform.scale = 16.0f;
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
	ImGui::PushStyleColor(ImGuiCol_Text, 0xFF11EEEE);
	ImGui::Text("\tFPS: %i", frame_counter().current_fps());
	ImGui::PopStyleColor();
	ImGui::PushStyleColor(ImGuiCol_Text, 0xFFEEFF77);
	ImGui::Text("\tZoom: %i%%", static_cast<int>(zoom * 100.0f));
	ImGui::PopStyleColor();
	ImGui::Text("\tPlayer Position: %s", CSTRING(world.player.transform.position));
	ImGui::EndMainMenuBar();
	no::imgui::end_frame();
	renderer.update();
	world.update();
	if (god_mode) {
		renderer.camera.transform.position.y -= keyboard().is_key_down(no::key::w) * 15.0f;
		renderer.camera.transform.position.x -= keyboard().is_key_down(no::key::a) * 15.0f;
		renderer.camera.transform.position.y += keyboard().is_key_down(no::key::s) * 15.0f;
		renderer.camera.transform.position.x += keyboard().is_key_down(no::key::d) * 15.0f;
	} else {
		controller.update(*this);
	}
}

void game_state::draw() {
	renderer.draw();
	no::imgui::draw();
}
