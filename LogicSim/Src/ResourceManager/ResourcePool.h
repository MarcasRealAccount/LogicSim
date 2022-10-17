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
	struct ResourcePool
	{
	public:
		using Ref          = Ref<T, IndexType>;
		using ConstRef     = ConstRef<T, IndexType>;
		using Region       = Utils::Region<IndexType>;
		using Resource     = Resource<T, IndexType>;
		using RegionList   = std::vector<Region>;
		using ResourceList = std::vector<Resource>;
		using IndexT       = IndexType;

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

		std::size_t allocatedSizeOf() const { return m_Regions.capacity() * sizeof(Region) + m_Resources.capacity() * sizeof(Resource); }
		std::size_t totalSizeOf() const { return sizeof(*this) + allocatedSizeOf(); }

		auto begin() { return m_Resources.begin(); }
		auto end() { return m_Resources.end(); }
		auto begin() const { return m_Resources.begin(); }
		auto end() const { return m_Resources.end(); }
		auto cbegin() const { return m_Resources.cbegin(); }
		auto cend() const { return m_Resources.cend(); }

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