//
// Created on 2025/2/10.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "LevelDB.h"
#include <string>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"

LevelDB::LevelDB() : _db(nullptr) {}

LevelDB::~LevelDB() {
    Close();
}

bool LevelDB::Open(const std::string &path) {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path, &_db);
    if (!status.ok()) {
        return false;
    }
    _writeOptions.sync = false;
    return true;
}

void LevelDB::Close() {
    if (_db) {
        delete _db;
        _db = nullptr;
    }
}

bool LevelDB::Remove(const std::string &key) {
    leveldb::Status status = _db->Delete(_writeOptions, key);
    return status.ok();
}

bool LevelDB::Remove(const std::vector<std::string> &arrKeys) {
    leveldb::WriteBatch batch;
    for (const auto &key : arrKeys) {
        batch.Delete(key);
    }
    leveldb::Status status = _db->Write(_writeOptions, &batch);
    return status.ok();
}

bool LevelDB::Put(const std::string& key, const ValueType& value) {
    leveldb::Slice key_slice(key);
    leveldb::Slice value_slice = ToSlice(value);
    leveldb::Status status = _db->Put(_writeOptions, key_slice, value_slice);
    return status.ok();
}

bool LevelDB::Get(const std::string& key, ValueType& value) {
    std::string value_str;
    leveldb::Status status = _db->Get(_readOptions, key, &value_str);
    if (status.ok()) {
        return FromSlice(leveldb::Slice(value_str), value);
    }
    return false;
}

std::vector<std::string> LevelDB::GetAllKeys() {
    std::vector<std::string> keys;
    leveldb::Iterator* it = _db->NewIterator(_readOptions);
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        keys.push_back(it->key().ToString());
    }
    delete it;
    return keys;
}

leveldb::Slice LevelDB::ToSlice(const ValueType& value) {
    switch (value.getTag()) {
        case ValueType::Tag::Double:
            return leveldb::Slice(std::to_string(value.getDouble()));
        case ValueType::Tag::Float:
            return leveldb::Slice(std::to_string(value.getFloat()));
        case ValueType::Tag::Int32:
            return leveldb::Slice(std::to_string(value.getInt32()));
        case ValueType::Tag::Int64:
            return leveldb::Slice(std::to_string(value.getInt64()));
        case ValueType::Tag::Bool:
            return leveldb::Slice(value.getBool() ? "1" : "0");
        case ValueType::Tag::String:
            return leveldb::Slice(value.getString());
        default:
            return leveldb::Slice("");
    }
}

bool LevelDB::FromSlice(const leveldb::Slice& slice, ValueType& value) {
    std::string value_str = slice.ToString();
    try {
        switch (value.getTag()) {
            case ValueType::Tag::Double:
                value = ValueType(std::stod(value_str));
                break;
            case ValueType::Tag::Float:
                value = ValueType(std::stof(value_str));
                break;
            case ValueType::Tag::Int32:
                value = ValueType(std::stoi(value_str));
                break;
            case ValueType::Tag::Int64:
                value = ValueType(int64_t(std::stoll(value_str)));
                break;
            case ValueType::Tag::Bool:
                value = ValueType(value_str == "1");
                break;
            case ValueType::Tag::String:
                value = ValueType(value_str);
                break;
            default:
                return false;
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}