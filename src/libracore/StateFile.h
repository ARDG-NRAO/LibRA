#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

class StateFile {
public:
    explicit StateFile(const std::string& fname = "state.txt")
        : filename_(fname) {}

    // Load file into memory
    void load() {
        data_.clear();
        std::ifstream in(filename_);
        if (!in.good()) return;  // file might not exist yet

        std::string key, value;
        while (in >> key >> value) {
            data_[key] = value;
        }
    }

    // Save all key-value pairs to disk
    void save() {
        std::ofstream out(filename_);
        for (const auto& kv : data_) {
            out << kv.first << " " << kv.second << "\n";
        }
    }

    // Generic getter/setters
    void set(const std::string& key, const std::string& val) {
        data_[key] = val;
    }

    std::string get(const std::string& key,
                    const std::string& defaultVal = "") const {
        auto it = data_.find(key);
        return it == data_.end() ? defaultVal : it->second;
    }

    // Typed helpers
    void setBool(const std::string& key, bool v) {
        set(key, v ? "true" : "false");
    }
    bool getBool(const std::string& key, bool defaultVal=false) const {
        std::string v = get(key, defaultVal ? "true" : "false");
        return (v == "true");
    }

    void setDouble(const std::string& key, double v) {
        set(key, std::to_string(v));
    }
    double getDouble(const std::string& key, double def=0.0) const {
        return std::stod(get(key, std::to_string(def)));
    }

    void setInt(const std::string& key, int v) {
        set(key, std::to_string(v));
    }
    int getInt(const std::string& key, int def=0) const {
        return std::stoi(get(key, std::to_string(def)));
    }

private:
    std::string filename_;
    std::unordered_map<std::string, std::string> data_;
};

