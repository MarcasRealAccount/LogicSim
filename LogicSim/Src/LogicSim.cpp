#include "LogicSim.h"

LogicSim& LogicSim::Get()
{
	static LogicSim* s_Instance = new LogicSim();
	return *s_Instance;
}

void LogicSim::Destroy()
{
	delete &Get();
}

LogicSim::ComponentRef LogicSim::getComponent(NamespaceName name, std::size_t minInputCount)
{
	ComponentRef closestComp;
	std::size_t  closestInputCount = ~0ULL;
	for (auto comp : m_Components)
	{
		if (comp->getName() != name)
			continue;

		std::size_t inputCount = comp->inputCount();
		if (inputCount >= minInputCount && inputCount < closestInputCount)
		{
			closestComp       = comp;
			closestInputCount = inputCount;
		}
	}
	return closestComp;
}