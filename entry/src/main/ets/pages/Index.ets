import { LevelDB } from '@devzeng/leveldb';

@Extend(Text)
function headerStyle() {
  .textAlign(TextAlign.Start)
  .width('100%')
  .padding({ left: 16, right: 16 })
  .fontSize(16)
  .fontColor('#7E8388')
}

@Extend(Text)
function outputStyle() {
  .textAlign(TextAlign.Start)
  .width('100%')
  .margin({ top: 10 })
  .padding({ left: 16, right: 16 })
  .fontSize(16)
  .backgroundColor('#F4F6FA')
  .fontColor('#020F22')
}

@Extend(List)
function separatorStyle() {
  .divider({
    strokeWidth: 0.5,
    startMargin: 80,
    endMargin: 0,
    color: '#E3E6ED'
  })
}

@Entry
@Component
struct Index {
  @State output: string = '';
  private levelDb?: LevelDB = undefined;
  private items: string[] = [
    'setStringValue / stringForKey',
    'setBoolValue / boolForKey',
    'setInt32Value / int32ForKey',
    'setUInt32Value / uint32ForKey',
    'setInt64Value / int64ForKey',
    'setUInt64Value / uint64ForKey',
    'setFloatValue / floatForKey',
    'setDoubleValue / doubleForKey',
    'allKeys',
    'removeValueForKey',
    'removeValuesForKeys'
  ];

  aboutToAppear(): void {
    const path = getContext(this).getApplicationContext().filesDir + '/tracker.ldb';
    this.levelDb = new LevelDB(path);
  }

  aboutToDisappear(): void {
    this.levelDb?.close();
  }

  build() {
    Column() {
      Column() {
        Text('Operation Log: ')
          .headerStyle()

        Text(this.output)
          .height('15%')
          .margin({ top: 10 })
          .outputStyle()
      }
      .margin({ top: 10 })
      .width('100%')

      Column() {
        Text('Operation Actions: ')
          .headerStyle()

        List() {
          ForEach(this.items, (item: string, index: number) => {
            ListItem() {
              Row() {
                Text(item)
                  .textAlign(TextAlign.Start)

                Blank()

                Image($r('sys.media.ohos_ic_public_arrow_right'))
                  .width(18)
                  .height(18)
              }
              .width('100%')
              .height('100%')
              .justifyContent(FlexAlign.SpaceBetween)
              .alignItems(VerticalAlign.Center)
            }
            .width('100%')
            .height(50)
            .onClick(() => {
              this.actionHandler(index);
            })
          })
        }
        .separatorStyle()
        .padding({ left: 10, right: 10 })
        .width('100%')
      }
      .margin({ top: 10 })
    }
    .height('100%')
    .width('100%')
  }

  private actionHandler(index: number): void {
    if (index === 0) {
      const key = 'string_value';
      this.levelDb?.setStringValue('test', 'hello world');
      this.levelDb?.setStringValue(key, 'test');
      const value = this.levelDb?.stringForKey(key);
      this.output = `stringForKey: ${value} to key: ${key}`;
    } else if (index === 1) {
      const key = 'bool_value';
      this.levelDb?.setBoolValue(key, true);
      const value = this.levelDb?.boolForKey(key);
      this.output = `boolForKey: ${value} to key: ${key}`;
    } else if (index === 2) {
      const key = 'int32_value';
      this.levelDb?.setInt32Value(key, -1);
      const value = this.levelDb?.int32ForKey(key);
      this.output = `int32ForKey: ${value} to key: ${key}`;
    } else if (index === 3) {
      const key = 'uint32_value';
      this.levelDb?.setUInt32Value(key, 2);
      const value = this.levelDb?.uint32ForKey(key);
      this.output = `uint32ForKey: ${value} to key: ${key}`;
    } else if (index === 4) {
      const key = 'int64_value';
      this.levelDb?.setInt64Value(key, BigInt(-Date.now()));
      const value = this.levelDb?.int64ForKey(key);
      this.output = `int64ForKey: ${value} to key: ${key}`;
    } else if (index === 5) {
      const key = 'uint64_value';
      this.levelDb?.setUInt64Value(key, BigInt(Date.now()));
      const value = this.levelDb?.uint64ForKey(key);
      this.output = `uint64ForKey: ${value} to key: ${key}`;
    } else if (index === 6) {
      const key = 'float_value';
      this.levelDb?.setFloatValue(key, 1.01);
      const value = this.levelDb?.floatForKey(key);
      this.output = `floatForKey: ${value?.toFixed(2)} to key: ${key}`;
    } else if (index === 7) {
      const key = 'double_value';
      this.levelDb?.setDoubleValue(key, 3.14159);
      const value = this.levelDb?.doubleForKey(key);
      this.output = `doubleForKey: ${value} to key: ${key}`;
    } else if (index === 8) {
      const keys = this.levelDb?.allKeys();
      this.output = `allKeys: ${JSON.stringify(keys)}`;
    } else if (index === 9) {
      const key = 'test';
      this.levelDb?.removeValueForKey('test');
      this.output = `removeValueForKey: ${key}`;
    } else if (index === 10) {
      const keys = ['string_value', 'bool_value', 'int32_value', 'int64_value', 'float_value', 'double_value'];
      this.levelDb?.removeValuesForKeys(keys);
      this.output = `removeValuesForKeys: ${JSON.stringify(keys)}`;
    }
  }
}