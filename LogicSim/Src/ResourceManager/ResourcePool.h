#pragma once

#include "Ref.h"
#include "Resource.h"
#include "Utils/Region.h"

#include <cstddef>
#include <cstdint>

#include <type_traits>
#include <vector>

namespace ResourceManager
{
	template <class T, class IndexType = std::size_t>
	struct ResourcePoolConstIterator;
	template <class T, class IndexType = std::size_t>
	struct ResourcePool;

	template <class T, class IndexType = std::size_t>
	struct ResourcePoolIterator
	{
	public:
		using ResourcePool = ResourcePool<T, IndexType>;
		using Ref          = Ref<T, IndexType>;
		using ConstRef     = ConstRef<T, IndexType>;
		using Region       = Utils::Region<IndexType>;
		using Resource     = Resource<T>;
		using IndexT       = IndexType;

		using iterator_category = std::forward_iterator_tag;
		using difference_type   = IndexT;
		using value_type        = Ref;

	public:
		ResourcePoolIterator()
		    : m_Pool(nullptr), m_RegionIndex(0), m_Index(0) {}
		ResourcePoolIterator(ResourcePool* pool, std::size_t regionIndex, IndexT index)
		    : m_Pool(pool), m_RegionIndex(regionIndex), m_Index(index) {}
		ResourcePoolIterator(const ResourcePoolIterator& copy)
		    : m_Pool(copy.m_Pool), m_RegionIndex(copy.m_RegionIndex), m_Index(copy.m_Index) {}
		ResourcePoolIterator(ResourcePoolIterator&& move) noexcept
		    : m_Pool(move.m_Pool), m_RegionIndex(move.m_RegionIndex), m_Index(move.m_Index)
		{
			move.m_Pool        = nullptr;
			move.m_RegionIndex = IndexT { 0 };
			move.m_Index       = IndexT { 0 };
		}
		ResourcePoolIterator& operator=(const ResourcePoolIterator& copy)
		{
			m_Pool        = copy.m_Pool;
			m_RegionIndex = copy.m_RegionIndex;
			m_Index       = copy.m_Index;
			return *this;
		}
		ResourcePoolIterator& operator=(ResourcePoolIterator&& move) noexcept
		{
			m_Pool             = move.m_Pool;
			m_RegionIndex      = move.m_RegionIndex;
			m_Index            = move.m_Index;
			move.m_Pool        = nullptr;
			move.m_RegionIndex = IndexT { 0 };
			move.m_Index       = IndexT { 0 };
			return *this;
		}

		operator ResourcePoolConstIterator<T, IndexType>() const;

		value_type            operator*();
		const value_type      operator*() const;
		ResourcePoolIterator& operator++();
		ResourcePoolIterator  operator++(int);

		friend std::weak_ordering operator<=>(ResourcePoolIterator lhs, ResourcePoolIterator rhs)
		{
			if (lhs.m_Pool != rhs.m_Pool)
				return std::weak_ordering::less;
			return lhs.m_Index <=> rhs.m_Index;
		}
		friend bool operator!=(ResourcePoolIterator lhs, ResourcePoolIterator rhs) { return (lhs <=> rhs) != std::weak_ordering::equivalent; }

	private:
		ResourcePool* m_Pool;
		std::size_t   m_RegionIndex;
		IndexT        m_Index;
	};

	template <class T, class IndexType>
	struct ResourcePoolConstIterator
	{
	public:
		using ResourcePool = ResourcePool<T, IndexType>;
		using Ref          = Ref<T, IndexType>;
		using ConstRef     = ConstRef<T, IndexType>;
		using Region       = Utils::Region<IndexType>;
		using Resource     = Resource<T>;
		using IndexT       = IndexType;

		using iterator_category = std::forward_iterator_tag;
		using difference_type   = IndexT;
		using value_type        = ConstRef;

	public:
		ResourcePoolConstIterator()
		    : m_Pool(nullptr), m_RegionIndex(0), m_Index(0) {}
		ResourcePoolConstIterator(const ResourcePool* pool, std::size_t regionIndex, IndexT index)
		    : m_Pool(pool), m_RegionIndex(regionIndex), m_Index(index) {}
		ResourcePoolConstIterator(const ResourcePoolConstIterator& copy)
		    : m_Pool(copy.m_Pool), m_RegionIndex(copy.m_RegionIndex), m_Index(copy.m_Index) {}
		ResourcePoolConstIterator(ResourcePoolConstIterator&& move) noexcept
		    : m_Pool(move.m_Pool), m_RegionIndex(move.m_RegionIndex), m_Index(move.m_Index)
		{
			move.m_Pool        = nullptr;
			move.m_RegionIndex = IndexT { 0 };
			move.m_Index       = IndexT { 0 };
		}
		ResourcePoolConstIterator& operator=(const ResourcePoolConstIterator& copy)
		{
			m_Pool        = copy.m_Pool;
			m_RegionIndex = copy.m_RegionIndex;
			m_Index       = copy.m_Index;
			return *this;
		}
		ResourcePoolConstIterator& operator=(ResourcePoolConstIterator&& move) noexcept
		{
			m_Pool             = move.m_Pool;
			m_RegionIndex      = move.m_RegionIndex;
			m_Index            = move.m_Index;
			move.m_Pool        = nullptr;
			move.m_RegionIndex = IndexT { 0 };
			move.m_Index       = IndexT { 0 };
			return *this;
		}

		const value_type           operator*() const;
		ResourcePoolConstIterator& operator++();
		ResourcePoolConstIterator  operator++(int);

		friend std::weak_ordering operator<=>(ResourcePoolConstIterator lhs, ResourcePoolConstIterator rhs)
		{
			if (lhs.m_Pool != rhs.m_Pool)
				return std::weak_ordering::less;
			return lhs.m_Index <=> rhs.m_Index;
		}
		friend bool operator!=(ResourcePoolConstIterator lhs, ResourcePoolConstIterator rhs) { return (lhs <=> rhs) != std::weak_ordering::equivalent; }

	private:
		const ResourcePool* m_Pool;
		std::size_t         m_RegionIndex;
		IndexT              m_Index;
	};

	template <class T, class IndexType>
	struct ResourcePool
	{
	public:
		template <class T2, class IndexType2>
		friend struct ResourcePoolIterator;
		template <class T2, class IndexType2>
		friend struct ResourcePoolConstIterator;

	public:
		using Ref          = Ref<T, IndexType>;
		using ConstRef     = ConstRef<T, IndexType>;
		using Region       = Utils::Region<IndexType>;
		using Resource     = Resource<T>;
		using RegionList   = std::vector<Region>;
		using ResourceList = std::vector<Resource>;
		using IndexT       = IndexType;

		using IteratorT      = ResourcePoolIterator<T, IndexType>;
		using ConstIteratorT = ResourcePoolConstIterator<T, IndexType>;

	public:
		ResourcePool() : m_CurrentIndex(0U) {}

		bool valid(IndexT index) const;

		template <class... Args>
		Ref emplaceBack(Args&&... args) requires std::is_constructible_v<T, Args...>;
		template <class... Args>
		Ref  emplace(IndexT index, Args&&... args) requires std::is_constructible_v<T, Args...>;
		void erase(IndexT index);

		T*       getResource(IndexT index);
		const T* getResource(IndexT index) const;

		auto& regions() const { return m_Regions; }
		auto& resources() const { return m_Resources; }
		auto  currentIndex() const { return m_CurrentIndex; }

		std::size_t allocatedSizeOf() const
		{
			return m_Regions.capacity() * sizeof(Region) + m_Resources.capacity() * sizeof(Resource);
		}

		std::size_t totalSizeOf() const
		{
			return sizeof(*this) + allocatedSizeOf();
		}

		IteratorT      begin();
		IteratorT      end();
		ConstIteratorT begin() const;
		ConstIteratorT end() const;
		ConstIteratorT cbegin() const;
		ConstIteratorT cend() const;

	protected:
		void nextOptimalIndex();

		Resource*       getResourcePtr(IndexT index);
		const Resource* getResourcePtr(IndexT index) const;

		RegionList::iterator       getLowerRegion(IndexT index);
		RegionList::const_iterator getLowerRegion(IndexT index) const;
		RegionList::iterator       getRegion(IndexT index);
		RegionList::const_iterator getRegion(IndexT index) const;

	private:
		RegionList   m_Regions;
		ResourceList m_Resources;

		IndexT m_CurrentIndex;
	};
} // namespace ResourceManager