local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")
local lzsa = require("wf.api.v1.process.tools.lzsa")

local output = superfamiconv.convert_tileset(
	"font.png",
	superfamiconv.config()
		:mode("ws")
)

output.tiles = lzsa.compress2(output.tiles)

process.emit_symbol("font_tiles", output.tiles)
process.emit_symbol("font_mono_palette", output.palette)

output = superfamiconv.convert_tileset(
	"font.png",
	superfamiconv.config()
		:mode("wsc"):bpp(2)
)

process.emit_symbol("font_color_palette", output.palette)
