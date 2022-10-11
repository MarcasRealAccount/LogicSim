#pragma once

#include "Graph.h"
#include "TruthTable.h"
#include "Utils/Utils.h"

#include <utility>
#include <variant>

enum class EComponentType : std::uint8_t
{
	TruthTable,
	Graph
};

struct Component
{
public:
	Component(TruthTable&& truthTable)
	    : m_Type(EComponentType::TruthTable),
	      m_Value(std::move(truthTable)) {}
	Component(Graph&& graph)
	    : m_Type(EComponentType::Graph),
	      m_Value(std::move(graph)) {}
	template <Callable<TruthTable> F>
	Component(F&& truthTableProducer)
	    : m_Type(EComponentType::TruthTable),
	      m_Value(truthTableProducer())
	{
	}
	template <Callable<Graph> F>
	Component(F&& graphProducer)
	    : m_Type(EComponentType::Graph),
	      m_Value(graphProducer())
	{
	}

	std::size_t inputCount() const
	{
		switch (m_Type)
		{
		case EComponentType::TruthTable:
			return getTruthTable().inputCount();
		case EComponentType::Graph:
			return getGraph().inputCount();
		default:
			return 0;
		}
	}

	std::size_t outputCount() const
	{
		switch (m_Type)
		{
		case EComponentType::TruthTable:
			return getTruthTable().outputCount();
		case EComponentType::Graph:
			return getGraph().outputCount();
		default:
			return 0;
		}
	}

	auto              getType() const { return m_Type; }
	TruthTable&       getTruthTable() { return std::get<TruthTable>(m_Value); }
	const TruthTable& getTruthTable() const { return std::get<TruthTable>(m_Value); }
	Graph&            getGraph() { return std::get<Graph>(m_Value); }
	const Graph&      getGraph() const { return std::get<Graph>(m_Value); }

private:
	EComponentType m_Type;
	std::variant<TruthTable,
	             Graph>
	    m_Value;
};