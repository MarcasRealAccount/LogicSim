#include "Graph.h"
#include "Component.h"
#include "GraphState.h"

Node::Node(ResourceManager::Ref<Component> component)
    : m_Component(component),
      m_InputPorts(component->inputCount(), ~0ULL),
      m_OutputPorts(component->outputCount(), ~0ULL) {}

Graph::NodeRef Graph::newNode(ResourceManager::Ref<Component> component)
{
	return m_Nodes.emplaceBack(component);
}

void Graph::removeNode(NodeRef node)
{
	m_Nodes.erase(node.index());
}

void Graph::connect(Port a, Port b)
{
	if (a.m_Node == b.m_Node && a.m_Port == b.m_Port)
		return;

	std::size_t aConnection = getPortConnection(a);
	std::size_t bConnection = getPortConnection(b);

	if (aConnection == ~0ULL && bConnection == ~0ULL)
	{
		aConnection = m_NumConnections;
		++m_NumConnections;
	}
	else if (bConnection != ~0ULL && aConnection == ~0ULL)
	{
		aConnection = bConnection;
	}
	else if (aConnection != ~0ULL && bConnection != ~0ULL)
	{
		// Merge connections, go with lowest
		std::size_t connection      = std::min<std::size_t>(aConnection, bConnection);
		std::size_t largeConnection = std::max<std::size_t>(aConnection, bConnection);
		aConnection                 = connection;
		for (std::size_t i = 0; i < m_InputPorts.size(); ++i)
			if (m_InputPorts[i] == largeConnection)
				m_InputPorts[i] = connection;
			else if (m_InputPorts[i] > largeConnection)
				--m_InputPorts[i];
		for (std::size_t i = 0; i < m_OutputPorts.size(); ++i)
			if (m_OutputPorts[i] == largeConnection)
				m_OutputPorts[i] = connection;
			else if (m_OutputPorts[i] > largeConnection)
				--m_OutputPorts[i];
		for (auto& node : m_Nodes)
		{
			auto& inputPorts  = node->getInputPorts();
			auto& outputPorts = node->getOutputPorts();
			for (std::size_t i = 0; i < inputPorts.size(); ++i)
				if (inputPorts[i] == largeConnection)
					inputPorts[i] = connection;
				else if (inputPorts[i] > largeConnection)
					--inputPorts[i];
			for (std::size_t i = 0; i < outputPorts.size(); ++i)
				if (outputPorts[i] == largeConnection)
					outputPorts[i] = connection;
				else if (outputPorts[i] > largeConnection)
					--outputPorts[i];
		}
	}

	setPortConnection(a, aConnection);
	setPortConnection(b, aConnection);
}

void Graph::disconnect(Port a, Port b)
{
	if (a.m_Node == b.m_Node && a.m_Port == b.m_Port)
		return;

	std::size_t aConnection = getPortConnection(a);
	std::size_t bConnection = getPortConnection(b);

	if (aConnection != bConnection)
		return;

	// TODO(MarcasRealAccount): Implement
}

TruthTable Graph::compile() const
{
	if (!compilable())
		return TruthTable { 1, 1, [](std::uint16_t inputs, std::size_t bit, BitSet& outputs)
			                {
			                    outputs.set(bit, inputs);
			                } };

	GraphState state { *this };
	return TruthTable { static_cast<std::uint8_t>(inputCount()), outputCount(), [&](std::uint16_t inputs, std::size_t bit, BitSet& outputs)
		                {
		                    state.setInputs({ static_cast<std::uint8_t>(inputs), static_cast<std::uint8_t>(inputs >> 8) });
		                    // TODO(MarcasRealAccount): Maybe make Graph compilation better?
		                    state.tick();
		                    state.getOutputs(outputs, bit);
		                } };
}

void Graph::setPortConnection(Port port, std::size_t connection)
{
	if (port.m_Node.valid())
	{
		auto& r = *port.m_Node;

		std::size_t inputLength = r.inputCount();
		if (port.m_Port < inputLength)
			r.getInputPorts()[port.m_Port] = connection;
		else
			r.getOutputPorts()[port.m_Port - inputLength] = connection;
	}
	else
	{
		std::size_t inputLength = inputCount();
		if (port.m_Port < inputLength)
			m_InputPorts[port.m_Port] = connection;
		else
			m_OutputPorts[port.m_Port - inputLength] = connection;
	}
}

std::size_t Graph::getPortConnection(Port port) const
{
	if (port.m_Node.valid())
	{
		auto& r = *port.m_Node;

		std::size_t inputLength = r.inputCount();
		if (port.m_Port < inputLength)
			return r.getInputPorts()[port.m_Port];
		else
			return r.getOutputPorts()[port.m_Port - inputLength];
	}
	else
	{
		std::size_t inputLength = inputCount();
		if (port.m_Port < inputLength)
			return m_InputPorts[port.m_Port];
		else
			return m_OutputPorts[port.m_Port - inputLength];
	}
}