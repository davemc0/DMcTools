#if 0

#include <iostream>
#include <map>
#include <string>

// If you use an enum or another similar type for the settings name.
// You can define a getConfigParam that takes the setting name and default value as template arguments.

enum class setting_key { isBlue };

template <setting_key key, bool value> struct getConfigParam {
    static bool default_value;
    operator bool()
    {
        // If (read from config)
        //   return other value;
        return default_value;
    }
};

template <setting_key key, bool value> bool getConfigParam<key, value>::default_value = (s_defaults().insert({key, value}), value);

#endif

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class Config {
public:
    Config() {}

    bool LoadFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream linestream(line);
            std::string key, value;
            if (std::getline(linestream, key, '=') && std::getline(linestream, value)) { parameters_[key] = value; }
        }
        file.close();
        return true;
    }

    template <typename T> T GetValue(const std::string& key, T default_value) const
    {
        auto it = parameters_.find(key);
        if (it == parameters_.end()) return default_value;
        T value;
        std::istringstream(it->second) >> value;
        return value;
    }

private:
    std::map<std::string, std::string> parameters_;
};

/*

int main()
{
    Config config;
    if (!config.LoadFromFile("config.txt")) {
        std::cout << "Failed to load config file" << std::endl;
        return 1;
    }
    int width = config.GetValue<int>("width", 800);
    int height = config.GetValue<int>("height", 600);
    std::cout << "Window size: " << width << "x" << height << std::endl;
    return 0;
}

*/

/*
This implementation uses a std::map to store the key -
value pairs of the configuration parameters.The parameters are loaded from a file using LoadFromFile method,
which opens the file and reads each line as a key -
value pair separated by an equal sign(=).The GetValue method is a template function that takes a key and a default value,
and returns the corresponding value of the given key, or the default value if the key is not found in the configuration.
*/
