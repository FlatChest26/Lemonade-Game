#include "inventory.h"
#include "entity.h"

Inventory::~Inventory()
{
	clear_items();
}

std::vector<ItemSlot> Inventory::get_filled_item_slots() const
{
	std::vector<ItemSlot> filled_item_slots;

	std::copy_if(
		m_item_slots.begin(), m_item_slots.end(), std::back_inserter(filled_item_slots),
		[](const ItemSlot& item_slot)
		{
			return item_slot.filled();
		}
	);

	return filled_item_slots;
}

std::vector<Entity*> Inventory::get_items() const
{
	std::vector<Entity*> items;

	for (auto& item_slot : get_filled_item_slots())
	{
		items.push_back(item_slot.item);
	}

	return items;
}


ItemSlot& Inventory::get_item_slot(size_t slot)
{
	if (slot < 0 || slot >= m_item_slots.size())
	{
		CERR("Invalid inventory slot!");
	}

	return m_item_slots.at(slot);
}

const ItemSlot& Inventory::get_item_slot(size_t slot) const
{
	if (slot < 0 || slot >= m_item_slots.size())
	{
		CERR("Invalid inventory slot!");
	}

	return m_item_slots.at(slot);
}

bool Inventory::is_item_slot_filled(size_t slot) const
{
	return get_item_slot(slot).filled();
}

bool Inventory::has_item(Entity* item) const
{
	return std::any_of(m_item_slots.begin(), m_item_slots.end(),
		[&](const ItemSlot& item_slot)
		{
			return item_slot.item == item;
		}
	);
}

Entity* Inventory::get_item(size_t slot)
{
	return get_item_slot(slot).item;
}

const Entity* Inventory::get_item(size_t slot) const
{
	return get_item_slot(slot).item;
}

bool Inventory::add_item(Entity* item)
{
	if (!can_carry(item))
		return false;

	auto carry_slots = item->carry_slots();

	for (auto carry_slots = item->carry_slots(); carry_slots > 0; carry_slots--)
	{
		m_item_slots[m_slots_taken].item = item;
		m_slots_taken += 1;
	}

	return true;
}

bool Inventory::remove_item(Entity* item)
{
	if (!has_item(item))
		return false;

	for (auto& item_slot : m_item_slots)
	{
		if (item_slot.item == item)
		{
			item_slot.item = nullptr;
			m_slots_taken -= 1;
		}
	}

	return false;
}

bool Inventory::can_carry(Entity* item) const
{
	if (!item)
		return false;

	if (!item->can_be_held())
		return false;

	if (m_slots_taken + item->carry_slots() > get_max_slots())
		return false;

	return true;
}

void Inventory::clear_items()
{
	for (auto& item_slot : get_filled_item_slots())
	{
		if (!item_slot.filled())
			continue;

		if (auto holder = item_slot.item->get_entity_holder())
			holder->remove_from_inventory(item_slot.item);

		m_slots_taken -= item_slot.item->carry_slots();
		item_slot.item = nullptr;
	}
}