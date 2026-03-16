local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")

local output = superfamiconv.convert_tilemap(
	"background.png",
	superfamiconv.config()
		:mode("ws"):bpp(2)
		:tile_base(0):palette_base(0)
)

process.emit_symbol("background_mono_tiles", output.tiles)
process.emit_symbol("background_mono_map", output.map)

output = superfamiconv.convert_tilemap(
	"background.png",
	superfamiconv.config()
		:mode("wsc"):bpp(2)
		:tile_base(0):palette_base(0)
)

process.emit_symbol("background_color_tiles", output.tiles)
process.emit_symbol("background_color_map", output.map)
process.emit_symbol("background_color_palette", output.palette)
