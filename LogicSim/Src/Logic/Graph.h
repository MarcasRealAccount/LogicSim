#pragma once

#include "ResourceManager/ResourceManager.h"
#include "TruthTable.h"

#include <vector>

struct Component;

struct Node
{
public:
	Node(ResourceManager::Ref<Component> component);
	Node(Node&& move) noexcept
	    : m_Component(std::move(move.m_Component)),
	      m_InputPorts(std::move(move.m_InputPorts)),
	      m_OutputPorts(std::move(move.m_OutputPorts)) {}
	Node& operator=(Node&& move) noexcept
	{
		m_Component   = std::move(move.m_Component);
		m_InputPorts  = std::move(move.m_InputPorts);
		m_OutputPorts = std::move(move.m_OutputPorts);
		return *this;
	}

	std::size_t inputCount() const { return m_InputPorts.size(); }
	std::size_t outputCount() const { return m_OutputPorts.size(); }

	auto& getComponent() const { return m_Component; }
	auto& getInputPorts() { return m_InputPorts; }
	auto& getInputPorts() const { return m_InputPorts; }
	auto& getOutputPorts() { return m_OutputPorts; }
	auto& getOutputPorts() const { return m_OutputPorts; }

private:
	ResourceManager::Ref<Component> m_Component;

	std::vector<std::size_t> m_InputPorts;
	std::vector<std::size_t> m_OutputPorts;
};

struct Port
{
public:
	Port(ResourceManager::ResourcePool<Node>::Ref node, std::size_t port)
	    : m_Node(node), m_Port(port) {}

public:
	ResourceManager::ResourcePool<Node>::Ref m_Node;

	std::size_t m_Port;
};

struct Graph
{
public:
	using NodeRef = ResourceManager::ResourcePool<Node>::Ref;

public:
	Graph(std::size_t numInputs, std::size_t numOutputs)
	    : m_InputPorts(numInputs, ~0ULL),
	      m_OutputPorts(numOutputs, ~0ULL),
	      m_NumConnections(0) {}

	NodeRef newNode(ResourceManager::Ref<Component> component);
	void    removeNode(NodeRef node);
	void    connect(Port a, Port b);
	void    disconnect(Port a, Port b);

	bool       compilable() const { return m_InputPorts.size() <= 16; }
	TruthTable compile() const;

	std::size_t inputCount() const { return m_InputPorts.size(); }
	std::size_t outputCount() const { return m_OutputPorts.size(); }
	std::size_t connectionCount() const { return m_NumConnections; }

	auto& getNodes() const { return m_Nodes; }
	auto& getInputPorts() const { return m_InputPorts; }
	auto& getOutputPorts() const { return m_OutputPorts; }

	std::size_t allocatedSizeOf() const
	{
		return m_Nodes.allocatedSizeOf() + m_InputPorts.capacity() * sizeof(std::size_t) + m_OutputPorts.capacity() * sizeof(std::size_t);
	}

	std::size_t totalSizeOf() const
	{
		return sizeof(*this) + allocatedSizeOf();
	}

private:
	void        setPortConnection(Port port, std::size_t connection);
	std::size_t getPortConnection(Port port) const;

private:
	ResourceManager::ResourcePool<Node> m_Nodes;

	std::vector<std::size_t> m_InputPorts;
	std::vector<std::size_t> m_OutputPorts;

	std::size_t m_NumConnections;
};