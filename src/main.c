/*
	Example program for wscerr.c
	March 16th 2026 by xdaniel -- https://bsky.app/profile/xdaniel.neocities.org
*/

#include <wonderful.h>
#include <ws.h>

#include "wscerr.h"

#include "background.h"
#include "font.h"

static uint8_t map_range(uint8_t, const uint8_t, const uint8_t, const uint8_t, const uint8_t);
static uint8_t sin(uint8_t);

const static ws_rom char greeting_text[] = "Hello, this is the Color part!";
const static uint16_t first_font_tile = (uint16_t)(((background_color_tiles_size * (uint32_t)256) / background_color_tiles_size) / (32 / sizeof(uint16_t)));

WSE_RESERVE_TILES(512, 1024);

void main(void)
{
	/* Disable all layers */
	ws_display_set_control(0);

	/* Reset screen 1 & 2 scroll */
	ws_display_scroll_screen1_to(0, 0);
	ws_display_scroll_screen2_to(0, 0);

	/* Set screen 1 & 2 addresses */
	ws_display_set_screen_addresses(&wse_screen1, &wse_screen2);

	/* Check for system model and try to set color mode */
	if (ws_system_is_color_model() && ws_system_set_mode(WS_MODE_COLOR))
	{
		/* Color mode switch succeeded, so setup the color part of this example */

		ws_screen_fill_tiles(&wse_screen1, 0x000 | WS_SCREEN_ATTR_PALETTE(0), 0, 0, 32, 32);
		ws_screen_fill_tiles(&wse_screen2, 0x000 | WS_SCREEN_ATTR_PALETTE(4), 0, 0, 32, 32);

		ws_gdma_copy(WS_SCREEN_COLOR_MEM(0), background_color_palette, background_color_palette_size);
		ws_gdma_copy(WS_SCREEN_COLOR_MEM(4), font_color_palette, font_color_palette_size);

#if defined(WSCERR_ENABLE_COMPRESSION_FONT_TILES)
		wsx_lzsa2_decompress(WS_TILE_MEM(first_font_tile), font_tiles);
#else
		ws_gdma_copy(WS_TILE_MEM(first_font_tile), font_tiles, font_tiles_size);
#endif

#if defined(WSCERR_ENABLE_COMPRESSION_BACKGROUND_TILES)
		wsx_lzsa2_decompress(WS_TILE_MEM(0), background_color_tiles);
#else
		ws_gdma_copy(WS_TILE_MEM(0), background_color_tiles, background_color_tiles_size);
#endif

#if defined(WSCERR_ENABLE_COMPRESSION_BACKGROUND_MAP)
		wsx_lzsa2_decompress(&wse_screen1, background_color_map);
#else
		ws_gdma_copy(&wse_screen1, background_color_map, background_color_map_size);
#endif

		uint8_t x = 0;
		for (const ws_rom char* ptr = greeting_text; *ptr != '\0'; ptr++)
		{
			if (*ptr != ' ') ws_screen_put_tile(&wse_screen2, ((*ptr - 0x20) + first_font_tile) | WS_SCREEN_ATTR_PALETTE(4), x, 0);
			x++;
		}

		ws_int_set_default_handler_vblank();
		ws_int_enable(WS_INT_ENABLE_VBLANK);
		ia16_enable_irq();

		ws_display_set_control(WS_DISPLAY_CTRL_SCR1_ENABLE | WS_DISPLAY_CTRL_SCR2_ENABLE);

		uint8_t text_y_scroll = -8;
		while (1)
		{
			ws_display_scroll_screen1_by(1, -1);

			/* Just having some fun with the text~ */
			ws_display_scroll_screen2_to(inportb(WS_SCR2_SCRL_X_PORT) - 1, -map_range(sin(text_y_scroll++), 0, 255, (WS_DISPLAY_TILE_HEIGHT * 4), WS_DISPLAY_HEIGHT_PIXELS - (WS_DISPLAY_TILE_HEIGHT * 4) - 8));

			ia16_halt();
		}
	}
	else
	{
		/* Switch to color mode did NOT succeed, thus run wscerr's error handling */
		wscerr_show_error_screen();
	}
}

static uint8_t map_range(uint8_t x, const uint8_t old_min, const uint8_t old_max, const uint8_t new_min, const uint8_t new_max)
{
  return (x - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;
}

/* Table generated in C#: sin_table[i] = (byte)(128f * float.Sin(2.0f * MathF.PI * i / 256f) + 128f); */
static const ws_rom uint8_t sin_table[64] =
{
	0x80, 0x83, 0x86, 0x89, 0x8C, 0x8F, 0x92, 0x95, 0x98, 0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAB, 0xAE,
	0xB0, 0xB3, 0xB6, 0xB9, 0xBC, 0xBF, 0xC1, 0xC4, 0xC7, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD8,
	0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xED, 0xEF, 0xF0, 0xF2, 0xF3, 0xF5,
	0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFC, 0xFD, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static uint8_t sin(uint8_t x)
{
    if (x >= 192) return (256 - sin_table[255 - x]);
    else if (x >= 128) return (256 - sin_table[x - 128]);
    else if (x >= 64) return sin_table[127 - x];
    else return sin_table[x];
}
