#pragma once

#ifndef LEMONADE_GAME_SRC_RAW_MASTER_H
#define LEMONADE_GAME_SRC_RAW_MASTER_H

#include "snowy_database.h"
#include "color.h"

namespace RawMaster
{
	inline bool _raws_loading_allowed = false;

	template<typename T>
	using Ref = std::shared_ptr<const T>;

	using RawID = snowy::StringID;

	extern void load_raws();

	template<typename T>
	inline Ref<T> get(const RawID& id)
	{
		using TDB = snowy::SingletonDatabase<Ref<T>>;

		if (TDB::has(id))
		{
			return TDB::get(id);
		}

		CERR("Unrecognized " << typeid(T).name() << " ID: " << id);
		return nullptr;
	}

	template<typename T>
	inline Ref<T> add(const RawID& id, const T& object)
	{
		if (!_raws_loading_allowed)
		{
			CERR("No loading new raws allowed!");
			return get<T>(id);
		}

		using TDB = snowy::SingletonDatabase<Ref<T>>;

		if (TDB::has(id))
		{
			return TDB::get(id);
		}

		TDB::add(id, std::make_shared<T>(object));
		return TDB::get(id);
	}

	template<typename T>
	inline bool has(const RawID& id)
	{
		using TDB = snowy::SingletonDatabase<Ref<T>>;
		return TDB::has(id);
	}

	template<typename T>
	inline std::vector<RawID> get_ids()
	{
		using TDB = snowy::SingletonDatabase<Ref<T>>;
		return TDB::get_keys();
	}

	template<typename T>
	inline std::vector<Ref<T>> get_raws()
	{
		using TDB = snowy::SingletonDatabase<Ref<T>>;
		return TDB::get_values();
	}

	inline std::vector<RawID> get_color_ids()
	{
		return color::ColorsDB::get_keys();
	}
};

#endif // !LEMONADE_GAME_SRC_RAW_MASTER_H