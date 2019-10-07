#include "game_ui.hpp"
#include "game.hpp"
#include "window.hpp"
#include "assets.hpp"
#include "item.hpp"

namespace uv {

constexpr no::vector2f sheet_size{ 256.0f };

constexpr no::vector4f overlay{ 0.0f, 0.0f, 192.0f, 32.0f };
constexpr no::vector4f bar_full{ 49.0f, 6.0f, 1.0f, 7.0f };
constexpr no::vector4f bar_empty{ 49.0f, 19.0f, 1.0f, 7.0f };
constexpr no::vector4f slot{ 192.0f, 0.0f, 32.0f, 32.0f };
constexpr no::vector4f active_slot{ 224.0f, 0.0f, 32.0f, 32.0f };

constexpr no::vector4f active_numbers[]{
	{ 0.0f, 32.0f, 32.0f, 12.0f },
	{ 32.0f, 32.0f, 32.0f, 12.0f },
	{ 64.0f, 32.0f, 32.0f, 12.0f },
	{ 96.0f, 32.0f, 32.0f, 12.0f },
	{ 128.0f, 32.0f, 32.0f, 12.0f },
	{ 160.0f, 32.0f, 32.0f, 12.0f },
	{ 192.0f, 32.0f, 32.0f, 12.0f },
	{ 224.0f, 32.0f, 32.0f, 12.0f }
};

constexpr no::vector4f numbers[]{
	{ 0.0f, 64.0f, 32.0f, 12.0f },
	{ 32.0f, 64.0f, 32.0f, 12.0f },
	{ 64.0f, 64.0f, 32.0f, 12.0f },
	{ 96.0f, 64.0f, 32.0f, 12.0f },
	{ 128.0f, 64.0f, 32.0f, 12.0f },
	{ 160.0f, 64.0f, 32.0f, 12.0f },
	{ 192.0f, 64.0f, 32.0f, 12.0f },
	{ 224.0f, 64.0f, 32.0f, 12.0f }
};

void set(no::rectangle& rectangle, const no::vector4f& uv) {
	rectangle.set_tex_coords(uv.x / sheet_size.x, uv.y / sheet_size.y, uv.z / sheet_size.x, uv.w / sheet_size.y);
}

}

game_ui::game_ui(game_state& game) : game{ game }, 
	stat_strength{ game, text_camera }, stat_attack_speed{ game, text_camera }, stat_critical{ game, text_camera },
	stat_defense{ game, text_camera }, stat_speed{ game, text_camera }, stat_regen_health{ game, text_camera },
	weapon_text{ game, text_camera }, chest_item_name{ game, text_camera }, chest_message{ game, text_camera } {
	ui_texture = no::require_texture("ui");
	font = no::require_font("leo", 16);
	critical_texture = no::create_texture(font->render("!", 0x000000FF));
	key_press = game.keyboard().press.listen([&](no::key key) {
		if (!chest_ui.open) {
			return;
		}
		if (key == no::key::escape) {
			chest_ui.open = false;
			game.world.player.locked_by_ui = false;
		} else if (key == no::key::space) {
			if (item_type::is_weapon(chest_ui.item)) {
				game.world.player.give_item(chest_ui.item, 0);
				chest_ui.open = false;
				game.world.player.locked_by_ui = false;
			} else {
				for (int i{ 2 }; i < 8; i++) {
					if (game.world.player.item_in_slot(i) < 0) {
						game.world.player.give_item(chest_ui.item, i);
						chest_ui.open = false;
						game.world.player.locked_by_ui = false;
						break;
					}
				}
			}
		} else if (key == no::key::num_3) {
			game.world.player.give_item(chest_ui.item, 2);
			chest_ui.open = false;
			game.world.player.locked_by_ui = false;
		} else if (key == no::key::num_4) {
			game.world.player.give_item(chest_ui.item, 3);
			chest_ui.open = false;
			game.world.player.locked_by_ui = false;
		} else if (key == no::key::num_5) {
			game.world.player.give_item(chest_ui.item, 4);
			chest_ui.open = false;
			game.world.player.locked_by_ui = false;
		} else if (key == no::key::num_6) {
			game.world.player.give_item(chest_ui.item, 5);
			chest_ui.open = false;
			game.world.player.locked_by_ui = false;
		} else if (key == no::key::num_7) {
			game.world.player.give_item(chest_ui.item, 6);
			chest_ui.open = false;
			game.world.player.locked_by_ui = false;
		} else if (key == no::key::num_8) {
			game.world.player.give_item(chest_ui.item, 7);
			chest_ui.open = false;
			game.world.player.locked_by_ui = false;
		}
	});
}

game_ui::~game_ui() {
	no::delete_texture(critical_texture);
	splats.clear();
	no::release_texture("ui");
	no::release_font("leo", 16);
}

void game_ui::update() {
	camera.zoom = game.zoom;
	camera.transform.scale = game.window().size().to<float>();
	text_camera.transform.scale = game.window().size().to<float>();
	uint32_t color{ 0x00000000 };
	const auto stats{ game.world.player.final_stats() };
	stat_strength.render(*font, std::to_string(static_cast<int>(stats.strength)), color);
	stat_attack_speed.render(*font, std::to_string(static_cast<int>(stats.attack_speed)), color);
	stat_critical.render(*font, std::to_string(static_cast<int>(stats.critical_strike_chance * 100.0f)) + "%", color);
	stat_defense.render(*font, std::to_string(static_cast<int>(stats.defense)), color);
	stat_speed.render(*font, std::to_string(static_cast<int>(stats.move_speed)), color);
	stat_regen_health.render(*font, std::to_string(static_cast<int>(stats.health_regeneration_rate * 60.0f)) + "/s", color);
	if (game.world.player.equipped_weapon() >= 0) {
		weapon_text.render(*font, item_type::get_name(game.world.player.equipped_weapon()));
	}
	update_hit_splats();
	if (chest_ui.open) {
		chest_item_name.render(*font, item_type::get_name(chest_ui.item));
		if (game.world.player.has_empty_slot() || item_type::is_weapon(chest_ui.item)) {
			chest_message.render(*font, "Press 'Space' to take this item.\n\nAlternatively, press 'Escape' to close.");
		} else {
			chest_message.render(*font, "Please press the digit of the slot to assign this item to.\nThe other item will be lost.\n\nAlternatively, press 'Escape' to close.");
		}
	}
}

void game_ui::draw() {
	auto& player{ game.world.player };

	no::set_shader_view_projection(camera);
	no::get_shader_variable("color").set(no::vector4f{ 1.0f });
	no::bind_texture(ui_texture);

	// draw overlay
	uv::set(rectangle, uv::overlay);
	no::draw_shape(rectangle, overlay_transform());

	const auto player_stats{ player.final_stats() };

	// draw health
	if (player.stats.max_health > 1.0f) {
		no::transform2 health_bar;
		health_bar.position = overlay_transform().position + uv::bar_full.xy;
		health_bar.scale = { 43.0f, 7.0f };
		uv::set(rectangle, uv::bar_empty);
		no::draw_shape(rectangle, health_bar);
		health_bar.scale.x *= player_stats.health / player_stats.max_health;
		uv::set(rectangle, uv::bar_full);
		no::draw_shape(rectangle, health_bar);
	}

	// draw mana
	if (player.stats.max_mana > 1.0f) {
		no::transform2 mana_bar;
		mana_bar.position = overlay_transform().position + uv::bar_empty.xy;
		mana_bar.scale = { 43.0f, 7.0f };
		uv::set(rectangle, uv::bar_empty);
		no::draw_shape(rectangle, mana_bar);
		mana_bar.scale.x *= player_stats.mana / player_stats.max_mana;
		uv::set(rectangle, uv::bar_full);
		no::draw_shape(rectangle, mana_bar);
	}

	// draw items
	for (int i = 0; i < 2; i++) {
		no::transform2 transform;
		transform.scale = 32.0f;
		transform.position = overlay_transform().position;
		transform.position.x += 224.0f;
		transform.position.x += static_cast<float>(i) * 32.0f;
		const auto item_transform{ transform };
		uv::set(rectangle, uv::active_slot);
		no::draw_shape(rectangle, transform);
		transform.position.y += 32.0f;
		transform.scale = uv::numbers[i].zw;
		uv::set(rectangle, uv::numbers[i]);
		no::draw_shape(rectangle, transform);
		// draw item
		uv::set(rectangle, item_type::get_uv(player.item_in_slot(i)));
		no::draw_shape(rectangle, item_transform);
	}
	for (int i = 2; i < 8; i++) {
		no::transform2 transform;
		transform.scale = 32.0f;
		transform.position = overlay_transform().position;
		transform.position.x += 256.0f;
		transform.position.x += static_cast<float>(i) * 32.0f;
		const auto item_transform{ transform };
		uv::set(rectangle, uv::slot);
		no::draw_shape(rectangle, transform);
		transform.position.y += 32.0f;
		transform.scale = uv::numbers[i].zw;
		uv::set(rectangle, uv::numbers[i]);
		no::draw_shape(rectangle, transform);
		// draw item
		uv::set(rectangle, item_type::get_uv(player.item_in_slot(i)));
		no::draw_shape(rectangle, item_transform);
	}

	// draw weapon icon, bottom right
	if (player.equipped_weapon() >= 0) {
		no::transform2 weapon_transform;
		weapon_transform.position = camera.size() - 96.0f;
		weapon_transform.scale = 64.0f;
		uv::set(rectangle, item_type::get_uv(player.equipped_weapon()));
		no::draw_shape(rectangle, weapon_transform);
		weapon_text.transform.position = weapon_transform.position;
		weapon_text.transform.position.x += weapon_transform.scale.x / 2.0f - weapon_text.transform.scale.x / 2.0f;
		weapon_text.transform.position.y += weapon_transform.scale.y;
		weapon_text.draw(static_rectangle);
	}

	// draw chest ui
	if (chest_ui.open) {
		no::transform2 chest_ui_transform;
		chest_ui_transform.scale = { 320.0f, 192.0f };
		chest_ui_transform.position = camera.size() / 2.0f - chest_ui_transform.scale / 2.0f;
		// draw background
		no::bind_texture(game.renderer.blank_texture);
		no::get_shader_variable("color").set(no::vector4f{ 0.4f, 0.4f, 0.4f, 0.8f });
		no::draw_shape(static_rectangle, chest_ui_transform);
		no::get_shader_variable("color").set(no::vector4f{ 1.0f });
		// draw item
		no::bind_texture(ui_texture);
		uv::set(rectangle, item_type::get_uv(chest_ui.item));
		no::transform2 item_transform;
		item_transform.position = chest_ui_transform.position + 16.0f;
		item_transform.scale = 32.0f;
		no::draw_shape(rectangle, item_transform);
		chest_item_name.transform.position = { chest_ui_transform.position.x + 64.0f, chest_ui_transform.position.y + 32.0f };
		chest_item_name.draw(static_rectangle);

		no::set_shader_view_projection(text_camera);
		chest_message.transform.position = { chest_ui_transform.position.x + 64.0f, chest_ui_transform.position.y + 64.0f };
		chest_message.transform.position *= game.zoom;
		chest_message.draw(static_rectangle);
	}

	// draw stat numbers
	no::set_shader_view_projection(text_camera);
	no::vector2f stat;
	stat = { 114.0f, 8.0f };
	stat += overlay_transform().position;
	stat_strength.transform.position = stat * camera.zoom;
	stat_strength.draw(static_rectangle);
	stat.x += 32.0f;
	stat_attack_speed.transform.position = stat * camera.zoom;
	stat_attack_speed.draw(static_rectangle);
	stat.x += 32.0f;
	stat_critical.transform.position = stat * camera.zoom;
	stat_critical.draw(static_rectangle);
	stat = { 114.0f, 22.0f };
	stat += overlay_transform().position;
	stat_defense.transform.position = stat * camera.zoom;
	stat_defense.draw(static_rectangle);
	stat.x += 32.0f;
	stat_speed.transform.position = stat * camera.zoom;
	stat_speed.draw(static_rectangle);
	stat.x += 32.0f;
	stat_regen_health.transform.position = stat * camera.zoom;
	stat_regen_health.draw(static_rectangle);

	no::set_shader_view_projection(game.renderer.camera);
	draw_hit_splats();
}

void game_ui::on_chest_open(int item) {
	game.world.player.locked_by_ui = true;
	if (game.world.player.equipped_weapon() < 0) {
		if (game.world.random.chance(0.5f)) {
			item = game.world.random.next<int>(24, 34);
		}
	}
	chest_ui.item = item;
	chest_ui.open = true;

}

no::transform2 game_ui::overlay_transform() const {
	no::transform2 transform;
	transform.position = 16.0f;
	transform.scale = uv::overlay.zw;
	return transform;
}

void game_ui::add_hit_splat(int target_id) {
	splats.emplace_back().target_id = target_id;
}

void game_ui::update_hit_splats() {
	for (int i{ 0 }; i < static_cast<int>(splats.size()); i++) {
		auto& splat{ splats[i] };
		if (splat.fade_in < 1.0f) {
			splat.fade_in += 0.04f;
			splat.alpha = splat.fade_in;
		} else if (splat.stay < 1.0f) {
			splat.stay += 0.05f;
			splat.alpha = 1.0f;
		} else if (splat.fade_out < 1.0f) {
			splat.fade_out += 0.05f;
			splat.alpha = 1.0f - splat.fade_out;
		}
		if (splat.target_id != -1) {
			if (auto target{ game.world.player.room->object_with_id(splat.target_id) }) {
				no::vector2f position = target->transform.position;
				splat.transform.position = position + 16.0f;
				splat.transform.position.y -= (splat.fade_in * 0.5f + splat.fade_out) * 32.0f;
			}
		}
		splat.transform.scale = no::texture_size(critical_texture).to<float>();
		if (!splat.is_visible()) {
			splats.erase(splats.begin() + i);
			i--;
		}
	}
}

void game_ui::draw_hit_splats() {
	auto color{ no::get_shader_variable("color") };
	for (auto& splat : splats) {
		auto shadow{ splat.transform };
		shadow.position += 1.0f;
		no::bind_texture(critical_texture);
		color.set({ 0.0f, 0.0f, 0.0f, splat.alpha });
		no::draw_shape(static_rectangle, shadow);
		color.set({ 1.0f, 1.0f, 1.0f, splat.alpha });
		no::draw_shape(static_rectangle, splat.transform);
	}
}
