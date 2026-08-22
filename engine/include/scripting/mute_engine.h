/**
 * @file mute_engine.h
 * @brief Mute VM integration interface
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#ifndef POKO_SCRIPTING_MUTE_ENGINE_H
#define POKO_SCRIPTING_MUTE_ENGINE_H

#include <string>
#include <cstddef>

namespace poko {

// Forward declaration for Mute VM
struct MuteVM;

/**
 * @brief Mute engine configuration
 */
struct MuteEngineConfig {
    size_t stack_size = 1024 * 1024;        // 1MB stack
    size_t heap_size = 16 * 1024 * 1024;    // 16MB heap
    size_t max_objects = 10000;
    double frame_budget_ms = 1.5;
    bool enable_gc = true;
    bool enable_profiling = false;
};

/**
 * @brief Mute C function type
 */
using MuteCFunction = void(*)(void*);

/**
 * @brief Mute VM engine wrapper
 *
 * Provides C++ interface to the Mute scripting VM.
 */
class MuteEngine {
public:
    /**
     * @brief Mute engine configuration
     */
    using Config = MuteEngineConfig;

    /**
     * @brief Construct Mute engine with configuration
     * @param config Mute engine configuration
     */
    explicit MuteEngine(const Config& config);

    /**
     * @brief Destructor
     */
    ~MuteEngine();

    /**
     * @brief Initialize the Mute VM
     * @return true if initialization succeeded
     */
    bool initialize();

    /**
     * @brief Shutdown the Mute VM
     */
    void shutdown();

    /**
     * @brief Execute a Mute script string
     * @param source Mute source code
     * @return true if execution succeeded
     */
    bool execute_string(const std::string& source);

    /**
     * @brief Execute a Mute script file
     * @param filename Path to Mute source file
     * @return true if execution succeeded
     */
    bool execute_file(const std::string& filename);

    /**
     * @brief Get last error message
     * @return Error message string
     */
    std::string get_last_error() const;

    /**
     * @brief Get last error code
     * @return Error code
     */
    int get_last_error_code() const;

    /**
     * @brief Get memory used by VM
     * @return Memory usage in bytes
     */
    size_t get_memory_used() const;

    /**
     * @brief Get memory allocated by VM
     * @return Allocated memory in bytes
     */
    size_t get_memory_allocated() const;

    /**
     * @brief Force garbage collection
     */
    void collect_garbage();

    /**
     * @brief Register a C function
     * @param name Function name
     * @param func C function pointer
     */
    void register_function(const std::string& name, MuteCFunction func);

    /**
     * @brief Register engine bindings
     * @param engine Pointer to engine instance
     */
    void register_engine_bindings(void* engine);

    /**
     * @brief Check if initialized
     * @return true if initialized
     */
    bool is_initialized() const { return initialized_; }

private:
    void register_standard_libraries();

    Config config_;
    bool initialized_ = false;
    MuteVM* vm_ = nullptr;
    std::string last_error_;
    int last_error_code_ = 0;
};

} // namespace poko

#endif // POKO_SCRIPTING_MUTE_ENGINE_H