#pragma once

#ifndef LEMONADE_GAME_SRC_RAW_MASTER_H
#define LEMONADE_GAME_SRC_RAW_MASTER_H

#include "snowy_database.h"
#include "color.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace RawMaster
{
	inline bool _raws_loading_allowed = false;

	template<typename T>
	using Ref = const T*;

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

		TDB::add(id, new T(object));
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

	template <typename T, typename Arg>
	concept HasFromJson = requires(Arg && arg)
	{
		{ T::from_json(std::forward<Arg>(arg)) } -> std::same_as<T>;
	};

	template<typename T>
	inline Ref<T> make_ref(json obj)
	{
		if constexpr (HasFromJson<T, json>)
		{
			return new T(T::from_json(obj));
		}

		CERR("Input type has no `from_json` method!" << obj);
		return nullptr;
	}
};

#endif // !LEMONADE_GAME_SRC_RAW_MASTER_H