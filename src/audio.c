/**
 * @file audio.c
 * @brief Implementation of the audio module for CHIP-8 beep sounds.
 *
 * This file contains the private data and definitions for handling
 * audio playback via SDL2_mixer. The public API is in audio.h.
 */

#include "audio.h"
#include "cli_logger.h"
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

static Mix_Chunk *gBeepChunk = NULL;
static int gBeepChannel = -1; // Store which channel is playing (-1 if none)

bool audio_init(const char *wav_path)
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        print_error("Mix_OpenAudio error: %s\n", Mix_GetError());
        return false;
    }

    gBeepChunk = Mix_LoadWAV(wav_path);
    if (!gBeepChunk)
    {
        print_error("Failed to load %s: %s\n", wav_path, Mix_GetError());
        return false;
    }

    // Optionally reserve a channel if you like (not strictly required)
    Mix_ReserveChannels(1);

    return true;
}

void audio_play_beep_loop(void)
{
    // If already playing, do nothing
    if (gBeepChannel >= 0 && Mix_Playing(gBeepChannel))
        return;

    // Play on first free channel (-1) with loop = -1 (infinite)
    gBeepChannel = Mix_PlayChannel(-1, gBeepChunk, -1);
    if (gBeepChannel == -1)
    {
        print_error("Mix_PlayChannel error: %s\n", Mix_GetError());
    }
}

void audio_stop_beep(void)
{
    if (gBeepChannel >= 0)
    {
        Mix_HaltChannel(gBeepChannel);
        gBeepChannel = -1;
    }
}

bool audio_is_beep_playing(void)
{
    if (gBeepChannel >= 0)
    {
        return Mix_Playing(gBeepChannel) != 0; // nonzero if still playing
    }
    return false;
}

void audio_cleanup(void)
{
    if (gBeepChunk)
    {
        Mix_FreeChunk(gBeepChunk);
        gBeepChunk = NULL;
    }
    Mix_CloseAudio();
}
