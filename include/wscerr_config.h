#pragma once

/*
    ==== Configuration for wscerr.c ====

    -- Optional feature toggles --

    WSCERR_ENABLE_BLINKING_TEXT:                           Enables error message text blinking; total loop duration is ~3 seconds, for 1/5 of which the text is not visible
    WSCERR_ENABLE_BACKGROUND:                              Enables background graphics support; requires the information inside the respective #ifdef (i.e. WSCERR_BACKGROUND_FILENAME, WSCERR_BACKGROUND_TILES, etc.)
    WSCERR_ENABLE_BACKGROUND_SCROLL:                       Enables scrolling of background; requires background graphics to be enabled; configure speed via WSCERR_BACKGROUND_SCROLL_SPEED
    WSCERR_ENABLE_COMPRESSION_FONT_TILES:                  Enables decompression of LZSA2 compressed font graphics tile data; if disabled, assumes uncompressed data
    WSCERR_ENABLE_COMPRESSION_BACKGROUND_TILES:            Enables decompression of LZSA2 compressed background graphics tile data; if disabled, assumes uncompressed data
    WSCERR_ENABLE_COMPRESSION_BACKGROUND_MAP:              Enables decompression of LZSA2 compressed background tilemap data; if disabled, assumes uncompressed data

    -- Required settings --

    WSCERR_FONT_FILENAME:                                  Filename of font header file, as converted by wf-superfamiconv
    WSCERR_FONT_TILES:                                     \
    WSCERR_FONT_TILES_SIZE:                                |- Names of respective variables from font header
    WSCERR_FONT_PALETTE:                                   |
    WSCERR_FONT_PALETTE_SIZE:                              /

    WSCERR_ERROR_MESSAGES_POSITION_X:                      \_ X and Y position in pixels at which
    WSCERR_ERROR_MESSAGES_POSITION_Y:                      /  the error message text should appear

    WSCERR_ERROR_MESSAGE_[1-5] (2-5 optional):             Error messages to print, with X and Y coordinates in tiles; minimum of 1 line, maximum of 5 lines supported

    WSCERR_BACKGROUND_SHADES:                              Shade values for background palette; if background graphic is disabled, only the first value is used for the background color

    -- If WSCERR_ENABLE_BACKGROUND is defined --

        WSCERR_BACKGROUND_FILENAME:                        Filename of background header file, as converted by wf-superfamiconv
        WSCERR_BACKGROUND_TILES:                           \
        WSCERR_BACKGROUND_TILES_SIZE:                      |- Names of respective variables from background header
        WSCERR_BACKGROUND_MAP:                             |
        WSCERR_BACKGROUND_MAP_SIZE:                        /

    -- If WSCERR_ENABLE_BACKGROUND and WSCERR_ENABLE_BACKGROUND_SCROLL are defined --

        WSCERR_BACKGROUND_SCROLL_SPEED:                    Speed at which the background scrolls, in subpixels (ex. 256 == 1 pixel per frame, 128 == "1/2 pixel per frame" i.e. 1 pixel every two frames)
*/

#define    WSCERR_ENABLE_BLINKING_TEXT
#define    WSCERR_ENABLE_BACKGROUND
#define    WSCERR_ENABLE_BACKGROUND_SCROLL
#define    WSCERR_ENABLE_COMPRESSION_FONT_TILES
#define    WSCERR_ENABLE_COMPRESSION_BACKGROUND_TILES
#define    WSCERR_ENABLE_COMPRESSION_BACKGROUND_MAP

#define    WSCERR_FONT_FILENAME                            "font.h"
#define    WSCERR_FONT_TILES                               font_tiles
#define    WSCERR_FONT_TILES_SIZE                          font_tiles_size
#define    WSCERR_FONT_PALETTE                             font_mono_palette
#define    WSCERR_FONT_PALETTE_SIZE                        font_mono_palette_size

#define    WSCERR_ERROR_MESSAGES_POSITION_X                12
#define    WSCERR_ERROR_MESSAGES_POSITION_Y                60
#define    WSCERR_ERROR_MESSAGE_1                          WSCERR_STRING(0, 0, "Sorry, this cartridge can")
#define    WSCERR_ERROR_MESSAGE_2                          WSCERR_STRING(3, 1, "only be played on a")
#define    WSCERR_ERROR_MESSAGE_3                          WSCERR_STRING(4, 2, "WonderSwan Color!")

#ifdef WSCERR_ENABLE_BACKGROUND
#    define    WSCERR_BACKGROUND_FILENAME                  "background.h"
#    define    WSCERR_BACKGROUND_TILES                     background_mono_tiles
#    define    WSCERR_BACKGROUND_TILES_SIZE                background_mono_tiles_size
#    define    WSCERR_BACKGROUND_MAP                       background_mono_map
#    define    WSCERR_BACKGROUND_MAP_SIZE                  background_mono_map_size
#    define    WSCERR_BACKGROUND_SHADES                    4, 7, 3, 2
#    define    WSCERR_BACKGROUND_SCROLL_SPEED              64
#else
#    define    WSCERR_BACKGROUND_SHADES                    3, 0, 0, 0
#endif
