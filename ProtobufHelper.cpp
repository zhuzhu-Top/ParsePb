//
// Created by 28264 on 2023/5/27.
//

#include "ProtobufHelper.h"


#define TAG_TYPE_BITS  3  //Number of bits used to hold type info in a proto tag.
#define TAG_TYPE_MASK  (1 << TAG_TYPE_BITS) - 1  //0x7

using namespace google::protobuf;

unsigned long getULongTimeStmp(){
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    unsigned long timestamp_ull = static_cast<unsigned long>(timestamp);

    return timestamp_ull;

}
//shared_ptr<uint8_t[]> ProtobufHelper::newMsg(uint32_t& pb_len) {
//
//    tutorial::NewSendMsgRequest request;
//    request.set_cnt(1);
//
//    tutorial::ChatInfo* chatInfo = request.mutable_info();
//    chatInfo->set_client_msg_id(getULongTimeStmp());
//    chatInfo->set_content("121212");
//    chatInfo->set_utc(getULongTimeStmp());
//    chatInfo->set_type(1);
//    chatInfo->set_msg_source("");
//
//    tutorial::wxid* toId = chatInfo->mutable_toid();
//    toId->set_toid("wxid_4zr616ir6fi122");
//
//    std::string string_ret = request.SerializeAsString();
//
//    std::vector<uint8_t> bytes(string_ret.begin(), string_ret.end());
//
//    pb_len=bytes.size();
//
//    auto buff = std::make_shared<uint8_t[]>(pb_len);
//    memcpy(buff.get(), bytes.data(), pb_len);
//
//
//    return buff;
//}
string byte2string(const std::uint8_t *buf, std::size_t buf_size) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < buf_size; i++) {
        oss << std::setw(2) << static_cast<int>(buf[i]);
    }
    return oss.str();
}

bool is_utf8(const uint8_t* hex,uint32_t size) {
    uint32_t index=0;
    do {
        int tmp_index=0;
        uint8_t By_te=hex[index++];
        auto tmp_flag=By_te & 0b11110000;
        bool one_byte_flag= false;
        if(!(tmp_flag>>7)){
            tmp_index=1;
            one_byte_flag= true;
        }else{
            switch (tmp_flag) {
                case 0b11000000:{//2字节
                    tmp_index=2-1;
                    break;
                }
                case 0b11100000:{//3字节
                    tmp_index=3-1;
                    break;
                }
                case 0b11110000:{//4字节
                    tmp_index=4-1;
                    break;
                }
            }
        }
        while (tmp_index>0){
            if(one_byte_flag && By_te<=0x1f){
                return false;
            }
            if(tmp_index==1 && !one_byte_flag){
                By_te=hex[index++];
                if(By_te<=0x1f || (By_te & 0b10000000) != 0b10000000) return false;
            }else if(!one_byte_flag){
                By_te=hex[index++];
                if ( !(By_te & 0b10000000)){
                    return false;
                }
            }
            tmp_index--;
        }


    } while (index<size);
    return true;
}
std::string to_utf8_string(const uint8_t* data, size_t size) {
    std::stringstream ss;
    for (size_t i = 0; i < size; i++) {
        ss << static_cast<char>(data[i]);
    }
    return ss.str();
}
///
/// \tparam T
/// \param parent_index_str     父级的key
/// \param childJson            解析为子json的时候才设置
/// \param json                 最顶级json
/// \param is_child             是否是子节点
/// \param parent_node          父节点
/// \param no_json_value        非json类型的值
/// \param is_json              判断是否是json类型的值
template <typename T>
void setJsonData(std::string& parent_index_str,JSON* childJson, JSON& json,bool is_child,JSON* parent_node,T no_json_value= nullptr,bool is_json= false){

    if (is_child){
        if(is_json && childJson!= nullptr){ //子节点 json返回值
            if(parent_node->find(parent_index_str)!=parent_node->end()){ //如果这个key重复了，说明这个字段是repeat
                if ((*parent_node)[parent_index_str].is_array()) {
                    (*parent_node)[parent_index_str].push_back(std::move(*childJson));
                } else {
                    JSON newArray;
                    newArray.push_back(std::move((*parent_node)[parent_index_str]));
                    newArray.push_back(std::move(*childJson));
                    (*parent_node)[parent_index_str] = std::move(newArray);
                }
            }else{
                (*parent_node)[parent_index_str]=*childJson;
            }
        }else{     //子节点 非json值  //不是json值的话 就是普通的值 uint64_t uint32_t
            if (parent_node->find(parent_index_str)!=parent_node->end()){
                (*parent_node)[parent_index_str].push_back(std::move(no_json_value));
            }else{
                (*parent_node)[parent_index_str]=std::move(no_json_value);
            }
        }
    }else{ //非子节点 直接操作顶级json

        if(is_json && childJson!= nullptr) { //json返回值
            if(json.find(parent_index_str)!=json.end()){
                if (json[parent_index_str].is_array()) { //如果是arry的话直接往里存
                    json[parent_index_str].push_back(*childJson);
                } else { //不是arry就修改成arry 在放入值
                    JSON newArray;
                    newArray.push_back(std::move(json[parent_index_str]));
                    newArray.push_back(std::move(*childJson));
                    json[parent_index_str] = std::move(newArray);
                }
            }else{
                json[parent_index_str]=std::move(*childJson);
            }

        }else{ //非json返回值
            if(json.find(parent_index_str)!=json.end()){
                json[parent_index_str].push_back(std::move(no_json_value));
            }else{
                json[parent_index_str]=std::move(no_json_value);
            }
        }
    }


}

bool ProtobufHelper::parse_pb(std::uint8_t *pb_data, std::uint32_t pb_len,JSON& json,bool is_child,JSON* parent_node){
    google::protobuf::io::ArrayInputStream raw_input(pb_data,pb_len);
    google::protobuf::io::CodedInputStream input(&raw_input);
    while (true){
        auto tag=input.ReadTag();
        if (tag==0) return true;
        auto Wire_Type =internal::WireFormatLite::GetTagWireType(tag);
        int field_number = internal::WireFormatLite::GetTagFieldNumber(tag);
        if(!field_number) return false;
        switch (Wire_Type) {
            case internal::WireFormatLite::WIRETYPE_START_GROUP :{//reptead 字段会使用这个作为开始
                return false;
            }
            case internal::WireFormatLite::WIRETYPE_END_GROUP :{//End-Group类型的字段，标识一个消息组的结束。
                return false;
            }
            case internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED:{//Length-Delimited类型的字段，使用长度前缀编码表示长度可变的字符串、字节数组和嵌套消息。
                std::uint32_t lenght=0x0;
                if(!input.ReadVarint32(&lenght)){
                    return false;
                }
                if (lenght>pb_len) return false;
                std::string str;
                input.ReadString(&str,lenght);

                if (lenght>2){  //去掉可能是tag的第一个字节
                    std::string parent_index_str=std::to_string(field_number);
                    const std::uint8_t * check_ptr= (std::uint8_t *)str.c_str();
//                    const char *message= nullptr;
                    int faulty_bytes = 0;
                    check_ptr=check_ptr+1;
//                    std::uint32_t is_utf8_ret=is_utf8(check_ptr,lenght-1,&message,&faulty_bytes);
                    bool is_utf8_ret=is_utf8(check_ptr,lenght-1);
                    if (is_utf8_ret){  //检测到就是字符串直接当字符串处理

                        str= to_utf8_string((uint8*)str.c_str(),str.size());
                        setJsonData(parent_index_str, nullptr,json,is_child,parent_node, str, true);
                        break;
                    }
                    std::uint32_t first_byte=str[0];
                    std::uint32_t child_wire=first_byte & 0b111;
                    std::uint32_t child_field_num=first_byte >>3;

//                    if (first_Byte==0x8 || str[0]==0xA || str[0]==0xD || str[0]==0x12){ //长度大于2才可能是嵌套类型 tag需要一个字节 后面一个字节的值
                    if (child_wire <= internal::WireFormatLite::WIRETYPE_FIXED32
                        && child_wire!=internal::WireFormatLite::WIRETYPE_START_GROUP
                        && child_wire!=internal::WireFormatLite::WIRETYPE_END_GROUP
                        && child_field_num < 0b1111  //field_num 0b1111 0xf 认为是最大的pb的起始字段号
                        ){ //长度大于2才可能是嵌套类型 tag需要一个字节 后面一个字节的值

                        JSON childJson={};
                        bool parse_sub_succ= parse_pb((uint8_t *) str.c_str(), str.size(),json,true,&childJson);
                        if (parse_sub_succ){
                            setJsonData(parent_index_str, &childJson,json,is_child,parent_node, 0, true);
                            break;
                        }
                        string strsss=json.dump();
                        //继续执行到下面的unhandle_content
                    }
                    goto unhandle_content;
                }else{  //长度小于2 还不是string和嵌套类型，无法解析 直接把内容返回
                    //repeat int32 和 bytes无法区分
                unhandle_content:
                    std::string string_hex =byte2string((std::uint8_t *)str.c_str(),str.size());
                    if(is_child){ //如果是嵌套类型的数据 放到父级下面
                        (*parent_node)[std::to_string(field_number)]=string_hex;
                    }else{
                        json[std::to_string(field_number)]=string_hex;
                    }
                    break;
                }
                break;
            }
            case internal::WireFormatLite::WIRETYPE_FIXED64:{
                std::uint64_t value=0x0;
//                input.ReadVarint64(&value);
                input.ReadLittleEndian64(&value);
                std::string parent_index_str=std::to_string(field_number);
                setJsonData(parent_index_str, nullptr,json,is_child,parent_node,value,true);
                break;
            }
            case internal::WireFormatLite::WIRETYPE_VARINT:{
                std::uint64_t value=0x0;
                input.ReadVarint64(&value);
                std::string parent_index_str=std::to_string(field_number);
                setJsonData(parent_index_str, nullptr,json,is_child,parent_node,value,true);
                break;
            }
            case internal::WireFormatLite::WIRETYPE_FIXED32:{
                std::uint32_t value=0x0;
                input.ReadLittleEndian32(&value);
                std::string parent_index_str=std::to_string(field_number);
                setJsonData(parent_index_str, nullptr,json,is_child,parent_node,value,true);
                break;
            }
            default:{
                return false;
            }
        }
    }
}
void ProtobufHelper::parse2json(JSON& json) {

}
ProtobufHelper::ProtobufHelper() {
    JSON json;
    parse2json(json);

}


