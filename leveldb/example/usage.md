```javascript
// 打开数据库
const path = getContext(this).getApplicationContext().filesDir + '/data.ldb';
const levelDb = new LevelDB(path);

// 读写数据
const key = 'string_value';
levelDb.setStringValue(key, 'test');
const value = levelDb.stringForKey(key);
console.log(`stringForKey: ${value} to key: ${key}`);

// 关闭数据库
levelDb.close();
```