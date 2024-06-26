/***************************************************************************
 *   Copyright (C) 2020 - 2023 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN                      *
 *                                Frederik Saraci IU2NRO                   *
 *                                Silvano Seva IU2KWO                      *
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

#include <ui.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <event.h>
#include <state.h>
#include <battery.h>
#include <hwconfig.h>
#include <interfaces/platform.h>
#include <interfaces/nvmem.h>
#include <interfaces/delays.h>

State_st         state ;
pthread_mutex_t state_mutex ;

// Commonly used frequency steps, expressed in Hz
uint32_t freq_steps[] = { 1000, 5000, 6250, 10000, 12500, 15000, 20000, 25000, 50000, 100000 };
size_t   n_freq_steps = sizeof( freq_steps ) / sizeof( freq_steps[ 0 ] );

void state_init( void )
{
    pthread_mutex_init( &state_mutex , NULL );

    /*
     * Try loading settings from nonvolatile memory and default to sane values
     * in case of failure.
     */
    if( nvm_readSettings( &state.settings ) < 0 )
    {
        state.settings = default_settings ;
        strncpy( state.settings.callsign , "OPNRTX" , 10 );
    }

    /*
     * Try loading VFO configuration from nonvolatile memory and default to sane
     * values in case of failure.
     */
    if( nvm_readVfoChannelData( &state.channel ) < 0 )
    {
        state.channel = cps_getDefaultChannel();
    }

    state.ui_prevPage   = ~0;
    state.ui_page       = 0 ;

    /*
     * Initialise remaining fields
     */
    #ifdef RTC_PRESENT
    state.time          = platform_getCurrentTime();
    #endif
    state.v_bat         = platform_getVbat();
    state.charge        = battery_getCharge( state.v_bat );
    state.rssi          = -127.0f ;

    state.channel_index = 0 ;       // Set default channel index (it is 0-based)
    state.bank_enabled  = false ;
    state.rtxStatus     = RTX_OFF ;
    state.emergency     = false ;
    state.txDisable     = false ;
    state.step_index    = 4 ;       // Default frequency step 12.5kHz

    // Force brightness field to be in range 0 - 100
    if( state.settings.brightness > 100 )
    {
        state.settings.brightness = 100 ;
    }

    state.contact_index = 0 ;
    state.contact       = cps_getDefaultContact();
}

void state_terminate( void )
{
    // Never store a brightness of 0 to avoid booting with a black screen
    if( state.settings.brightness == 0 )
    {
        state.settings.brightness = 5 ;
    }

    nvm_writeSettingsAndVfo( &state.settings , &state.channel );
    pthread_mutex_destroy( &state_mutex );
}

enum
{
    STATE_TASK_UPDATE_PERIOD            =  100 ,
    STATE_TASK_DISPLAY_TIME_TICK_PERIOD = 1000 ,
    STATE_TASK_DEVICE_STATUS_PERIOD     = 1000 ,
    STATE_TASK_BATTERY_HYSTERESIS       =   10 ,
    STATE_TASK_RSSI_HYSTERESIS          =   10
};

void state_task( void )
{
    static long long int  lastUpdateTimeTick        = 0 ;
    static long long int  lastTimeDisplayUpdateTick = 0 ;
    static long long int  lastDeviceUpdateTick      = 0 ;
    static uint16_t       v_bat_prev                = ~0 ;
           uint16_t       v_bat ;
    static float          rssi_prev                 = ~0 ;
           EventStatus_en eventPayload              = 0 ;

    // Update radio state once every 100ms
    if( ( getTick() - lastUpdateTimeTick ) >= STATE_TASK_UPDATE_PERIOD )
    {
#ifdef RTC_PRESENT
        state.time          = platform_getCurrentTime();
#endif
        lastUpdateTimeTick  = getTick();
        eventPayload       |= EVENT_STATUS_TIME_TICK ;

        if( ( lastUpdateTimeTick - lastTimeDisplayUpdateTick ) >= STATE_TASK_DISPLAY_TIME_TICK_PERIOD )
        {
            lastTimeDisplayUpdateTick  = lastUpdateTimeTick ;
            eventPayload              |= EVENT_STATUS_DISPLAY_TIME_TICK ;
        }

        pthread_mutex_lock( &state_mutex );

        if( ( lastUpdateTimeTick - lastDeviceUpdateTick ) >= STATE_TASK_DEVICE_STATUS_PERIOD )
        {
            lastDeviceUpdateTick  = lastUpdateTimeTick ;
            eventPayload         |= EVENT_STATUS_DEVICE_TIME_TICK ;

            /*
             * Low-pass filtering with a time constant of 10s when updated at 1Hz
             * Original computation: state.v_bat = 0.02*vbat + 0.98*state.v_bat
             * Peak error is 18mV when input voltage is 49mV.
             */
            v_bat         = platform_getVbat();
            state.v_bat  -= ( state.v_bat * 2 ) / 100 ;
            state.v_bat  += ( v_bat * 2 ) / 100 ;

            state.charge  = battery_getCharge( state.v_bat );

            if( state.v_bat > v_bat_prev )
            {
                if( state.v_bat >= ( v_bat_prev + STATE_TASK_BATTERY_HYSTERESIS ) )
                {
                    eventPayload |= EVENT_STATUS_BATTERY ;
                    v_bat_prev    = state.v_bat ;
                }
            }
            else
            {
                if( state.v_bat < v_bat_prev )
                {
                    if( state.v_bat <= ( v_bat_prev - STATE_TASK_BATTERY_HYSTERESIS ) )
                    {
                        eventPayload |= EVENT_STATUS_BATTERY ;
                        v_bat_prev    = state.v_bat ;
                    }
                }
            }

            state.rssi    = rtx_getRssi();

            if( state.rssi > rssi_prev )
            {
                if( state.rssi >= ( rssi_prev + STATE_TASK_RSSI_HYSTERESIS ) )
                {
                    eventPayload |= EVENT_STATUS_RSSI ;
                    rssi_prev     = state.rssi ;
                }
            }
            else
            {
                if( state.rssi < rssi_prev )
                {
                    if( state.rssi <= ( rssi_prev - STATE_TASK_RSSI_HYSTERESIS ) )
                    {
                        eventPayload |= EVENT_STATUS_RSSI ;
                        rssi_prev     = state.rssi ;
                    }
                }
            }

        }

        pthread_mutex_unlock( &state_mutex );

        if( eventPayload  )
        {
            ui_pushEvent( EVENT_TYPE_STATUS , (uint32_t)eventPayload );
        }

    }

}

void state_resetSettingsAndVfo( void )
{
    state.settings = default_settings;
    state.channel  = cps_getDefaultChannel();
}
