#ifndef  __GLTF_BUFFER_VIEW_H_
#define  __GLTF_BUFFER_VIEW_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

namespace mesh {
using json = nlohmann::json;

/* demo here
{
  "buffer": 0,
  "byteLength": 5964,
  "byteOffset": 0,
  "byteStride": 12,
  "target": 34962
}
*/

class BufferView {

public:

    int buffer_index = -1;
    int byte_length = 0;
    int byte_offset = 0;
    int byte_stride = 0;
    int target = 0;

public:

    int read(json &doc) {
        buffer_index = json_get_int(doc, "buffer", -1);
        byte_length = json_get_int(doc, "byteLength", 0);
        byte_offset = json_get_int(doc, "byteOffset", 0);
        byte_stride = json_get_int(doc, "byteStride", 0);
        target = json_get_int(doc, "target", 0);
        return 0;
    }

    int write(json &doc) {
        doc["buffer"] = buffer_index;
        doc["byteLength"] = byte_length;
        doc["byteOffset"] = byte_offset;
        if (byte_stride != 0) {
            doc["byteStride"] = byte_stride;
        }
        if (target != 0) {
            doc["target"] = target;
        }
        return 0;
    }
};
}

#endif