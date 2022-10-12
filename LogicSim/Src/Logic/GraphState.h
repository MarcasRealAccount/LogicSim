#pragma once

#include "Component.h"
#include "Graph.h"
#include "ResourceManager/ResourceManager.h"
#include "Utils/BitSet.h"

struct GraphNode;

struct GraphState
{
public:
	GraphState(const Graph& graph);
	GraphState(GraphState&& move) noexcept
	    : m_Graph(move.m_Graph),
	      m_Connections(std::move(move.m_Connections)),
	      m_Inputs(std::move(move.m_Inputs)),
	      m_Outputs(std::move(move.m_Outputs)),
	      m_BuiltinOutputs(std::move(move.m_BuiltinOutputs)),
	      m_GraphNodes(std::move(move.m_GraphNodes)) {}
	GraphState& operator=(GraphState&& move) noexcept
	{
		*std::bit_cast<const Graph**>(this) = &move.m_Graph;

		m_Connections    = std::move(move.m_Connections);
		m_Inputs         = std::move(move.m_Inputs);
		m_Outputs        = std::move(move.m_Outputs);
		m_BuiltinOutputs = std::move(move.m_BuiltinOutputs);
		m_GraphNodes     = std::move(move.m_GraphNodes);
		return *this;
	}

	void setInput(std::size_t bit, bool value)
	{
		m_Inputs.set(bit, value);
	}

	void setInputs(const BitSet& values, std::size_t offset = 0, std::size_t start = 0, std::size_t count = ~0ULL)
	{
		m_Inputs.setBits(values, offset, start, std::min(count, m_Inputs.size()));
	}

	void getOutputs(BitSet& result, std::size_t offset = 0, std::size_t start = 0, std::size_t count = ~0ULL)
	{
		m_Outputs.getBits(result, offset, start, std::min(count, m_Outputs.size()));
	}

	void tick();

	std::size_t allocatedSizeOf() const
	{
		return m_Graph.allocatedSizeOf() + m_Connections.allocatedSizeOf() + m_Inputs.allocatedSizeOf() + m_Outputs.allocatedSizeOf() + m_BuiltinOutputs.allocatedSizeOf() + m_GraphNodes.allocatedSizeOf();
	}

	std::size_t totalSizeOf() const
	{
		return sizeof(*this) + allocatedSizeOf();
	}

private:
	const Graph& m_Graph;
	BitSet       m_Connections;
	BitSet       m_Inputs;
	BitSet       m_Outputs;
	BitSet       m_BuiltinOutputs;

	ResourceManager::ResourcePool<GraphNode> m_GraphNodes;
};

struct GraphNode
{
public:
	GraphNode(const Graph& graph) : m_State(graph) {}
	GraphNode(GraphNode&& move) noexcept : m_State(std::move(move.m_State)) {}

	GraphNode& operator=(GraphNode&& move) noexcept
	{
		m_State = std::move(move.m_State);
		return *this;
	}

public:
	GraphState m_State;
};