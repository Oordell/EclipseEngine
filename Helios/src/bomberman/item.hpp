#pragma once

#include <Eclipse.h>
#include "common_types.hpp"

namespace bomberman {

enum class ItemState { idle, taken, exploded };

class Item : public EntityInterface {
public:
	explicit Item(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	              const eclipse::ref<eclipse::Scene>& context, const SubTextureIndex& texture_sheet_coordinates);

	void on_update(au::QuantityF<au::Seconds> timestep) override {}

	void destroy() override {
		entity_.remove_component<eclipse::component::SubTexture>();
		context_->destroy_entity(entity_);
	}

	void on_bomb_ray_hit() override { state_ = ItemState::exploded; }

	[[nodiscard]] bool has_expired() const override { return state_ != ItemState::idle; }

	[[nodiscard]] const eclipse::Point2D& get_position() const override { return position_; }

protected:
	void update_texture() override {};
	ItemState state_ {ItemState::idle};

private:
	void create_entity();

	eclipse::Point2D position_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::Scene> context_;
	SubTextureIndex texture_sheet_coordinates_;
	eclipse::Entity entity_;
	eclipse::ref<eclipse::SubTexture2D> texture_item_;
};

class ItemBombIncrement : public Item {
public:
	explicit class ItemBombIncrement(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                                 const eclipse::ref<eclipse::Scene>& context)
	    : Item(texture_sheet, position, context, {.x = eclipse::units::pixels(22), .y = eclipse::units::pixels(10)}) {}

	CellContent on_player_interaction() override {
		state_ = ItemState::taken;
		return CellContent::item_bomb_increment;
	}
};

class ItemBombReach : public Item {
public:
	explicit class ItemBombReach(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                             const eclipse::ref<eclipse::Scene>& context)
	    : Item(texture_sheet, position, context, {.x = eclipse::units::pixels(25), .y = eclipse::units::pixels(10)}) {}

	CellContent on_player_interaction() override {
		state_ = ItemState::taken;
		return CellContent::item_bomb_reach;
	}
};

class ItemSpeedBoost : public Item {
public:
	explicit class ItemSpeedBoost(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                              const eclipse::ref<eclipse::Scene>& context)
	    : Item(texture_sheet, position, context, {.x = eclipse::units::pixels(27), .y = eclipse::units::pixels(9)}) {}

	CellContent on_player_interaction() override {
		state_ = ItemState::taken;
		return CellContent::item_speed_boost;
	}
};

class ItemInvincibility : public Item {
public:
	explicit class ItemInvincibility(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                                 const eclipse::ref<eclipse::Scene>& context)
	    : Item(texture_sheet, position, context, {.x = eclipse::units::pixels(26), .y = eclipse::units::pixels(10)}) {}

	CellContent on_player_interaction() override {
		state_ = ItemState::taken;
		return CellContent::item_invincibility;
	}
};

}  // namespace bomberman