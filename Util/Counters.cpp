#include "Util/Counters.h"

#include <iostream>

PerThread<Counters> g_CountersMT;

void Counters::print()
{
    for (auto kv : m_counterMap) { std::cerr << kv.first << "," << m_counterMap[kv.first] << '\n'; }
}
