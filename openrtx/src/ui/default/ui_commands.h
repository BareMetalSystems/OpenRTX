/***************************************************************************
 *   Copyright (C) 2020 - 2024 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN                      *
 *                                Frederik Saraci IU2NRO                   *
 *                                Silvano Seva IU2KWO                      *
 *                                Kim Lyon VK6KL                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#ifndef UI_COMMANDS_H
#define UI_COMMANDS_H

#include <ui/ui_default.h>
#include "ui_scripts.h"

// System Parameter Values
enum
{
    GUI_CMD_PARA_SIGNED_BIT  = 0x80 ,
    GUI_CMD_PARA_SIGNED_FLAG = 0x40 ,

    GUI_CMD_PARA_VALUE_MASK  = ~GUI_CMD_PARA_SIGNED_BIT
};

#define ST_VAL( val )   ( (uint8_t)( val & GUI_CMD_PARA_VALUE_MASK ) + \
                          (uint8_t)GUI_CMD_DATA_AREA )

#define LD_VAL( val )   ( (uint8_t)val - (uint8_t)GUI_CMD_DATA_AREA )

#define ST_EXT_VAL( o , v )     ST_VAL( o ) , ST_VAL( v )
#define ST_LOAD_EXT_VAL( v )
#define ST_ADD_EXT_VAL( v )
#define ST_SUB_EXT_VAL( v )

// GUI Commands
enum
{
    GUI_CMD_NULL             = 0x00 ,
    GUI_CMD_EVENT_START      = 0x01 , // dynamic display - variables updated
    GUI_CMD_EVENT_END        = 0x02 , //  used with page sections that respond to events
    GUI_CMD_TIMER_CHECK      = 0x03 ,
    GUI_CMD_TIMER_SET        = 0x04 ,
    GUI_CMD_GOTO_TEXT_LINE   = 0x05 ,
    GUI_CMD_LOAD_STYLE       = 0x06 ,
    GUI_CMD_BG_COLOR         = 0x07 ,
    GUI_CMD_FG_COLOR         = 0x08 ,
    GUI_CMD_FONT_SIZE        = 0x09 ,
    GUI_CMD_LINE_END         = 0x0A ,
    GUI_CMD_ALIGN            = 0x0B ,
    GUI_CMD_RUN_SCRIPT       = 0x0C ,
    GUI_CMD_LINK             = 0x0D ,
    GUI_CMD_LINK_END         = 0x0E ,
    GUI_CMD_PAGE             = 0x0F ,
    GUI_CMD_TITLE            = 0x10 ,
    GUI_CMD_TEXT             = 0x11 ,
    GUI_CMD_VALUE_DSP        = 0x12 ,
    GUI_CMD_VALUE_INP        = 0x13 ,
    GUI_CMD_GOTO_POS_X       = 0x14 ,
    GUI_CMD_GOTO_POS_Y       = 0x15 ,
    GUI_CMD_ADD_TO_POS_X     = 0x16 ,
    GUI_CMD_ADD_TO_POS_Y     = 0x17 ,
    GUI_CMD_DRAW_LINE        = 0x18 ,
    GUI_CMD_DRAW_RECT        = 0x19 ,
    GUI_CMD_DRAW_RECT_FILLED = 0x1A ,
    GUI_CMD_OPERATION        = 0x1B ,
#ifdef DISPLAY_DEBUG_MSG
    GUI_CMD_SET_DBG_MSG      = 0x1D ,
#endif // DISPLAY_DEBUG_MSG
    GUI_CMD_STUBBED          = 0x1E ,
    GUI_CMD_PAGE_END         = 0x1F ,
    GUI_CMD_NUM_OF                  ,
    GUI_CMD_DATA_AREA        = 0x20
};

//#define ENABLE_ALIGN_VERTICAL

enum
{
    GUI_CMD_ALIGN_PARA_LEFT   = 0x01 ,
    GUI_CMD_ALIGN_PARA_CENTER = 0x02 ,
    GUI_CMD_ALIGN_PARA_RIGHT  = 0x03 ,
    GUI_CMD_ALIGN_PARA_MASK_X = 0x07
#ifdef ENABLE_ALIGN_VERTICAL
    ,
    GUI_CMD_ALIGN_PARA_TOP    = 0x01 << 3 ,
    GUI_CMD_ALIGN_PARA_MIDDLE = 0x02 << 3 ,
    GUI_CMD_ALIGN_PARA_BOTTOM = 0x03 << 3 ,
    GUI_CMD_ALIGN_PARA_MASK_Y = 0x07 << 3
#endif // ENABLE_ALIGN_VERTICAL
};

// Script Command Macros
#define ALIGN_LEFT      GUI_CMD_ALIGN , ST_VAL( GUI_CMD_ALIGN_PARA_LEFT   )
#define ALIGN_CENTER    GUI_CMD_ALIGN , ST_VAL( GUI_CMD_ALIGN_PARA_CENTER )
#define ALIGN_RIGHT     GUI_CMD_ALIGN , ST_VAL( GUI_CMD_ALIGN_PARA_RIGHT  )
//#define ALIGN_TOP       GUI_CMD_ALIGN , ST_VAL( GUI_CMD_ALIGN_PARA_TOP    )
//#define ALIGN_MIDDLE    GUI_CMD_ALIGN , ST_VAL( GUI_CMD_ALIGN_PARA_MIDDLE )
//#define ALIGN_BOTTOM    GUI_CMD_ALIGN , ST_VAL( GUI_CMD_ALIGN_PARA_BOTTOM )

/*
// Color Variable Field Unshifted Masks and Shifts
enum
{
    COLOR_ENC_RED_SHIFT   =   24 ,
    COLOR_ENC_RED_MASK    = 0xFF ,
    COLOR_ENC_GREEN_SHIFT =   16 ,
    COLOR_ENC_GREEN_MASK  = 0xFF ,
    COLOR_ENC_BLUE_SHIFT  =    8 ,
    COLOR_ENC_BLUE_MASK   = 0xFF ,
    COLOR_ENC_ALPHA_SHIFT =    0 ,
    COLOR_ENC_ALPHA_MASK  = 0xFF
};

// Color Variable Field Encoder
#define COLOR_DEF_ENC( r , g , b , a ) (const uint32_t)(                        \
    ( ( (const uint32_t)r & COLOR_ENC_RED_MASK   ) << COLOR_ENC_RED_SHIFT   ) | \
    ( ( (const uint32_t)g & COLOR_ENC_GREEN_MASK ) << COLOR_ENC_GREEN_SHIFT ) | \
    ( ( (const uint32_t)b & COLOR_ENC_BLUE_MASK  ) << COLOR_ENC_BLUE_SHIFT  ) | \
    ( ( (const uint32_t)a & COLOR_ENC_ALPHA_MASK ) << COLOR_ENC_ALPHA_SHIFT )   )

// Color Definitions
#define COLOR_DEF_WHITE         COLOR_DEF_ENC( 255 , 255 , 255 , 255 )
#define COLOR_DEF_BLACK         COLOR_DEF_ENC(   0 ,   0 ,  0  , 255 )
#define COLOR_DEF_GREY          COLOR_DEF_ENC(  60 ,  60 ,  60 , 255 )
#define COLOR_DEF_ALPHA_GREY    COLOR_DEF_ENC(   0 ,   0 ,   0 , 255 )
#define COLOR_DEF_YELLOW_FAB413 COLOR_DEF_ENC( 250 , 180 ,  19 , 255 )
#define COLOR_DEF_RED           COLOR_DEF_ENC( 255 ,   0 ,   0 , 255 )
#define COLOR_DEF_GREEN         COLOR_DEF_ENC(   0 , 255 ,   0 , 255 )
#define COLOR_DEF_BLUE          COLOR_DEF_ENC(   0 ,   0 , 255 , 255 )

// Color Table Definition
#define COLOR_TABLE             \
    COLOR_DEF_BLACK         ,   \
    COLOR_DEF_WHITE         ,   \
    COLOR_DEF_GREY          ,   \
    COLOR_DEF_ALPHA_GREY    ,   \
    COLOR_DEF_YELLOW_FAB413 ,   \
    COLOR_DEF_RED           ,   \
    COLOR_DEF_GREEN         ,   \
    COLOR_DEF_BLUE

// Color Selectors
typedef enum
{
    COLOR_BLACK         ,
    COLOR_WHITE         ,
    COLOR_GREY          ,
    COLOR_ALPHA_GREY    ,
    COLOR_YELLOW_FAB413 ,
    COLOR_RED           ,
    COLOR_GREEN         ,
    COLOR_BLUE          ,
    COLOR_NUM_OF        ,

    COLOR_BG  = COLOR_BLACK         , // background
    COLOR_FG  = COLOR_WHITE         , // foreground
    COLOR_GG  = COLOR_GREY          , // grey background
    COLOR_AGG = COLOR_ALPHA_GREY    , // alpha grey overlay foreground
    COLOR_HL  = COLOR_YELLOW_FAB413 , // highlight background
    COLOR_OP0 = COLOR_RED           , // option 0 foreground
    COLOR_OP1 = COLOR_GREEN         , // option 1 foreground
    COLOR_OP2 = COLOR_BLUE          , // option 2 foreground
    COLOR_OP3 = COLOR_YELLOW_FAB413 , // option 3 foreground
    COLOR_AL  = COLOR_RED             // alarm foreground

}ColorSelector_en;

// Load the Color_st structure with the color variable fields
#define COLOR_LD( c , cv )                                                       \
c->red   = (uint8_t)( ( cv >> COLOR_ENC_RED_SHIFT   ) & COLOR_ENC_RED_MASK   ) ; \
c->green = (uint8_t)( ( cv >> COLOR_ENC_GREEN_SHIFT ) & COLOR_ENC_GREEN_MASK ) ; \
c->blue  = (uint8_t)( ( cv >> COLOR_ENC_BLUE_SHIFT  ) & COLOR_ENC_BLUE_MASK  ) ; \
c->alpha = (uint8_t)( ( cv >> COLOR_ENC_ALPHA_SHIFT ) & COLOR_ENC_ALPHA_MASK ) ;
*/

// Script Command Macros
#define GOTO_TEXT_LINE( l ) GUI_CMD_GOTO_TEXT_LINE , ST_VAL( l )

// Script Command Macros
#define BG_COLOR( c )       GUI_CMD_BG_COLOR   , ST_VAL( COLOR_##c )
#define FG_COLOR( c )       GUI_CMD_FG_COLOR   , ST_VAL( COLOR_##c )

// Script Command Macros
#define GOTO_X( x )         GUI_CMD_GOTO_POS_X   , ST_VAL( x )
#define GOTO_Y( y )         GUI_CMD_GOTO_POS_Y   , ST_VAL( y )
#define ADD_TO_X( x )       GUI_CMD_ADD_TO_POS_X , ST_VAL( x )
#define ADD_TO_Y( y )       GUI_CMD_ADD_TO_POS_Y , ST_VAL( y )

#define LINE( w , h )       GUI_CMD_DRAW_LINE , ST_VAL( w ) , ST_VAL( h )
#define RECT( w , h )       GUI_CMD_DRAW_RECT , ST_VAL( w ) , ST_VAL( h )
#define RECT_FILL( w , h )  GUI_CMD_DRAW_RECT_FILLED , ST_VAL( w ) , ST_VAL( h )

// Script Command Macros

enum
{
    GUI_CMD_OPR_GOTO_SCREEN_LEFT  ,
    GUI_CMD_OPR_GOTO_SCREEN_RIGHT ,
    GUI_CMD_OPR_GOTO_SCREEN_TOP   ,
    GUI_CMD_OPR_GOTO_SCREEN_BASE  ,
    GUI_CMD_OPR_GOTO_LINE_TOP     ,
    GUI_CMD_OPR_GOTO_LINE_BASE
};

#define GOTO_SCREEN_LEFT    GUI_CMD_OPERATION , ST_VAL( GUI_CMD_OPR_GOTO_SCREEN_LEFT )
#define GOTO_SCREEN_RIGHT   GUI_CMD_OPERATION , ST_VAL( GUI_CMD_OPR_GOTO_SCREEN_RIGHT )
#define GOTO_SCREEN_TOP     GUI_CMD_OPERATION , ST_VAL( GUI_CMD_OPR_GOTO_SCREEN_TOP )
#define GOTO_SCREEN_BASE    GUI_CMD_OPERATION , ST_VAL( GUI_CMD_OPR_GOTO_SCREEN_BASE )
#define GOTO_LINE_TOP       GUI_CMD_OPERATION , ST_VAL( GUI_CMD_OPR_GOTO_LINE_TOP )
#define GOTO_LINE_BASE      GUI_CMD_OPERATION , ST_VAL( GUI_CMD_OPR_GOTO_LINE_BASE )

// Color load fn.
extern void ui_ColorLoad( Color_st* color , ColorSelector_en colorSelector );

extern bool ui_DisplayPage( GuiState_st* guiState );
extern void ui_States_SetPageNum( GuiState_st* guiState , uint8_t pageNum );

#endif // UI_COMMANDS_H
