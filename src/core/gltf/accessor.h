#ifndef  __GLTF_ACCESSOR_H_
#define  __GLTF_ACCESSOR_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

namespace mesh {
using json = nlohmann::json;

/* demo here
{
  "bufferView": 0,
  "componentType": 5126,
  "count": 497,
  "max": [
    40.5,
    19.799999237060547,
    24.399999618530273
  ],
  "min": [
    -39,
    -13.899999618530273,
    -76.0999984741211
  ],
  "type": "VEC3"
}
*/

class Accessor {

public:

    int buffer_view_index = -1;
    int byte_offset = 0;
    int count = 0;

    int component_type = 0;     //
    int type = 0;               // VEC3/VEC2/SCALAR

    // ATTENTION: max or min not read from glb
    // std::vector<double> max;
    // std::vector<double> min;

public:

    int read(json &doc) {
        buffer_view_index = json_get_int(doc, "bufferView", 0);
        byte_offset = json_get_int(doc, "byteOffset", 0);
        count = json_get_int(doc, "count", 0);
        component_type = json_get_int(doc, "componentType", 0);

        std::string raw_type = json_get_string(doc, "type");
        if (raw_type == "VEC3") {
            type = gltf_const::TYPE_VEC3;
        } else if (raw_type == "VEC2") {
            type = gltf_const::TYPE_VEC2;
        } else if (raw_type == "SCALAR") {
            type = gltf_const::TYPE_SCALAR;
        } else {
            return -1;
        }

        return 0;
    }

    int write(json &doc) {
        doc["bufferView"] = buffer_view_index;
        doc["byteOffset"] = byte_offset;
        doc["count"] = count;
        doc["componentType"] = component_type;
       
        if (type == gltf_const::TYPE_VEC3) {
            doc["type"] = std::string("VEC3");
        } else if (type == gltf_const::TYPE_VEC2) {
            doc["type"] = std::string("VEC2");
        } else if (type == gltf_const::TYPE_SCALAR) {
            doc["type"] = std::string("SCALAR");
        }

        // TODO: write max, min
        return 0;
    }
};
}

#endif