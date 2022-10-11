require("Premake/Common")

require("Premake/ThirdParty/glm")
require("Premake/ThirdParty/glad")
require("Premake/ThirdParty/glfw")
require("Premake/ThirdParty/spdlog")
require("Premake/ThirdParty/stb")

workspace("LogicSim")
	common:setConfigsAndPlatforms()
	common:addCoreDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("Off")
	flags("MultiProcessorCompile")

	startproject("LogicSim")

	group("Dependencies")
	project("GLFW")
		location("ThirdParty/GLFW/")
		warnings("Off")
		libs.glfw:setup()
		location("ThirdParty/")

	project("SpdLog")
		location("ThirdParty/SpdLog/")
		warnings("Off")
		libs.spdlog:setup()
		location("ThirdParty/")

	project("glad")
		location("ThirdParty/glad/")
		warnings("Off")
		libs.glad:setup()
		location("ThirdParty/")

	project("GLM")
		location("ThirdParty/GLM/")
		warnings("Off")
		libs.glm:setup()
		location("ThirdParty/")

	project("STB")
		location("ThirdParty/STB/")
		warnings("Off")
		libs.stb:setup()
		location("ThirdParty/")

	group("Apps")
	project("LogicSim")
		location("LogicSim/")
		warnings("Extra")

		common:outDirs()
		common:debugDir()

		filter("configurations:Debug")
			kind("ConsoleApp")

		filter("configurations:not Debug")
			kind("WindowedApp")

		filter({})

		includedirs({ "%{prj.location}/Src/" })
		files({ "%{prj.location}/Src/**" })
		removefiles({ "*.DS_Store" })

		libs.glm:setupDep()
		libs.glad:setupDep()
		libs.glfw:setupDep()
		libs.spdlog:setupDep()
		libs.stb:setupDep()

		common:addActions()
