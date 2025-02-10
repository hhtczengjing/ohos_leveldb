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

// 读写数据
const key = 'string_value';
levelDb.setStringValue(key, 'test');
const value = levelDb.stringForKey(key);
console.log(`stringForKey: ${value} to key: ${key}`);

// 关闭数据库
levelDb.close();
```