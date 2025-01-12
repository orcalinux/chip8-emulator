/**
 * @file audio.h
 * @brief Audio module for playing beep sounds in the CHIP-8 emulator.
 *
 * This header declares a simple interface for initializing and managing
 * continuous beep audio using SDL2_mixer. The actual implementation details
 * are hidden in audio.c, following an OOP-like design pattern in C.
 */

#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initializes the audio system and loads a beep sound.
     *
     * This function sets up the SDL2_mixer library and loads the specified
     * WAV file into memory. The loaded sound is used for continuous playback
     * when the CHIP-8 sound timer is non-zero.
     *
     * @param wav_path The file path to the .wav file to load.
     * @return True on success, false on failure.
     *
     * Example usage:
     * @code
     *   if (!audio_init("assets/beep.wav")) {
     *       fprintf(stderr, "Failed to init audio.\n");
     *       return EXIT_FAILURE;
     *   }
     * @endcode
     */
    bool audio_init(const char *wav_path);

    /**
     * @brief Plays the beep sound in a continuous loop.
     *
     * If the sound is already playing, this function will do nothing.
     * Otherwise, it starts looping the beep indefinitely on a free channel.
     *
     * Example usage:
     * @code
     *   // Start the beep when sound_timer is set
     *   audio_play_beep_loop();
     * @endcode
     */
    void audio_play_beep_loop(void);

    /**
     * @brief Stops the looping beep sound.
     *
     * This function halts playback on the channel currently playing the beep.
     * If no beep is playing, it does nothing.
     *
     * Example usage:
     * @code
     *   // Stop the beep when sound_timer reaches zero
     *   audio_stop_beep();
     * @endcode
     */
    void audio_stop_beep(void);

    /**
     * @brief Checks if the beep sound is currently playing.
     *
     * @return True if the beep is playing, false otherwise.
     *
     * Example usage:
     * @code
     *   if (!audio_is_beep_playing()) {
     *       audio_play_beep_loop();
     *   }
     * @endcode
     */
    bool audio_is_beep_playing(void);

    /**
     * @brief Cleans up audio resources and shuts down SDL_mixer.
     *
     * This function frees the loaded sound chunk and calls Mix_CloseAudio().
     * It should be invoked once before your program exits to properly
     * release any allocated resources.
     *
     * Example usage:
     * @code
     *   audio_cleanup();
     * @endcode
     */
    void audio_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_H
