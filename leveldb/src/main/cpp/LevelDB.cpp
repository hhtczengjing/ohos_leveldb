#include "LevelDB.h"

LevelDB::LevelDB() : _db(nullptr) {}

LevelDB::~LevelDB() {
    Close();
}

bool LevelDB::Open(const std::string &path) {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path, &_db);
    return status.ok();
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
    for (const auto& key : arrKeys) {
        batch.Delete(key);
    }
    leveldb::Status status = _db->Write(_writeOptions, &batch);
    return status.ok();
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