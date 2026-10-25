#include "inventory.h"
#include "entity.h"

Inventory::~Inventory()
{
	if (auto owner = get_owner())
	{
		for (auto& item : m_items)
		{
			owner->remove_from_inventory(item);
		}
	}
}

bool Inventory::has_item(Entity* item) const
{
	return std::find_if(m_items.begin(), m_items.end(), [&item](Entity* other) { return item == other; }) != m_items.end();
}

Entity* Inventory::get_item(int slot)
{
	if (slot < 0 || slot >= m_items.size())
		return nullptr;

	return m_items[size_t(slot)];
}

size_t Inventory::get_item_slot(Entity* item) const
{
	return size_t(std::find_if(m_items.begin(), m_items.end(), [&item](Entity* other) { return item == other; }) - m_items.begin());
}

bool Inventory::add_item(Entity* item)
{
	if (m_items.size() >= m_max_slots)
		return false;

	m_items.push_back(item);

	return true;
}

bool Inventory::remove_item(Entity* item)
{
	auto it = std::find(m_items.begin(), m_items.end(), item);

	if (it != m_items.end())
	{
		m_items.erase(it);
		return true;
	}

	return false;
}