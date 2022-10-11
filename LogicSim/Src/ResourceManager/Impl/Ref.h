#pragma once

#include "ResourceManager/Ref.h"
#include "ResourceManager/ResourcePool.h"

namespace ResourceManager
{
	template <class T, class IndexType>
	Ref<T, IndexType>::Ref()
	    : m_Pool(nullptr), m_Index(0)
	{
	}

	template <class T, class IndexType>
	Ref<T, IndexType>::Ref(ResourcePool* pool, IndexT index)
	    : m_Pool(pool), m_Index(index)
	{
	}

	template <class T, class IndexType>
	bool Ref<T, IndexType>::valid() const
	{
		return m_Pool;
	}

	template <class T, class IndexType>
	T* Ref<T, IndexType>::get()
	{
		return m_Pool->getResource(m_Index);
	}

	template <class T, class IndexType>
	const T* Ref<T, IndexType>::get() const
	{
		return m_Pool->getResource(m_Index);
	}

	template <class T, class IndexType>
	ConstRef<T, IndexType>::ConstRef()
	    : m_Pool(nullptr), m_Index(0)
	{
	}

	template <class T, class IndexType>
	ConstRef<T, IndexType>::ConstRef(const ResourcePool* pool, IndexT index)
	    : m_Pool(pool), m_Index(index)
	{
	}

	template <class T, class IndexType>
	bool ConstRef<T, IndexType>::valid() const
	{
		return m_Pool;
	}

	template <class T, class IndexType>
	const T* ConstRef<T, IndexType>::get() const
	{
		return m_Pool->getResource(m_Index);
	}
} // namespace ResourceManager