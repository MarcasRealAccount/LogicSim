libs     = libs     or {}
libs.stb = libs.stb or {
	location = ""
}

local stb = libs.stb

function stb:setup()
	self.location = common:projectLocation()

	kind("Utility")

	includedirs({ self.location })

	files({ self.location .. "/stb_image.h" })
end

function stb:setupDep()
	externalincludedirs({ self.location })
end