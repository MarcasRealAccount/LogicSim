#pragma once

#include "Graph.h"
#include "TruthTable.h"
#include "Utils/Flags.h"
#include "Utils/Log.h"
#include "Utils/Utils.h"

#include <string>
#include <string_view>
#include <utility>

struct NamespaceName
{
public:
	constexpr NamespaceName(std::string_view name)
	{
		std::size_t namespaceEnd = name.find_first_of(':');
		if (namespaceEnd >= name.size())
		{
			m_Namespace = "default";
			m_Name      = name;
			return;
		}

		m_Namespace = name.substr(0, namespaceEnd);
		m_Name      = name.substr(namespaceEnd + 1);
	}
	constexpr NamespaceName(std::string namespace_, std::string name)
	    : m_Namespace(std::move(namespace_)), m_Name(std::move(name)) {}

	constexpr friend bool operator==(const NamespaceName& lhs, const NamespaceName& rhs)
	{
		return lhs.m_Namespace == rhs.m_Namespace && lhs.m_Name == rhs.m_Name;
	}
	constexpr friend bool operator!=(const NamespaceName& lhs, const NamespaceName& rhs) { return !(lhs == rhs); }

	std::string m_Namespace;
	std::string m_Name;
};

constexpr NamespaceName operator""_nn(const char* str, std::size_t len)
{
	return NamespaceName { std::string_view { str, len } };
}

struct Component
{
public:
	template <Callable<void, std::vector<std::string>&, std::vector<std::string>&> F>
	Component(NamespaceName name, F&& portNameFiller)
	    : m_Name(std::move(name)),
	      m_TruthTable(nullptr),
	      m_Graph(nullptr)
	{
		portNameFiller(m_InputNames, m_OutputNames);
	}
	Component(const Component& copy) = delete;
	Component(Component&& move) noexcept
	    : m_Name(std::move(move.m_Name)),
	      m_InputNames(std::move(move.m_InputNames)),
	      m_OutputNames(std::move(move.m_OutputNames)),
	      m_TruthTable(move.m_TruthTable),
	      m_Graph(move.m_Graph)
	{
		move.m_TruthTable = nullptr;
		move.m_Graph      = nullptr;
	}
	Component& operator=(const Component& copy) = delete;
	Component& operator=(Component&& move) noexcept
	{
		m_Name            = std::move(move.m_Name);
		m_InputNames      = std::move(move.m_InputNames);
		m_OutputNames     = std::move(move.m_OutputNames);
		m_TruthTable      = move.m_TruthTable;
		m_Graph           = move.m_Graph;
		move.m_TruthTable = nullptr;
		move.m_Graph      = nullptr;
		return *this;
	}
	~Component()
	{
		delete m_TruthTable;
		delete m_Graph;
	}

	template <Callable<TruthTable> F>
	void setTruthTable(F&& truthTableProvider)
	{
		if (m_TruthTable)
		{
			Log::Warn("Trying to override truth table for component {}:{};{}", m_Name.m_Namespace, m_Name.m_Name, inputCount());
			return;
		}

		TruthTable temp = truthTableProvider();
		if (temp.inputCount() != inputCount() && temp.outputCount() != outputCount())
		{
			Log::Warn("Trying to set truth table with incorrect input and output count, received {}/{}, requires {}/{}", temp.inputCount(), temp.outputCount(), inputCount(), outputCount());
			return;
		}
		m_TruthTable = new TruthTable { std::move(temp) };
	}

	template <Callable<Graph> F>
	void setGraph(F&& graphProvider)
	{
		if (m_Graph)
		{
			Log::Warn("Trying to override graph for component {}:{};{}", m_Name.m_Namespace, m_Name.m_Name, inputCount());
			return;
		}

		Graph temp = graphProvider();
		if (temp.inputCount() != inputCount() && temp.outputCount() != outputCount())
		{
			Log::Warn("Trying to set graph with incorrect input and output count, received {}/{}, requires {}/{}", temp.inputCount(), temp.outputCount(), inputCount(), outputCount());
			return;
		}
		m_Graph = new Graph { std::move(temp) };
	}

	std::size_t inputCount() const { return m_InputNames.size(); }
	std::size_t outputCount() const { return m_OutputNames.size(); }

	auto&             getName() const { return m_Name; }
	auto&             getInputNames() const { return m_InputNames; }
	auto&             getOutputNames() const { return m_OutputNames; }
	bool              hasTruthTable() const { return m_TruthTable; }
	bool              hasGraph() const { return m_Graph; }
	TruthTable&       getTruthTable() { return *m_TruthTable; }
	const TruthTable& getTruthTable() const { return *m_TruthTable; }
	Graph&            getGraph() { return *m_Graph; }
	const Graph&      getGraph() const { return *m_Graph; }

private:
	NamespaceName            m_Name;
	std::vector<std::string> m_InputNames;
	std::vector<std::string> m_OutputNames;

	TruthTable* m_TruthTable;
	Graph*      m_Graph;
};