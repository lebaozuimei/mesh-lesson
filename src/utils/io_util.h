#ifndef  __IO_UTIL_H_
#define  __IO_UTIL_H_

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <assert.h>
#include <nlohmann/json.hpp>
#include "../core/basetypes.h"

using json = nlohmann::json;

#define DEBUG(v)  std::cout << #v << ": " << v << std::endl; 
#define DEBUG2(a, b)  std::cout << #a << ": " << a << ", " << #b << ": " << b << std::endl; 
#define DEBUG3(a, b, c)  std::cout << #a << ": " << a << ", " << #b << ": " << b << ", " << #c << ": " << c << std::endl; 
#define DEBUG4(a, b, c, d)  std::cout << #a << ": " << a << ", " << #b << ": " << b << ", " << #c << ": " << c << ", " << #d << ": " << d << std::endl; 

#define DEBUGP(v) std::cout << std::setprecision(16) << #v << ": " << v << std::endl;
#define DEBUG2P(a, b)  std::cout << std::setprecision(16) << #a << ": " << a << ", " << #b << ": " << b << std::endl; 
#define DEBUG3P(a, b, c)  std::cout << std::setprecision(16) << #a << ": " << a << ", " << #b << ": " << b << ", " << #c << ": " << c << std::endl; 
#define DEBUG4P(a, b, c, d)  std::cout << std::setprecision(16) << #a << ": " << a << ", " << #b << ": " << b << ", " << #c << ": " << c << ", " << #d << ": " << d << std::endl; 

#define CHECK_RET(code) if (ret != 0) { return code; }

int json_get_int(json &doc, const std::string &key, int default_value = -1) {
    if (doc.find(key) != doc.end()) {
        return doc[key].get<int>();
    }
    return default_value;
}

bool json_get_bool(json &doc, const std::string &key, bool default_value = -1) {
    if (doc.find(key) != doc.end()) {
        return doc[key].get<bool>();
    }
    return default_value;
}

double json_get_double(json &doc, const std::string &key, double default_value = -1) {
    if (doc.find(key) != doc.end()) {
        return doc[key].get<double>();
    }
    return default_value;
}

std::string json_get_string(json &doc, const std::string &key, const std::string &default_value = "") {
    if (doc.find(key) != doc.end() && !doc[key].is_null()) {
        return doc[key].get<std::string>();
    }
    return default_value;
}

std::vector<std::string> json_get_string_array(json &doc, const std::string &key) {
    std::vector<std::string> result;
    if (doc.find(key) != doc.end()) {
        json &sub = doc[key];
        if (sub.is_array()) {
            for (auto iter = sub.begin(); iter != sub.end(); ++iter) {
                result.push_back(*iter);
            }
        }
    }
    return result;
}

std::vector<double> json_get_double_array(json &doc, const std::string &key) {
    std::vector<double> result;
    if (doc.find(key) != doc.end()) {
        json &sub = doc[key];
        if (sub.is_array()) {
            for (auto iter = sub.begin(); iter != sub.end(); ++iter) {
                result.push_back(*iter);
            }
        }
    }
    return result;
}

std::unordered_map<std::string, std::string> json_get_string_dict(json &doc, const std::string &key) {
    std::unordered_map<std::string, std::string> result;
    if (doc.find(key) != doc.end()) {
        json &sub = doc[key];
        if (sub.is_object()) {
            // for (auto iter = sub.begin(); iter != sub.end(); ++iter) {
            //     result[iter->key()] = iter->value();
            // }
            for (auto &item : sub.items()) {
                result[item.key()] = item.value();
            }
        }
    }
    return result;
}

bool json_has_key(json &doc, const std::string &key) {
    return doc.find(key) != doc.end();
}

// template<typename T>
// T json_get(json &doc, const std::string &key, T default_value) {
//     if (doc.find(key) != doc.end()) {
//         return doc[key].get<T>();
//     }
//     return default_value;
// }

int get_fstream_size(std::ifstream &fs) {
    fs.seekg(0, std::ios::end);
    int size = fs.tellg();
    fs.seekg(0, std::ios::beg);
    return size;
}

int read_binary(const std::string &path, Binary &data) {
    std::ifstream fs(path, std::ios::binary);
    if (!fs) {
        std::cerr << "open file failed: " << path << std::endl;
        return -1;
    }
    int size = get_fstream_size(fs);
    if (size == 0) {
        return -2;
    }
    data.resize(size);
    fs.read(&data[0], size);
    return fs.good() ? 0 : -3;
}

template<typename T>
int read_binary(const std::string &path, std::vector<T> &output) {
    std::ifstream fs(path, std::ios::binary);
    if (!fs) {
        std::cerr << "open file failed: " << path << std::endl;
        return -1;
    }

    uint32_t count;
    fs.read((char *)&count, sizeof(uint32_t));
    std::cout << "read path:" << path << ", count: " << count << std::endl;
    output.resize(count);
    fs.read((char *)&output[0], sizeof(T) * count);
    return fs.good() ? 0 : -2;
}

template<typename T>
int write_binary(const std::string &path, std::vector<T> &data) {
    std::cout << "write path:" << path << ", count: " << data.size() << std::endl;
    std::ofstream fs(path, std::ios::binary);
    uint32_t count = data.size();
    fs.write((char *)&count, sizeof(uint32_t));
    if (data.size() > 0) {
        fs.write((char *)&data[0], sizeof(T) * data.size());
    }
    return fs.good() ? 0 : -2;
}

template<typename T>
T read_value(std::ifstream &fs) {
    T value;
    fs.read((char *)&value, sizeof(T));
    return value;
}

template<typename T>
void write_value(std::ofstream &fs, T data) {
    fs.write((char *)&data, sizeof(T));
}

template<typename T>
std::string join_list(const T &container, char spliter=',') {
    std::ostringstream oss;
    size_t index = 0;
    if (container.size() > 0) {
        for (auto &v: container) {
            oss << v;
            if (index < container.size() - 1) {
                oss << spliter;
            }
            ++index;
        }
    }

    return oss.str();
}

template<typename T>
std::string join_list_with_precision(const T &container, int precision=16, char spliter=',') {
    std::ostringstream oss;
    oss.precision(precision);
    size_t index = 0;
    if (container.size() > 0) {
        for (auto &v: container) {
            oss << v;
            if (index < container.size() - 1) {
                oss << spliter;
            }
            ++index;
        }
    }
    return oss.str();
}

template<typename T>
std::string join_list_with_precision(const T &container, int count, int precision, char spliter=',') {
    std::ostringstream oss;
    oss.precision(precision);
    int index = 0;
    for (int i = 0; i < count; ++i) {
        oss << container[i];
        if (index < count - 1) {
            oss << spliter;
        }
        ++index;
    }
    return oss.str();
}

std::string replace_string(const std::string &str, const std::string &from, const std::string &to) {
    size_t pos = str.find_first_of(from, 0);
    if (pos == std::string::npos) {
        return str;
    }
    std::string result = str;
    result.replace(pos, from.size(), to);
    return result;
}

std::string trim_string(std::string str) {
    if (!str.empty()) {
        str.erase(0, str.find_first_not_of(" "));
        str.erase(str.find_last_not_of(" ") + 1);
    }
    return str;
}

std::vector<std::string> split_string(const std::string &str, const char spliter) {
    std::vector<std::string> res;
    size_t pos_beg = 0;
    size_t pos_end = str.find_first_of(spliter, 0);
    while (pos_end != std::string::npos) {
        res.push_back(str.substr(pos_beg, pos_end - pos_beg));
        pos_beg = pos_end + 1;
        if (pos_beg == str.size()) {
            res.push_back("");
            break;
        }
        pos_end = str.find_first_of(spliter, pos_beg);
    }
    if (pos_beg < str.size()) {
        res.push_back(str.substr(pos_beg));
    }
    return res;
}

std::string lower_string(const std::string &str) {
    std::string result = str;
    std::transform(str.begin(), str.end(), result.begin(), ::tolower);
    return result;
}

template<typename T>
std::string join_point(const T &point) {
    std::ostringstream oss;
    oss.precision(9);
    oss << ((double)point.x / 1e5) << "," << ((double)point.y / 1e5);
    return oss.str();
}

template<typename T>
std::string join_points(const std::vector<T> &points) {
    std::ostringstream oss;
    oss.precision(9);
    if (points.size() > 0) {
        for (size_t i = 0; i < points.size() - 1; ++i) {
            oss << (double)points[i].x / 1e5 << "," << (double)points[i].y / 1e5 << ";";
        }
        oss << (double)points[points.size() - 1].x / 1e5 << "," << (double)points[points.size() - 1].y / 1e5;
    }
    return oss.str();
}

std::string get_path_suffix(const std::string &path) {
    size_t pos = path.find_last_of(".");
    if (pos == std::string::npos) {
        return "";
    }
    return path.substr(pos + 1);
}

int read_file_to_string(const std::string &path, std::vector<std::string> &result) {
    std::ifstream fs(path);
    if (!fs) {
        std::cout << "open fs failed" << std::endl;
        return -1;
    }
    std::string raw;
    while (getline(fs, raw)) {
        raw.erase(std::remove(raw.begin(), raw.end(), '\n'), raw.end());
        raw.erase(std::remove(raw.begin(), raw.end(), '\r'), raw.end());
        raw.erase(std::remove(raw.begin(), raw.end(), '\n'), raw.end());
        result.emplace_back(trim_string(raw));
    }
    return 0;
}

#endif