import levelDb from 'libleveldb.so';
import fs from '@ohos.file.fs';

export class LevelDB {
  private dbPtr: number;
  private path: string = '';

  constructor(path: string) {
    // 判断文件夹是否存在，不存在直接创建文件夹
    if (!fs.accessSync(path)) {
      fs.mkdir(path, true);
    }
    this.path = path;
    this.dbPtr = levelDb.open(path);
  }

  close() {
    levelDb.close(this.dbPtr);
  }

  delete() {
    this.close();
    if (this.path) {
      fs.rmdirSync(this.path);
    }
  }

  allKeys(): string[] {
    return levelDb.allKeys(this.dbPtr);
  }

  removeValueForKey(key: string) {
    levelDb.removeValueForKey(this.dbPtr, key);
  }

  removeValuesForKeys(keys: string[]) {
    levelDb.removeValuesForKeys(this.dbPtr, keys);
  }

  stringForKey(key: string): string {
    return levelDb.stringForKey(this.dbPtr, key);
  }

  boolForKey(key: string): boolean {
    return levelDb.boolForKey(this.dbPtr, key);
  }

  int32ForKey(key: string): number {
    return levelDb.int32ForKey(this.dbPtr, key);
  }

  uint32ForKey(key: string): number {
    return levelDb.uint32ForKey(this.dbPtr, key);
  }

  int64ForKey(key: string): bigint {
    return levelDb.int64ForKey(this.dbPtr, key);
  }

  uint64ForKey(key: string): bigint {
    return levelDb.uint64ForKey(this.dbPtr, key);
  }

  floatForKey(key: string): number {
    return levelDb.floatForKey(this.dbPtr, key);
  }

  doubleForKey(key: string): number {
    return levelDb.doubleForKey(this.dbPtr, key);
  }

  setStringValue(key: string, value: string) {
    levelDb.setStringValue(this.dbPtr, key, value);
  }

  setBoolValue(key: string, value: boolean) {
    levelDb.setBoolValue(this.dbPtr, key, value);
  }

  setInt32Value(key: string, value: number) {
    levelDb.setInt32Value(this.dbPtr, key, value);
  }

  setUInt32Value(key: string, value: number) {
    levelDb.setUInt32Value(this.dbPtr, key, value);
  }

  setInt64Value(key: string, value: bigint) {
    levelDb.setInt64Value(this.dbPtr, key, value);
  }

  setUInt64Value(key: string, value: bigint) {
    levelDb.setUInt64Value(this.dbPtr, key, value);
  }

  setFloatValue(key: string, value: number) {
    levelDb.setFloatValue(this.dbPtr, key, value);
  }

  setDoubleValue(key: string, value: number) {
    levelDb.setDoubleValue(this.dbPtr, key, value);
  }
}