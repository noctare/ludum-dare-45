#include "renderer.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "window.hpp"
#include "surface.hpp"

game_renderer::game_renderer(game_state& game) : game{ game } {
	test_texture = no::require_texture("test");
	shader = no::require_shader("sprite");
	animation.frames = 6;
	animation.set_tex_coords(0.0f, { 1.0f, 1.0f / 5.0f });
	blank_texture = no::create_texture({ 2, 2, no::pixel_format::rgba, 0xFFFFFFFF });
}

game_renderer::~game_renderer() {
	no::delete_texture(blank_texture);
}

void game_renderer::update() {
	camera.zoom = game.zoom;
	camera.transform.scale = game.window().size().to<float>();
	camera.transform.position = game.world.player.transform.position - camera.transform.scale / 2.0f;
	animation.update(1.0f / 60.0f);
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
	auto& rendered_room{ rendered_rooms.emplace_back() };
	rendered_room.room = &room;
	//rendered_room.transform.position = { static_cast<float>(room.index.x * 32), static_cast<float>(room.index.y * 32) };
	rendered_room.transform.scale = 32.0f;
	auto& shape{ rendered_room.shape };
	for (int x{ room.left() }; x < room.right(); x++) {
		for (int y{ room.top() }; y < room.bottom(); y++) {
			const int local_x{ x - room.index.x };
			const int local_y{ y - room.index.y };
			const bool is_floor{ room.tile_at(local_x, local_y) == tile_type::floor };
			no::sprite_vertex top_left;
			no::sprite_vertex top_right;
			no::sprite_vertex bottom_right;
			no::sprite_vertex bottom_left;
			top_left.position = { static_cast<float>(x), static_cast<float>(y) };
			top_right.position = { static_cast<float>(x + 1), static_cast<float>(y) };
			bottom_right.position = { static_cast<float>(x + 1), static_cast<float>(y + 1) };
			bottom_left.position = { static_cast<float>(x), static_cast<float>(y + 1) };
			top_left.color = (!is_floor ? no::vector4f{ 0.5f, 0.0f, 0.0f, 1.0f } : 1.0f);
			top_right.color = top_left.color;
			bottom_right.color = top_left.color;
			bottom_left.color = top_left.color;
			shape.append(top_left, top_right, bottom_right, bottom_left);
		}
	}
	shape.refresh();
}

void game_renderer::hide_room(const game_world_room& room) {
	for (int i{ 0 }; i < static_cast<int>(rendered_rooms.size()); i++) {
		if (rendered_rooms[i].room == &room) {
			rendered_rooms.erase(rendered_rooms.begin() + i);
			i--;
		}
	}
}

bool game_renderer::is_rendered(const game_world_room& room) {
	for (auto& rendered_room : rendered_rooms) {
		if (rendered_room.room == &room) {
			return true;
		}
	}
	return false;
}

void game_renderer::draw_world(const game_world& world) {
	no::get_shader_variable("color").set(no::vector4f{ 1.0f });
	no::bind_texture(blank_texture);
	for (const auto& room : rendered_rooms) {
		no::draw_shape(room.shape, room.transform);
	}
	no::bind_texture(test_texture);
	const no::vector2f test_size{ no::texture_size(test_texture).to<float>() };
	const no::vector2f size{ test_size.x / 6.0f, test_size.y / 5.0f };
	animation.draw(world.player.transform.position, size);
}

void game_renderer::draw_room(const game_world_room& room) {
	/*no::transform2 transform;
	transform.scale = static_cast<float>(tile_size);
	for (int x{ room.left() }; x < room.right(); x++) {
		transform.position.x = static_cast<float>(x * tile_size);
		for (int y{ room.top() }; y < room.bottom(); y++) {
			transform.position.y = static_cast<float>(y * tile_size);
			const no::vector4f color{ 0.5f, 0.5f, room.tile_at(x - room.index.x, y - room.index.y) == tile_type::floor ? 1.0f : 0.0f, 1.0f };
			no::get_shader_variable("color").set(color);
			no::draw_shape(rectangle, transform);
		}
	}*/
}
