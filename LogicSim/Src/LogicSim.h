#pragma once

#include "Logic/Component.h"
#include "ResourceManager/ResourceManager.h"

#include <unordered_map>

class LogicSim
{
public:
	using ComponentRef = ResourceManager::ResourcePool<Component>::Ref;

public:
	static LogicSim& Get();
	static void      Destroy();

public:
	template <Callable<void, std::vector<std::string>&, std::vector<std::string>&> F>
	ComponentRef newComponent(NamespaceName name, F&& portNameFiller)
	{
		return m_Components.emplaceBack(std::move(name), std::forward<F>(portNameFiller));
	}
	void removeComponent(ComponentRef component)
	{
		m_Components.erase(component.index());
	}

	ComponentRef getComponent(NamespaceName name, std::size_t minInputCount);
	auto&        getComponents() const { return m_Components; }

private:
	LogicSim()  = default;
	~LogicSim() = default;

private:
	ResourceManager::ResourcePool<Component> m_Components;
};