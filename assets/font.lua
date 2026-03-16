local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")

local output = superfamiconv.convert_tileset(
	"font.png",
	superfamiconv.config()
		:mode("ws")
)

process.emit_symbol("font_mono", output)

output = superfamiconv.convert_tileset(
	"font.png",
	superfamiconv.config()
		:mode("wsc"):bpp(2)
)

process.emit_symbol("font_color", output)
