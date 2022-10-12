#pragma once

#include "Utils/BitSet.h"
#include "Utils/Utils.h"

struct TruthTable
{
public:
	using FillerFunctionType = void (*)(std::uint16_t input, std::size_t bit, BitSet& bitSet);

public:
	template <Callable<void, std::uint16_t, std::size_t, BitSet&> F>
	TruthTable(std::uint8_t numInputs, std::size_t numOutputs, F&& filler)
	    : m_Outputs((1ULL << numInputs) * numOutputs),
	      m_MaxPossibilities(static_cast<std::uint32_t>(1 << numInputs)),
	      m_NumInputs(numInputs),
	      m_NumOutputs(numOutputs)
	{
		for (std::uint16_t i = 0; i < m_MaxPossibilities; ++i)
			filler(i, i * m_NumOutputs, m_Outputs);
	}

	void getOutput(std::uint16_t inputs, BitSet& outputs) const
	{
		m_Outputs.getBits(outputs, 0, inputs * m_NumOutputs, m_NumOutputs);
	}

	std::size_t allocatedSize() const
	{
		std::size_t staticSize = sizeof(*this);
		staticSize += m_Outputs.size() >> 3;
		return staticSize;
	}

	std::size_t inputCount() const { return m_NumInputs; }
	std::size_t outputCount() const { return m_NumOutputs; }

private:
	BitSet        m_Outputs;
	std::uint32_t m_MaxPossibilities : 24;
	std::uint32_t m_NumInputs : 8;
	std::size_t   m_NumOutputs;
};