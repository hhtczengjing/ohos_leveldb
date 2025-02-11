#include "napi/native_api.h"
#include "LevelDB.h"
#include <cstdint>

// assuming env is defined
#define NAPI_CALL_RET(call, return_value)                                                                              \
    do {                                                                                                               \
        napi_status status = (call);                                                                                   \
        if (status != napi_ok) {                                                                                       \
            const napi_extended_error_info *error_info = nullptr;                                                      \
            napi_get_last_error_info(env, &error_info);                                                                \
            bool is_pending;                                                                                           \
            napi_is_exception_pending(env, &is_pending);                                                               \
            if (!is_pending) {                                                                                         \
                auto message = error_info->error_message ? error_info->error_message : "null";                         \
                napi_throw_error(env, nullptr, message);                                                               \
                return return_value;                                                                                   \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define NAPI_CALL(call) NAPI_CALL_RET(call, nullptr)

bool IsNValueUndefined(napi_env env, napi_value value) {
    napi_valuetype type;
    if (napi_typeof(env, value, &type) == napi_ok && type == napi_undefined) {
        return true;
    }
    return false;
}

static std::string NValueToString(napi_env env, napi_value value, bool maybeUndefined = false) {
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return "";
    }

    size_t size;
    NAPI_CALL_RET(napi_get_value_string_utf8(env, value, nullptr, 0, &size), "");
    std::string result(size, '\0');
    NAPI_CALL_RET(napi_get_value_string_utf8(env, value, (char *) result.data(), size + 1, nullptr), "");
    return result;
}

static napi_value StringToNValue(napi_env env, const std::string &value) {
    napi_value result;
    napi_create_string_utf8(env, value.data(), value.size(), &result);
    return result;
}

static napi_value NAPIUndefined(napi_env env) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value NAPINull(napi_env env) {
    napi_value result;
    napi_get_null(env, &result);
    return result;
}

static napi_value BoolToNValue(napi_env env, bool value) {
    napi_value result;
    napi_value resultBool;
    napi_create_double(env, value, &result);
    napi_coerce_to_bool(env, result, &resultBool);
    return resultBool;
}

static bool NValueToBool(napi_env env, napi_value value) {
    bool result;
    if (napi_get_value_bool(env, value, &result) == napi_ok) {
        return result;
    }
    return false;
}

static napi_value Int32ToNValue(napi_env env, int32_t value) {
    napi_value result;
    napi_create_int32(env, value, &result);
    return result;
}

static int32_t NValueToInt32(napi_env env, napi_value value) {
    int32_t result;
    napi_get_value_int32(env, value, &result);
    return result;
}

static napi_value UInt32ToNValue(napi_env env, uint32_t value) {
    napi_value result;
    napi_create_uint32(env, value, &result);
    return result;
}

static uint32_t NValueToUInt32(napi_env env, napi_value value) {
    uint32_t result;
    napi_get_value_uint32(env, value, &result);
    return result;
}

static napi_value DoubleToNValue(napi_env env, double value) {
    napi_value result;
    napi_create_double(env, value, &result);
    return result;
}

static double NValueToDouble(napi_env env, napi_value value) {
    double result;
    napi_get_value_double(env, value, &result);
    return result;
}

static napi_value Int64ToNValue(napi_env env, int64_t value) {
    napi_value result;
    napi_create_bigint_int64(env, value, &result);
    return result;
}

static int64_t NValueToInt64(napi_env env, napi_value value) {
    int64_t result;
    bool lossless;
    napi_get_value_bigint_int64(env, value, &result, &lossless);
    return result;
}

static napi_value UInt64ToNValue(napi_env env, uint64_t value) {
    napi_value result;
    napi_create_bigint_uint64(env, value, &result);
    return result;
}

static uint64_t NValueToUInt64(napi_env env, napi_value value, bool maybeUndefined = false) {
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return 0;
    }
    uint64_t result;
    bool lossless;
    napi_get_value_bigint_uint64(env, value, &result, &lossless);
    return result;
}

static std::vector<std::string> NValueToStringArray(napi_env env, napi_value value, bool maybeUndefined = false) {
    std::vector<std::string> keys;
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return keys;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, value, &length) != napi_ok || length == 0) {
        return keys;
    }
    keys.reserve(length);

    for (uint32_t index = 0; index < length; index++) {
        napi_value jsKey = nullptr;
        if (napi_get_element(env, value, index, &jsKey) != napi_ok) {
            continue;
        }
        keys.push_back(NValueToString(env, jsKey));
    }
    return keys;
}

static napi_value StringArrayToNValue(napi_env env, const std::vector<std::string> &value) {
    napi_value jsArr = nullptr;
    napi_create_array_with_length(env, value.size(), &jsArr);
    for (size_t index = 0; index < value.size(); index++) {
        auto jsKey = StringToNValue(env, value[index]);
        napi_set_element(env, jsArr, index, jsKey);
    }
    return jsArr;
}

// export const open: (path: string) => number;
static napi_value open(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    std::string path = NValueToString(env, args[0]);
    
    LevelDB *_db = new LevelDB();
    bool flag = _db->Open(path);
    if (!flag) {
        return NAPIUndefined(env);
    }
    return UInt64ToNValue(env, (uint64_t) _db);
}

// export const close: (ptr: number) => void;
static napi_value close(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    _db->Close();
    delete _db;
    return NAPIUndefined(env);
}

// export const allKeys: (ptr: number) => string[];
static napi_value allKeys(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::vector<std::string> keys = _db->GetAllKeys();
    return StringArrayToNValue(env, keys);
}

// export const removeValueForKey: (ptr: number, key: string) => void;
static napi_value removeValueForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    _db->Remove(key);
    return NAPIUndefined(env);
}

// export const removeValuesForKeys: (ptr: number, keys: string[]) => void;
static napi_value removeValuesForKeys(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::vector<std::string> keys = NValueToStringArray(env, args[1]);
    _db->Remove(keys);
    return NAPIUndefined(env);
}

// export const stringForKey: (ptr: number, key: string) => string;
static napi_value stringForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    std::string value;
    if (!_db->Get(key, value)) {
        return NAPIUndefined(env);
    }
    return StringToNValue(env, value);
}

// export const boolForKey: (ptr: number, key: string) => boolean;
static napi_value boolForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    bool value;
    if (!_db->Get(key, value)) {
        return NAPIUndefined(env);
    }
    return BoolToNValue(env, value);
}

// export const int32ForKey: (ptr: number, key: string) => number;
static napi_value int32ForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    int32_t value;
    if (!_db->Get(key, value)) {
        return NAPIUndefined(env);
    }
    return Int32ToNValue(env, value);
}

// export const uint32ForKey: (ptr: number, key: string) => number;
static napi_value uint32ForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    uint32_t value;
    if (!_db->Get(key, value)) {
        return NAPIUndefined(env);
    }
    return UInt32ToNValue(env, value);
}

// export const int64ForKey: (ptr: number, key: string) => number;
static napi_value int64ForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    int64_t value;
    if (!_db->Get(key, value)) {
        return NAPIUndefined(env);
    }
    return Int64ToNValue(env,value);
}

// export const uint64ForKey: (ptr: number, key: string) => number;
static napi_value uint64ForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    uint64_t value;
    if (!_db->Get(key, value)) {
        return NAPIUndefined(env);
    }
    return UInt64ToNValue(env,value);
}

// export const floatForKey: (ptr: number, key: string) => number;
static napi_value floatForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    float value;
    if (!_db->Get(key, value)) {
        return NAPIUndefined(env);
    }
    return DoubleToNValue(env, value);
}

// export const doubleForKey: (ptr: number, key: string) => number;
static napi_value doubleForKey(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    double value;
    if (!_db->Get(key, value)) {
        return NAPIUndefined(env);
    }
    return DoubleToNValue(env, value);
}

// export const setStringValue: (ptr: number, key: string, value: string) => void;
static napi_value setStringValue(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    std::string value = NValueToString(env, args[2]);
    _db->Put(key,value);
    return NAPIUndefined(env);
}

// export const setBoolValue: (ptr: number, key: string, value: boolean) => void;
static napi_value setBoolValue(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    bool value = NValueToBool(env, args[2]);
    _db->Put(key, value);
    return NAPIUndefined(env);
}

// export const setInt32Value: (ptr: number, key: string, value: number) => void;
static napi_value setInt32Value(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    int32_t value = NValueToInt32(env, args[2]);
    _db->Put(key, static_cast<int32_t>(value));
    return NAPIUndefined(env);
}

// export const setUInt32Value: (ptr: number, key: string, value: number) => void;
static napi_value setUInt32Value(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    uint32_t value = NValueToUInt32(env, args[2]);
    _db->Put(key, static_cast<uint32_t>(value));
    return NAPIUndefined(env);
}

// export const setInt64Value: (ptr: number, key: string, value: number) => void;
static napi_value setInt64Value(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    int64_t value = NValueToInt64(env, args[2]);
    _db->Put(key, static_cast<int64_t>(value));
    return NAPIUndefined(env);
}

// export const setUInt64Value: (ptr: number, key: string, value: number) => void;
static napi_value setUInt64Value(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    uint64_t value = NValueToUInt64(env, args[2]);
    _db->Put(key, static_cast<uint64_t>(value));
    return NAPIUndefined(env);
}

// export const setFloatValue: (ptr: number, key: string, value: number) => void;
static napi_value setFloatValue(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    float value = NValueToDouble(env, args[2]);
    _db->Put(key, static_cast<float>(value));
    return NAPIUndefined(env);
}

// export const setDoubleValue: (ptr: number, key: string, value: number) => void;
static napi_value setDoubleValue(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    
    int64_t _db_instance_ptr = NValueToInt64(env, args[0]);
    LevelDB* _db = reinterpret_cast<LevelDB*>(_db_instance_ptr);
    if (!_db) {
        return NAPIUndefined(env);
    }
    
    std::string key = NValueToString(env, args[1]);
    double value = NValueToDouble(env, args[2]);
    _db->Put(key, value);
    return NAPIUndefined(env);
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        { "open", nullptr, open, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "close", nullptr, close, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "allKeys", nullptr, allKeys, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "removeValueForKey", nullptr, removeValueForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "removeValuesForKeys", nullptr, removeValuesForKeys, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "stringForKey", nullptr, stringForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "boolForKey", nullptr, boolForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "int32ForKey", nullptr, int32ForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "int64ForKey", nullptr, int64ForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "uint32ForKey", nullptr, uint32ForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "uint64ForKey", nullptr, uint64ForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "floatForKey", nullptr, floatForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "doubleForKey", nullptr, doubleForKey, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setStringValue", nullptr, setStringValue, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setBoolValue", nullptr, setBoolValue, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setInt32Value", nullptr, setInt32Value, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setInt64Value", nullptr, setInt64Value, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setUInt32Value", nullptr, setUInt32Value, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setUInt64Value", nullptr, setUInt64Value, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setFloatValue", nullptr, setFloatValue, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setDoubleValue", nullptr, setDoubleValue, nullptr, nullptr, nullptr, napi_default, nullptr },
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "leveldb",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterLeveldbModule(void) {
    napi_module_register(&demoModule);
}
