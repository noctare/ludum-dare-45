#pragma once

#include "draw.hpp"
#include "camera.hpp"
#include "monster.hpp"

class game_state;
class game_world;
class game_world_room;
class player_object;
class chest_object;

class game_renderer {
public:

	int blank_texture{ -1 };

	no::ortho_camera camera;

	game_renderer(game_state& game);
	~game_renderer();

	void update();
	void draw();
	void render();

	void render_room(const game_world_room& room);
	void hide_room(const game_world_room& room);
	bool is_rendered(const game_world_room& room) const;
	void draw_world(const game_world& world);
	void draw_objects(const game_world& world);
	void draw_player(const player_object& player);
	void draw_monster(const monster_object& monster);
	void draw_chest(const chest_object& chest);

	void clear_rendered();

private:

	game_state& game;
	int shader{ -1 };
	no::rectangle rectangle;
	int fire_tiles_texture{ -1 };
	int water_tiles_texture{ -1 };
	int light_tiles_texture{ -1 };
	int player_texture{ -1 };
	int monster_texture[monster_type::total_types];
	int fire_weapon_texture{ -1 };
	int normal_weapon_texture{ -1 };
	int water_weapon_texture{ -1 };
	int chest_texture{ -1 };
	int magic_texture{ -1 };
	int slash_texture{ -1 };
	no::sprite_animation slash_animation;

	no::transform2 room_transform;

	no::rectangle open_chest_rectangle;
	no::rectangle closed_chest_rectangle;
	no::rectangle crate_rectangle;
	no::rectangle broken_crate_rectangle;

	struct rendered_room {
		no::quad_array<no::sprite_vertex, unsigned short> shape;
		no::quad_array<no::sprite_vertex, unsigned short> doors;
		const game_world_room* room{ nullptr };
	};

	std::vector<rendered_room> rendered_rooms;

};
