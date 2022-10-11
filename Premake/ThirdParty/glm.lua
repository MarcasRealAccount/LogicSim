libs     = libs     or {}
libs.glm = libs.glm or {
	name       = "",
	location   = ""
}

local glm = libs.glm

function glm:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	includedirs({
		self.location,
		self.location .. "/glm/"
	})

	files({ self.location .. "/glm/**" })
end

function glm:setupDep()
	externalincludedirs({ self.location })
end