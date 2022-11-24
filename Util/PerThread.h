// PerThread.h - Manages per-thread data structures, such as counters, to provide performant, lock-free functionality
// Many cases will require reducing across the per-thread structures at the end.

#pragma once

#include <map>
#include <thread>

template <class Tracked_T> class PerThread {
public:
    std::shared_ptr<Tracked_T> get_mine()
    {
        std::thread::id tid = std::this_thread::get_id();
        if (m_trackedMap.find(tid) == m_trackedMap.end()) m_trackedMap[tid] = std::shared_ptr<Tracked_T>(new Tracked_T);

        return m_trackedMap[tid];
    }

    // Create a new Tracked_T that is a reduction of all the ones in the map
    // TODO: Instead take a lambda
    std::shared_ptr<Tracked_T> reduce()
    {
        std::shared_ptr<Tracked_T> merged(new Tracked_T);
        for (const auto& tPtr : m_trackedMap) merged->merge(tPtr);
    }

private:
    std::map<std::thread::id, std::shared_ptr<Tracked_T>> m_trackedMap;
};
