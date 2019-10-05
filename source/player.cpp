#include "player.hpp"

void player_object::update() {

}

void player_object::move_up(float speed) {
	transform.position.y -= speed;
}

void player_object::move_left(float speed) {
	transform.position.x -= speed;
}

void player_object::move_down(float speed) {
	transform.position.y += speed;
}

void player_object::move_right(float speed) {
	transform.position.x += speed;
}
