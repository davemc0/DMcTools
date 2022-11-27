#include "Util/Counters.h"

#include <fstream>
#include <iostream>

PerThread<Counters> g_CountersMT;

void Counters::print(const std::string& fname, bool csv)
{
    std::ofstream fstrm;
    if (!fname.empty()) fstrm.open(fname);
    std::ostream& strm = fstrm.is_open() ? static_cast<std::ostream&>(fstrm) : std::cerr;

    for (auto kv : m_counterMap) { std::cerr << kv.first << "," << m_counterMap[kv.first] << '\n'; }
}
