/**
 * @file error.h
 * @brief Core error handling system for Poko Engine
 * @details Provides structured error handling with error codes, categories, and context
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <string>
#include <system_error>
#include <stdexcept>
#include <memory>
#include <vector>

namespace poko {
namespace core {

/**
 * @enum ErrorCode
 * @brief Standard error codes for Poko Engine
 */
enum class ErrorCode {
    SUCCESS = 0,
    UNKNOWN_ERROR,
    INVALID_ARGUMENT,
    OUT_OF_MEMORY,
    FILE_NOT_FOUND,
    PERMISSION_DENIED,
    NETWORK_ERROR,
    TIMEOUT,
    SERIALIZATION_ERROR,
    DESERIALIZATION_ERROR,
    ENGINE_NOT_INITIALIZED,
    RESOURCE_LOAD_FAILED,
    RENDERING_ERROR,
    PHYSICS_ERROR,
    AUDIO_ERROR,
    SCRIPT_ERROR,
    INVALID_STATE,
    OPERATION_FAILED
};

/**
 * @enum ErrorCategory
 * @brief Categories for error classification
 */
enum class ErrorCategory {
    GENERAL,
    IO,
    NETWORK,
    RENDERING,
    PHYSICS,
    AUDIO,
    SCRIPT,
    MEMORY,
    SYSTEM
};

/**
 * @class Error
 * @brief Comprehensive error information container
 * @details Contains error code, category, message, and context information
 */
class Error {
public:
    /**
     * @brief Construct an error with basic information
     * @param code The error code
     * @param category The error category
     * @param message Human-readable error message
     */
    Error(ErrorCode code, ErrorCategory category, const std::string& message);

    /**
     * @brief Construct an error with source location information
     * @param code The error code
     * @param category The error category
     * @param message Human-readable error message
     * @param file Source file where error occurred
     * @param line Source line where error occurred
     * @param function Function where error occurred
     */
    Error(ErrorCode code, ErrorCategory category, const std::string& message,
          const std::string& file, int line, const std::string& function);

    /**
     * @brief Get the error code
     * @return The error code
     */
    ErrorCode code() const { return code_; }

    /**
     * @brief Get the error category
     * @return The error category
     */
    ErrorCategory category() const { return category_; }

    /**
     * @brief Get the error message
     * @return Human-readable error message
     */
    const std::string& message() const { return message_; }

    /**
     * @brief Get the source file
     * @return Source file name (if available)
     */
    const std::string& file() const { return file_; }

    /**
     * @brief Get the source line
     * @return Source line number (if available)
     */
    int line() const { return line_; }

    /**
     * @brief Get the function name
     * @return Function name (if available)
     */
    const std::string& function() const { return function_; }

    /**
     * @brief Get a formatted error string
     * @return Formatted error description
     */
    std::string to_string() const;

    /**
     * @brief Check if this error represents success
     * @return true if error code is SUCCESS
     */
    bool is_success() const { return code_ == ErrorCode::SUCCESS; }

    /**
     * @brief Add context information to the error
     * @param key Context key
     * @param value Context value
     */
    void add_context(const std::string& key, const std::string& value);

    /**
     * @brief Get context information
     * @param key Context key
     * @return Context value if exists, empty string otherwise
     */
    std::string get_context(const std::string& key) const;

private:
    ErrorCode code_;
    ErrorCategory category_;
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::vector<std::pair<std::string, std::string>> context_;
};

/**
 * @class Result
 * @brief Type-safe result wrapper for operations that can fail
 * @details Provides either a value or an error, similar to Rust's Result type
 * @tparam T The type of the success value
 */
template<typename T>
class Result {
public:
    /**
     * @brief Construct a successful result
     * @param value The success value
     */
    explicit Result(T value) : value_(std::move(value)), error_(ErrorCode::SUCCESS, ErrorCategory::GENERAL, "") {}

    /**
     * @brief Construct a failed result
     * @param error The error that occurred
     */
    explicit Result(Error error) : error_(std::move(error)) {}

    /**
     * @brief Check if the result is successful
     * @return true if successful
     */
    bool is_success() const { return error_.is_success(); }

    /**
     * @brief Check if the result failed
     * @return true if failed
     */
    bool is_error() const { return !is_success(); }

    /**
     * @brief Get the success value
     * @return Reference to the value
     * @throws std::runtime_error if result is an error
     */
    T& value() {
        if (is_error()) {
            throw std::runtime_error("Attempted to get value from error result");
        }
        return value_;
    }

    /**
     * @brief Get the success value (const)
     * @return Const reference to the value
     * @throws std::runtime_error if result is an error
     */
    const T& value() const {
        if (is_error()) {
            throw std::runtime_error("Attempted to get value from error result");
        }
        return value_;
    }

    /**
     * @brief Get the error
     * @return Reference to the error
     */
    Error& error() { return error_; }

    /**
     * @brief Get the error (const)
     * @return Const reference to the error
     */
    const Error& error() const { return error_; }

    /**
     * @brief Get the value or a default if error
     * @param default_value Default value to return on error
     * @return The value or default
     */
    T value_or(T default_value) const {
        if (is_error()) {
            return default_value;
        }
        return value_;
    }

private:
    T value_;
    Error error_;
};

/**
 * @brief Specialization for void results
 */
template<>
class Result<void> {
public:
    /**
     * @brief Construct a successful void result
     */
    Result() : error_(ErrorCode::SUCCESS, ErrorCategory::GENERAL, "") {}

    /**
     * @brief Construct a failed result
     * @param error The error that occurred
     */
    explicit Result(Error error) : error_(std::move(error)) {}

    /**
     * @brief Check if the result is successful
     * @return true if successful
     */
    bool is_success() const { return error_.is_success(); }

    /**
     * @brief Check if the result failed
     * @return true if failed
     */
    bool is_error() const { return !is_success(); }

    /**
     * @brief Get the error
     * @return Reference to the error
     */
    Error& error() { return error_; }

    /**
     * @brief Get the error (const)
     * @return Const reference to the error
     */
    const Error& error() const { return error_; }

private:
    Error error_;
};

/**
 * @class Exception
 * @brief Poko-specific exception class
 * @details Extends std::exception with Poko error information
 */
class Exception : public std::runtime_error {
public:
    /**
     * @brief Construct an exception from an Error
     * @param error The error to wrap
     */
    explicit Exception(const Error& error);

    /**
     * @brief Get the wrapped error
     * @return Reference to the error
     */
    const Error& error() const { return error_; }

private:
    Error error_;
};

// Convenience macros for error creation with source location
#define POKO_ERROR(code, category, message) \
    ::poko::core::Error(code, category, message, __FILE__, __LINE__, __FUNCTION__)

#define POKO_MAKE_ERROR(code, message) \
    POKO_ERROR(code, ::poko::core::ErrorCategory::GENERAL, message)

// Try-catch macro for error propagation
#define POKO_TRY(expr) \
    do { \
        auto result = (expr); \
        if (result.is_error()) { \
            return ::poko::core::Result<decltype(result.value())>(result.error()); \
        } \
    } while(0)

} // namespace core
} // namespace poko
