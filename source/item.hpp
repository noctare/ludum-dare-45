#pragma once

#include "math.hpp"
#include "object.hpp"

namespace item_type {

constexpr int ring_of_pain{ 0 };
constexpr int ring_of_luck{ 1 };
constexpr int ring_of_strength{ 2 };
constexpr int bracelet_of_power{ 3 };
constexpr int pendant_of_precision{ 4 };
constexpr int pendant_of_thunder{ 5 };
constexpr int necklace_of_protection{ 6 };
constexpr int necklace_of_stability{ 7 };
constexpr int shoes_of_sneaking{ 8 };
constexpr int fur_cape_of_the_beast{ 9 };
constexpr int band_of_patience{ 10 };
constexpr int warriors_gloves{ 11 };
constexpr int gloves_of_greed{ 12 };
constexpr int gloves_of_wind{ 13 };
constexpr int shield_of_dark_magic{ 14 };
constexpr int hat_of_fortune{ 15 };
constexpr int eye_of_the_eagle{ 16 };
constexpr int gauntlets_of_success{ 17 };
constexpr int moon_tiara{ 18 };
constexpr int armor_of_neglect{ 19 };
constexpr int talisman_of_protection{ 20 };
constexpr int pendant_of_life{ 21 };
constexpr int helm_of_doom{ 22 };
constexpr int helm_of_hatred{ 23 };
constexpr int sword{ 24 };
constexpr int battle_axe{ 25 };
constexpr int katana{ 26 };
constexpr int khopesh{ 27 };
constexpr int scimitar{ 28 };
constexpr int halberd{ 29 };
constexpr int axe{ 30 };
constexpr int spear{ 31 };
constexpr int fire_staff{ 32 };
constexpr int water_staff{ 33 };
constexpr int staff_of_life{ 34 };
constexpr int life_potion{ 35 };
constexpr int mana_potion{ 36 };
constexpr int water_head{ 37 };
constexpr int fire_head{ 38 };

no::vector4f get_uv(int type);
std::string get_name(int type);

bool is_sword(int type);
bool is_staff(int type);
bool is_weapon(int type);
bool is_consumable(int type);
bool is_power(int type);

object_stats get_stats(int type);

}
