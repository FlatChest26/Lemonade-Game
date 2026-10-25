#pragma once

#ifndef LEMONADE_GAME_SRC_INVENTORY_H
#define LEMONADE_GAME_SRC_INVENTORY_H

#include <vector>

class Entity;

class Inventory
{
protected:
	Entity* m_owner{ };
	size_t m_max_slots{ 1 };

	std::vector<Entity*> m_items;

public:
	Inventory(Entity* owner = {}, size_t max_slots = 1, std::vector<Entity*> items = {}) :
		m_owner(owner), m_items(items), m_max_slots(max_slots)
	{}

	~Inventory();

	Entity* get_owner() const { return m_owner; }
	std::vector<Entity*> get_items() const { return m_items; }

	size_t get_max_slots() const { return m_max_slots; }
	void set_max_slots(size_t new_max) { m_max_slots = new_max; }

	size_t get_item_count() const { return m_items.size(); }
	bool empty() const { return m_items.empty(); }
	bool has_item(Entity* item) const;

	Entity* get_item(int slot);
	const Entity* get_item(int slot) const { return get_item(slot); }

	size_t get_item_slot(Entity* item) const;
	bool add_item(Entity* item);
	bool remove_item(Entity* item);
	void clear() { m_items.clear(); }
};

#endif // !LEMONADE_GAME_SRC_INVENTORY_H