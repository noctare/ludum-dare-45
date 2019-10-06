#include "player.hpp"
#include "world.hpp"

void player_object::update() {
	if (!room || !room->is_position_within(transform.position)) {
		room = world->find_room(transform.position);
	}
}

void player_object::move(bool left, bool right, bool up, bool down) {
	const float speed{ 2.0f };
	const auto position{ transform.position + collision::offset };
	auto delta{ world->get_allowed_movement_delta(room, left, right, up, down, speed, position, collision::size) };
	transform.position += delta;
	is_moving = (delta.x != 0.0f || delta.y != 0.0f);
	if (delta.x != 0.0f) {
		facing_right = (delta.x > 0.0f);
	}
	if (delta.y != 0.0f) {
		facing_down = (delta.y > 0.0f);
	}
}
