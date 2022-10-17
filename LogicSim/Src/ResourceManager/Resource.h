#pragma once

#include <cstdint>

#include <type_traits>
#include <utility>

namespace ResourceManager
{
	template <class T, class IndexType = std::size_t>
	struct Resource
	{
	public:
		using IndexT = IndexType;

	public:
		template <class... Args>
		Resource(IndexT index, Args&&... args) requires std::is_constructible_v<T, Args...>;
		Resource(Resource&& move) noexcept;
		Resource& operator=(Resource&& move) noexcept;

		auto index() const { return m_Index; }

		T*       value() { return &m_Value; }
		const T* value() const { return &m_Value; }

		         operator T&() { return m_Value; }
		         operator T&() const { return m_Value; }
		T*       operator->() { return &m_Value; }
		const T* operator->() const { return &m_Value; }
		T&       operator*() { return m_Value; }
		const T& operator*() const { return m_Value; }

	private:
		IndexT m_Index;
		T      m_Value;
	};
} // namespace ResourceManager

//----------------
// Implementation
//----------------

namespace ResourceManager
{
	template <class T, class IndexType>
	template <class... Args>
	Resource<T, IndexType>::Resource(IndexT index, Args&&... args) requires std::is_constructible_v<T, Args...>
	    : m_Index(index), m_Value(std::forward<Args>(args)...)
	{
	}

	template <class T, class IndexType>
	Resource<T, IndexType>::Resource(Resource&& move) noexcept
	    : m_Index(move.m_Index), m_Value(std::move(move.m_Value))
	{
		move.m_Index = IndexT { ~0ULL };
	}

	template <class T, class IndexType>
	Resource<T, IndexType>& Resource<T, IndexType>::operator=(Resource&& move) noexcept
	{
		m_Index      = move.m_Index;
		m_Value      = std::move(move.m_Value);
		move.m_Index = IndexT { ~0ULL };
		return *this;
	}
} // namespace ResourceManager