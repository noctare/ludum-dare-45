#include "renderer.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "window.hpp"
#include "surface.hpp"

class world_autotiler {
public:

	std::unordered_map<unsigned int, no::vector2i> uv;

	world_autotiler() {
		load_main_tiles();
		load_group(tile_type::wall, tile_type::floor, 0, 1);
	}

	no::vector2i get_uv(const game_world_tile& tile) const {
		if (const auto it{ uv.find(tile.get_uv_index()) }; it != uv.end()) {
			return it->second * tile_size;
		} else {
			return 0;
		}
	}

	void load_main_tiles() {
		for (unsigned char i{ 0 }; i < tile_type::total_types; i++) {
			const game_world_tile tile{ i };
			uv[tile.get_uv_index()] = { i, 0 };
		}
	}

	void load_tile(unsigned char top_left, unsigned char top_right, unsigned char bottom_left, unsigned char bottom_right, int x, int y) {
		game_world_tile tile;
		tile.corner[0] = top_left;
		tile.corner[1] = top_right;
		tile.corner[2] = bottom_left;
		tile.corner[3] = bottom_right;
		uv[tile.get_uv_index()] = { x, y };
	}

	void load_group(int primary, int sub, int x, int y) {
		load_tile(primary, primary, primary, sub, x, y);
		load_tile(primary, primary, sub, sub, x + 1, y);
		load_tile(primary, primary, sub, primary, x + 2, y);
		load_tile(primary, sub, primary, primary, x, y + 1);
		load_tile(sub, sub, primary, primary, x + 1, y + 1);
		load_tile(sub, primary, primary, primary, x + 2, y + 1);
		y += 2;
		load_tile(sub, sub, sub, primary, x, y);
		load_tile(sub, sub, primary, primary, x + 1, y);
		load_tile(sub, sub, primary, sub, x + 2, y);
		load_tile(sub, primary, sub, sub, x, y + 1);
		load_tile(primary, primary, sub, sub, x + 1, y + 1);
		load_tile(primary, sub, sub, sub, x + 2, y + 1);
		y += 2;
		load_tile(sub, primary, primary, sub, x, y);
		load_tile(primary, sub, primary, sub, x + 2, y);
		load_tile(primary, sub, sub, primary, x, y + 1);
		load_tile(sub, primary, sub, primary, x + 2, y + 1);
	}

};

game_renderer::game_renderer(game_state& game) : game{ game } {
	shader = no::require_shader("sprite");
	blank_texture = no::create_texture({ 2, 2, no::pixel_format::rgba, 0xFFFFFFFF });
	room_transform.scale = static_cast<float>(tile_size);
	tiles_texture = no::require_texture("tiles");
	//animation.frames = 6;
	//animation.set_tex_coords(0.0f, { 1.0f, 1.0f / 5.0f });
}

game_renderer::~game_renderer() {
	no::delete_texture(blank_texture);
	no::release_texture("tiles");
}

void game_renderer::update() {
	camera.zoom = game.zoom;
	camera.transform.scale = game.window().size().to<float>();
	if (!game.god_mode) {
		camera.transform.position = game.world.player.transform.position - camera.transform.scale / 2.0f;
	}
	//animation.update(1.0f / 60.0f);
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
	world_autotiler autotiler;
	auto& rendered_room{ rendered_rooms.emplace_back() };
	rendered_room.room = &room;
	auto& shape{ rendered_room.shape };
	for (int x{ room.left() }; x < room.right(); x++) {
		for (int y{ room.top() }; y < room.bottom(); y++) {
			const int local_x{ x - room.index.x };
			const int local_y{ y - room.index.y };
			const auto& tile{ room.tile_at(local_x, local_y) };
			const auto auto_uv{ autotiler.get_uv(tile) };
			const no::vector2f uv_1{ auto_uv.to<float>() / tileset_size };
			const no::vector2f uv_2{ uv_1 + uv_step };
			no::sprite_vertex top_left;
			no::sprite_vertex top_right;
			no::sprite_vertex bottom_right;
			no::sprite_vertex bottom_left;
			top_left.position = { static_cast<float>(x), static_cast<float>(y) };
			top_right.position = { static_cast<float>(x + 1), static_cast<float>(y) };
			bottom_right.position = { static_cast<float>(x + 1), static_cast<float>(y + 1) };
			bottom_left.position = { static_cast<float>(x), static_cast<float>(y + 1) };
			top_left.tex_coords = uv_1;
			top_right.tex_coords = { uv_2.x, uv_1.y };
			bottom_left.tex_coords = { uv_1.x, uv_2.y };
			bottom_right.tex_coords = uv_2;
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

bool game_renderer::is_rendered(const game_world_room& room) const {
	for (auto& rendered_room : rendered_rooms) {
		if (rendered_room.room == &room) {
			return true;
		}
	}
	return false;
}

void game_renderer::draw_world(const game_world& world) const {
	no::get_shader_variable("color").set(no::vector4f{ 1.0f });
	no::bind_texture(tiles_texture);
	no::set_shader_model(room_transform);
	for (const auto& room : rendered_rooms) {
		room.shape.bind();
		room.shape.draw();
	}
	no::bind_texture(blank_texture);
	//animation.draw(world.player.transform.position, { texture_size.x / 6.0f, texture_size.y / 5.0f });
}
