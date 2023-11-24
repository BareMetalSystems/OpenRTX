/***************************************************************************
 *   Copyright (C) 2022 - 2023 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN,                     *
 *                                Silvano Seva IU2KWO                      *
 *                                Joseph Stephen VK7JS                     *
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
#ifndef VOICE_PROMPT_UTILS_H
#define VOICE_PROMPT_UTILS_H

#include "cps.h"
#include "ui/ui_strings.h"
#include "voicePrompts.h"

/*
Please Note!

Many of the functions take queue flags because sometimes messages must be
played in sequence (i.e. the announceXX functions may be called one after the
other) and thus the init must only be sent prior to the first message queued
and the play must only be invoked after the last message queued.

When an announceXX function is called in isolation, VPQ_INIT|VPQ_PLAY_IMMEDIATELY
should be used to ensure that the message interupts the current prompt and
plays immediately.
*/


/**
 * Note: channelNumber is 1-based, index is 0-based.
 */
void vp_announceChannelName(const channel_t* channel,
                            const uint16_t channelNumber,
                            const VPQueueFlags_en flags);

/**
 *
 */
void vp_queueFrequency(const freq_t freq);

/**
 *
 */
void vp_announceFrequencies(const freq_t rx, const freq_t tx,
                            const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceRadioMode(const uint8_t mode, const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceBandwidth(const uint8_t bandwidth, const VPQueueFlags_en flags);

/**
 * channelNumber is 1-based, channelIndex is 0-based.
 */
void vp_announceChannelSummary(const channel_t* channel,
                               const uint16_t channelNumber, const uint16_t bank,
                               const VPSummaryInfoFlags_en infoFlags);

/**
 *
 */
void vp_announceInputChar(const char ch);

/**
 *
 */
void vp_announceInputReceiveOrTransmit(const bool tx, const VPQueueFlags_en flags);

/**
 *
 */
void vp_replayLastPrompt();

/**
 *
 */
void vp_announceError(const VPQueueFlags_en flags);

/*
This function first tries to see if we have a prompt for the text
passed in and if so, queues it, but if not, just spells the text
character by character.
*/

/**
 *
 */
void vp_announceText(const char* text, const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceCTCSS(const bool rxToneEnabled, const uint8_t rxTone,
                      const bool txToneEnabled, const uint8_t txTone,
                      const VPQueueFlags_en flags);

/**
 *
 */
void vp_anouncePower(const float power, const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceSquelch(const uint8_t squelch,const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceContact(const contact_t* contact, const VPQueueFlags_en flags);

/**
 *
 */
bool vp_announceContactWithIndex(const uint16_t index, const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceTimeslot(const uint8_t timeslot, const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceColorCode(const uint8_t rxColorCode, const uint8_t txColorCode,
                          const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceBank(const uint16_t bank, const VPQueueFlags_en flags);

/**
 *
 */
void vp_announceM17Info(const channel_t* channel, bool isEditing,
                        const VPQueueFlags_en flags);

/**
 *
 */
#ifdef GPS_PRESENT
void vp_announceGPSInfo(VPGPSInfoFlags_t gpsInfoFlags);
#endif

/**
 *
 */
void vp_announceAboutScreen();

/**
 *
 */
void vp_announceBackupScreen();

/**
 *
 */
void vp_announceRestoreScreen();

/**
 *
 */
#ifdef RTC_PRESENT
void vp_announceSettingsTimeDate();
#endif

/**
 * This is called to speak voice prompt level changes.
 */
void vp_announceSettingsVoiceLevel(const VPQueueFlags_en flags);

/**
 * This is called to speak generic settings on/off toggles.
 */
void vp_announceSettingsOnOffToggle(const char* const* stringTableStringPtr,
                                    const VPQueueFlags_en flags, bool val);

/**
 * This is called to speak generic settings int values.
 */
void vp_announceSettingsInt(const char* const* stringTableStringPtr,
                            const VPQueueFlags_en flags, int val);

/**
 * This function is called from  ui_updateFSM to speak informational screens.
 */
void vp_announceScreen(uint8_t ui_screen);

/**
 * This function is called from  ui_updateFSM to speak string buffers.
 */
void vp_announceBuffer(const char* const* stringTableStringPtr, bool editMode,
                       bool callsign, const char* buffer);

/**
 * This function is called from  ui_updateFSM to speak display timer changes.
 */
void vp_announceDisplayTimer();

/**
 *
 */
VPQueueFlags_en vp_getVoiceLevelQueueFlags();

/**
 *
 */
void vp_playMenuBeepIfNeeded(bool firstItem);

/**
 *
 */
void vp_announceSplashScreen();

/**
 * Announce timezone value.
 *
 * @param timeZone: timezone value.
 * @param flags: control flags.
 */
void vp_announceTimeZone(const int8_t timeZone, const VPQueueFlags_en flags);

#endif  // VOICE_PROMPT_UTILS_H
