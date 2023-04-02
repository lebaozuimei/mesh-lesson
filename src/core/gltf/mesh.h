#ifndef  __GLTF_MESH_H_
#define  __GLTF_MESH_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"
#include "primitive.h"

namespace mesh {
using json = nlohmann::json;

/* demo here
{
  "extras": {
    "targetNames": [
      "parrot_A_001",
      "parrot_A_002",
      "parrot_A_003",
      "parrot_A_004",
      "parrot_A_005",
      "parrot_A_006",
      "parrot_A_007",
      "parrot_A_008",
      "parrot_A_009",
      "parrot_A_010",
      "parrot_A_011",
      "parrot_A_012"
    ]
  },
  "primitives": [
    {
      "attributes": {
        "COLOR_0": 1,
        "POSITION": 0,
        "TEXCOORD_0": 2
      },
      "indices": 15,
      "material": 0,
      "mode": 4,
      "targets": [
        {
          "POSITION": 3
        },
        {
          "POSITION": 4
        },
        {
          "POSITION": 5
        },
        {
          "POSITION": 6
        },
        {
          "POSITION": 7
        },
        {
          "POSITION": 8
        },
        {
          "POSITION": 9
        },
        {
          "POSITION": 10
        },
        {
          "POSITION": 11
        },
        {
          "POSITION": 12
        },
        {
          "POSITION": 13
        },
        {
          "POSITION": 14
        }
      ]
    }
  ],
  "weights": [
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  ]
}
*/

class Mesh {

public:
    std::vector<Primitive> primitives;

    int node_index = -1;

public:

    int read(json &doc, 
        const std::vector<Binary> &buffer_list, 
        const std::vector<BufferView> &buffer_views,
        const std::vector<Accessor> &accessors) {
        json &primitives_doc = doc["primitives"];
        if (primitives_doc.is_array()) {
            for (size_t i = 0; i < primitives_doc.size(); ++i) {
                Primitive primitive;
                primitive.read(primitives_doc[i], buffer_list, buffer_views, accessors);
                primitives.push_back(primitive);
            }
        }
        return 0;
    }

    int write(json &root, json &doc, Binary &output,
        std::vector<Binary> &buffer_list, 
        std::vector<BufferView> &buffer_views,
        std::vector<Accessor> &accessors) {
        json primitives_doc;
        for (size_t i = 0; i < primitives.size(); ++i) {
            json primitive_doc;
            Primitive &primitive = primitives[i];
            primitive.write(root, primitive_doc, output, buffer_list, buffer_views, accessors);
            primitives_doc.push_back(primitive_doc);
        }
        doc["primitives"] = primitives_doc;
        return 0;
    }

    void add_primitive(Primitive &p) {
        primitives.push_back(p);
    }

};
}

#endif