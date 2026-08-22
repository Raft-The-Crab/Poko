/**
 * @file job.cpp
 * @brief Job implementation
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/jobs/job.h"
#include <chrono>

namespace Poko {
namespace Jobs {

Job::Job()
    : m_function(nullptr)
    , m_priority(JobPriority::Normal)
{
}

Job::Job(JobFunction function, JobPriority priority)
    : m_function(function)
    , m_priority(priority)
{
}

void Job::Execute()
{
    if (m_function) {
        m_function();
    }
}

bool Job::IsValid() const
{
    return m_function != nullptr;
}

JobPriority Job::GetPriority() const
{
    return m_priority;
}

void Job::SetPriority(JobPriority priority)
{
    m_priority = priority;
}

JobHandle::JobHandle()
    : m_id(0)
{
}

JobHandle::JobHandle(uint32_t id)
    : m_id(id)
{
}

uint32_t JobHandle::GetID() const
{
    return m_id;
}

bool JobHandle::IsValid() const
{
    return m_id != 0;
}

JobCompletion::JobCompletion()
    : m_state(JobState::Pending)
{
}

void JobCompletion::Complete()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_state = JobState::Completed;
    }
    m_condition.notify_all();
}

void JobCompletion::Fail()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_state = JobState::Failed;
    }
    m_condition.notify_all();
}

void JobCompletion::Cancel()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_state = JobState::Cancelled;
    }
    m_condition.notify_all();
}

bool JobCompletion::Wait(uint32_t timeout_ms)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    
    if (timeout_ms == 0) {
        m_condition.wait(lock, [this] {
            return m_state == JobState::Completed || 
                   m_state == JobState::Failed || 
                   m_state == JobState::Cancelled;
        });
    } else {
        auto timeout = std::chrono::milliseconds(timeout_ms);
        if (!m_condition.wait_for(lock, timeout, [this] {
            return m_state == JobState::Completed || 
                   m_state == JobState::Failed || 
                   m_state == JobState::Cancelled;
        })) {
            return false; // Timeout
        }
    }
    
    return true;
}

bool JobCompletion::IsCompleted() const
{
    return m_state == JobState::Completed;
}

bool JobCompletion::IsFailed() const
{
    return m_state == JobState::Failed;
}

bool JobCompletion::IsCancelled() const
{
    return m_state == JobState::Cancelled;
}

JobState JobCompletion::GetState() const
{
    return m_state;
}

} // namespace Jobs
} // namespace Poko