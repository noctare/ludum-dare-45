#include "renderer.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "window.hpp"
#include "surface.hpp"

game_renderer::game_renderer(game_state& game) : game{ game } {
	shader = no::require_shader("sprite");
	blank_texture = no::create_texture({ 2, 2, no::pixel_format::rgba, 0xFFFFFFFF });
	room_transform.scale = static_cast<float>(tile_size);
	tiles_texture = no::require_texture("tiles");
	player_texture = no::require_texture("player");
	skeleton_texture = no::require_texture("skeleton");
	normal_weapon_texture = no::require_texture("normal");
	fire_weapon_texture = no::require_texture("fire");
	water_weapon_texture = no::require_texture("water");
}

game_renderer::~game_renderer() {
	no::delete_texture(blank_texture);
	no::release_texture("tiles");
	no::release_texture("player");
	no::release_texture("skeleton");
	no::release_texture("normal");
	no::release_texture("fire");
	no::release_texture("water");
	no::release_shader("sprite");
}

void game_renderer::update() {
	camera.zoom = game.zoom;
	camera.transform.scale = game.window().size().to<float>();
	if (game.god_mode) {
		camera.target = nullptr;
	} else {
		camera.target = &game.world.player.transform;
		camera.target_chase_speed = 0.075f;
		camera.target_chase_aspect = { 2.0f, 2.0f };
	}
	camera.update();
}

void game_renderer::draw() {
	render();
	no::bind_shader(shader);
	no::set_shader_view_projection(camera);
	draw_world(game.world);
}

void game_renderer::render() {
	for (const auto& room : game.world.rooms) {
		render_room(room);
	}
}

void game_renderer::render_room(const game_world_room& room) {
	if (is_rendered(room)) {
		return;
	}
	no::vector2f tileset_size{ no::texture_size(tiles_texture).to<float>() };
	no::vector2f uv_step{ 32.0f / tileset_size };
	auto& rendered_room{ rendered_rooms.emplace_back() };
	rendered_room.room = &room;
	no::sprite_vertex top_left;
	no::sprite_vertex top_right;
	no::sprite_vertex bottom_right;
	no::sprite_vertex bottom_left;
	for (int x{ room.left() }; x < room.right(); x++) {
		for (int y{ room.top() }; y < room.bottom(); y++) {
			const int local_x{ x - room.index.x };
			const int local_y{ y - room.index.y };
			const auto& tile{ room.tile_at(local_x, local_y) };
			const auto auto_uv{ game.world.autotiler.get_uv(tile) };
			const no::vector2f uv_1{ auto_uv.to<float>() / tileset_size };
			const no::vector2f uv_2{ uv_1 + uv_step };
			top_left.position = { static_cast<float>(x), static_cast<float>(y) };
			top_right.position = { static_cast<float>(x + 1), static_cast<float>(y) };
			bottom_right.position = { static_cast<float>(x + 1), static_cast<float>(y + 1) };
			bottom_left.position = { static_cast<float>(x), static_cast<float>(y + 1) };
			top_left.tex_coords = uv_1;
			top_right.tex_coords = { uv_2.x, uv_1.y };
			bottom_left.tex_coords = { uv_1.x, uv_2.y };
			bottom_right.tex_coords = uv_2;
			rendered_room.shape.append(top_left, top_right, bottom_right, bottom_left);
		}
	}
	for (const auto door : room.doors) {
		const int x{ room.left() + door.from_tile.x };
		const int y{ room.top() + door.from_tile.y };
		top_left.position = { static_cast<float>(x), static_cast<float>(y) };
		top_right.position = { static_cast<float>(x + 1), static_cast<float>(y) };
		bottom_right.position = { static_cast<float>(x + 1), static_cast<float>(y + 1) };
		bottom_left.position = { static_cast<float>(x), static_cast<float>(y + 1) };
		no::vector2f uv_1;
		if (door.from_tile.x == 1) {
			uv_1 = { 128.0f, 0.0f }; // left
		} else if (door.from_tile.y == 1) {
			uv_1 = { 64.0f, 0.0f }; // top
		} else if (door.from_tile.x == room.width() - 2) {
			uv_1 = { 160.0f, 0.0f }; // right
		} else if (door.from_tile.y == room.height() - 2) {
			uv_1 = { 96.0f, 0.0f }; // bottom
		}
		uv_1 /= tileset_size;
		const no::vector2f uv_2{ uv_1 + uv_step };
		top_left.tex_coords = uv_1;
		top_right.tex_coords = { uv_2.x, uv_1.y };
		bottom_left.tex_coords = { uv_1.x, uv_2.y };
		bottom_right.tex_coords = uv_2;
		rendered_room.doors.append(top_left, top_right, bottom_right, bottom_left);
	}
	rendered_room.shape.refresh();
	if (!room.doors.empty()) {
		rendered_room.doors.refresh();
	}
}

void game_renderer::hide_room(const game_world_room& room) {
	for (int i{ 0 }; i < static_cast<int>(rendered_rooms.size()); i++) {
		if (rendered_rooms[i].room == &room) {
			rendered_rooms.erase(rendered_rooms.begin() + i);
			i--;
		}
	}
}

bool game_renderer::is_rendered(const game_world_room& room) const {
	for (auto& rendered_room : rendered_rooms) {
		if (rendered_room.room == &room) {
			return true;
		}
	}
	return false;
}

void game_renderer::draw_world(const game_world& world) {
	no::get_shader_variable("color").set(no::vector4f{ 1.0f });
	no::bind_texture(tiles_texture);
	no::set_shader_model(room_transform);
	for (const auto& room : rendered_rooms) {
		if (room.room == world.player.room || game.show_all_rooms) {
			room.shape.bind();
			room.shape.draw();
			room.doors.bind();
			room.doors.draw();
		}
	}
	no::bind_texture(skeleton_texture);
	for (const auto& room : rendered_rooms) {
		if (room.room == world.player.room || game.show_all_rooms) {
			for (const auto& skeleton : room.room->monsters) {
				no::vector2f size{ no::texture_size(skeleton_texture).to<float>() / no::vector2f{ 4.0f, 7.0f } };
				no::vector2f position{ skeleton.transform.position };
				if (!skeleton.facing_right) {
					position.x += size.x;
					size.x = -size.x;
				}
				skeleton.animation.draw(position, size);
			}
		}
	}
	draw_player(world.player);
	if (game.show_attacks && world.player.room) {
		no::bind_texture(blank_texture);
		no::get_shader_variable("color").set(no::vector4f{ 1.0f, 0.0f, 0.0f, 1.0f });
		for (auto& attack : world.player.room->attacks) {
			no::draw_shape(rectangle, no::transform2{ attack.position, attack.size });
		}
		no::get_shader_variable("color").set(no::vector4f{ 1.0f });
	}
}

void game_renderer::draw_player(const player_object& player) {
	no::vector2f size{ no::texture_size(player_texture).to<float>() / no::vector2f{ 4.0f, player_animation_rows } };
	no::vector2f position{ player.transform.position };
	if (!player.facing_right) {
		position.x += size.x;
		size.x = -size.x;
	}
	no::bind_texture(player_texture);
	player.animation.draw(position, size);
	if (player.weapon != weapon_type::none) {
		switch (player.weapon) {
		case weapon_type::normal_sword:
		case weapon_type::normal_staff:
			no::bind_texture(normal_weapon_texture);
			break;
		case weapon_type::fire_sword:
		case weapon_type::fire_staff:
			no::bind_texture(fire_weapon_texture);
			break;
		case weapon_type::water_sword:
		case weapon_type::water_staff:
			no::bind_texture(water_weapon_texture);
			break;
		}
		player.animation.draw(position, size);
	}
}
