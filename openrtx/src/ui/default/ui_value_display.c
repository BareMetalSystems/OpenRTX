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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <input.h>
#include <hwconfig.h>
#include <voicePromptUtils.h>
#include <ui/ui_default.h>
#include <rtx.h>
#include <interfaces/platform.h>
#include <interfaces/display.h>
#include <interfaces/cps_io.h>
#include <interfaces/nvmem.h>
#include <interfaces/delays.h>
#include <string.h>
#include <battery.h>
#include <utils.h>
#include <beeps.h>
#include <memory_profiling.h>

#ifdef PLATFORM_TTWRPLUS
#include <SA8x8.h>
#endif

//@@@KL #include "ui_m17.h"

#include "ui.h"
#include "ui_value_arrays.h"
#include "ui_scripts.h"
#include "ui_commands.h"
#include "ui_value_display.h"
#include "ui_states.h"
#include "ui_value_input.h"
#include "ui_menu.h"

static void GuiVal_DispVal( GuiState_st* guiState , char* valueBuffer );

static void GuiVal_CurrentTime( GuiState_st* guiState );
static void GuiVal_BatteryLevel( GuiState_st* guiState );
static void GuiVal_LockState( GuiState_st* guiState );
static void GuiVal_ModeInfo( GuiState_st* guiState );
static void GuiVal_BankChannel( GuiState_st* guiState );
static void GuiVal_Frequency( GuiState_st* guiState );
static void GuiVal_RSSIMeter( GuiState_st* guiState );

static void GuiVal_Banks( GuiState_st* guiState );
static void GuiVal_Channels( GuiState_st* guiState );
static void GuiVal_Contacts( GuiState_st* guiState );
#ifdef GPS_PRESENT
static void GuiVal_GPS( GuiState_st* guiState );
#endif // GPS_PRESENT
// Settings
// Display
#ifdef SCREEN_BRIGHTNESS
static void GuiVal_ScreenBrightness( GuiState_st* guiState );
#endif // SCREEN_BRIGHTNESS
#ifdef SCREEN_CONTRAST
static void GuiVal_ScreenContrast( GuiState_st* guiState );
#endif // SCREEN_CONTRAST
static void GuiVal_Timer( GuiState_st* guiState );
// Time and Date
static void GuiVal_Date( GuiState_st* guiState );
static void GuiVal_Time( GuiState_st* guiState );
// GPS
#ifdef GPS_PRESENT
static void GuiVal_GPSEnables( GuiState_st* guiState );
static void GuiVal_GPSSetTime( GuiState_st* guiState );
static void GuiVal_GPSTimeZone( GuiState_st* guiState );
#endif // GPS_PRESENT
// Radio
static void GuiVal_RadioOffset( GuiState_st* guiState );
static void GuiVal_RadioDirection( GuiState_st* guiState );
static void GuiVal_RadioStep( GuiState_st* guiState );
// M17
static void GuiVal_M17Callsign( GuiState_st* guiState );
static void GuiVal_M17Can( GuiState_st* guiState );
static void GuiVal_M17CanRxCheck( GuiState_st* guiState );
// Accessibility - Voice
static void GuiVal_Voice( GuiState_st* guiState );
static void GuiVal_Phonetic( GuiState_st* guiState );
// Info
static void GuiVal_BatteryVoltage( GuiState_st* guiState );
static void GuiVal_BatteryCharge( GuiState_st* guiState );
static void GuiVal_Rssi( GuiState_st* guiState );
static void GuiVal_UsedHeap( GuiState_st* guiState );
static void GuiVal_Band( GuiState_st* guiState );
static void GuiVal_Vhf( GuiState_st* guiState );
static void GuiVal_Uhf( GuiState_st* guiState );
static void GuiVal_HwVersion( GuiState_st* guiState );
#ifdef PLATFORM_TTWRPLUS
static void GuiVal_Radio( GuiState_st* guiState );
static void GuiVal_RadioFw( GuiState_st* guiState );
#endif // PLATFORM_TTWRPLUS
static void GuiVal_BackupRestore( GuiState_st* guiState );
static void GuiVal_LowBattery( GuiState_st* guiState );
static void GuiVal_Stubbed( GuiState_st* guiState );

typedef void (*ui_GuiVal_fn)( GuiState_st* guiState );

static const ui_GuiVal_fn ui_GuiVal_Table[ GUI_VAL_DSP_NUM_OF ] =
{
    GuiVal_CurrentTime      ,
    GuiVal_BatteryLevel     ,
    GuiVal_LockState        ,
    GuiVal_ModeInfo         ,
    GuiVal_BankChannel      ,
    GuiVal_Frequency        ,
    GuiVal_RSSIMeter        ,

    GuiVal_Banks            ,
    GuiVal_Channels         ,
    GuiVal_Contacts         ,
    GuiVal_GPS              ,
    // Settings
    // Display
#ifdef SCREEN_BRIGHTNESS
    GuiVal_ScreenBrightness ,
#endif
#ifdef SCREEN_CONTRAST
    GuiVal_ScreenContrast   ,
#endif
    GuiVal_Timer            ,
    // Time and Date
    GuiVal_Date             ,
    GuiVal_Time             ,
    // GPS
    GuiVal_GPSEnables       ,
    GuiVal_GPSSetTime       ,
    GuiVal_GPSTimeZone      ,
    // Radio
    GuiVal_RadioOffset      ,
    GuiVal_RadioDirection   ,
    GuiVal_RadioStep        ,
    // M17
    GuiVal_M17Callsign      ,
    GuiVal_M17Can           ,
    GuiVal_M17CanRxCheck    ,
    // Accessibility - Voice
    GuiVal_Voice            ,
    GuiVal_Phonetic         ,
    // Info
    GuiVal_BatteryVoltage   ,
    GuiVal_BatteryCharge    ,
    GuiVal_Rssi             ,
    GuiVal_UsedHeap         ,
    GuiVal_Band             ,
    GuiVal_Vhf              ,
    GuiVal_Uhf              ,
    GuiVal_HwVersion        ,
#ifdef PLATFORM_TTWRPLUS
    GuiVal_Radio            ,
    GuiVal_RadioFw          ,
#endif // PLATFORM_TTWRPLUS
    GuiVal_BackupRestore    ,
    GuiVal_LowBattery       ,

    GuiVal_Stubbed
};

void GuiVal_DisplayValue( GuiState_st* guiState , uint8_t valueNum )
{
    uint8_t valNum = valueNum ;

    if( valNum >= GUI_VAL_DSP_NUM_OF )
    {
        valNum = GUI_VAL_DSP_STUBBED ;
    }

    ui_GuiVal_Table[ valNum ]( guiState );
    guiState->page.renderPage = true ;
}

static void GuiVal_CurrentTime( GuiState_st* guiState )
{
    Line_st*  line     = &guiState->layout.lines[ GUI_LINE_TOP ] ;
    Style_st* style    = &guiState->layout.styles[ GUI_STYLE_TOP ] ;
    Pos_st    start ;
    uint16_t  height ;
    uint16_t  width ;
    Color_st  color_bg ;
    Color_st  color_fg ;

    ui_ColorLoad( &color_bg , COLOR_BG );
    ui_ColorLoad( &color_fg , COLOR_FG );

#ifdef RTC_PRESENT
    // clear the time display area
    //@@@KL needs to be more objectively determined
    height    = line->height ;
    width     = 68 ;
    start.y   = ( line->pos.y - height ) + 1 ;
    start.x   = 44 ;
    gfx_clearRectangle( start , width , height );
    // Print clock on top bar
    datetime_t local_time = utcToLocalTime( last_state.time , last_state.settings.utc_timezone );
    gfx_print( line->pos , style->font.size , ALIGN_CENTER ,
               color_fg , "%02d:%02d:%02d" , local_time.hour , local_time.minute , local_time.second );
#endif // RTC_PRESENT

}

static void GuiVal_BatteryLevel( GuiState_st* guiState )
{
    Line_st*  lineTop  = &guiState->layout.lines[ GUI_LINE_TOP ] ;
#ifdef BAT_NONE
    Style_st* styleTop = &guiState->layout.styles[ GUI_STYLE_TOP ] ;
#endif // BAT_NONE
    Pos_st    start ;
    uint16_t  height ;
    uint16_t  width ;
    Color_st  color_bg ;
    Color_st  color_fg ;

    ui_ColorLoad( &color_bg , COLOR_BG );
    ui_ColorLoad( &color_fg , COLOR_FG );

    // clear the time display area
    width   = SCREEN_WIDTH / 9 ;
    height  = lineTop->height - ( guiState->layout.status_v_pad * 2 );
    start.x = SCREEN_WIDTH - width - guiState->layout.horizontal_pad ;
    start.y = guiState->layout.status_v_pad ;
    gfx_clearRectangle( start , width , height );
    // If the radio has no built-in battery, print input voltage
#ifdef BAT_NONE
    gfx_print( lineTop->pos ,
               styleTop->font.size ,
               ALIGN_RIGHT , color_fg , "%.1fV" , last_state.v_bat );
#else // BAT_NONE
    // Otherwise print battery icon on top bar, use 4 px padding
    gfx_drawBattery( start , width , height , last_state.charge );
#endif // BAT_NONE

}

static void GuiVal_LockState( GuiState_st* guiState )
{
    Line_st*  lineTop     = &guiState->layout.lines[ GUI_LINE_TOP ] ;
    Style_st* styleTop    = &guiState->layout.styles[ GUI_STYLE_TOP ] ;
    Pos_st    start ;
    uint16_t  width ;
    Color_st  color_bg ;
    Color_st  color_fg ;

    ui_ColorLoad( &color_bg , COLOR_BG );
    ui_ColorLoad( &color_fg , COLOR_FG );

    if( guiState->uiState.input_locked == true )
    {
        start  = lineTop->pos ;
        width  = styleTop->symbolSize + FONT_SIZE_24PT + 1 ;
        gfx_drawSymbol( start , width , ALIGN_LEFT , color_fg , SYMBOL_LOCK );
    }

}

static void GuiVal_ModeInfo( GuiState_st* guiState )
{
    Line_st*  line1           = &guiState->layout.lines[ GUI_LINE_1 ] ;
    Style_st* style1          = &guiState->layout.styles[ GUI_STYLE_1 ] ;
    Line_st*  line2           = &guiState->layout.lines[ GUI_LINE_2 ] ;
    Style_st* style2          = &guiState->layout.styles[ GUI_STYLE_2 ] ;
    Line_st*  line3           = &guiState->layout.lines[ GUI_LINE_3 ] ;
    Style_st* style3          = &guiState->layout.styles[ GUI_STYLE_3 ] ;
    Line_st*  line4           = &guiState->layout.lines[ GUI_LINE_4 ] ;
    Style_st* style4          = &guiState->layout.styles[ GUI_STYLE_4 ] ;
    char      bw_str[ 8 ]     = { 0 };
    char      encdec_str[ 9 ] = { 0 };
    Color_st  color_fg ;

    ui_ColorLoad( &color_fg , COLOR_FG );

    switch( last_state.channel.mode )
    {
        case OPMODE_FM :
        {
            // Get Bandwidth string
            if( last_state.channel.bandwidth == BW_12_5 )
            {
                snprintf( bw_str , 8 , "NFM" );
            }
            else
            {
                if( last_state.channel.bandwidth == BW_20 )
                {
                    snprintf( bw_str , 8 , "FM20" );
                }
                else
                {
                    if( last_state.channel.bandwidth == BW_25 )
                    {
                        snprintf( bw_str , 8 , "FM" );
                    }
                }
            }

            // Get encdec string
            bool tone_tx_enable = last_state.channel.fm.txToneEn ;
            bool tone_rx_enable = last_state.channel.fm.rxToneEn ;

            if( tone_tx_enable && tone_rx_enable )
            {
                snprintf( encdec_str , 9 , "ED" );
            }
            else
            {
                if( tone_tx_enable && !tone_rx_enable )
                {
                    snprintf( encdec_str , 9 , " E" );
                }
                else
                {
                    if( !tone_tx_enable && tone_rx_enable )
                    {
                        snprintf( encdec_str , 9 , " D" );
                    }
                    else
                    {
                        snprintf( encdec_str , 9 , "  " );
                    }
                }
            }

            // Print Bandwidth, Tone and encdec info
            if( tone_tx_enable || tone_rx_enable )
            {
                gfx_print( line2->pos , style2->font.size , ALIGN_CENTER ,
                           color_fg , "%s %4.1f %s" , bw_str ,
                           ctcss_tone[ last_state.channel.fm.txTone ] / 10.0f , encdec_str );
            }
            else
            {
                gfx_print( line2->pos , style2->font.size , ALIGN_CENTER ,
                           color_fg , "%s" , bw_str );
            }
            break ;
        }
        case OPMODE_DMR :
        {
            // Print Contact
            gfx_print( line2->pos , style2->font.size , ALIGN_CENTER ,
                       color_fg , "%s" , last_state.contact.name );
            break ;
        }
        case OPMODE_M17 :
        {
            // Print M17 Destination ID on line 3 of 3
            rtxStatus_t rtxStatus = rtx_getCurrentStatus();

            if( rtxStatus.lsfOk )
            {
                // Destination address
                gfx_drawSymbol( line2->pos , style2->symbolSize , ALIGN_LEFT ,
                                color_fg , SYMBOL_CALL_RECEIVED );

                gfx_print( line2->pos , style2->font.size , ALIGN_CENTER ,
                           color_fg , "%s" , rtxStatus.M17_dst );

                // Source address
                gfx_drawSymbol( line1->pos , style1->symbolSize , ALIGN_LEFT ,
                                color_fg , SYMBOL_CALL_MADE );

                gfx_print( line1->pos , style2->font.size , ALIGN_CENTER ,
                           color_fg , "%s" , rtxStatus.M17_src );

                // RF link (if present)
                if( rtxStatus.M17_link[0] != '\0' )
                {
                    gfx_drawSymbol( line4->pos , style3->symbolSize , ALIGN_LEFT ,
                                    color_fg , SYMBOL_ACCESS_POINT );
                    gfx_print( line4->pos , style2->font.size , ALIGN_CENTER ,
                               color_fg , "%s" , rtxStatus.M17_link );
                }

                // Reflector (if present)
                if( rtxStatus.M17_refl[0] != '\0' )
                {
                    gfx_drawSymbol( line3->pos , style4->symbolSize , ALIGN_LEFT ,
                                    color_fg , SYMBOL_NETWORK );
                    gfx_print( line3->pos , style2->font.size , ALIGN_CENTER ,
                               color_fg , "%s" , rtxStatus.M17_refl );
                }
            }
            else
            {
                const char* dst = NULL ;
                if( guiState->uiState.edit_mode )
                {
                    dst = guiState->uiState.new_callsign ;
                }
                else
                {
                    if( strnlen( rtxStatus.destination_address , 10 ) == 0 )
                    {
                        dst = currentLanguage->broadcast ;
                    }
                    else
                    {
                        dst = rtxStatus.destination_address ;
                    }
                }

                gfx_print( line2->pos , style2->font.size , ALIGN_CENTER ,
                           color_fg , "M17 #%s" , dst );
            }
            break ;
        }
    }

}

static void GuiVal_BankChannel( GuiState_st* guiState )
{
    Line_st*  line1       = &guiState->layout.lines[ GUI_LINE_1 ] ;
    Style_st* style1      = &guiState->layout.styles[ GUI_STYLE_1 ] ;
    uint16_t bank_enabled = ( last_state.bank_enabled ) ? last_state.bank : 0 ;
    Color_st color_fg ;

    ui_ColorLoad( &color_fg , COLOR_FG );

    // Print Bank number, channel number and Channel name
    gfx_print( line1->pos ,
               style1->font.size , ALIGN_CENTER ,
               color_fg , "%01d-%03d: %.12s" ,
               bank_enabled , last_state.channel_index + 1 , last_state.channel.name );
}

static void GuiVal_Frequency( GuiState_st* guiState )
{
    Line_st*  line3Large  = &guiState->layout.lines[ GUI_LINE_3_LARGE ] ;
    Style_st* style3Large = &guiState->layout.styles[ GUI_STYLE_3_LARGE ] ;

    // Show VFO frequency if the OpMode is not M17 or there is no valid LSF data
    rtxStatus_t status = rtx_getCurrentStatus();

    if( ( status.opMode != OPMODE_M17 ) || ( status.lsfOk == false ) )
    {
        unsigned long frequency = platform_getPttStatus() ? last_state.channel.tx_frequency
                                                          : last_state.channel.rx_frequency;
        Color_st color_fg ;
        ui_ColorLoad( &color_fg , COLOR_FG );

        // Print big numbers frequency
        gfx_print( line3Large->pos , style3Large->font.size , ALIGN_CENTER ,
                   color_fg , "%.7g" , (float)frequency / 1000000.0f );
    }

}

static void GuiVal_RSSIMeter( GuiState_st* guiState )
{
    Line_st*  lineBottom   = &guiState->layout.lines[ GUI_LINE_BOTTOM ] ;
//    Style_st* styleBottom  = &guiState->layout.styles[ GUI_STYLE_BOTTOM ] ;
    float     rssi         = last_state.rssi ;
    float     squelch      = last_state.settings.sqlLevel / 16.0f ; // squelch bar
    Pos_st    meter_pos ;
    uint16_t  meter_width ;
    uint16_t  meter_height ;
    uint8_t   mic_level    = platform_getMicLevel();
    Color_st  color_op3 ;

    meter_width  = SCREEN_WIDTH - ( 2 * guiState->layout.horizontal_pad ) ;
    meter_height = lineBottom->height ;
    meter_pos.x  = guiState->layout.horizontal_pad ;
    meter_pos.y  = ( SCREEN_HEIGHT - meter_height ) - guiState->layout.bottom_pad ;

    ui_ColorLoad( &color_op3 , COLOR_OP3 );

    switch( last_state.channel.mode )
    {
        case OPMODE_FM :
        {
            gfx_drawSmeter( meter_pos , meter_width , meter_height ,
                            rssi , squelch , color_op3 );
            break ;
        }
        case OPMODE_DMR :
        {
            gfx_drawSmeter( meter_pos , meter_width , meter_height ,
                            rssi , squelch , color_op3 );
            break ;
        }
        case OPMODE_M17 :
        {
            gfx_drawSmeterLevel( meter_pos , meter_width , meter_height ,
                                 rssi , mic_level );
            break ;
        }
    }

}

static void GuiVal_Banks( GuiState_st* guiState )
{
    _ui_drawMenuList( guiState , PAGE_MENU_BANK );
}

static void GuiVal_Channels( GuiState_st* guiState )
{
    _ui_drawMenuList( guiState , PAGE_MENU_CHANNEL );
}

static void GuiVal_Contacts( GuiState_st* guiState )
{
    _ui_drawMenuList( guiState , PAGE_MENU_CONTACTS );
}

#ifdef GPS_PRESENT
static void GuiVal_GPS( GuiState_st* guiState )
{
    Line_st*  lineTop     = &guiState->layout.lines[ GUI_LINE_TOP ] ;
    Style_st* styleTop    = &guiState->layout.styles[ GUI_STYLE_TOP ] ;
    Line_st*  line1       = &guiState->layout.lines[ GUI_LINE_1 ] ;
//    Style_st* style1      = &guiState->layout.styles[ GUI_STYLE_1 ] ;
    Line_st*  line2       = &guiState->layout.lines[ GUI_LINE_2 ] ;
//    Style_st* style2      = &guiState->layout.styles[ GUI_STYLE_2 ] ;
    Line_st*  line3Large  = &guiState->layout.lines[ GUI_LINE_3_LARGE ] ;
    Style_st* style3Large = &guiState->layout.styles[ GUI_STYLE_3_LARGE ] ;
    Line_st*  lineBottom  = &guiState->layout.lines[ GUI_LINE_BOTTOM ] ;
    Style_st* styleBottom = &guiState->layout.styles[ GUI_STYLE_BOTTOM ] ;
    char*     fix_buf ;
    char*     type_buf ;
    Color_st  color_fg ;

    ui_ColorLoad( &color_fg , COLOR_FG );

    // Print "GPS" on top bar
    gfx_print( lineTop->pos , styleTop->font.size , ALIGN_CENTER ,
               color_fg , currentLanguage->gps );
    Pos_st fix_pos = { line2->pos.x , ( SCREEN_HEIGHT * 2 ) / 5 };
    // Print GPS status, if no fix, hide details
    if( !last_state.settings.gps_enabled )
    {
        gfx_print( fix_pos , style3Large->font.size , ALIGN_CENTER ,
                   color_fg , currentLanguage->gpsOff );
    }
    else if( last_state.gps_data.fix_quality == 0 )
    {
        gfx_print( fix_pos , style3Large->font.size , ALIGN_CENTER ,
                   color_fg , currentLanguage->noFix );
    }
    else if( last_state.gps_data.fix_quality == 6 )
    {
        gfx_print( fix_pos , style3Large->font.size , ALIGN_CENTER ,
                   color_fg , currentLanguage->fixLost );
    }
    else
    {
        switch(last_state.gps_data.fix_quality)
        {
            case 1 :
            {
                fix_buf = "SPS" ;
                break ;
            }
            case 2 :
            {
                fix_buf = "DGPS" ;
                break ;
            }
            case 3:
            {
                fix_buf = "PPS" ;
                break ;
            }
            default:
            {
                fix_buf = (char*)currentLanguage->error ;
                break ;
            }
        }

        switch(last_state.gps_data.fix_type)
        {
            case 1:
            {
                type_buf = "" ;
                break ;
            }
            case 2:
            {
                type_buf = "2D" ;
                break ;
            }
            case 3:
            {
                type_buf = "3D" ;
                break ;
            }
            default:
            {
                type_buf = (char*)currentLanguage->error ;
                break ;
            }
        }
        gfx_print( line1->pos , styleTop->font.size , ALIGN_LEFT ,
                   color_fg , fix_buf );
        gfx_print( line1->pos , styleTop->font.size , ALIGN_CENTER ,
                   color_fg , "N     " );
        gfx_print( line1->pos , styleTop->font.size , ALIGN_RIGHT ,
                   color_fg , "%8.6f" , last_state.gps_data.latitude );
        gfx_print( line2->pos , styleTop->font.size , ALIGN_LEFT ,
                   color_fg , type_buf);
        // Convert from signed longitude, to unsigned + direction
        float longitude = last_state.gps_data.longitude ;
        char* direction = (longitude < 0) ? "W     " : "E     " ;
        longitude = (longitude < 0) ? -longitude : longitude ;
        gfx_print( line2->pos , styleTop->font.size , ALIGN_CENTER ,
                   color_fg , direction );
        gfx_print( line2->pos , styleTop->font.size , ALIGN_RIGHT ,
                   color_fg , "%8.6f" , longitude );
        gfx_print( lineBottom->pos , styleBottom->font.size , ALIGN_CENTER ,
                   color_fg , "S %4.1fkm/h  A %4.1fm" ,
                   last_state.gps_data.speed , last_state.gps_data.altitude );
    }
    // Draw compass
    Pos_st compass_pos = { guiState->layout.horizontal_pad * 2 , SCREEN_HEIGHT / 2 };
    gfx_drawGPScompass( compass_pos , SCREEN_WIDTH / 9 + 2 ,
                        last_state.gps_data.tmg_true ,
                        last_state.gps_data.fix_quality != 0 &&
                        last_state.gps_data.fix_quality != 6 );
    // Draw satellites bar graph
    Pos_st bar_pos = { line3Large->pos.x + SCREEN_WIDTH * 1 / 3 , SCREEN_HEIGHT / 2 };
    gfx_drawGPSgraph( bar_pos ,
                      ( ( SCREEN_WIDTH * 2 ) / 3) - guiState->layout.horizontal_pad ,
                      SCREEN_HEIGHT / 3 ,
                      last_state.gps_data.satellites ,
                      last_state.gps_data.active_sats );
}
#endif // GPS_PRESENT

#ifdef SCREEN_BRIGHTNESS
static void GuiVal_ScreenBrightness( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%d" , last_state.settings.brightness );
    GuiVal_DispVal( guiState , valueBuffer );

}
#endif // SCREEN_BRIGHTNESS
#ifdef SCREEN_CONTRAST
static void GuiVal_ScreenContrast( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%d" , last_state.settings.contrast );
    GuiVal_DispVal( guiState , valueBuffer );

}
#endif
static void GuiVal_Timer( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%s" ,
              display_timer_values[ last_state.settings.display_timer ] );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_Date( GuiState_st* guiState )
{
    char       valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    datetime_t local_time = utcToLocalTime( last_state.time , last_state.settings.utc_timezone );

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%02d/%02d/%02d" ,
              local_time.date , local_time.month , local_time.year );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_Time( GuiState_st* guiState )
{
    char       valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    datetime_t local_time = utcToLocalTime( last_state.time , last_state.settings.utc_timezone );

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%02d:%02d:%02d" ,
              local_time.hour , local_time.minute , local_time.second );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_GPSEnables( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%s" ,
              (last_state.settings.gps_enabled) ? currentLanguage->on : currentLanguage->off );
    GuiVal_DispVal( guiState , valueBuffer );

}

#ifdef GPS_PRESENT
static void GuiVal_GPSSetTime( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%s" ,
              (last_state.gps_set_time) ? currentLanguage->on : currentLanguage->off );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_GPSTimeZone( GuiState_st* guiState )
{
    char   valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    int8_t tz_hr = ( last_state.settings.utc_timezone / 2 ) ;
    int8_t tz_mn = ( last_state.settings.utc_timezone % 2 ) * 5 ;
    char   sign  = ' ';

    if(last_state.settings.utc_timezone > 0)
    {
        sign = '+' ;
    }
    else if(last_state.settings.utc_timezone < 0)
    {
        sign   = '-' ;
        tz_hr *= (-1) ;
        tz_mn *= (-1) ;
    }

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%c%d.%d" , sign , tz_hr , tz_mn );
    GuiVal_DispVal( guiState , valueBuffer );

}
#endif // GPS_PRESENT

// Radio
static void GuiVal_RadioOffset( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    int32_t offset = 0 ;

    offset = abs( (int32_t)last_state.channel.tx_frequency -
                  (int32_t)last_state.channel.rx_frequency );
    snprintf( valueBuffer , MAX_ENTRY_LEN , "%gMHz" , (float)offset / 1000000.0f );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_RadioDirection( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    valueBuffer[ 0 ] = ( last_state.channel.tx_frequency >= last_state.channel.rx_frequency ) ? '+' : '-';
    valueBuffer[ 1 ] = '\0';
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_RadioStep( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    // Print in kHz if it is smaller than 1MHz
    if( freq_steps[ last_state.step_index ] < 1000000 )
    {
        snprintf( valueBuffer , MAX_ENTRY_LEN , "%gkHz" , (float)freq_steps[last_state.step_index] / 1000.0f );
    }
    else
    {
        snprintf( valueBuffer , MAX_ENTRY_LEN , "%gMHz" , (float)freq_steps[last_state.step_index] / 1000000.0f );
    }
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_M17Callsign( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%s" , last_state.settings.callsign );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_M17Can( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%d" , last_state.settings.m17_can );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_M17CanRxCheck( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%s" ,
              (last_state.settings.m17_can_rx) ? currentLanguage->on : currentLanguage->off );
    GuiVal_DispVal( guiState , valueBuffer );

}

// Accessibility - Voice
static void GuiVal_Voice( GuiState_st* guiState )
{
    char    valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    uint8_t value = last_state.settings.vpLevel ;

    switch( value )
    {
        case VPP_NONE :
        {
            snprintf( valueBuffer , MAX_ENTRY_LEN , "%s" ,
                      currentLanguage->off );
            break ;
        }
        case VPP_BEEP :
        {
            snprintf( valueBuffer , MAX_ENTRY_LEN , "%s" ,
                      currentLanguage->beep );
            break ;
        }
        default :
        {
            snprintf( valueBuffer , MAX_ENTRY_LEN , "%d" ,
                      ( value - VPP_BEEP ) );
            break ;
        }
    }
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_Phonetic( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%s" ,
              last_state.settings.vpPhoneticSpell ? currentLanguage->on : currentLanguage->off );
    GuiVal_DispVal( guiState , valueBuffer );

}

// Info
static void GuiVal_BatteryVoltage( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    // Compute integer part and mantissa of voltage value, adding 50mV
    // to mantissa for rounding to nearest integer
    uint16_t volt  = ( last_state.v_bat + 50 ) / 1000 ;
    uint16_t mvolt = ( ( last_state.v_bat - volt * 1000 ) + 50 ) / 100 ;
    snprintf( valueBuffer , MAX_ENTRY_LEN , "%d.%dV" , volt, mvolt );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_BatteryCharge( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%d%%" , last_state.charge );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_Rssi( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%.1fdBm" , last_state.rssi );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_UsedHeap( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%dB" , getHeapSize() - getCurrentFreeHeap() );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_Band( GuiState_st* guiState )
{
    char      valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    hwInfo_t* hwinfo = (hwInfo_t*)platform_getHwInfo();

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%s %s" , hwinfo->vhf_band ? currentLanguage->VHF : "" , hwinfo->uhf_band ? currentLanguage->UHF : "" );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_Vhf( GuiState_st* guiState )
{
    char      valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    hwInfo_t* hwinfo = (hwInfo_t*)platform_getHwInfo();

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%d - %d" , hwinfo->vhf_minFreq, hwinfo->vhf_maxFreq );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_Uhf( GuiState_st* guiState )
{
    char      valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    hwInfo_t* hwinfo = (hwInfo_t*)platform_getHwInfo();

    snprintf( valueBuffer , MAX_ENTRY_LEN , "%d - %d" , hwinfo->uhf_minFreq, hwinfo->uhf_maxFreq );
    GuiVal_DispVal( guiState , valueBuffer );

}

static void GuiVal_HwVersion( GuiState_st* guiState )
{
    char      valueBuffer[ MAX_ENTRY_LEN + 1 ] = "" ;
    hwInfo_t* hwinfo = (hwInfo_t*)platform_getHwInfo();
    snprintf( valueBuffer , MAX_ENTRY_LEN , "%d" , hwinfo->hw_version );

    GuiVal_DispVal( guiState , valueBuffer );

}

#ifdef PLATFORM_TTWRPLUS
static void GuiVal_Radio( GuiState_st* guiState )
{
    (void)guiState ;
    //@@@KL Populate
//    GuiVal_DispVal( guiState , valueBuffer );
}

static void GuiVal_RadioFw( GuiState_st* guiState )
{
    (void)guiState ;
    //@@@KL Populate
//    GuiVal_DispVal( guiState , valueBuffer );
}
#endif // PLATFORM_TTWRPLUS

static void GuiVal_BackupRestore( GuiState_st* guiState )
{
    _ui_drawMenuList( guiState , PAGE_MENU_BACKUP_RESTORE );
}

static void GuiVal_LowBattery( GuiState_st* guiState )
{
    (void)guiState ;
    uint16_t bat_width  = SCREEN_WIDTH / 2 ;
    uint16_t bat_height = SCREEN_HEIGHT / 3 ;
    Pos_st   bat_pos    = { SCREEN_WIDTH / 4 , SCREEN_HEIGHT / 8 };

    gfx_drawBattery( bat_pos , bat_width , bat_height , 10 );

}

// Default
static void GuiVal_Stubbed( GuiState_st* guiState )
{
    char valueBuffer[ MAX_ENTRY_LEN + 1 ] ;

    valueBuffer[ 0 ] = '?' ;
    valueBuffer[ 1 ] = '\0' ;
    GuiVal_DispVal( guiState , valueBuffer );

}

void GuiVal_DispVal( GuiState_st* guiState , char* valueBuffer )
{
    Color_st color_fg ;
    Color_st color_bg ;
    Color_st color_text ;

    ui_ColorLoad( &color_fg , COLOR_FG );
    ui_ColorLoad( &color_bg , COLOR_BG );
    color_text = color_fg ;

    if( guiState->layout.printDisplayOn )
    {
        if( guiState->layout.inSelect )
        {
            color_text = color_bg ;
        }
        gfx_print( guiState->layout.line.pos        ,
                   guiState->layout.style.font.size ,
                   guiState->layout.style.align     ,
                   color_text , valueBuffer           );
    }

}