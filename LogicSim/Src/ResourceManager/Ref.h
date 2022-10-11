#pragma once

#include <cstddef>
#include <cstdint>

#include <compare>

namespace ResourceManager
{
	template <class T, class IndexType>
	struct ResourcePool;

	template <class T, class IndexType = std::size_t>
	struct Ref
	{
	public:
		using ResourcePool = ResourcePool<T, IndexType>;
		using IndexT       = IndexType;

	public:
		Ref();
		Ref(ResourcePool* pool, IndexT index);

		inline bool valid() const;

		inline T*       get();
		inline const T* get() const;

		ResourcePool*       pool() { return m_Pool; }
		const ResourcePool* pool() const { return m_Pool; }
		IndexT              index() const { return m_Index; }

		         operator T&() { return *get(); }
		         operator const T&() const { return *get(); }
		T*       operator->() { return get(); }
		const T* operator->() const { return get(); }
		T&       operator*() { return *get(); }
		const T& operator*() const { return *get(); }

		friend bool operator==(Ref lhs, Ref rhs)
		{
			return lhs.m_Pool == rhs.m_Pool && lhs.m_Index == rhs.m_Index;
		}
		friend bool operator!=(Ref lhs, Ref rhs) { return !(lhs == rhs); }
		friend bool operator<(Ref lhs, Ref rhs)
		{
			if (lhs.m_Pool < rhs.m_Pool)
				return true;
			return lhs.m_Index < rhs.m_Index;
		}
		friend bool operator<=(Ref lhs, Ref rhs)
		{
			if (lhs.m_Pool <= rhs.m_Pool)
				return true;
			return lhs.m_Index <= rhs.m_Index;
		}
		friend bool operator>(Ref lhs, Ref rhs)
		{
			if (lhs.m_Pool > rhs.m_Pool)
				return true;
			return lhs.m_Index > rhs.m_Index;
		}
		friend bool operator>=(Ref lhs, Ref rhs)
		{
			if (lhs.m_Pool >= rhs.m_Pool)
				return true;
			return lhs.m_Index >= rhs.m_Index;
		}

	private:
		ResourcePool* m_Pool;
		IndexT        m_Index;
	};

	template <class T, class IndexType = std::size_t>
	struct ConstRef
	{
	public:
		using ResourcePool = ResourcePool<T, IndexType>;
		using IndexT       = IndexType;

	public:
		ConstRef();
		ConstRef(const ResourcePool* pool, IndexT index);

		inline bool valid() const;

		inline const T* get() const;

		const ResourcePool* pool() const { return m_Pool; }
		IndexT              index() const { return m_Index; }

		         operator const T&() const { return *get(); }
		const T* operator->() const { return get(); }
		const T& operator*() const { return *get(); }

		friend bool operator==(ConstRef lhs, ConstRef rhs)
		{
			return lhs.m_Pool == rhs.m_Pool && lhs.m_Index == rhs.m_Index;
		}
		friend bool operator!=(ConstRef lhs, ConstRef rhs) { return !(lhs == rhs); }
		friend bool operator<(ConstRef lhs, ConstRef rhs)
		{
			if (lhs.m_Pool < rhs.m_Pool)
				return true;
			return lhs.m_Index < rhs.m_Index;
		}
		friend bool operator<=(ConstRef lhs, ConstRef rhs)
		{
			if (lhs.m_Pool <= rhs.m_Pool)
				return true;
			return lhs.m_Index <= rhs.m_Index;
		}
		friend bool operator>(ConstRef lhs, ConstRef rhs)
		{
			if (lhs.m_Pool > rhs.m_Pool)
				return true;
			return lhs.m_Index > rhs.m_Index;
		}
		friend bool operator>=(ConstRef lhs, ConstRef rhs)
		{
			if (lhs.m_Pool >= rhs.m_Pool)
				return true;
			return lhs.m_Index >= rhs.m_Index;
		}

	private:
		const ResourcePool* m_Pool;
		IndexT              m_Index;
	};
} // namespace ResourceManager