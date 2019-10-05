#pragma once

#include "transform.hpp"

class player_object {
public:

	no::transform2 transform;

	void update();

	void move_up(float speed);
	void move_left(float speed);
	void move_down(float speed);
	void move_right(float speed);

};
