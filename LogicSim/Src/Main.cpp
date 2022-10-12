#include "Logic/Component.h"
#include "Logic/Graph.h"
#include "Logic/GraphState.h"
#include "Logic/TruthTable.h"
#include "LogicSim.h"
#include "Utils/Core.h"
#include "Utils/Log.h"

#include <chrono>
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	auto& logicSim = LogicSim::Get();

	std::vector<std::string> builtinInputNames { "A", "B", "C", "D", "E", "F", "G", "H" };

	std::uint8_t i = 2;

	auto builtinIONameFiller = [&builtinInputNames, &i](std::vector<std::string>& inputNames, std::vector<std::string>& outputNames)
	{
		inputNames.insert(inputNames.begin(), builtinInputNames.begin(), builtinInputNames.begin() + i);
		outputNames.emplace_back("Q");
	};

	for (i = 2; i < 9; ++i)
	{
		auto andC = logicSim.newComponent("builtin:and"_nn, builtinIONameFiller);
		andC->setTruthTable(
		    [i]() -> TruthTable
		    {
			    return TruthTable {
				    i, 1,
				    [i](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
				    {
				        std::size_t Q = 1;
				        for (std::size_t j = 0; j < i; ++j)
					        Q *= (inputs >> j) & 1;
				        bitSet.set(bit, Q);
				    }
			    };
		    });

		auto orC = logicSim.newComponent("builtin:or"_nn, builtinIONameFiller);
		orC->setTruthTable(
		    [i]() -> TruthTable
		    {
			    return TruthTable {
				    i, 1,
				    [i](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
				    {
				        std::size_t Q = 0;
				        for (std::size_t j = 0; j < i; ++j)
					        Q += (inputs >> j) & 1;
				        bitSet.set(bit, Q >= 1);
				    }
			    };
		    });

		auto nandC = logicSim.newComponent("builtin:nand"_nn, builtinIONameFiller);
		nandC->setTruthTable(
		    [i]() -> TruthTable
		    {
			    return TruthTable {
				    i, 1,
				    [i](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
				    {
				        std::size_t Q = 1;
				        for (std::size_t j = 0; j < i; ++j)
					        Q *= (inputs >> j) & 1;
				        bitSet.set(bit, !Q);
				    }
			    };
		    });

		auto norC = logicSim.newComponent("builtin:nor"_nn, builtinIONameFiller);
		norC->setTruthTable(
		    [i]() -> TruthTable
		    {
			    return TruthTable {
				    i, 1,
				    [i](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
				    {
				        std::size_t Q = 0;
				        for (std::size_t j = 0; j < i; ++j)
					        Q += (inputs >> j) & 1;
				        bitSet.set(bit, Q == 0);
				    }
			    };
		    });

		auto xorC = logicSim.newComponent("builtin:xor"_nn, builtinIONameFiller);
		xorC->setTruthTable(
		    [i]() -> TruthTable
		    {
			    return TruthTable {
				    i, 1,
				    [i](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
				    {
				        std::size_t Q = 0;
				        for (std::size_t j = 0; j < i; ++j)
					        Q += (inputs >> j) & 1;
				        bitSet.set(bit, Q == 1);
				    }
			    };
		    });

		auto xnorC = logicSim.newComponent("builtin:xnor"_nn, builtinIONameFiller);
		xnorC->setTruthTable(
		    [i]() -> TruthTable
		    {
			    return TruthTable {
				    i, 1,
				    [i](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
				    {
				        std::size_t Q = 0;
				        for (std::size_t j = 0; j < i; ++j)
					        Q += (inputs >> j) & 1;
				        bitSet.set(bit, Q != 1);
				    }
			    };
		    });
	}

	logicSim.removeComponent(logicSim.getComponent("builtin:and"_nn, 3));

	for (auto comp : logicSim.getComponents())
	{
		Log::Trace("{} => {}:{};{}", comp.index(), comp->getName().m_Namespace, comp->getName().m_Name, comp->inputCount());
	}

	//Component andC {
	//	TruthTable {
	//	    2, 1,
	//	    [](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
	//	    {
	//	        bool A = inputs & 1;
	//	        bool B = (inputs >> 1) & 1;
	//	        bitSet.set(bit, A && B);
	//	    } }
	//};
	//Component orC {
	//	TruthTable {
	//	    2, 1,
	//	    [](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
	//	    {
	//	        bool A = inputs & 1;
	//	        bool B = (inputs >> 1) & 1;
	//	        bitSet.set(bit, A || B);
	//	    } }
	//};
	//Component xorC {
	//	TruthTable {
	//	    2, 1,
	//	    [](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
	//	    {
	//	        bool A = inputs & 1;
	//	        bool B = (inputs >> 1) & 1;
	//	        bitSet.set(bit, A ^ B);
	//	    } }
	//};

	//Component fullAdder {
	//	[&]() -> Graph
	//	{
	//	    // 0: A
	//	    // 1: B
	//	    // 2: Cin
	//	    // 3: S
	//	    // 4: Cout
	//	    Graph graph { 3, 2 };
	//	    auto  xor_1 = graph.newNode(xorC);
	//	    auto  xor_2 = graph.newNode(xorC);
	//	    auto  and_1 = graph.newNode(andC);
	//	    auto  and_2 = graph.newNode(andC);
	//	    auto  or_1  = graph.newNode(orC);
	//	    graph.connect({ {}, 0 }, { xor_1, 0 });    // A -> xor_1:A
	//	    graph.connect({ {}, 1 }, { xor_1, 1 });    // B -> xor_1:B
	//	    graph.connect({ xor_1, 2 }, { xor_2, 0 }); // xor_1:Q -> xor_2:A
	//	    graph.connect({ {}, 2 }, { xor_2, 1 });    // Cin -> xor_2:B
	//	    graph.connect({ xor_2, 2 }, { {}, 3 });    // xor_2:Q -> S
	//	    graph.connect({ xor_1, 2 }, { and_1, 0 }); // xor_1:Q -> and_1:A
	//	    graph.connect({ {}, 2 }, { and_1, 1 });    // Cin -> and_1:B
	//	    graph.connect({ {}, 0 }, { and_2, 0 });    // A -> and_2:A
	//	    graph.connect({ {}, 1 }, { and_2, 1 });    // B -> and_2:B
	//	    graph.connect({ and_1, 2 }, { or_1, 0 });  // and_1:Q -> or_1:A
	//	    graph.connect({ and_2, 2 }, { or_1, 1 });  // and_2:Q -> or_1:B
	//	    graph.connect({ or_1, 2 }, { {}, 4 });     // or_1:Q -> Cout
	//	    return graph;
	//	}
	//};

	//Component fullAdder4 {
	//	TruthTable {
	//	    9, 5,
	//	    [](std::uint16_t inputs, std::size_t bit, BitSet& bitSet)
	//	    {
	//	        std::uint8_t a    = inputs & 0xF;
	//	        std::uint8_t b    = (inputs >> 4) & 0xF;
	//	        std::uint8_t c_in = (inputs >> 8) & 1;

	//	        std::uint8_t s     = a + b + c_in;
	//	        std::uint8_t c_out = s >> 4 & 1;
	//	        s &= 0xF;
	//	        bitSet.setBits({ static_cast<std::uint8_t>(s | (c_out << 4)) }, 0, bit, 5);
	//	    } }
	//};

	//Component fullAdder8 {
	//	[&]() -> Graph
	//	{
	//	    Graph graph { 17, 9 };
	//	    auto  add_1 = graph.newNode(fullAdder4);
	//	    auto  add_2 = graph.newNode(fullAdder4);
	//	    graph.connect({ {}, 16 }, { add_1, 8 });
	//	    for (std::size_t i = 0; i < 2; ++i)
	//	    {
	//		    if (i > 0 && i < 2)
	//			    graph.connect({ { add_1.pool(), add_1.index() + (i - 1) }, 13 }, { { add_1.pool(), add_1.index() + i }, 8 });
	//		    for (std::size_t j = 0; j < 4; ++j)
	//		    {
	//			    graph.connect({ {}, 4 * i + j }, { { add_1.pool(), add_1.index() + i }, j });
	//			    graph.connect({ {}, 8 + 4 * i + j }, { { add_1.pool(), add_1.index() + i }, 4 + j });
	//			    graph.connect({ { add_1.pool(), add_1.index() + i }, 9 + j }, { {}, 17 + 4 * i + j });
	//		    }
	//	    }
	//	    graph.connect({ add_2, 13 }, { {}, 25 });
	//	    return graph;
	//	}
	//};

	//Graph graph { 257, 129 };
	//{
	//	auto add_0 = graph.newNode(fullAdder8);
	//	auto add_1 = graph.newNode(fullAdder8);
	//	auto add_2 = graph.newNode(fullAdder8);
	//	auto add_3 = graph.newNode(fullAdder8);

	//	auto add_4 = graph.newNode(fullAdder8);
	//	auto add_5 = graph.newNode(fullAdder8);
	//	auto add_6 = graph.newNode(fullAdder8);
	//	auto add_7 = graph.newNode(fullAdder8);

	//	auto add_8  = graph.newNode(fullAdder8);
	//	auto add_9  = graph.newNode(fullAdder8);
	//	auto add_10 = graph.newNode(fullAdder8);
	//	auto add_11 = graph.newNode(fullAdder8);

	//	auto add_12 = graph.newNode(fullAdder8);
	//	auto add_13 = graph.newNode(fullAdder8);
	//	auto add_14 = graph.newNode(fullAdder8);
	//	auto add_15 = graph.newNode(fullAdder8);
	//	graph.connect({ {}, 256 }, { add_0, 16 });
	//	for (std::size_t i = 0; i < 16; ++i)
	//	{
	//		if (i > 0 && i < 16)
	//			graph.connect({ { add_0.pool(), add_0.index() + (i - 1) }, 25 }, { { add_0.pool(), add_0.index() + i }, 16 });
	//		for (std::size_t j = 0; j < 8; ++j)
	//		{
	//			graph.connect({ {}, 8 * i + j }, { { add_0.pool(), add_0.index() + i }, j });
	//			graph.connect({ {}, 128 + 8 * i + j }, { { add_0.pool(), add_0.index() + i }, 8 + j });
	//			graph.connect({ { add_0.pool(), add_0.index() + i }, 17 + j }, { {}, 257 + 8 * i + j });
	//		}
	//	}
	//	graph.connect({ add_15, 25 }, { {}, 385 });
	//}
	//GraphState state { graph };

	//Log::Critical("Total size of 128 bit full adder: {}", graph.totalSizeOf());
	//Log::Critical("Total size of 128 bit full adder state: {}", state.totalSizeOf());

	//state.setInputs({ 0xAE, 0x01, 0x07, 0xCE, 0x00, 0xFF, 0x81, 0x56, 0xEF, 0x23, 0x7C, 0x5E, 0x45, 0x09, 0xED, 0xCF, 0x23, 0xFE, 0x00, 0xCB, 0x4E, 0xB7, 0xF4, 0x00, 0x20, 0xC8, 0xE9, 0xD5, 0xF7, 0x91, 0x21, 0xF6, 0b0 });
	//auto start = std::chrono::high_resolution_clock::now();
	//state.tick();
	//auto end = std::chrono::high_resolution_clock::now();
	//Log::Critical("Tick speed {} ms", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start).count());
}

#if BUILD_IS_SYSTEM_WINDOWS

#undef APIENTRY
#include <Windows.h>

int WinMain([[maybe_unused]] _In_ HINSTANCE hInstance, [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance, [[maybe_unused]] _In_ LPSTR lpszCmdLine, [[maybe_unused]] _In_ int nCmdShow)
{
	return main(__argc, __argv);
}

#endif