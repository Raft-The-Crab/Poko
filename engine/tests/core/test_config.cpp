/**
 * @file test_config.cpp
 * @brief Basic tests for configuration system
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/config/config.h"
#include <iostream>
#include <cassert>
#include <fstream>

using namespace Poko;

void test_config_creation()
{
    std::cout << "Testing config creation..." << std::endl;
    
    Config config;
    
    // Check default values
    assert(config.GetWindowWidth() == 1280);
    assert(config.GetWindowHeight() == 720);
    assert(config.GetWindowTitle() == "PokoEngine");
    assert(config.IsFullscreen() == false);
    assert(config.IsVSyncEnabled() == true);
    assert(config.GetTargetFPS() == 60);
    
    std::cout << "✓ Config creation test passed" << std::endl;
}

void test_config_modification()
{
    std::cout << "Testing config modification..." << std::endl;
    
    Config config;
    
    // Modify values
    config.SetWindowWidth(1920);
    config.SetWindowHeight(1080);
    config.SetWindowTitle("TestGame");
    config.SetFullscreen(true);
    config.SetVSync(false);
    config.SetTargetFPS(144);
    
    // Verify changes
    assert(config.GetWindowWidth() == 1920);
    assert(config.GetWindowHeight() == 1080);
    assert(config.GetWindowTitle() == "TestGame");
    assert(config.IsFullscreen() == true);
    assert(config.IsVSyncEnabled() == false);
    assert(config.GetTargetFPS() == 144);
    
    std::cout << "✓ Config modification test passed" << std::endl;
}

void test_config_file_operations()
{
    std::cout << "Testing config file operations..." << std::endl;
    
    // Skip file operations for now to keep tests simple
    // File operations will be tested when we have proper file system setup
    std::cout << "✓ Config file operations test skipped (filesystem not yet set up)" << std::endl;
}

void test_config_audio_settings()
{
    std::cout << "Testing config audio settings..." << std::endl;
    
    Config config;
    
    // Test default audio values
    assert(config.GetMasterVolume() == 1.0f);
    assert(config.GetMusicVolume() == 0.8f);
    assert(config.GetSFXVolume() == 1.0f);
    
    // Modify audio settings
    config.SetMasterVolume(0.5f);
    config.SetMusicVolume(0.3f);
    config.SetSFXVolume(0.7f);
    
    assert(config.GetMasterVolume() == 0.5f);
    assert(config.GetMusicVolume() == 0.3f);
    assert(config.GetSFXVolume() == 0.7f);
    
    std::cout << "✓ Config audio settings test passed" << std::endl;
}

void test_config_physics_settings()
{
    std::cout << "Testing config physics settings..." << std::endl;
    
    Config config;
    
    // Test default physics values
    assert(config.IsPhysicsEnabled() == true);
    assert(config.GetMaxPhysicsSubsteps() == 4);
    
    // Modify physics settings
    config.SetPhysicsEnabled(false);
    config.SetMaxPhysicsSubsteps(8);
    
    assert(config.IsPhysicsEnabled() == false);
    assert(config.GetMaxPhysicsSubsteps() == 8);
    
    std::cout << "✓ Config physics settings test passed" << std::endl;
}

int main()
{
    std::cout << "=== Configuration System Tests ===" << std::endl;
    
    try {
        test_config_creation();
        test_config_modification();
        // test_config_file_operations(); // Skipped for now
        test_config_audio_settings();
        test_config_physics_settings();
        
        std::cout << "\n=== All configuration tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}