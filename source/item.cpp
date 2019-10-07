#include "item.hpp"

namespace item_type {

constexpr int items_per_row{ 8 };

no::vector4f get_uv(int type) {
	return {
		static_cast<float>((type % items_per_row) * 32), 
		static_cast<float>((type / items_per_row) * 32 + 96),
		32.0f,
		32.0f
	};
}

std::string get_name(int type) {
	switch (type) {
	case ring_of_pain: return "Ring of Pain";
	case ring_of_luck: return "Ring of Luck";
	case ring_of_strength: return "Ring of Strength";
	case bracelet_of_power: return "Bracelet of Power";
	case pendant_of_precision: return "Pendant of Precision";
	case pendant_of_thunder: return "Pendant of Thunder";
	case necklace_of_protection: return "Necklace of Protection";
	case necklace_of_stability: return "Necklace of Stability";
	case shoes_of_sneaking: return "Shoes of Sneaking";
	case fur_cape_of_the_beast: return "Fur Cape of the Beast";
	case band_of_patience: return "Band of Patience";
	case warriors_gloves: return "Warrior's Gloves";
	case gloves_of_greed: return "Gloves of Greed";
	case gloves_of_wind: return "Gloves of Wind";
	case shield_of_dark_magic: return "Shield of Dark Magic";
	case hat_of_fortune: return "Hat of Fortune";
	case eye_of_the_eagle: return "Eye of the Eagle";
	case gauntlets_of_success: return "Gauntlets of Success";
	case moon_tiara: return "Moon Tiara";
	case armor_of_neglect: return "Armor of Neglect";
	case talisman_of_protection: return "Talisman of Protection";
	case pendant_of_life: return "Pendant of Life";
	case helm_of_doom: return "Helm of Doom";
	case helm_of_hatred: return "Helm of Hatred";
	case sword: return "Sword";
	case battle_axe: return "Battle Axe";
	case katana: return "Katana";
	case khopesh: return "Khopesh";
	case scimitar: return "Scimitar";
	case halberd: return "Halberd";
	case axe: return "Axe";
	case spear: return "Spear";
	case fire_staff: return "Fire Staff";
	case water_staff: return "Water Staff";
	case staff_of_life: return "Staff of Life";
	case life_potion: return "Life Potion";
	case mana_potion: return "Mana Potion";
	case water_head: return "Head of the Water Beast";
	case fire_head: return "Head of the Fire Beast";
	default: return "Invalid Item";
	}
}

bool is_sword(int type) {
	return type == sword || type == battle_axe || type == katana || type == khopesh
		|| type == scimitar || type == halberd || type == axe || type == spear;
}

bool is_staff(int type) {
	return type == fire_staff || type == water_staff || type == staff_of_life;
}

bool is_weapon(int type) {
	return is_sword(type) || is_staff(type);
}

bool is_consumable(int type) {
	return type == life_potion || type == mana_potion;
}

bool is_power(int type) {
	return type == water_head || type == fire_head;
}

object_stats get_stats(int type) {
	object_stats stats;
	switch (type) {
	case ring_of_pain:
		stats.strength = 15.0f;
		break;
	case ring_of_luck:
		stats.critical_strike_chance = 0.2f;
		break;
	case ring_of_strength:
		stats.defense = 5.0f;
		stats.attack_speed = 0.2f;
		break;
	case bracelet_of_power:
		stats.strength = 12.0f;
		stats.defense = 5.0f;
		break;
	case pendant_of_precision:
		stats.critical_strike_chance = 0.3f;
		break;
	case pendant_of_thunder:
		stats.strength = 10.0f;
		stats.critical_strike_chance = 0.2f;
		break;
	case necklace_of_protection:
		stats.defense = 10.0f;
		break;
	case necklace_of_stability:
		stats.max_health = 30.0f;
		stats.max_mana = 20.0f;
		break;
	case shoes_of_sneaking:
		stats.move_speed = 0.5f;
		break;
	case fur_cape_of_the_beast:
		stats.strength = 9.0f;
		stats.defense = 4.0f;
		break;
	case band_of_patience:
		stats.health_regeneration_rate = 3.0f / 60.0f;
		stats.mana_regeneration_rate = 10.0f / 60.0f;
		break;
	case warriors_gloves:
		stats.strength = 5.0f;
		stats.attack_speed = 0.3f;
		break;
	case gloves_of_greed:
		stats.strength = 10.0f;
		stats.critical_strike_chance = 0.3f;
		break;
	case gloves_of_wind:
		stats.attack_speed = 0.5f;
		break;
	case shield_of_dark_magic:
		stats.defense = 10.0f;
		break;
	case hat_of_fortune:
		stats.critical_strike_chance = 0.3f;
		stats.health_regeneration_rate = 1.0f / 60.0f;
		break;
	case eye_of_the_eagle:
		stats.critical_strike_chance = 0.2f;
		stats.strength = 8.0f;
		break;
	case gauntlets_of_success:
		stats.critical_strike_chance = 0.2f;
		stats.attack_speed = 0.5f;
		break;
	case moon_tiara:
		stats.mana_regeneration_rate = 20.0f / 60.0f;
		break;
	case armor_of_neglect:
		stats.defense = 15.0f;
		break;
	case talisman_of_protection:
		stats.defense = 5.0f;
		break;
	case pendant_of_life:
		stats.max_health = 50.0f;
		stats.health_regeneration_rate = 3.0f / 60.0f;
		break;
	case helm_of_doom:
		stats.strength = 10.0f;
		stats.attack_speed = 0.3f;
		break;
	case helm_of_hatred:
		stats.defense = 5.0f;
		stats.health_regeneration_rate = 2.0f / 60.0f;
		break;
	case sword:
		stats.strength = 10.0f;
		stats.attack_speed = 1.0f;
		break;
	case battle_axe:
		stats.strength = 15.0f;
		stats.attack_speed = 1.5f;
		break;
	case katana:
		stats.strength = 12.0f;
		stats.attack_speed = 1.0f;
		break;
	case khopesh:
		stats.strength = 8.0f;
		stats.attack_speed = 1.2f;
		break;
	case scimitar:
		stats.strength = 18.0f;
		stats.attack_speed = 2.0f;
		break;
	case halberd:
		stats.strength = 25.0f;
		stats.attack_speed = 2.5f;
		break;
	case axe:
		stats.strength = 12.0f;
		stats.attack_speed = 1.9f;
		break;
	case spear:
		stats.strength = 14.0f;
		stats.attack_speed = 1.2f;
		break;
	case fire_staff:
		stats.strength = 50.0f;
		stats.attack_speed = 2.0f;
		stats.on_item_use.mana = -10.0f;
		break;
	case water_staff:
		stats.strength = 10.0f;
		stats.attack_speed = 0.5f;
		stats.on_item_use.mana = -10.0f;
		break;
	case staff_of_life:
		stats.attack_speed = 10.0f;
		stats.on_item_use.health = 50.0f;
		stats.on_item_use.mana = -100.0f;
		break;
	case life_potion:
		stats.on_item_use.health = 20.0f;
		break;
	case mana_potion:
		stats.on_item_use.mana = 100.0f;
		break;
	}
	return stats;
}

}
