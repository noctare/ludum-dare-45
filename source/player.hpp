#pragma once

#include "object.hpp"

constexpr float player_animation_rows{ 10.0f };

class player_object : public game_object {
public:

	bool locked_by_ui{ false };

	struct collision {
		static constexpr no::vector2f offset{ 10.0f, 16.0f };
		static constexpr no::vector2f size{ 11.0f, 13.0f };
	};

	player_object();

	void update();
	void move(bool left, bool right, bool up, bool down);
	void attack();
	void on_being_hit();

	no::transform2 collision_transform() const override;
	
	int equipped_weapon() const;
	void change_weapon(int scroll);
	void give_item(int type, int slot);
	int active_power() const;
	int item_in_slot(int slot) const;
	bool has_empty_slot() const {
		for (int i{ 2 }; i < 8; i++) {
			if (items[i] < 0) {
				return true;
			}
		}
		return false;
	}

	object_stats final_stats() const;

	int class_type() const override {
		return 1;
	}

	void open_chest();

	void set_die_animation();
private:

	std::vector<int> weapons; // item_type
	int weapon{ -1 }; // weapons index
	int items[8]; // item_type
	int power{ -1 }; // item_type

	void set_walk_animation();
	void set_idle_animation();
	void set_stab_animation();
	void set_cast_animation();
	void set_hit_animation();
	void set_hit_flash_animation();

};
