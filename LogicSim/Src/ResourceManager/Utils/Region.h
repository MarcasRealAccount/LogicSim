#pragma once

#include <cstddef>
#include <cstdint>

namespace ResourceManager::Utils
{
	template <class IndexType = std::uint32_t>
	struct Region
	{
	public:
		using IndexT = IndexType;

		bool contains(IndexT index) const { return m_Start <= index && m_End >= index; }

	public:
		IndexT      m_Start;
		IndexT      m_End;
		std::size_t m_Offset;
	};
} // namespace ResourceManager::Utils