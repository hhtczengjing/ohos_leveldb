//
// Created on 2025/2/10.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OHOS_LEVELDB_LEVELDB_H
#define OHOS_LEVELDB_LEVELDB_H

#include "leveldb/db.h"
#include <string>

class ValueType {
public:
    enum class Tag {
        Double,
        Float,
        Int32,
        Int64,
        Bool,
        String,
        None
    };

    ValueType() : tag(Tag::None) {}
    ValueType(double value) : tag(Tag::Double), doubleValue(value) {}
    ValueType(float value) : tag(Tag::Float), floatValue(value) {}
    ValueType(int32_t value) : tag(Tag::Int32), int32Value(value) {}
    ValueType(int64_t value) : tag(Tag::Int64), int64Value(value) {}
    ValueType(bool value) : tag(Tag::Bool), boolValue(value) {}
    ValueType(const std::string& value) : tag(Tag::String), stringValue(value) {}

    Tag getTag() const { return tag; }

    double getDouble() const { return doubleValue; }
    float getFloat() const { return floatValue; }
    int32_t getInt32() const { return int32Value; }
    int64_t getInt64() const { return int64Value; }
    bool getBool() const { return boolValue; }
    const std::string& getString() const { return stringValue; }

private:
    Tag tag;
    union {
        double doubleValue;
        float floatValue;
        int32_t int32Value;
        int64_t int64Value;
        bool boolValue;
    };
    std::string stringValue;
};

class LevelDB {
public:
    LevelDB();
    
    ~LevelDB();
    
    bool Open(const std::string &path);
    void Close();
    
    bool Remove(const std::string &key);
    bool Remove(const std::vector<std::string> &arrKeys);
    
    bool Put(const std::string& key, const ValueType& value);
    bool Get(const std::string& key, ValueType& value);
    
    std::vector<std::string> GetAllKeys();
    
private:
    leveldb::DB *_db;
    leveldb::ReadOptions _readOptions;
    leveldb::WriteOptions _writeOptions;
    
    leveldb::Slice ToSlice(const ValueType& value);
    bool FromSlice(const leveldb::Slice& slice, ValueType& value);
};

#endif //OHOS_LEVELDB_LEVELDB_H
