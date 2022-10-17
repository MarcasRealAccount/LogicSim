#include "GraphState.h"

GraphState::GraphState(const Graph& graph)
    : m_Graph(graph),
      m_Connections(m_Graph.connectionCount()),
      m_Inputs(m_Graph.inputCount()),
      m_Outputs(m_Graph.outputCount())
{
	std::size_t maxOutputs = 0;
	for (auto& node : m_Graph.getNodes())
	{
		auto& component = node->getComponent();
		if (component->hasGraph())
			m_GraphNodes.emplace(node.index(), component->getGraph());
		else if (node->outputCount() > maxOutputs)
			maxOutputs = node->outputCount();
	}

	m_BuiltinOutputs.resize(maxOutputs);
}

void GraphState::tick()
{
	{
		auto& inputPorts = m_Graph.getInputPorts();
		for (std::size_t i = 0; i < inputPorts.size(); ++i)
		{
			std::size_t connection = inputPorts[i];
			if (connection == ~0ULL)
				continue;
			m_Connections.set(connection, m_Inputs.get(i));
		}
	}

	for (auto& node : m_Graph.getNodes())
	{
		auto& component   = node->getComponent();
		auto& inputPorts  = node->getInputPorts();
		auto& outputPorts = node->getOutputPorts();

		if (component->hasTruthTable())
		{
			std::uint16_t inputs = 0;
			for (std::size_t i = 0; i < inputPorts.size(); ++i)
			{
				std::size_t connection = inputPorts[i];
				if (connection == ~0ULL)
					continue;
				inputs |= m_Connections.get(connection) << i;
			}
			component->getTruthTable().getOutput(inputs, m_BuiltinOutputs);
			for (std::size_t i = 0; i < outputPorts.size(); ++i)
			{
				std::size_t connection = outputPorts[i];
				if (connection == ~0ULL)
					continue;
				m_Connections.set(connection, m_BuiltinOutputs.get(i));
			}
		}
		else if (component->hasGraph())
		{
			auto graphState = m_GraphNodes.getResource(node.index());
			if (!graphState)
				break;
			auto& inputs  = graphState->m_State.m_Inputs;
			auto& outputs = graphState->m_State.m_Outputs;
			for (std::size_t i = 0; i < inputPorts.size(); ++i)
			{
				std::size_t connection = inputPorts[i];
				if (connection == ~0ULL)
					continue;
				inputs.set(i, m_Connections.get(connection));
			}
			graphState->m_State.tick();
			for (std::size_t i = 0; i < outputPorts.size(); ++i)
			{
				std::size_t connection = outputPorts[i];
				if (connection == ~0ULL)
					continue;
				m_Connections.set(connection, outputs.get(i));
			}
		}
	}

	{
		auto& outputPorts = m_Graph.getOutputPorts();
		for (std::size_t i = 0; i < outputPorts.size(); ++i)
		{
			std::size_t connection = outputPorts[i];
			bool        value      = false;
			if (connection != ~0ULL)
				value = m_Connections.get(connection);
			m_Outputs.set(i, value);
		}
	}
}