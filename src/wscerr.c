/*
	wscerr.c -- a quick and dirty "only for WonderSwan Color" screen, for Color-only games booted on a monochrome system
	March 16th 2026 by xdaniel -- https://bsky.app/profile/xdaniel.neocities.org

	--------------------------------------------------------------------------------

	MIT License

	Copyright (c) 2026 xdaniel (Daniel R.)

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "wscerr.h"

#include WSCERR_FONT_FILENAME
#if defined(WSCERR_ENABLE_BACKGROUND)
#	include WSCERR_BACKGROUND_FILENAME
#endif

#define ARRAY_LENGTH(array)			sizeof(array) / sizeof(array[0])

#if defined(WSCERR_ENABLE_TEXT_BLINK)
#	define APPROX_REFRESH_RATE		(12000 / WS_DISPLAY_VTOTAL)
#	define TEXT_DURATION_VISIBLE		(APPROX_REFRESH_RATE * 2)
#	define TEXT_DURATION_INVISIBLE	(APPROX_REFRESH_RATE / 5)
#	define TEXT_DURATION_TOTAL		(TEXT_DURATION_VISIBLE + TEXT_DURATION_INVISIBLE)
#endif

#if defined(WSCERR_ENABLE_BACKGROUND)
const static uint16_t first_font_tile = (uint16_t)(((WSCERR_BACKGROUND_TILES_SIZE * (uint32_t)256) / WSCERR_BACKGROUND_TILES_SIZE) / (32 / sizeof(uint16_t)));
#else
const static uint16_t first_font_tile = 0x000;
#endif

const ws_rom struct wscerr_string wscerr_error_messages[] =
{
	WSCERR_ERROR_MESSAGE_1,
#ifdef WSCERR_ERROR_MESSAGE_2
	WSCERR_ERROR_MESSAGE_2,
#endif
#ifdef WSCERR_ERROR_MESSAGE_3
	WSCERR_ERROR_MESSAGE_3,
#endif
#ifdef WSCERR_ERROR_MESSAGE_4
	WSCERR_ERROR_MESSAGE_4,
#endif
#ifdef WSCERR_ERROR_MESSAGE_5
	WSCERR_ERROR_MESSAGE_5,
#endif
};

#if defined(WSCERR_ENABLE_TEXT_BLINK)
static uint8_t is_text_visible = 0;
#endif

static volatile uint32_t vblank_counter = 0;

static __attribute__((assume_ss_data, interrupt)) void __far wscerr_vblank_handler(void);

void wscerr_show_error_screen(void)
{
	/* Setup screens */
	ws_display_set_screen_addresses(&wse_screen1, &wse_screen2);
	ws_display_scroll_screen1_to(0, 0);
	ws_display_scroll_screen2_to(-WSCERR_ERROR_MESSAGES_POSITION_X, -WSCERR_ERROR_MESSAGES_POSITION_Y);

	/* Clear screen 2 */
	ws_screen_fill_tiles(&wse_screen2, first_font_tile | WS_SCREEN_ATTR_PALETTE(4), 0, 0, 32, 32);

	/* Copy tiles and screen 1 map */
#if defined(WSCERR_ENABLE_BACKGROUND)
	memcpy(WS_TILE_MEM(0), WSCERR_BACKGROUND_TILES, WSCERR_BACKGROUND_TILES_SIZE);
	memcpy(&wse_screen1, WSCERR_BACKGROUND_MAP, WSCERR_BACKGROUND_MAP_SIZE);
	memcpy(WS_TILE_MEM(first_font_tile), WSCERR_FONT_TILES, WSCERR_FONT_TILES_SIZE);
#else
	ws_screen_fill_tiles(&wse_screen1, first_font_tile | WS_SCREEN_ATTR_PALETTE(0), 0, 0, 32, 32);
	memcpy(WS_TILE_MEM(first_font_tile), WSCERR_FONT_TILES, WSCERR_FONT_TILES_SIZE);
#endif

	/* Setup palettes */
	ws_display_set_shade_lut(WS_DISPLAY_SHADE_LUT_DEFAULT);
	ws_portcpy(WS_SCR_PAL_4_PORT, WSCERR_FONT_PALETTE, WSCERR_FONT_PALETTE_SIZE);
	outportw(WS_SCR_PAL_0_PORT, WSCERR_SET_PALETTE(WSCERR_BACKGROUND_SHADES));

	/* Print error messages to screen 2 */
	for (uint8_t i = 0; i < ARRAY_LENGTH(wscerr_error_messages); i++)
	{
		uint8_t x = wscerr_error_messages[i].x, y = wscerr_error_messages[i].y;
		for (const ws_rom char* ptr = wscerr_error_messages[i].ptr; *ptr != '\0'; ptr++)
		{
			if (*ptr != ' ') ws_screen_put_tile(&wse_screen2, ((*ptr - 0x20) + first_font_tile) | WS_SCREEN_ATTR_PALETTE(4), x, y);
			x++;
		}
	}

	/* Enable screens */
	ws_display_set_control(WS_DISPLAY_CTRL_SCR1_ENABLE | WS_DISPLAY_CTRL_SCR2_ENABLE);

	/* Setup Vblank interrupt */
	ws_int_set_handler(WS_INT_VBLANK, wscerr_vblank_handler);
	ws_int_enable(WS_INT_ENABLE_VBLANK);
	ia16_enable_irq();

	/* Infinite loop! */
#if defined(WSCERR_ENABLE_BACKGROUND) && defined(WSCERR_ENABLE_BACKGROUND_SCROLL) && WSCERR_BACKGROUND_SCROLL_SPEED != 0
	static uint16_t scroll = 0;
#endif
	while (1)
	{
#if defined(WSCERR_ENABLE_BACKGROUND) && defined(WSCERR_ENABLE_BACKGROUND_SCROLL) && WSCERR_BACKGROUND_SCROLL_SPEED != 0
		/* Scroll background */
		scroll += WSCERR_BACKGROUND_SCROLL_SPEED;
		ws_display_scroll_screen1_to(scroll >> 8, scroll >> 8);
#endif

#if defined(WSCERR_ENABLE_TEXT_BLINK)
		/* Handle text blinking */
		uint8_t prev_is_text_visible = is_text_visible;
		is_text_visible = ((vblank_counter % TEXT_DURATION_TOTAL) < TEXT_DURATION_VISIBLE);
		if (prev_is_text_visible != is_text_visible)
			ws_display_set_control(WS_DISPLAY_CTRL_SCR1_ENABLE | (is_text_visible ? WS_DISPLAY_CTRL_SCR2_ENABLE : 0));
#endif

		/* Wait for Vblank */
		uint32_t prev_vblank_counter = vblank_counter;
		while (prev_vblank_counter == vblank_counter) ia16_halt();
	}
}

static __attribute__((assume_ss_data, interrupt)) void __far wscerr_vblank_handler(void)
{
	vblank_counter++;

	ws_int_ack(WS_INT_ACK_VBLANK);
	ia16_enable_irq();
}
