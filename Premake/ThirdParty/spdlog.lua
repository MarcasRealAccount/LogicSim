libs        = libs        or {}
libs.spdlog = libs.spdlog or {
	name       = "",
	location   = ""
}

local spdlog = libs.spdlog

function spdlog:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	defines({ "SPDLOG_COMPILED_LIB", "FMT_CONSTEVAL=" })

	externalincludedirs({ self.location .. "/include/" })
	includedirs({ self.location .. "/include/" })

	files({
		self.location .. "/include/**",
		self.location .. "/src/**"
	})
end

function spdlog:setupDep()
	links({ self.name })
	externalincludedirs({ self.location .. "/include/" })

	defines({ "SPDLOG_COMPILED_LIB", "FMT_CONSTEVAL=" })
end