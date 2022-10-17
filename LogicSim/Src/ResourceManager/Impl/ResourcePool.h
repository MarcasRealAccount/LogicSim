#pragma once

#include "ResourceManager/Ref.h"
#include "ResourceManager/ResourcePool.h"

#include "ResourceManager/Impl/Ref.h"

#include <algorithm>
#include <utility>

namespace ResourceManager
{
	template <class T, class IndexType>
	bool ResourcePool<T, IndexType>::valid(IndexT index) const
	{
		auto ptr = getResourcePtr(index);
		return ptr ? ptr->valid() : false;
	}

	template <class T, class IndexType>
	template <class... Args>
	ResourcePool<T, IndexType>::Ref ResourcePool<T, IndexType>::emplaceBack(Args&&... args) requires std::is_constructible_v<T, Args...>
	{
		IndexT index    = m_CurrentIndex;
		auto   iterator = getLowerRegion(m_CurrentIndex);
		if (iterator != m_Regions.end())
		{
			if (iterator->m_End == m_CurrentIndex - 1)
			{
				// On region boundary, extend region, merge with next region if they touch
				for (auto it = iterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + iterator->m_Offset + (iterator->m_End - iterator->m_Start) + 1, m_CurrentIndex, std::forward<Args>(args)...);
				iterator->m_End = m_CurrentIndex;

				auto nextIterator = iterator + 1;
				if (nextIterator != m_Regions.end() && nextIterator->m_Start == m_CurrentIndex + 1)
				{
					iterator->m_End = nextIterator->m_End;
					m_Regions.erase(nextIterator);
					nextOptimalIndex();
				}
				else
				{
					++m_CurrentIndex;
				}
				return Ref { this, index };
			}

			if (iterator->m_Start == m_CurrentIndex + 1)
			{
				// On lower region boundary, extend region, merge with previous region if they touch
				iterator->m_Start = m_CurrentIndex;
				for (auto it = iterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + iterator->m_Offset, m_CurrentIndex, std::forward<Args>(args)...);
				nextOptimalIndex();

				if (iterator != m_Regions.begin())
				{
					auto prevIterator = iterator - 1;
					if (prevIterator->m_End == iterator->m_Start - 1)
					{
						prevIterator->m_End = iterator->m_End;
						m_Regions.erase(iterator);
					}
				}
				return Ref { this, index };
			}

			auto nextIterator = iterator + 1;
			if (nextIterator != m_Regions.end() && nextIterator->m_Start == m_CurrentIndex + 1)
			{
				// On lower region boundary, extend region, merge with previous region if they touch
				nextIterator->m_Start = m_CurrentIndex;
				for (auto it = nextIterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + nextIterator->m_Offset, m_CurrentIndex, std::forward<Args>(args)...);
				nextOptimalIndex();

				if (iterator->m_End == nextIterator->m_Start - 1)
				{
					iterator->m_End = nextIterator->m_End;
					m_Regions.erase(nextIterator);
				}
				return Ref { this, index };
			}

			if (iterator->m_End < m_CurrentIndex)
			{
				// Between two regions
				for (auto it = nextIterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + nextIterator->m_Offset + (nextIterator->m_End - nextIterator->m_Start) + 1, m_CurrentIndex, std::forward<Args>(args)...);
				m_Regions.insert(nextIterator, { m_CurrentIndex, m_CurrentIndex, nextIterator->m_Offset + (nextIterator->m_End - nextIterator->m_Start) });
				++m_CurrentIndex;
				return Ref { this, index };
			}
			else
			{
				// First index
				for (auto it = iterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + iterator->m_Offset + (iterator->m_End - iterator->m_Start) + 1, m_CurrentIndex, std::forward<Args>(args)...);
				m_Regions.insert(iterator, { m_CurrentIndex, m_CurrentIndex, iterator->m_Offset + (iterator->m_End - iterator->m_Start) });
				++m_CurrentIndex;
				return Ref { this, index };
			}
		}
		else
		{
			// New region is last
			m_Resources.emplace_back(m_CurrentIndex, std::forward<Args>(args)...);
			m_Regions.push_back({ m_CurrentIndex, m_CurrentIndex, m_Regions.size() });
			++m_CurrentIndex;
			return Ref { this, index };
		}
	}

	template <class T, class IndexType>
	template <class... Args>
	ResourcePool<T, IndexType>::Ref ResourcePool<T, IndexType>::emplace(IndexType index, Args&&... args) requires std::is_constructible_v<T, Args...>
	{
		auto iterator = getLowerRegion(index);
		if (iterator != m_Regions.end())
		{
			if (iterator->m_End == index - 1)
			{
				// On region boundary, extend region, merge with next region if they touch
				for (auto it = iterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + iterator->m_Offset + (iterator->m_End - iterator->m_Start) + 1, index, std::forward<Args>(args)...);
				iterator->m_End = index;

				auto nextIterator = iterator + 1;
				if (nextIterator != m_Regions.end() && nextIterator->m_Start == index + 1)
				{
					iterator->m_End = nextIterator->m_End;
					m_Regions.erase(nextIterator);
					nextOptimalIndex();
				}
				else
				{
					m_CurrentIndex = index + 1;
				}
				return Ref { this, index };
			}

			if (iterator->m_Start == index + 1)
			{
				// On lower region boundary, extend region, merge with previous region if they touch
				iterator->m_Start = index;
				for (auto it = iterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + iterator->m_Offset, index, std::forward<Args>(args)...);
				nextOptimalIndex();

				if (iterator != m_Regions.begin())
				{
					auto prevIterator = iterator - 1;
					if (prevIterator->m_End == iterator->m_Start - 1)
					{
						prevIterator->m_End = iterator->m_End;
						m_Regions.erase(iterator);
					}
				}
				return Ref { this, index };
			}

			auto nextIterator = iterator + 1;
			if (nextIterator != m_Regions.end() && nextIterator->m_Start == index + 1)
			{
				// On lower region boundary, extend region, merge with previous region if they touch
				nextIterator->m_Start = index;
				for (auto it = nextIterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + nextIterator->m_Offset, index, std::forward<Args>(args)...);
				nextOptimalIndex();

				if (iterator->m_End == nextIterator->m_Start - 1)
				{
					iterator->m_End = nextIterator->m_End;
					m_Regions.erase(nextIterator);
				}
				return Ref { this, index };
			}

			if (iterator->m_End < index)
			{
				// Between two regions
				for (auto it = nextIterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + nextIterator->m_Offset + (nextIterator->m_End - nextIterator->m_Start) + 1, index, std::forward<Args>(args)...);
				m_Regions.insert(nextIterator, { index, index, nextIterator->m_Offset + (nextIterator->m_End - nextIterator->m_Start) });
				m_CurrentIndex = index + 1;
				return Ref { this, index };
			}
			else
			{
				// First index
				for (auto it = iterator + 1; it != m_Regions.end(); ++it)
					++it->m_Offset;
				m_Resources.emplace(m_Resources.begin() + iterator->m_Offset + (iterator->m_End - iterator->m_Start) + 1, index, std::forward<Args>(args)...);
				m_Regions.insert(iterator, { index, index, iterator->m_Offset + (iterator->m_End - iterator->m_Start) });
				m_CurrentIndex = index + 1;
				return Ref { this, index };
			}
		}
		else
		{
			// New region is last
			m_Resources.emplace_back(index, std::forward<Args>(args)...);
			m_Regions.push_back({ index, index, m_Regions.size() });
			m_CurrentIndex = index + 1;
			return Ref { this, index };
		}
	}

	template <class T, class IndexType>
	void ResourcePool<T, IndexType>::erase(IndexT index)
	{
		auto region = getRegion(index);
		if (region == m_Regions.end())
			return;

		if (index == region->m_Start) // On lower boundary, shrink region
		{
			m_Resources.erase(m_Resources.begin() + region->m_Offset);
			if (++region->m_Start > region->m_End)
				region = m_Regions.erase(region);
			else
				++region;
			for (auto it = region; it != m_Regions.end(); ++it)
				--it->m_Offset;
		}
		else if (index == region->m_End) // On upper boundary, shrink region
		{
			m_Resources.erase(m_Resources.begin() + region->m_Offset + (region->m_End - region->m_Start));
			if (--region->m_End < region->m_Start)
				region = m_Regions.erase(region);
			else
				++region;
			for (auto it = region; it != m_Regions.end(); ++it)
				--it->m_Offset;
		}
		else // In middle of region, split
		{
			m_Resources.erase(m_Resources.begin() + region->m_Offset + (index - region->m_Start));
			for (auto it = region + 1; it != m_Regions.end(); ++it)
				--it->m_Offset;
			IndexT end    = region->m_End;
			region->m_End = index - 1;
			m_Regions.insert(region + 1, { index + 1, end, region->m_Offset + (index - region->m_Start) });
		}

		nextOptimalIndex();
	}

	template <class T, class IndexType>
	T* ResourcePool<T, IndexType>::getResource(IndexT index)
	{
		auto ptr = getResourcePtr(index);
		return ptr ? ptr->value() : nullptr;
	}

	template <class T, class IndexType>
	const T* ResourcePool<T, IndexType>::getResource(IndexT index) const
	{
		auto ptr = getResourcePtr(index);
		return ptr ? ptr->value() : nullptr;
	}

	template <class T, class IndexType>
	void ResourcePool<T, IndexType>::nextOptimalIndex()
	{
		if (m_Regions.empty())
		{
			m_CurrentIndex = 0;
		}
		else
		{
			IndexT bestIndex    = IndexT { 0 };
			IndexT bestDistance = ~IndexT { 0 };
			for (auto it = m_Regions.begin(); it != m_Regions.end() - 1; ++it)
			{
				auto   nextIt   = it + 1;
				IndexT distance = nextIt->m_Start - it->m_End;
				if (distance <= bestDistance)
				{
					bestIndex    = it->m_End + 1;
					bestDistance = distance;
				}
			}
			auto region = getRegion(bestIndex);
			if (region != m_Regions.end())
				bestIndex = region->m_End + 1;
			m_CurrentIndex = bestIndex;
		}
	}

	template <class T, class IndexType>
	ResourcePool<T, IndexType>::Resource* ResourcePool<T, IndexType>::getResourcePtr(IndexT index)
	{
		auto region = getRegion(index);
		return region == m_Regions.end() ? nullptr : &m_Resources[region->m_Offset + (index - region->m_Start)];
	}

	template <class T, class IndexType>
	const typename ResourcePool<T, IndexType>::Resource* ResourcePool<T, IndexType>::getResourcePtr(IndexT index) const
	{
		auto region = getRegion(index);
		return region == m_Regions.end() ? nullptr : &m_Resources[region->m_Offset + (index - region->m_Start)];
	}

	template <class T, class IndexType>
	ResourcePool<T, IndexType>::RegionList::iterator ResourcePool<T, IndexType>::getLowerRegion(IndexT index)
	{
		if (m_Regions.empty())
			return m_Regions.end();

		auto start = m_Regions.begin();
		auto end   = m_Regions.end() - 1;
		while (start != end)
		{
			auto mid = start + (end - start + 1) / 2;
			if (mid->m_End < index)
				start = mid;
			else if (mid->m_Start > index)
				end = mid - 1;
			else
				return mid;
		}
		return start;
	}

	template <class T, class IndexType>
	ResourcePool<T, IndexType>::RegionList::const_iterator ResourcePool<T, IndexType>::getLowerRegion(IndexT index) const
	{
		if (m_Regions.empty())
			return m_Regions.end();

		return std::lower_bound(m_Regions.begin(), m_Regions.end(), index,
		                        [](Region region, IndexT value) -> bool
		                        {
			                        return region.m_End < value;
		                        });
	}

	template <class T, class IndexType>
	ResourcePool<T, IndexType>::RegionList::iterator ResourcePool<T, IndexType>::getRegion(IndexT index)
	{
		if (m_Regions.empty())
			return m_Regions.end();

		auto it = std::lower_bound(m_Regions.begin(), m_Regions.end(), index,
		                           [](Region region, IndexT value) -> bool
		                           {
			                           return region.m_End < value;
		                           });
		return it != m_Regions.end() && it->contains(index) ? it : m_Regions.end();
	}

	template <class T, class IndexType>
	ResourcePool<T, IndexType>::RegionList::const_iterator ResourcePool<T, IndexType>::getRegion(IndexT index) const
	{
		if (m_Regions.empty())
			return m_Regions.end();

		auto it = std::lower_bound(m_Regions.begin(), m_Regions.end(), index,
		                           [](Region region, IndexT value) -> bool
		                           {
			                           return region.m_End < value;
		                           });
		return it != m_Regions.end() && it->contains(index) ? it : m_Regions.end();
	}
} // namespace ResourceManager