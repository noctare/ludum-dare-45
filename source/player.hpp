#pragma once

#include "object.hpp"

class player_object : public game_object {
public:

	struct collision {
		static constexpr no::vector2f offset{ 10.0f, 16.0f };
		static constexpr no::vector2f size{ 11.0f, 13.0f };
	};

	void update();
	void move(bool left, bool right, bool up, bool down);
	void attack();

private:

	void set_walk_animation();
	void set_idle_animation();
	void set_stab_animation();

};
