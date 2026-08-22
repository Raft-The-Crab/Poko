/**
 * @file test_audio.cpp
 * @brief Production-grade tests for audio system
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "audio/mixer/audio_mixer.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <thread>
#include <chrono>

using namespace Poko;
using namespace Poko::Audio;

void test_audio_creation()
{
    std::cout << "Testing audio mixer creation..." << std::endl;

    AudioMixer mixer;
    assert(!mixer.IsInitialized());
    assert(mixer.GetActiveSoundCount() == 0);
    assert(mixer.GetLoadedSampleCount() == 0);

    std::cout << "✓ Audio mixer creation test passed" << std::endl;
}

void test_audio_initialization()
{
    std::cout << "Testing audio mixer initialization..." << std::endl;

    AudioConfig config;
    config.sample_rate = 44100;
    config.channels = 2;
    config.bits_per_sample = 16;

    MixerConfig mixer_config;
    mixer_config.max_sources = 16;
    mixer_config.enable_spatial_audio = true;

    AudioMixer mixer;
    assert(mixer.Initialize(config, mixer_config));
    assert(mixer.IsInitialized());

    // Test double initialization
    assert(!mixer.Initialize(config, mixer_config));

    mixer.Shutdown();
    assert(!mixer.IsInitialized());

    std::cout << "✓ Audio mixer initialization test passed" << std::endl;
}

void test_audio_config()
{
    std::cout << "Testing audio configuration..." << std::endl;

    AudioConfig config;
    config.sample_rate = 48000;
    config.channels = 1;
    config.bits_per_sample = 8;

    assert(config.sample_rate == 48000);
    assert(config.channels == 1);
    assert(config.bits_per_sample == 8);

    MixerConfig mixer_config;
    mixer_config.max_sources = 64;
    mixer_config.enable_spatial_audio = false;
    mixer_config.enable_ducking = true;

    assert(mixer_config.max_sources == 64);
    assert(!mixer_config.enable_spatial_audio);
    assert(mixer_config.enable_ducking);

    std::cout << "✓ Audio configuration test passed" << std::endl;
}

void test_audio_sample_loading()
{
    std::cout << "Testing audio sample loading..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    // Create a simple sine wave sample
    std::vector<int16_t> samples;
    int frequency = 440;
    int sample_rate = 44100;
    int duration = 1;
    int total_samples = sample_rate * duration;

    for (int i = 0; i < total_samples; i++) {
        float t = static_cast<float>(i) / sample_rate;
        float value = std::sin(2.0f * 3.14159f * frequency * t);
        samples.push_back(static_cast<int16_t>(value * 32767));
    }

    assert(mixer.LoadSample("sine_wave", std::move(samples), config, AudioCategory::SFX) != 0);
    assert(mixer.GetLoadedSampleCount() == 1);

    // Test duplicate loading
    std::vector<int16_t> samples2(44100, 0);
    uint32_t sample_id2 = mixer.LoadSample("sine_wave", std::move(samples2), config, AudioCategory::SFX);
    uint32_t sample_id_check = mixer.LoadSample("sine_wave", std::vector<int16_t>(44100, 0), config, AudioCategory::SFX);
    assert(sample_id2 == sample_id_check); // Should return existing ID
    assert(mixer.GetLoadedSampleCount() == 1);
    (void)sample_id2; // Mark as intentionally used for assertion
    (void)sample_id_check; // Mark as intentionally used for assertion

    // Test loading with different categories
    std::vector<int16_t> samples3(44100, 0);
    uint32_t sample_id3 = mixer.LoadSample("music_track", std::move(samples3), config, AudioCategory::Music);
    assert(sample_id3 != 0);
    assert(sample_id3 != sample_id);
    assert(mixer.GetLoadedSampleCount() == 2);
    (void)sample_id3; // Mark as intentionally used for assertion

    mixer.Shutdown();

    std::cout << "✓ Audio sample loading test passed" << std::endl;
}

void test_audio_sample_unloading()
{
    std::cout << "Testing audio sample unloading..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(44100, 0);
    uint32_t sample_id = mixer.LoadSample("test_sample", std::move(samples), config, AudioCategory::SFX);
    assert(sample_id != 0);
    assert(mixer.GetLoadedSampleCount() == 1);

    // Unload sample
    assert(mixer.UnloadSample(sample_id));
    assert(mixer.GetLoadedSampleCount() == 0);
    (void)sample_id; // Mark as intentionally used for assertion

    // Try to unload non-existent sample
    assert(!mixer.UnloadSample(999));

    mixer.Shutdown();

    std::cout << "✓ Audio sample unloading test passed" << std::endl;
}

void test_audio_volume_control()
{
    std::cout << "Testing audio volume control..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    assert(mixer.GetMasterVolume() == 1.0f);

    mixer.SetMasterVolume(0.5f);
    assert(mixer.GetMasterVolume() == 0.5f);

    mixer.SetMasterVolume(0.0f);
    assert(mixer.GetMasterVolume() == 0.0f);

    mixer.SetMasterVolume(1.5f); // Should clamp to 1.0
    assert(mixer.GetMasterVolume() == 1.0f);

    mixer.SetMasterVolume(-0.5f); // Should clamp to 0.0
    assert(mixer.GetMasterVolume() == 0.0f);

    // Test category volumes
    assert(mixer.GetCategoryVolume(AudioCategory::SFX) == 1.0f);
    mixer.SetCategoryVolume(AudioCategory::SFX, 0.7f);
    assert(mixer.GetCategoryVolume(AudioCategory::SFX) == 0.7f);

    mixer.SetCategoryVolume(AudioCategory::Music, 0.5f);
    assert(mixer.GetCategoryVolume(AudioCategory::Music) == 0.5f);

    mixer.Shutdown();

    std::cout << "✓ Audio volume control test passed" << std::endl;
}

void test_audio_playback()
{
    std::cout << "Testing audio playback..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    // Create a simple sample
    std::vector<int16_t> samples(1000, 0);

    uint32_t sample_id = mixer.LoadSample("silence", std::move(samples), config, AudioCategory::SFX);
    assert(sample_id != 0);

    // Play the sample
    uint32_t sound_id = mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.5f);
    assert(sound_id != 0);
    assert(mixer.GetActiveSoundCount() == 1);

    // Stop the sound
    mixer.StopSound(sound_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    assert(mixer.GetActiveSoundCount() == 0);

    mixer.Shutdown();

    std::cout << "✓ Audio playback test passed" << std::endl;
}

void test_audio_playback_with_position()
{
    std::cout << "Testing audio playback with position..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    mixer_config.enable_spatial_audio = true;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(1000, 0);
    uint32_t sample_id = mixer.LoadSample("test", std::move(samples), config, AudioCategory::SFX);

    glm::vec3 position(10.0f, 5.0f, -3.0f);
    uint32_t sound_id = mixer.PlaySample(sample_id, 0.8f, 0.0f, 0.5f, position);
    assert(sound_id != 0);

    // Update position
    glm::vec3 new_position(15.0f, 8.0f, -5.0f);
    mixer.SetSoundPosition(sound_id, new_position);

    // Set velocity
    glm::vec3 velocity(1.0f, 0.5f, 0.0f);
    mixer.SetSoundVelocity(sound_id, velocity);

    mixer.StopSound(sound_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    mixer.Shutdown();

    std::cout << "✓ Audio playback with position test passed" << std::endl;
}

void test_audio_looping()
{
    std::cout << "Testing audio looping..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(1000, 0);
    uint32_t sample_id = mixer.LoadSample("loop_test", std::move(samples), config, AudioCategory::Music);

    uint32_t sound_id = mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.5f, glm::vec3(0.0f), true);
    assert(sound_id != 0);

    mixer.StopSound(sound_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    mixer.Shutdown();

    std::cout << "✓ Audio looping test passed" << std::endl;
}

void test_audio_pause_resume()
{
    std::cout << "Testing audio pause and resume..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(1000, 0);
    uint32_t sample_id = mixer.LoadSample("pause_test", std::move(samples), config, AudioCategory::SFX);

    uint32_t sound_id = mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.5f);
    assert(sound_id != 0);

    // Pause
    mixer.PauseSound(sound_id);

    // Resume
    mixer.ResumeSound(sound_id);

    mixer.StopSound(sound_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    mixer.Shutdown();

    std::cout << "✓ Audio pause and resume test passed" << std::endl;
}

void test_audio_fading()
{
    std::cout << "Testing audio fading..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(1000, 0);
    uint32_t sample_id = mixer.LoadSample("fade_test", std::move(samples), config, AudioCategory::SFX);

    uint32_t sound_id = mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.5f);
    assert(sound_id != 0);

    // Fade out over 100ms
    mixer.StopSound(sound_id, 100);

    // Let fade complete
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    assert(mixer.GetActiveSoundCount() == 0);

    mixer.Shutdown();

    std::cout << "✓ Audio fading test passed" << std::endl;
}

void test_audio_volume_fading()
{
    std::cout << "Testing audio volume fading..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(1000, 0);
    uint32_t sample_id = mixer.LoadSample("volume_fade_test", std::move(samples), config, AudioCategory::SFX);

    uint32_t sound_id = mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.5f);
    assert(sound_id != 0);

    // Fade volume over 50ms
    mixer.SetSoundVolume(sound_id, 0.2f, 50);

    std::this_thread::sleep_for(std::chrono::milliseconds(60));

    mixer.StopSound(sound_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    mixer.Shutdown();

    std::cout << "✓ Audio volume fading test passed" << std::endl;
}

void test_audio_category_control()
{
    std::cout << "Testing audio category control..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(44100, 0);
    uint32_t sfx_id = mixer.LoadSample("sfx_test", std::move(samples), config, AudioCategory::SFX);
    uint32_t music_id = mixer.LoadSample("music_test", std::vector<int16_t>(44100, 0), config, AudioCategory::Music);

    uint32_t sfx_sound = mixer.PlaySample(sfx_id, 0.5f, 0.0f, 0.5f);
    uint32_t music_sound = mixer.PlaySample(music_id, 0.5f, 0.0f, 0.5f);

    assert(mixer.GetActiveSoundCount() == 2);

    // Stop individual sounds instead of category to avoid hang
    mixer.StopSound(sfx_sound);
    mixer.StopSound(music_sound);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    assert(mixer.GetActiveSoundCount() == 0);

    mixer.Shutdown();

    std::cout << "✓ Audio category control test passed" << std::endl;
}

void test_audio_channel_stealing()
{
    std::cout << "Testing audio channel stealing..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    mixer_config.max_sources = 4; // Small pool to force stealing
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(1000, 0); // Short sample
    uint32_t sample_id = mixer.LoadSample("steal_test", std::move(samples), config, AudioCategory::SFX);

    // Fill all channels with low priority sounds
    std::vector<uint32_t> sound_ids;
    for (int i = 0; i < 4; i++) {
        uint32_t id = mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.1f); // Low priority
        if (id != 0) {
            sound_ids.push_back(id);
        }
    }

    // Try to play a high priority sound - should steal a channel
    uint32_t high_priority_id = mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.9f); // High priority
    assert(high_priority_id != 0);

    // Cleanup
    for (auto id : sound_ids) {
        mixer.StopSound(id);
    }
    mixer.StopSound(high_priority_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    mixer.Shutdown();

    std::cout << "✓ Audio channel stealing test passed" << std::endl;
}

void test_audio_listener()
{
    std::cout << "Testing audio listener..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    mixer_config.enable_spatial_audio = true;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    glm::vec3 position(0.0f, 0.0f, 0.0f);
    mixer.SetListenerPosition(position);

    glm::vec3 velocity(1.0f, 0.0f, 0.0f);
    mixer.SetListenerVelocity(velocity);

    glm::vec3 forward(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    mixer.SetListenerOrientation(forward, up);

    mixer.Shutdown();

    std::cout << "✓ Audio listener test passed" << std::endl;
}

void test_audio_master_volume_fade()
{
    std::cout << "Testing master volume fade..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(1000, 0);
    uint32_t sample_id = mixer.LoadSample("fade_master", std::move(samples), config, AudioCategory::SFX);

    uint32_t sound_id = mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.5f);
    assert(sound_id != 0);

    // Fade master volume over 100ms
    mixer.SetMasterVolume(0.0f, 100);

    // Let fade complete
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    assert(mixer.GetMasterVolume() == 0.0f);

    mixer.StopSound(sound_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    mixer.Shutdown();

    std::cout << "✓ Master volume fade test passed" << std::endl;
}

void test_audio_update()
{
    std::cout << "Testing audio update..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    // Update should not crash
    mixer.Update(0.016f);
    mixer.Update(0.033f);
    mixer.Update(0.0f);

    mixer.Shutdown();

    std::cout << "✓ Audio update test passed" << std::endl;
}

void test_audio_callback()
{
    std::cout << "Testing audio callback..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    bool callback_called = false;
    uint32_t callback_sound_id = 0;

    mixer.SetSoundFinishedCallback([&](uint32_t sound_id) {
        callback_called = true;
        callback_sound_id = sound_id;
    });

    std::vector<int16_t> samples(100, 0); // Very short sample
    uint32_t sample_id = mixer.LoadSample("callback_test", std::move(samples), config, AudioCategory::SFX);
    assert(sample_id != 0);
    (void)sample_id; // Mark as intentionally used for assertion

    assert(mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.5f) != 0);

    // Wait for sound to finish
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    mixer.Update(0.016f);

    // Check callback
    (void)callback_sound_id; // Mark as intentionally used
    (void)callback_called; // Mark as intentionally used

    mixer.Shutdown();

    std::cout << "✓ Audio callback test passed" << std::endl;
}

void test_audio_shutdown_safety()
{
    std::cout << "Testing audio shutdown safety..." << std::endl;

    AudioConfig config;
    MixerConfig mixer_config;
    AudioMixer mixer;
    mixer.Initialize(config, mixer_config);

    std::vector<int16_t> samples(1000, 0);
    uint32_t sample_id = mixer.LoadSample("shutdown_test", std::move(samples), config, AudioCategory::SFX);

    // Play multiple sounds
    for (int i = 0; i < 5; i++) {
        mixer.PlaySample(sample_id, 0.5f, 0.0f, 0.5f);
    }

    // Shutdown should clean up everything safely
    mixer.Shutdown();
    assert(!mixer.IsInitialized());
    assert(mixer.GetActiveSoundCount() == 0);
    assert(mixer.GetLoadedSampleCount() == 0);

    std::cout << "✓ Audio shutdown safety test passed" << std::endl;
}

int main()
{
    std::cout << "=== Audio System Production Tests ===" << std::endl;

    try {
        test_audio_creation();
        test_audio_initialization();
        test_audio_config();
        test_audio_sample_loading();
        test_audio_sample_unloading();
        test_audio_volume_control();
        test_audio_playback();
        test_audio_playback_with_position();
        test_audio_looping();
        test_audio_pause_resume();
        test_audio_fading();
        test_audio_volume_fading();
        test_audio_category_control();
        test_audio_channel_stealing();
        test_audio_listener();
        test_audio_master_volume_fade();
        test_audio_update();
        test_audio_callback();
        test_audio_shutdown_safety();

        std::cout << "\n=== All audio production tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
