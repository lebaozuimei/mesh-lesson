#ifndef  __GLTF_SAMPLER_H_
#define  __GLTF_SAMPLER_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

namespace mesh {
using json = nlohmann::json;

// wrapping
// [RepeatWrapping]: 10497,
// [ClampToEdgeWrapping]: 33071,
// [MirroredRepeatWrapping]: 33648

// filter
// [NearestFilter]: 9728,
// [NearestMipmapNearestFilter]: 9984,
// [NearestMipmapLinearFilter]: 9986,
// [LinearFilter]: 9729,
// [LinearMipmapNearestFilter]: 9985,
// [LinearMipmapLinearFilter]: 9987

class Sampler {

public:

    int mag_filter = 9729;
    int min_filter = 9729;
    int wrap_r = -1;
    int wrap_s = 10497;
    int wrap_t = 10497;

public:

    int read(json &doc) {
        mag_filter = json_get_int(doc, "magFilter", 9729);
        min_filter = json_get_int(doc, "minFilter", 9729);
        wrap_r = json_get_int(doc, "wrapR", -1);
        wrap_s = json_get_int(doc, "wrapS", 10497);
        wrap_t = json_get_int(doc, "wrapT", 10497);
        return 0;
    }

    int write(json &doc) {
        doc["magFilter"] = mag_filter;
        doc["minFilter"] = min_filter;
        if (wrap_r != -1) {
            doc["wrapR"] = wrap_r;
        }
        doc["wrapS"] = wrap_s;
        doc["wrapT"] = wrap_t;
        return 0;
    }
};
}

#endif