#include "renderer.hpp"
#include "game.hpp"
#include "item.hpp"
#include "assets.hpp"
#include "window.hpp"
#include "surface.hpp"

namespace uv {
constexpr no::vector4f chest_closed{ 0.0f, 0.0f, 0.25f, 1.0f };
constexpr no::vector4f chest_open{ 0.25f, 0.0f, 0.25f, 1.0f };
constexpr no::vector4f crate{ 0.5f, 0.0f, 0.25f, 1.0f };
constexpr no::vector4f broken_crate{ 0.75f, 0.0f, 0.25f, 1.0f };
}

game_renderer::game_renderer(game_state& game) : game{ game } {
	shader = no::require_shader("sprite");
	blank_texture = no::create_texture({ 2, 2, no::pixel_format::rgba, 0xFFFFFFFF });
	room_transform.scale = static_cast<float>(tile_size);
	fire_tiles_texture = no::require_texture("fire_tiles");
	water_tiles_texture = no::require_texture("water_tiles");
	light_tiles_texture = no::require_texture("light_tiles");
	player_texture = no::require_texture("player");
	normal_weapon_texture = no::require_texture("normal");
	fire_weapon_texture = no::require_texture("fire");
	water_weapon_texture = no::require_texture("water");
	chest_texture = no::require_texture("chest");
	magic_texture = no::require_texture("magic");
	monster_texture[monster_type::skeleton] = no::require_texture("skeleton");
	monster_texture[monster_type::life_wizard] = no::require_texture("life_wizard");
	monster_texture[monster_type::dark_wizard] = no::require_texture("dark_wizard");
	monster_texture[monster_type::toxic_wizard] = no::require_texture("toxic_wizard");
	monster_texture[monster_type::big_fire_slime] = no::require_texture("big_fire_slime");
	monster_texture[monster_type::small_fire_slime] = no::require_texture("small_fire_slime");
	monster_texture[monster_type::big_water_slime] = no::require_texture("big_water_slime");
	monster_texture[monster_type::small_water_slime] = no::require_texture("small_water_slime");
	monster_texture[monster_type::knight] = no::require_texture("knight");
	monster_texture[monster_type::water_fish] = no::require_texture("water_fish");
	monster_texture[monster_type::fire_imp] = no::require_texture("fire_imp");
	monster_texture[monster_type::fire_boss] = no::require_texture("fire_boss");
	monster_texture[monster_type::water_boss] = no::require_texture("water_boss");
	monster_texture[monster_type::final_boss] = no::require_texture("final_boss");
	slash_texture = no::require_texture("slash");
	open_chest_rectangle.set_tex_coords(uv::chest_open.x, uv::chest_open.y, uv::chest_open.z, uv::chest_open.w);
	closed_chest_rectangle.set_tex_coords(uv::chest_closed.x, uv::chest_closed.y, uv::chest_closed.z, uv::chest_closed.w);
	crate_rectangle.set_tex_coords(uv::crate.x, uv::crate.y, uv::crate.z, uv::crate.w);
	broken_crate_rectangle.set_tex_coords(uv::broken_crate.x, uv::broken_crate.y, uv::broken_crate.z, uv::broken_crate.w);
}

game_renderer::~game_renderer() {
	no::delete_texture(blank_texture);
	no::release_texture("fire_tiles");
	no::release_texture("water_tiles");
	no::release_texture("light_tiles");
	no::release_texture("chest");
	no::release_texture("magic");
	no::release_texture("player");
	no::release_texture("normal");
	no::release_texture("fire");
	no::release_texture("water");
	no::release_shader("sprite");
	no::release_texture("skeleton");
	no::release_texture("life_wizard");
	no::release_texture("dark_wizard");
	no::release_texture("toxic_wizard");
	no::release_texture("big_fire_slime");
	no::release_texture("small_fire_slime");
	no::release_texture("big_water_slime");
	no::release_texture("small_water_slime");
	no::release_texture("knight");
	no::release_texture("water_fish");
	no::release_texture("fire_imp");
	no::release_texture("fire_boss");
	no::release_texture("water_boss");
	no::release_texture("final_boss");
	no::release_texture("slash");
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
	slash_animation.update(1.0f / 60.0f);
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
	no::vector2f tileset_size{ no::texture_size(fire_tiles_texture).to<float>() };
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
	if (room.is_boss_room) {
		if (room.type == 'f' || room.type == 'w') {
			const int x{ room.left() + room.width() / 2 - 1 };
			const int y{ room.top() };
			top_left.position = { static_cast<float>(x), static_cast<float>(y) };
			top_right.position = { static_cast<float>(x + 3), static_cast<float>(y) };
			bottom_right.position = { static_cast<float>(x + 3), static_cast<float>(y + 2) };
			bottom_left.position = { static_cast<float>(x), static_cast<float>(y + 2) };
			no::vector2f uv_1;
			uv_1 = { 96.0f, 64.0f };
			uv_1 /= tileset_size;
			const no::vector2f uv_2{ uv_1.x + uv_step.x * 3.0f, uv_1.y + uv_step.y * 2.0f };
			top_left.tex_coords = uv_1;
			top_right.tex_coords = { uv_2.x, uv_1.y };
			bottom_left.tex_coords = { uv_1.x, uv_2.y };
			bottom_right.tex_coords = uv_2;
			rendered_room.doors.append(top_left, top_right, bottom_right, bottom_left);
		}
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
	no::set_shader_model(room_transform);
	for (const auto& room : rendered_rooms) {
		if (room.room == world.player.room || game.show_all_rooms) {
			if (room.room->type == 'f') {
				no::bind_texture(fire_tiles_texture);
			} else if (room.room->type == 'w') {
				no::bind_texture(water_tiles_texture);
			} else if (room.room->type == 'l') {
				no::bind_texture(light_tiles_texture);
			}
			room.shape.bind();
			room.shape.draw();
			room.doors.bind();
			room.doors.draw();
		}
	}
	draw_objects(world);

	for (const auto& room : rendered_rooms) {
		if (room.room == world.player.room || game.show_all_rooms) {
			for (const auto& attack : room.room->attacks) {
				int projectile{ -1 };
				if (attack.by_player) {
					if (!item_type::is_staff(attack.type)) {
						continue;
					}
					projectile = 1;
				} else {
					if (!monster_type::is_magic(attack.type)) {
						continue;
					}
					projectile = 1;
				}
				no::transform2 transform;
				transform.position = attack.position;
				transform.scale = 16.0f;
				no::bind_texture(magic_texture);
				rectangle.set_tex_coords(0.0f, 0.0f, 1.0f / 7.0f, 1.0f / 3.0f);
				no::draw_shape(rectangle, transform);
			}
		}
	}

	if (game.show_collisions && world.player.room) {
		no::bind_texture(blank_texture);
		no::get_shader_variable("color").set(no::vector4f{ 1.0f, 0.0f, 0.0f, 1.0f });
		for (auto& attack : world.player.room->attacks) {
			no::draw_shape(rectangle, no::transform2{ attack.position, attack.size });
		}
		for (auto& monster : world.player.room->monsters) {
			no::draw_shape(rectangle, monster.collision_transform());
		}
		for (auto& chest : world.player.room->chests) {
			no::draw_shape(rectangle, chest.collision_transform());
		}
		no::draw_shape(rectangle, world.player.collision_transform());
		no::get_shader_variable("color").set(no::vector4f{ 1.0f });
	}
}

void game_renderer::draw_objects(const game_world& world) {
	// didn't think it would come to doing it like this... but o'well. time is running out. need it to sort.
	std::vector<const game_object*> objects;
	objects.reserve(50);
	for (const auto& room : rendered_rooms) {
		if (room.room == world.player.room || game.show_all_rooms) {
			for (const auto& monster : room.room->monsters) {
				objects.push_back(&monster);
			}
			for (const auto& chest : room.room->chests) {
				objects.push_back(&chest);
			}
		}
	}
	objects.push_back(&world.player);
	std::sort(objects.begin(), objects.end(), [&](const game_object* a, const game_object* b) {
		return b->transform.position.y > a->transform.position.y;
	});
	for (const auto object : objects) {
		switch (object->class_type()) {
		case 1:
			draw_player(*(player_object*)object);
			break;
		case 2:
			draw_monster(*(monster_object*)object);
			break;
		case 3:
			draw_chest(*(chest_object*)object);
			break;
		}
	}
}

void game_renderer::draw_monster(const monster_object& monster) {
	no::bind_texture(monster_texture[monster.type]);
	no::vector2f size{ no::texture_size(monster_texture[monster.type]).to<float>() / monster_type::sheet_frames(monster.type) };
	no::vector2f position{ monster.transform.position };
	if (!monster.facing_right) {
		position.x += size.x;
		size.x = -size.x;
	}
	monster.animation.draw(position, size);
}

void game_renderer::draw_chest(const chest_object& chest) {
	no::transform2 chest_transform;
	chest_transform.position = chest.transform.position;
	chest_transform.scale = 32.0f;
	no::bind_texture(chest_texture);
	if (chest.is_crate) { // yep. it's ld after all.
		no::draw_shape(chest.open ? broken_crate_rectangle : crate_rectangle, chest_transform);
	} else {
		no::draw_shape(chest.open ? open_chest_rectangle : closed_chest_rectangle, chest_transform);
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
	if (item_type::is_weapon(player.equipped_weapon())) {
		switch (player.active_power()) {
		case item_type::fire_head:
			no::bind_texture(fire_weapon_texture);
			break;
		case item_type::water_head:
			no::bind_texture(water_weapon_texture);
			break;
		default:
			no::bind_texture(normal_weapon_texture);
			break;
		}
		player.animation.draw(position, size);
	}
}

void game_renderer::clear_rendered() {
	rendered_rooms.clear();
}
