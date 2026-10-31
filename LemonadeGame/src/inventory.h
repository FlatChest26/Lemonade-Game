#pragma once

#ifndef LEMONADE_GAME_SRC_INVENTORY_H
#define LEMONADE_GAME_SRC_INVENTORY_H

#include <vector>

class Entity;


struct ItemSlot
{
	Entity* item;
	bool filled() const { return item != nullptr; }
};

class Inventory
{
protected:
	Entity* m_owner{ nullptr };
	size_t m_slots_taken{ 0 };

	std::vector<ItemSlot> m_item_slots;

public:
	Inventory(Entity* owner = {}, size_t max_slots = 1) :
		m_owner(owner)
	{
		m_item_slots.resize(max_slots);
	}

	~Inventory();

	Entity* get_owner() const { return m_owner; }

	std::vector<ItemSlot> get_item_slots() const { return m_item_slots; }
	std::vector<ItemSlot> get_filled_item_slots() const;
	std::vector<Entity*> get_items() const;

	size_t get_max_slots() const { return m_item_slots.size(); }
	void set_max_slots(size_t new_max) { m_item_slots.resize(new_max); }

	size_t get_slots_taken() const { return m_slots_taken; }

	bool empty() const { return m_slots_taken < 1; }

	ItemSlot& get_item_slot(size_t slot);
	const ItemSlot& get_item_slot(size_t slot) const;

	bool is_item_slot_filled(size_t slot) const;

	Entity* get_item(size_t slot);
	const Entity* get_item(size_t slot) const;

	bool has_item(Entity* item) const;

	bool add_item(Entity* item);
	bool remove_item(Entity* item);
	
	void clear_items();

	bool can_carry(Entity* item) const;

	bool is_full() const { return m_slots_taken == m_item_slots.size(); }
};

#endif // !LEMONADE_GAME_SRC_INVENTORY_H