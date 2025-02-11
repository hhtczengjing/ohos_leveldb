//
// Created on 2025/2/10.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include <string>
#include <vector>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <sstream>
#include <stdint.h>

class LevelDB {
public:
    LevelDB();
    ~LevelDB();
    
    bool Open(const std::string &path);
    void Close();
    
    bool Remove(const std::string &key);
    bool Remove(const std::vector<std::string> &arrKeys);
    
    template<typename T>
    bool Put(const std::string& key, const T& value);

    template<typename T>
    bool Get(const std::string& key, T& value);
    
    std::vector<std::string> GetAllKeys();
private:
    leveldb::DB *_db;
    leveldb::ReadOptions _readOptions;
    leveldb::WriteOptions _writeOptions;
    
    template<typename T>
    std::string Serialize(const T& value);

    template<typename T>
    T Deserialize(const std::string& serialized_value);
};

template<typename T>
bool LevelDB::Put(const std::string& key, const T& value) {
    std::string serialized_value = Serialize(value);
    leveldb::Status status = _db->Put(_writeOptions, key, serialized_value);
    return status.ok();
}

template<typename T>
bool LevelDB::Get(const std::string& key, T& value) {
    std::string serialized_value;
    leveldb::Status status = _db->Get(_readOptions, key, &serialized_value);
    if (!status.ok()) {
        return false;
    }
    value = Deserialize<T>(serialized_value);
    return true;
}

template<typename T>
std::string LevelDB::Serialize(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<typename T>
T LevelDB::Deserialize(const std::string& serialized_value) {
    std::istringstream iss(serialized_value);
    T value;
    iss >> value;
    return value;
}