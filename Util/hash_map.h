#if ((defined(__GNUC__) || defined(__ICC)) && !defined(USE_STLPORT))
    #include <ext/hash_map>
    #include <ext/hash_set>
    namespace stdext {
        using __gnu_cxx::hash_map;
        using __gnu_cxx::hash_set;
    }
#else
    #include <hash_map>
    #include <hash_set>
    #if (!defined(_MSC_VER) || (_MSC_VER < 1300))
        namespace stdext {
            using std::hash_map;
            using std::hash_set;
        }
    #endif
#endif
