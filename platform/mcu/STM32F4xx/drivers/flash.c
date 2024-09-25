/***************************************************************************
 *   Copyright (C) 2021 - 2022  by Federico Amedeo Izzo IU2NUO,            *
 *                                 Niccol� Izzo IU2KIN                     *
 *                                 Frederik Saraci IU2NRO                  *
 *                                 Silvano Seva IU2KWO                     *
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

#include <stm32f4xx.h>
#include <stddef.h>
#include "flash.h"

/**
 * \internal
 * Utility function performing unlock of flash erase and write access.
 *
 * @true on success, false on failure.
 */
static inline bool unlock( void )
{
    // Flash already unlocked
    if( ( FLASH->CR & FLASH_CR_LOCK ) == 0 )
    {
        return true ;
    }

    FLASH->KEYR = 0x45670123 ;
    FLASH->KEYR = 0xCDEF89AB ;

    // Succesful unlock
    if( ( FLASH->CR & FLASH_CR_LOCK ) == 0 )
    {
        return true ;
    }

    return false ;
}

bool flash_eraseSector( const uint8_t secNum )
{
    bool result = true ;

    if( secNum > 11 )
    {
        result = false ;
    }
    else
    {
        if( unlock() == false )
        {
            result = false ;
        }
        else
        {
            // Flash busy, wait until previous operation finishes
            while( ( FLASH->SR & FLASH_SR_BSY ) != 0 );

            FLASH->CR |= FLASH_CR_SER ;     // Sector erase
            FLASH->CR &= ~FLASH_CR_SNB ;
            FLASH->CR |= secNum << 3 ;      // Sector number
            FLASH->CR |= FLASH_CR_STRT ;    // Start erase

            // Wait until erase ends
            while( ( FLASH->SR & FLASH_SR_BSY ) != 0 );

            FLASH->CR &= ~FLASH_CR_SER ;
        }
    }

    return result ;
}

int flash_write( const uint32_t address , const void* data , const size_t len )
{
    const uint8_t* buf ;
    uint8_t*       mem ;
    size_t         index ;
    int            result = 0 ;

    if( unlock() == false )
    {
        result = -1 ;
    }
    else
    {
        if( ( data != NULL ) &&
            ( len  != 0    )    )
        {
            // Write data to memory, 8 bit at a time
            buf = (uint8_t*)data ;
            mem = (uint8_t*)address ;

            for( index = 0 ; index < len ; index++ , mem++ )
            {
                while( ( FLASH->SR & FLASH_SR_BSY ) != 0 ) ;

                FLASH->CR  = FLASH_CR_PG ;

                *mem       = buf[ index ] ;

                while( ( FLASH->SR & FLASH_SR_BSY ) != 0 ) ;

                FLASH->CR &= ~FLASH_CR_PG ;

            }
        }
    }

    return result ;

}
