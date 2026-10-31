#pragma once

#ifndef _SNOWY_DATABASE_H
#define _SNOWY_DATABASE_H

#include <unordered_map>
#include <vector>
#include <array>
#include <initializer_list>
#include "snowy_macros.h"
#include <string>
#include <iostream>
#include <iterator>

namespace snowy
{
	using StringID = std::string;

	template<typename K, typename T>
	class GenericDatabase
	{
		using KeyType = K;
		using DataType = T;

		using DataIterator = std::list<T>::iterator;
		using Const_DataIterator = std::list<T>::const_iterator;
		using Reverse_DataIterator = std::list<T>::reverse_iterator;
		using Const_Reverse_DataIterator = std::list<T>::const_reverse_iterator;

	private:
		std::unordered_map<KeyType, size_t> m_indexes;
		std::vector<DataType> m_data;

	public:
		constexpr GenericDatabase() = default;

		constexpr GenericDatabase(const std::initializer_list<std::tuple<KeyType, DataType>>& init)
		{
			m_data.reserve(init.size());
			for (const auto& [key, value] : init)
			{
				insert(key, value);
			}
		}

		constexpr ~GenericDatabase()
		{
		}

		constexpr bool empty() const { return m_data.size() < 1; }

		constexpr DataType* data() { return m_data.data(); }
		constexpr const DataType* data() const { return m_data.data(); }

		constexpr size_t size() const { return m_data.size(); }
		constexpr size_t capacity() const { return m_data.capacity(); }

		constexpr DataType& front() { return m_data.front(); }
		constexpr const DataType& front() const { return m_data.front(); }

		constexpr DataType& back() { return m_data.back(); }
		constexpr const DataType& back() const { return m_data.back(); }

		// Iterators //

		// Data Iterators

		constexpr DataIterator begin() { return m_data.begin(); }
		constexpr Const_DataIterator begin() const { return m_data.begin(); }

		constexpr Reverse_DataIterator rbegin() { return m_data.rbegin(); }
		constexpr Const_Reverse_DataIterator rbegin() const { return m_data.rbegin(); }

		constexpr Const_DataIterator cbegin() const { return m_data.cbegin(); }
		constexpr Const_Reverse_DataIterator crbegin() const { return m_data.crbegin(); }

		constexpr DataIterator end() { return m_data.end(); }
		constexpr Const_DataIterator end() const { return m_data.end(); }

		constexpr Reverse_DataIterator rend() { return m_data.rend(); }
		constexpr Const_Reverse_DataIterator rend() const { return m_data.rend(); }

		constexpr Const_DataIterator cend() const { return m_data.cend(); }
		constexpr Const_Reverse_DataIterator crend() const { return m_data.crend(); }

		// Indexing //

		constexpr DataType& at(size_t idx) { ASSERT(idx < size(), "database subscript out of range");  return m_data.at(idx); }
		constexpr const DataType& at(size_t idx) const { ASSERT(idx < size(), "database subscript out of range");  return m_data.at(idx); }

		constexpr DataType& at(const KeyType& key) { ASSERT(m_indexes.contains(key), "database key does not exist");  return get_or_insert(key); }
		constexpr DataType& at(KeyType&& key) { ASSERT(m_indexes.contains(key), "database key does not exist");  return get_or_insert(key); }
		constexpr const DataType& at(const KeyType& key) const { ASSERT(m_indexes.contains(key), "database key does not exist"); return get_or_insert(key); }

		constexpr DataType& operator[](size_t idx) { DEBUG_ASSERT(idx < size(), "database subscript out of range");  return m_data[idx]; }
		constexpr const DataType& operator[](size_t idx) const { DEBUG_ASSERT(idx < size(), "database subscript out of range");  return m_data[idx]; }

		constexpr DataType& operator[](const KeyType& key) { return get_or_insert(key); }
		constexpr DataType& operator[](KeyType&& key) { return get_or_insert(key); }
		constexpr const DataType& operator[](const KeyType& key) const { return get_or_insert(key); }

		// Misc //

		constexpr void erase(const DataType& data)
		{
			m_indexes.erase(find_key(data));
			m_data.erase(m_data.begin() + find(data));
		}

		constexpr void remove(const KeyType& key)
		{
			m_data.erase(m_data.begin() + m_indexes[key]);
			m_indexes.erase(key);
		}

		constexpr void insert(const KeyType& key, const DataType& data) { get_or_insert(key) = data; }
		constexpr void clear() { m_data.clear(); m_indexes.clear(); }
		constexpr bool contains(const KeyType& key) const { return m_indexes.contains(key); }

		constexpr void reserve(size_t reserve_capacity) { m_data.reserve(reserve_capacity); }
		constexpr void resize(size_t new_size) { m_data.resize(new_size); }
		constexpr void shrink_to_fit() { m_data.shrink_to_fit(); }

		constexpr size_t find(const DataType& data) const
		{
			for (size_t i = 0; i < m_data.size(); i++)
				if (m_data[i] == data) return i;

			return m_data.size();
		}

		constexpr KeyType find_key(const DataType& data) const
		{
			size_t data_index = find(data);
			for (const auto& [key, index] : m_indexes)
				if (index == data_index) return key;

			return KeyType{};
		}

		constexpr void print() const
		{
			for (const auto& data : m_data)
				std::cout << find_key(data) << ": " << data << std::endl;
		}

	private:

		constexpr DataType& get_or_insert(const KeyType& key, const DataType& data = DataType{})
		{
			if (m_indexes.count(key) == 0)
			{
				m_indexes[key] = m_data.size();
				m_data.push_back(data);
			}
			return m_data[m_indexes[key]];
		}

		constexpr const DataType get_or_insert(const KeyType& key, const DataType& data = DataType{}) const
		{
			ASSERT(m_indexes.count(key) > 0, "database index out of bounds!");
			return m_data.at(m_indexes.at(key));
		}
	};

	template<typename T>
	using Database = GenericDatabase<StringID, T>;

	template<typename K, typename T, size_t Size>
	class StaticGenericDatabase
	{
		using KeyType = K;
		using DataType = T;

		using DataIterator = std::array<T, Size>::iterator;
		using Const_DataIterator = std::array<T, Size>::const_iterator;
		using Reverse_DataIterator = std::array<T, Size>::reverse_iterator;
		using Const_Reverse_DataIterator = std::array<T, Size>::const_reverse_iterator;

	private:
		std::unordered_map<KeyType, size_t> m_indexes;
		std::array<DataType, Size> m_data{};
		size_t current_index{ 0 };

	public:
		constexpr explicit StaticGenericDatabase() = default;

		constexpr StaticGenericDatabase(const std::initializer_list<std::tuple<K, T>>& init)
		{
			for (const auto& [key, value] : init)
			{
				if (current_index >= Size) break;
				insert(key, value);
			}
		}

		constexpr ~StaticGenericDatabase()
		{
		}

		constexpr bool empty() const { return m_data.size() < 1; }

		constexpr DataType* data() { return m_data.data(); }
		constexpr const DataType* data() const { return m_data.data(); }

		constexpr size_t size() const { return m_data.size(); }

		constexpr DataType& front() { return m_data.front(); }
		constexpr const DataType& front() const { return m_data.front(); }

		constexpr DataType& back() { return m_data.back(); }
		constexpr const DataType& back() const { return m_data.back(); }

		// Iterators //

		// Data Iterators

		constexpr DataIterator begin() { return m_data.begin(); }
		constexpr Const_DataIterator begin() const { return m_data.begin(); }

		constexpr Reverse_DataIterator rbegin() { return m_data.rbegin(); }
		constexpr Const_Reverse_DataIterator rbegin() const { return m_data.rbegin(); }

		constexpr Const_DataIterator cbegin() const { return m_data.cbegin(); }
		constexpr Const_Reverse_DataIterator crbegin() const { return m_data.crbegin(); }

		constexpr DataIterator end() { return m_data.end(); }
		constexpr Const_DataIterator end() const { return m_data.end(); }

		constexpr Reverse_DataIterator rend() { return m_data.rend(); }
		constexpr Const_Reverse_DataIterator rend() const { return m_data.rend(); }

		constexpr Const_DataIterator cend() const { return m_data.cend(); }
		constexpr Const_Reverse_DataIterator crend() const { return m_data.crend(); }

		// Indexing //

		constexpr DataType& at(size_t idx) { ASSERT(idx < Size, "static database subscript out of range");  return m_data.at(idx); }
		constexpr const DataType& at(size_t idx) const { ASSERT(idx < Size, "static database subscript out of range");  return m_data.at(idx); }

		constexpr DataType& at(const KeyType& key) { ASSERT(m_indexes.contains(key), "static database key does not exist");  return get_or_insert(key); }
		constexpr DataType& at(KeyType&& key) { ASSERT(m_indexes.contains(key), "static database key does not exist");  return get_or_insert(key); }
		constexpr const DataType& at(const KeyType& key) const { ASSERT(m_indexes.contains(key), "static database key does not exist"); return get_or_insert(key); }

		constexpr DataType& operator[](size_t idx) { DEBUG_ASSERT(idx < Size, "static database subscript out of range");  return m_data[idx]; }
		constexpr const DataType& operator[](size_t idx) const { DEBUG_ASSERT(idx < Size, "static database subscript out of range");  return m_data[idx]; }

		constexpr DataType& operator[](const KeyType& key) { return get_or_insert(key); }
		constexpr DataType& operator[](KeyType&& key) { return get_or_insert(key); }
		constexpr const DataType& operator[](const KeyType& key) const { return get_or_insert(key); }

		// Misc //

		constexpr void erase(const DataType& data)
		{
			m_indexes.erase(find_key(data));
			m_data.erase(find(data));
		}

		constexpr void remove(const KeyType& key)
		{
			m_data.erase(find(m_indexes[key]));
			m_indexes.erase(key);
		}

		constexpr void insert(const KeyType& key, const DataType& data) { get_or_insert(key) = data; }
		constexpr void clear() { m_data.fill(DataType{}); m_indexes.clear(); }
		constexpr bool contains(const KeyType& key) const { return m_indexes.contains(key); }
		constexpr bool contains(const DataType& data) const { return find(data) < size(); }

		constexpr size_t find(const DataType& data) const
		{
			for (size_t i = 0; i < m_data.size(); i++)
				if (m_data[i] == data) return i;

			return size();
		}

		constexpr KeyType find_key(const DataType& data) const
		{
			size_t data_index = find(data);
			for (const auto& [key, index] : m_indexes)
				if (index == data_index) return key;

			return KeyType{};
		}

		constexpr void print() const
		{
			for (const auto& data : m_data)
				std::cout << find_key(data) << ": " << data << std::endl;
		}

	private:

		constexpr DataType& get_or_insert(const KeyType& key, const DataType& data = DataType{})
		{
			if (m_indexes.count(key) == 0)
			{
				ASSERT(current_index < Size, "static database index out of bounds!");

				m_indexes[key] = current_index;
				m_data[current_index] = data;

				current_index++;
			}
			return m_data[m_indexes[key]];
		}

		constexpr const DataType get_or_insert(const KeyType& key, const DataType& data = DataType{}) const
		{
			ASSERT(m_indexes.count(key) > 0, "static database index out of bounds!");
			return m_data.at(m_indexes.at(key));
		}
	};

	template<typename T, size_t Size>
	using StaticDatabase = StaticGenericDatabase<StringID, T, Size>;

	template<typename K, typename T>
	class GenericSingletonDatabase
	{
		using KeyType = K;
		using DataType = T;

		using GenericSingletonDatabaseType = GenericSingletonDatabase<K, T>;
		using MapType = std::unordered_map<K, T>;

	private:
		MapType m_data;

		GenericSingletonDatabase() {}
		~GenericSingletonDatabase() {}

		static GenericSingletonDatabaseType& get()
		{
			static GenericSingletonDatabaseType singleton;
			return singleton;
		}

	public:

		GenericSingletonDatabase(GenericSingletonDatabaseType const&) = delete;
		void operator=(GenericSingletonDatabaseType const&) = delete;

		constexpr static void init()
		{
			get();
		}

		constexpr static bool has(const KeyType& id)
		{
			return get().m_data.contains(id);
		}

		constexpr static DataType& get(const KeyType& id)
		{
			return get().m_data[id];
		}

		constexpr static void add(const KeyType& id, DataType data)
		{
			get().m_data.insert({ id, std::move(data) });
		}

		constexpr static void erase(const KeyType& id)
		{
			get().m_data.erase(id);
		}

		constexpr static void add(const std::initializer_list<std::tuple<KeyType, DataType>>& init)
		{
			for (const auto& [id, data] : init)
			{
				get().m_data.insert({ id, std::move(data) });
			}
		}

		constexpr static std::vector<K> get_keys()
		{
			std::vector<K> keys;
			keys.reserve(get().m_data.size());

			for (const auto& [id, data] : get().m_data)
			{
				keys.push_back(id);
			}

			return keys;
		}

		constexpr static std::vector<T> get_values()
		{
			std::vector<T> values;
			values.reserve(get().m_data.size());

			for (const auto& [id, data] : get().m_data)
			{
				values.push_back(data);
			}

			return values;
		}
	};

	template<typename T>
	using SingletonDatabase = GenericSingletonDatabase<StringID, T>;
}

#endif // !_SNOWY_DATABASE_H
