#ifndef  __GLTF_TEXTURE_H_
#define  __GLTF_TEXTURE_H_

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

class Texture {

public:

    int sampler_index = -1;     // index in samplers
    int source_index = -1;      // index in images

public:

    int read(json &doc) {
        sampler_index = json_get_int(doc, "sampler", 0);
        source_index = json_get_int(doc, "source", 0);
        return 0;
    }

    int write(json &doc) {
        doc["sampler"] = sampler_index;
        doc["source"] = source_index;
        return 0;
    }
};
}

#endif