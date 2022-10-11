libs      = libs      or {}
libs.glad = libs.glad or {
	name     = "",
	location = ""
}

local glad = libs.glad

function glad:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	includedirs({ self.location .. "/include/" })

	files({ self.location .. "/**" })

	filter("system:windows")
		links({ "opengl32" })
	filter("system:macosx")
		links({ "OpenGL.framework" })
	filter({})
end

function glad:setupDep()
	links({ self.name })
	externalincludedirs({ self.location .. "/include/" })
end