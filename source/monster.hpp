#pragma once

#include "object.hpp"
#include "draw.hpp"

class monster_object : public game_object {
public:

	monster_object();

	void update();
	void attack();

private:

	void set_walk_animation();
	void set_idle_animation();
	void set_stab_animation();

};
