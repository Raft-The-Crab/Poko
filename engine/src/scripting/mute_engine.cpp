/**
 * @file mute_engine.cpp
 * @brief Mute VM integration implementation
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#include "scripting/mute_engine.h"
#include "scripting/engine_bindings.h"
#include "public/mute.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace poko {

MuteEngine::MuteEngine(const Config& config)
    : config_(config)
{
}

MuteEngine::~MuteEngine() {
    shutdown();
}

bool MuteEngine::initialize() {
    std::cout << "Initializing Mute VM..." << std::endl;
    
    // Create Mute configuration
    MuteConfig mute_config;
    mute_config.stack_size = config_.stack_size;
    mute_config.heap_size = config_.heap_size;
    mute_config.max_objects = config_.max_objects;
    mute_config.frame_budget_ms = config_.frame_budget_ms;
    mute_config.enable_gc = config_.enable_gc;
    mute_config.enable_profiling = config_.enable_profiling;
    
    // Create VM
    vm_ = mute_create_vm(&mute_config);
    if (!vm_) {
        last_error_ = "Failed to create Mute VM";
        last_error_code_ = MUTE_ERROR_MEMORY;
        std::cerr << "Mute VM initialization failed: " << last_error_ << std::endl;
        return false;
    }
    
    // Register standard libraries
    register_standard_libraries();
    
    initialized_ = true;
    std::cout << "Mute VM initialized successfully" << std::endl;
    std::cout << "Stack size: " << config_.stack_size << " bytes" << std::endl;
    std::cout << "Heap size: " << config_.heap_size << " bytes" << std::endl;
    std::cout << "Frame budget: " << config_.frame_budget_ms << " ms" << std::endl;
    
    return true;
}

void MuteEngine::shutdown() {
    if (!initialized_) return;
    
    std::cout << "Shutting down Mute VM..." << std::endl;
    
    if (vm_) {
        mute_destroy_vm(vm_);
        vm_ = nullptr;
    }
    
    initialized_ = false;
    std::cout << "Mute VM shutdown complete" << std::endl;
}

bool MuteEngine::execute_string(const std::string& source) {
    if (!initialized_) {
        last_error_ = "Mute VM not initialized";
        last_error_code_ = MUTE_ERROR_RUNTIME;
        return false;
    }
    
    int result = mute_execute_string(vm_, source.c_str());
    
    if (result != MUTE_OK) {
        last_error_code_ = result;
        const char* error = mute_get_last_error(vm_);
        last_error_ = error ? error : "Unknown error";
        std::cerr << "Mute execution error: " << last_error_ << std::endl;
        return false;
    }
    
    return true;
}

bool MuteEngine::execute_file(const std::string& filename) {
    if (!initialized_) {
        last_error_ = "Mute VM not initialized";
        last_error_code_ = MUTE_ERROR_RUNTIME;
        return false;
    }
    
    // Read file
    std::ifstream file(filename);
    if (!file.is_open()) {
        last_error_ = "Failed to open file: " + filename;
        last_error_code_ = MUTE_ERROR_RUNTIME;
        std::cerr << last_error_ << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    return execute_string(source);
}

std::string MuteEngine::get_last_error() const {
    return last_error_;
}

int MuteEngine::get_last_error_code() const {
    return last_error_code_;
}

size_t MuteEngine::get_memory_used() const {
    if (!initialized_ || !vm_) return 0;
    return mute_get_memory_used(vm_);
}

size_t MuteEngine::get_memory_allocated() const {
    if (!initialized_ || !vm_) return 0;
    return mute_get_memory_allocated(vm_);
}

void MuteEngine::collect_garbage() {
    if (!initialized_ || !vm_) return;
    mute_collect_garbage(vm_);
}

void MuteEngine::register_standard_libraries() {
    if (!initialized_ || !vm_) return;
    
    std::cout << "Registering Mute standard libraries..." << std::endl;
    
    mute_register_math_library(vm_);
    mute_register_string_library(vm_);
    mute_register_table_library(vm_);
    mute_register_world_library(vm_);
    
    std::cout << "Standard libraries registered" << std::endl;
}

void MuteEngine::register_function(const std::string& name, MuteCFunction func) {
    if (!initialized_ || !vm_) return;
    
    mute_register_c_function(vm_, name.c_str(), func);
}

void MuteEngine::register_engine_bindings(void* engine) {
    if (!initialized_ || !vm_) return;
    
    ::poko::register_engine_bindings(vm_, engine);
}

} // namespace poko