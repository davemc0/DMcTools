#pragma once

#include "Util/PerThread.h"

#include <map>

// TODO: Implement a custom map find()/[] that first compares char* and if it can't find it, does string compares

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
    void merge(const Counters& C) {}

    // Get the map for printing
    void print();

private:
    std::map<const char*, uint64_t> m_counterMap;
};

extern PerThread<Counters> g_CountersMT;

void CINC(const char* statName) { g_CountersMT.get_mine()->inc(statName); }
void CINC(const char* statName, uint64_t val) { g_CountersMT.get_mine()->inc(statName, val); }
void CSET(const char* statName, uint64_t val) { g_CountersMT.get_mine()->set(statName, val); }
