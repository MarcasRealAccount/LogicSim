#pragma once

#include <type_traits>
#include <utility>

namespace ResourceManager
{
	template <class T>
	struct Resource
	{
	public:
		template <class... Args>
		Resource(Args&&... args) requires std::is_constructible_v<T, Args...>;
		Resource(Resource&& move) noexcept;
		Resource& operator=(Resource&& move) noexcept;

		T*       getValue() { return &m_Value; }
		const T* getValue() const { return &m_Value; }

		         operator T&() { return m_Value; }
		         operator T&() const { return m_Value; }
		T*       operator->() { return &m_Value; }
		const T* operator->() const { return &m_Value; }
		T&       operator*() { return m_Value; }
		const T& operator*() const { return m_Value; }

	private:
		T m_Value;
	};
} // namespace ResourceManager

//----------------
// Implementation
//----------------

namespace ResourceManager
{
	template <class T>
	template <class... Args>
	Resource<T>::Resource(Args&&... args) requires std::is_constructible_v<T, Args...>
	    : m_Value(std::forward<Args>(args)...)
	{
	}

	template <class T>
	Resource<T>::Resource(Resource&& move) noexcept
	    : m_Value(std::move(move.m_Value))
	{
	}

	template <class T>
	Resource<T>& Resource<T>::operator=(Resource&& move) noexcept
	{
		m_Value = std::move(move.m_Value);
		return *this;
	}
} // namespace ResourceManager