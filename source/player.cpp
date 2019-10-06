#include "player.hpp"
#include "world.hpp"

void player_object::update() {

}

void player_object::move(bool left, bool right, bool up, bool down) {
	const float speed{ 2.0f };
	const auto position{ transform.position + collision::offset };
	auto delta{ world->get_allowed_movement_delta(left, right, up, down, speed, position, collision::size) };
	transform.position += delta;
	is_moving = (delta.x != 0.0f || delta.y != 0.0f);
	if (delta.x != 0.0f) {
		facing_right = (delta.x > 0.0f);
	}
	if (delta.y != 0.0f) {
		facing_down = (delta.y > 0.0f);
	}
}
