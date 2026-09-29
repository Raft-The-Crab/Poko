# Core Jobs

## Overview

The Core Jobs module provides a thread-safe job system with a worker thread pool, job priorities, and dependency management for parallel task execution.

## Features

### Worker Thread Pool
- Configurable worker count
- Thread-safe job submission
- Graceful shutdown
- Worker thread management

### Job Priorities
- Priority levels (Low, Normal, High, Critical)
- Priority-based execution ordering
- Configurable priority system

### Job Dependencies
- Job-to-job dependencies
- Wait for individual jobs
- Wait for all jobs
- Dependency tracking

### Job Management
- Job submission
- Job cancellation
- Statistics tracking
- Clear finished jobs

### Thread Safety
- Mutex-protected operations
- Atomic statistics counters
- Safe for concurrent job submission

## API

### Job

```cpp
class Job {
public:
    using Task = std::function<void()>;
    
    Job(Task task, JobPriority priority = JobPriority::Normal);
    
    void execute();
    void cancel();
    void addDependency(JobId dependency);
    JobId getId() const;
    JobPriority getPriority() const;
    bool isFinished() const;
    bool isFailed() const;
};
```

### JobSystem

```cpp
class JobSystem {
public:
    JobSystem(size_t workerCount = std::thread::hardware_concurrency());
    ~JobSystem();
    
    JobId submit(Job::Task task, JobPriority priority = JobPriority::Normal);
    void cancel(JobId jobId);
    void waitFor(JobId jobId);
    void waitForAll();
    JobStats getStats() const;
    void clearFinishedJobs();
    void shutdown();
};
```

### Global Access

```cpp
JobSystem& getGlobalJobSystem();
```

## Usage Example

```cpp
#include "core/jobs/job.h"

using namespace poko::core::jobs;

// Get global job system
JobSystem& jobSystem = getGlobalJobSystem();

// Submit a job
JobId job1 = jobSystem.submit([]() {
    // Do work
});

// Submit high-priority job
JobId job2 = jobSystem.submit([]() {
    // Critical work
}, JobPriority::High);

// Wait for specific job
jobSystem.waitFor(job1);

// Wait for all jobs
jobSystem.waitForAll();

// Clear finished jobs
jobSystem.clearFinishedJobs();
```

## Fine-Grained Translation Units

The Core Jobs module is split into 11 separate source files for fast incremental builds:

- `interface/interface.cpp` - Global job system
- `job/job.cpp` - Job class implementation
- `constructor/constructor.cpp` - Constructor
- `destructor/destructor.cpp` - Destructor
- `submit/submit.cpp` - Job submission
- `cancellation/cancel.cpp` - Job cancellation
- `wait/wait.cpp` - Wait operations
- `statistics/statistics.cpp` - Statistics tracking
- `clear/clear.cpp` - Clear operations
- `shutdown/shutdown.cpp` - Shutdown logic
- `worker/worker.cpp` - Worker thread implementation

## Building

```bash
cd shared/engine
cmake -B build
cmake --build build
```

## Testing

```bash
cd build
./test_job.exe
```

## Notes

- All operations are thread-safe
- Worker count defaults to hardware concurrency
- Jobs with dependencies wait for dependencies to complete
- High-priority jobs are executed before lower-priority jobs
- Statistics track pending, running, completed, and failed jobs
- Graceful shutdown waits for all jobs to complete
