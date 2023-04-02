#ifndef  __OFF_READER_H_
#define  __OFF_READER_H_

#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <assert.h>
#include "../core/basetypes.h"
#include "../utils/io_util.h"
#include "../core/gltf/gltf.h"
#include "../core/gltf/primitive.h"

namespace mesh {
class OffReader {

public:

    int read_from_file(const std::string &path, Primitive &out) {
        std::vector<std::string> raw_list;
        int ret = read_file_to_string(path, raw_list);
        CHECK_RET(ret);
        bool has_title = false;
        bool has_count = false;
        int vertex_count = 0;
        int face_count = 0;
        int current_vertex_count = 0;
        int current_face_count = 0;
        std::vector<IndexType> global_indices;
        for (const std::string &raw : raw_list) {
            const std::string content = trim_string(raw);
            if (content.empty()) {
                continue;
            }
            if (!has_title) {
                if (content != "OFF") {
                    return -1;
                }
                has_title = true;
                continue;
            }

            if (!has_count) {
                std::istringstream iss(content);
                iss >> vertex_count >> face_count;
                if (vertex_count == 0 || face_count == 0) {
                    return -2;
                }
                has_count = true;
                continue;
            }

            if (current_vertex_count < vertex_count) {
                std::istringstream iss(content);
                PositionType x, y, z;
                iss >> x >> y >> z;
                out.positions.push_back(x);
                out.positions.push_back(y);
                out.positions.push_back(z);
                ++current_vertex_count;
            } else if (current_face_count < face_count) {
                std::istringstream iss(content);
                int c;
                IndexType i, j, k;
                iss >> c >> i >> j >> k;
                out.indices.push_back(i);
                out.indices.push_back(j);
                out.indices.push_back(k);
                ++current_face_count;
            } else {
                return -3;
            }
        }
        if (current_vertex_count != vertex_count || current_face_count != face_count) {
            return -4;
        }
        return 0;
    }

};
}

#endif