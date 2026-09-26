/**
 * @file error.cpp
 * @brief Implementation of the core error handling system
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "core/error.h"
#include <sstream>
#include <algorithm>

namespace poko {
namespace core {

// Error implementation
Error::Error(ErrorCode code, ErrorCategory category, const std::string& message)
    : code_(code), category_(category), message_(message),
      file_(""), line_(0), function_("") {}

Error::Error(ErrorCode code, ErrorCategory category, const std::string& message,
             const std::string& file, int line, const std::string& function)
    : code_(code), category_(category), message_(message),
      file_(file), line_(line), function_(function) {}

std::string Error::to_string() const {
    std::stringstream ss;

    // Add error code name
    switch (code_) {
        case ErrorCode::SUCCESS: ss << "SUCCESS"; break;
        case ErrorCode::UNKNOWN_ERROR: ss << "UNKNOWN_ERROR"; break;
        case ErrorCode::INVALID_ARGUMENT: ss << "INVALID_ARGUMENT"; break;
        case ErrorCode::OUT_OF_MEMORY: ss << "OUT_OF_MEMORY"; break;
        case ErrorCode::FILE_NOT_FOUND: ss << "FILE_NOT_FOUND"; break;
        case ErrorCode::PERMISSION_DENIED: ss << "PERMISSION_DENIED"; break;
        case ErrorCode::NETWORK_ERROR: ss << "NETWORK_ERROR"; break;
        case ErrorCode::TIMEOUT: ss << "TIMEOUT"; break;
        case ErrorCode::SERIALIZATION_ERROR: ss << "SERIALIZATION_ERROR"; break;
        case ErrorCode::DESERIALIZATION_ERROR: ss << "DESERIALIZATION_ERROR"; break;
        case ErrorCode::ENGINE_NOT_INITIALIZED: ss << "ENGINE_NOT_INITIALIZED"; break;
        case ErrorCode::RESOURCE_LOAD_FAILED: ss << "RESOURCE_LOAD_FAILED"; break;
        case ErrorCode::RENDERING_ERROR: ss << "RENDERING_ERROR"; break;
        case ErrorCode::PHYSICS_ERROR: ss << "PHYSICS_ERROR"; break;
        case ErrorCode::AUDIO_ERROR: ss << "AUDIO_ERROR"; break;
        case ErrorCode::SCRIPT_ERROR: ss << "SCRIPT_ERROR"; break;
        case ErrorCode::INVALID_STATE: ss << "INVALID_STATE"; break;
        case ErrorCode::OPERATION_FAILED: ss << "OPERATION_FAILED"; break;
        default: ss << "UNKNOWN_CODE"; break;
    }

    ss << ": " << message_;

    // Add source location if available
    if (!file_.empty()) {
        ss << " (" << file_;
        if (line_ > 0) {
            ss << ":" << line_;
        }
        if (!function_.empty()) {
            ss << " in " << function_;
        }
        ss << ")";
    }

    // Add context information if available
    if (!context_.empty()) {
        ss << " [";
        for (size_t i = 0; i < context_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << context_[i].first << "=" << context_[i].second;
        }
        ss << "]";
    }

    return ss.str();
}

void Error::add_context(const std::string& key, const std::string& value) {
    context_.emplace_back(key, value);
}

std::string Error::get_context(const std::string& key) const {
    auto it = std::find_if(context_.begin(), context_.end(),
        [&key](const auto& pair) { return pair.first == key; });

    if (it != context_.end()) {
        return it->second;
    }
    return "";
}

// Exception implementation
Exception::Exception(const Error& error)
    : std::runtime_error(error.to_string()), error_(error) {}

} // namespace core
} // namespace poko
