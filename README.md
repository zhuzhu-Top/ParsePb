## ParsePb
无proto文件解析protobuf序列化后的内容

#### 欢迎提issue,给出一些解析不了我pb，我会尽力去修bug的
## 使用方法

## 支持解析protbuf
## 支持json转protbuf
### 特殊案例
fix32和fix64较为特殊,如下案例 字段2是protobuf的repeat fixed32类型,需要特别著名采用fix32_num
fix64则为 fix64_num,否则默认为普通的int32
```json
{
    "2":["fix32_num",1,2,3]
}

```
直接拷贝如下两个文件到自己项目(本质上就一个函数)
- ProtobufHelper.h
- ProtobufHelper.cpp
### test.cpp是写的测试 作为参考案例

## 参考项目
- PyProto https://github.com/Ccccccccvvm/PyProto

## 参考文章
- UTF-8编码的原理 https://blog.csdn.net/whahu1989/article/details/118314154
- 官网对protobuf编码解释 https://protobuf.dev/programming-guides/encoding/
- Protobuf编码解析 https://blog.csdn.net/u013688006/article/details/114009522