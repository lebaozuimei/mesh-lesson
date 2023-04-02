#ifndef  __GLTF_SCENE_H_
#define  __GLTF_SCENE_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

namespace mesh {
using json = nlohmann::json;

/* demo here
{
  "scene": 0
}

[
  {
    "name": "AuxScene",
    "nodes": [
      0
    ]
  }
]
*/

class Extension {

public:

    std::vector<std::string> items;

public:

    int read(json &doc) {
        name = json_get_string(doc, "name");
        for (int index : doc["nodes"]) {
            indices.push_back(index);
        }
        return 0;
    }

    int write(json &doc) {
        if (name.size() > 0) {
            doc["name"] = name;
        }
        if (indices.size() > 0) {
            doc["nodes"] = indices;
        }
        return 0;
    }
};
}

#endif