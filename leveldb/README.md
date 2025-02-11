# ohos_leveldb

Google LevelDB for HarmonyOS.

## 安装

```shell
ohpm i @devzeng/leveldb
```

OpenHarmony ohpm 环境配置等更多内容，请参考[如何安装 OpenHarmony ohpm 包](https://ohpm.openharmony.cn/#/cn/help/downloadandinstall)

## 使用

```javascript
// 打开数据库
const path = getContext(this).getApplicationContext().filesDir + '/data.ldb';
const levelDb = new LevelDB(path);

// 读写数据(支持string、number、boolean等基础数据类型)
levelDb.setStringValue('string_value', 'test');
const value = levelDb.stringForKey('string_value');

// 删除数据
levelDb.removeValueForKey('key1');
levelDb.removeValuesForKeys(['key1', 'key2']);

// 遍历数据
const allKeys = levelDb.allKeys();

// 关闭数据库
levelDb.close();
```