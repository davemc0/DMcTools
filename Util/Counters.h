#pragma once

#include "Util/PerThread.h"

#include <map>
#include <string>

// TODO: Implement a custom map find()/[] that first compares char* and if it can't find it, does string compares
// TODO: Histogram stats
// TODO: Reduction operators

class Counters {
public:
    void inc(const char* statName)
    {
        if (m_counterMap.find(statName) == m_counterMap.end())
            m_counterMap[statName] = 1;
        else
            m_counterMap[statName]++;
    }

    void inc(const char* statName, uint64_t val)
    {
        if (m_counterMap.find(statName) == m_counterMap.end())
            m_counterMap[statName] = 1;
        else
            m_counterMap[statName] += val;
    }

    void set(const char* statName, uint64_t val) { m_counterMap[statName] = val; }

    // Merge C into this
    void merge(const Counters& C)
    {
        for (auto kv : C.m_counterMap) {
            if (m_counterMap.find(kv.first) == m_counterMap.end())
                m_counterMap[kv.first] = kv.second;
            else
                m_counterMap[kv.first] += kv.second; // TODO: Generalize reduction operator
        }
    }

    // Get the map for printing
    void print(const std::string& fname = std::string(), bool csv = true);

private:
    std::map<const char*, uint64_t> m_counterMap;
};

extern PerThread<Counters> g_CountersMT;

// User interface
inline void CINC(const char* statName) { g_CountersMT.get_mine()->inc(statName); }
inline void CINC(const char* statName, uint64_t val) { g_CountersMT.get_mine()->inc(statName, val); }
inline void CSET(const char* statName, uint64_t val) { g_CountersMT.get_mine()->set(statName, val); }

inline void CPRINT()
{
    auto aggregated = g_CountersMT.reduce();
    aggregated->print();
}
