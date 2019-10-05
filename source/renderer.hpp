#pragma once

#include "draw.hpp"
#include "camera.hpp"

class game_state;
class game_world;
class game_world_room;

class game_renderer {
public:

	no::ortho_camera camera;

	game_renderer(game_state& game);
	~game_renderer();

	void update();
	void draw();
	void render();

	void render_room(const game_world_room& room);
	void hide_room(const game_world_room& room);
	bool is_rendered(const game_world_room& room) const;
	void draw_world(const game_world& world) const;

private:

	game_state& game;
	no::sprite_animation animation;
	int shader{ -1 };
	no::rectangle rectangle;
	int blank_texture{ -1 };
	int tiles_texture{ -1 };

	no::transform2 room_transform;

	struct rendered_room {
		no::quad_array<no::sprite_vertex, unsigned short> shape;
		const game_world_room* room{ nullptr };
	};

	std::vector<rendered_room> rendered_rooms;

};
