#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

struct BitReference
{
public:
	BitReference() : m_Byte(nullptr), m_Bit(0), m_Mask(0) {}
	BitReference(std::uint8_t* byte, std::uint8_t bit)
	    : m_Byte(byte), m_Bit(bit), m_Mask(~(1 << bit)) {}

	operator bool() const { return m_Byte ? (*m_Byte >> m_Bit) & 1 : false; }

	BitReference& operator=(bool value)
	{
		if (m_Byte)
			*m_Byte = *m_Byte & m_Mask | value << m_Bit;
		return *this;
	}

private:
	std::uint8_t* m_Byte;
	std::uint8_t  m_Bit;
	std::uint8_t  m_Mask;
};

struct BitSet
{
public:
	BitSet()
	    : m_Data(nullptr),
	      m_Size(0) {}
	BitSet(std::size_t initialSize)
	    : m_Data(new std::uint8_t[(initialSize + 7) >> 3]),
	      m_Size((initialSize + 7) >> 3 << 3)
	{
		std::memset(m_Data, 0, m_Size >> 3);
	}
	BitSet(const std::initializer_list<std::uint8_t>& values)
	    : m_Data(new std::uint8_t[values.size()]),
	      m_Size(values.size() << 3)
	{
		std::size_t i = 0;
		for (auto itr = values.begin(); itr != values.end(); ++itr, ++i)
			m_Data[i] = *itr;
	}
	BitSet(const BitSet& copy)
	    : m_Data(new std::uint8_t[copy.m_Size >> 3]),
	      m_Size(copy.m_Size)
	{
		std::memcpy(m_Data, copy.m_Data, m_Size >> 3);
	}
	BitSet(BitSet&& move) noexcept
	    : m_Data(move.m_Data),
	      m_Size(move.m_Size)
	{
		move.m_Data = nullptr;
		move.m_Size = 0;
	}
	BitSet& operator=(const BitSet& copy)
	{
		if (m_Size < copy.m_Size)
		{
			delete[] m_Data;
			m_Size = copy.m_Size;
			m_Data = new std::uint8_t[m_Size >> 3];
		}
		else if (m_Size > copy.m_Size << 1)
		{
			delete[] m_Data;
			m_Size = copy.m_Size;
			m_Data = new std::uint8_t[m_Size >> 3];
		}
		std::memcpy(m_Data, copy.m_Data, m_Size >> 3);
		return *this;
	}
	BitSet& operator=(BitSet&& move) noexcept
	{
		delete[] m_Data;
		m_Data      = move.m_Data;
		m_Size      = move.m_Size;
		move.m_Data = nullptr;
		move.m_Size = 0;
		return *this;
	}
	~BitSet()
	{
		delete[] m_Data;
	}

	std::size_t size() const
	{
		return m_Size;
	}

	void resize(std::size_t size)
	{
		std::size_t newSize = (size + 7) >> 3 << 3;
		if (newSize != m_Size)
		{
			std::uint8_t* newData   = new std::uint8_t[newSize >> 3];
			std::size_t   copyCount = (newSize < m_Size ? newSize : m_Size) >> 3;
			std::memcpy(newData, m_Data, copyCount);
			if (newSize > m_Size)
				std::memset(newData + copyCount, 0, (newSize >> 3) - copyCount);
			delete[] m_Data;
			m_Data = newData;
			m_Size = newSize;
		}
	}

	bool get(std::size_t bit) const
	{
		std::size_t byteIndex = bit >> 3;
		if (byteIndex >= m_Size)
			return false;

		std::uint8_t bitIndex = bit & 0b111;
		return (m_Data[byteIndex] >> bitIndex) & 1;
	}

	void set(std::size_t bit, bool value)
	{
		std::size_t byteIndex = bit >> 3;
		if (byteIndex >= m_Size)
			return;
		std::uint8_t bitIndex = bit & 0b111;
		std::size_t  mask     = ~(1 << bitIndex);
		m_Data[byteIndex]     = m_Data[byteIndex] & mask | static_cast<std::uint8_t>(value << bitIndex);
	}

	BitReference operator[](std::size_t bit)
	{
		std::size_t byteIndex = bit >> 3;
		if (byteIndex >= m_Size)
			return BitReference {};
		std::uint8_t bitIndex = bit & 0b111;

		return BitReference(m_Data + byteIndex, bitIndex);
	}

	bool operator[](std::size_t bit) const { return get(bit); }

	void getBits(BitSet& result, std::size_t offset, std::size_t start, std::size_t count) const
	{
		std::size_t end = start + count;
		if (end < start)
			return;
		std::size_t startByte = start >> 3;
		std::size_t endByte   = end >> 3;
		if (startByte >= m_Size || endByte >= m_Size)
			return;
		std::size_t startBit = start & 0b111;

		if (result.m_Size < (offset + count))
		{
			if (offset & 0b111)
			{
				std::uint8_t startMask = (1 << (offset & 0b111)) - 1;
				std::uint8_t endMask   = ~startMask;
				result.m_Data[offset >> 3] &= startMask;
				std::memset(result.m_Data + (offset >> 3) + 1, 0, ((result.m_Size - offset) >> 3) - 2);
				result.m_Data[(result.m_Size >> 3) - 1] &= endMask;
			}
			else
			{
				std::memset(result.m_Data + (offset >> 3), 0, (result.m_Size - offset) >> 3);
			}
			return;
		}

		std::size_t i = offset >> 3, j = startByte + 1;
		std::size_t resultBit  = offset & 0b111;
		std::size_t resultMask = (1 << resultBit) - 1;
		auto        writeValue = [&](std::uint8_t value, std::size_t readBits)
		{
			if (count > (8 - resultBit))
			{
				result.m_Data[i]     = result.m_Data[i] & resultMask | static_cast<std::uint8_t>(value << resultBit);
				result.m_Data[i + 1] = value >> (8 - resultBit);
			}
			else if (resultBit == 0)
			{
				result.m_Data[i] = value;
			}
			else
			{
				result.m_Data[i] = result.m_Data[i] & resultMask | static_cast<std::uint8_t>(value << resultBit);
			}

			resultBit += readBits;
			if (resultBit >= 8)
			{
				resultBit -= 8;
				++i;
			}
			resultMask = (1 << resultBit) - 1;
			count -= readBits;
		};
		if (count <= 8 - startBit)
		{
			std::uint8_t mask = (1 << count) - 1;
			writeValue((m_Data[startByte] >> startBit) & mask, count);
			return;
		}

		writeValue(m_Data[startByte] >> startBit, 8 - startBit);

		for (; count > 8; ++j)
			writeValue(m_Data[j], 8);

		if (count > 0)
		{
			std::uint8_t mask = (1 << count) - 1;
			writeValue(m_Data[j] & mask, count);
		}
	}

	void setBits(const BitSet& values, std::size_t offset, std::size_t start, std::size_t count)
	{
		values.getBits(*this, start, offset, count);
	}

	std::size_t allocatedSizeOf() const
	{
		return m_Size;
	}

	std::size_t totalSizeOf() const
	{
		return sizeof(*this) + allocatedSizeOf();
	}

private:
	std::uint8_t* m_Data;
	std::size_t   m_Size;
};