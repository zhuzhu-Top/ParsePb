//
// Created by 28264 on 2023/5/27.
//

#ifndef WEHELPER_PROTOBUFHELPER_H
#define WEHELPER_PROTOBUFHELPER_H
#include <iostream>
#include <vector>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/wire_format.h"
#include "google/protobuf/wire_format_lite.h"
#include <nlohmann/json.hpp>
using JSON = nlohmann::json;

using namespace std;


class ProtobufHelper {


//
public:
    ProtobufHelper();

    bool parse_pb(std::uint8_t *pb_data, std::uint32_t pb_len,JSON& json,bool is_child= false,JSON* parent_node= nullptr);

    std::uint32_t json2pb(JSON& input_json,std::uint8_t* pb_data,std::uint32_t pb_buff_size,std::uint32_t arry_field_num=0);
    void append_arry2pb(google::protobuf::io::CodedOutputStream& output,JSON& json,bool fix_size=false);
};


#endif //WEHELPER_PROTOBUFHELPER_H
