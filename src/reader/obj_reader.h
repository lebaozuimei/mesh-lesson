#ifndef  __OBJ_READER_H_
#define  __OBJ_READER_H_

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
class ObjReader {

public:

    struct MtlInfo {
        std::string map_kd_path;
    };

    int read_from_file(const std::string &path, Primitive &out, Gltf *gltf,
        std::vector<std::vector<IndexType>> &unrecognized_faces) {
        std::vector<std::string> raw_list;
        int ret = read_file_to_string(path, raw_list);
        CHECK_RET(ret);
        std::vector<IndexType> global_indices;
        for (const std::string &raw : raw_list) {
            std::string content;
            std::string command = _get_command_and_content(raw, content);
            if (command.empty() || command == "#") {
                continue;
            }
            // DEBUG2(command, content);
            std::istringstream iss(content);
            if (command == "v") {
                PositionType x, y, z;
                iss >> x >> y >> z;
                // DEBUG3(x, y, z);
                out.positions.push_back(x);
                out.positions.push_back(y);
                out.positions.push_back(z);
            } else if (command == "vt") {
                TexcoordType u, v;
                iss >> u >> v;
                // DEBUG2(u, v);
                out.texcoords0.push_back(u);
                out.texcoords0.push_back(v);
            }  else if (command == "vn") {
                // The normal is what tells it which way the polygon faces. If you don't give one, it is determined by the order the verticies are given. They are assumed to be in counter-clockwise direction. 
                NormalType x, y, z;
                iss >> x >> y >> z;
                // DEBUG3(x, y, z);
                out.normals.push_back(x);
                out.normals.push_back(y);
                out.normals.push_back(z);
            } else if (command == "f") {
                std::vector<IndexType> local_indices;
                std::vector<std::string> raw_face_list = split_string(content, ' ');
                for (size_t k = 0; k < raw_face_list.size(); ++k) {
                    std::string raw_face = trim_string(raw_face_list[k]);
                    // DEBUG(raw_face);
                    if (!raw_face.empty()) {
                        IndexType index;
                        std::istringstream iss(raw_face);
                        iss >> index;
                        local_indices.push_back(index - 1);
                    }
                }
                if (local_indices.size() != 3) {
                    unrecognized_faces.push_back(local_indices);
                } else {
                    global_indices.push_back(local_indices[0]);
                    global_indices.push_back(local_indices[1]);
                    global_indices.push_back(local_indices[2]);
                }
            } else if (command == "mtllib" && gltf != nullptr) {
                const std::string mtpath = join_path(get_parent_path(path), content);
                MtlInfo mtl_info;
                int ret = _read_mtl(mtpath, mtl_info);
                CHECK_RET(-1);
                if (!mtl_info.map_kd_path.empty()) {
                    const std::string kdpath = join_path(get_parent_path(path), mtl_info.map_kd_path);
                    int material_index = gltf->add_image_material(kdpath);
                    if (material_index == -1) {
                        std::cout << "read tex failed: " << kdpath << std::endl;
                        return -2;
                    }
                    out.attach_material(material_index);
                }
            }
        }

        out.set_indices(global_indices);
        return 0;
    }

protected:

    int _read_mtl(const std::string &fpath, MtlInfo &mtl_info) {
        std::vector<std::string> raw_list;
        int ret = read_file_to_string(fpath, raw_list);
        if (ret != 0) {
            return ret;
        }
        for (const std::string &raw : raw_list) {
            std::string content;
            std::string command = _get_command_and_content(raw, content);
            if (command == "map_Kd") {
                mtl_info.map_kd_path = content;
            }
        }
        return 0;
    }

    std::string _get_command(const std::string &str) {
        size_t pos = str.find_first_of(" ");
        if (pos == std::string::npos) {
            return "";
        }
        return str.substr(0, pos);
    }

    std::string _get_command_and_content(const std::string &str, std::string &content) {
        std::string command = _get_command(str);
        if (command.empty()) {
            content.clear();
        } else {
            content = trim_string(str.substr(command.size() + 1));
        }
        return command;    
    }

};
}

#endif