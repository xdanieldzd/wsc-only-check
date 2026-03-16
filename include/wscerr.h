#pragma once

#include <wonderful.h>
#include <ws.h>
#include <wse.h>
#include <wsx/lzsa.h>

#include "wscerr_config.h"

void wscerr_show_error_screen(void);

struct wscerr_string
{
	const uint8_t x;
	const uint8_t y;
	const char ptr[WS_DISPLAY_WIDTH_TILES + 1];
};

#define WSCERR_STRING(_x, _y, _ptr)	\
{								\
	.x = _x,						\
	.y = _y,						\
	.ptr = _ptr					\
}

#define WSCERR_SET_PALETTE2(_c0, _c1, _c2, _c3)		WS_DISPLAY_MONO_PALETTE(_c0, _c1, _c2, _c3)
#define WSCERR_SET_PALETTE(_params)				WSCERR_SET_PALETTE2(_params)
