#ifndef  __GLTF_NODE_H_
#define  __GLTF_NODE_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"
#include "../math/matrix4.h"

namespace mesh {
using json = nlohmann::json;

/* demo here
{
  "scene": 0
}
*/

class Node {

public:
    int mesh_index = -1;
    std::string name;
    std::vector<int> children;

    bool has_matrix = false;
    Matrix4 matrix = Matrix4(1.0f);

    bool has_rotation = false;
    double rotation[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    bool has_scale = false;
    double scale[3] = { 1.0f, 1.0f, 1.0f };

    bool has_translation = false;
    double translation[3] = { 0.0f, 0.0f, 0.0f };

    int parent_scene_index = -1;
    int parent_node_index = -1;

    Matrix4 merged_matrix = Matrix4(1.0f);
    Matrix4 stacked_matrix = Matrix4(1.0f);

public:

    int read(json &doc) {
        if (json_has_key(doc, "mesh")) {
            mesh_index = doc["mesh"];
        }
        if (json_has_key(doc, "name")) {
            name = doc["name"];
        }
        if (json_has_key(doc, "children")) {
            for (int index : doc["children"]) {
                children.push_back(index);
            }
        }
        if (json_has_key(doc, "matrix")) {
            has_matrix = true;
            matrix = Matrix4(doc["matrix"]);
        }

        if (json_has_key(doc, "rotation")) {
            has_rotation = true;
            for (size_t i = 0; i < doc["rotation"].size(); ++i) {
                rotation[i] = doc["rotation"][i];
            }
        }

        if (json_has_key(doc, "scale")) {
            has_scale = true;
            for (size_t i = 0; i < doc["scale"].size(); ++i) {
                scale[i] = doc["scale"][i];
            }
        }

        if (json_has_key(doc, "translation")) {
            has_translation = true;
            for (size_t i = 0; i < doc["translation"].size(); ++i) {
                translation[i] = doc["translation"][i];
            }
        }

        if (has_rotation || has_scale || has_translation) {
            _calc_merged_matrix();
            // merged_matrix.debug("merged_matrix");
        }
        return 0;
    }

    int write(json &doc) {
        if (mesh_index != -1) {
            doc["mesh"] = mesh_index;
        }
        if (name.size() > 0) {
            doc["name"] = name;
        }
        if (has_matrix) {
            doc["matrix"] = matrix.data;
        } else {
            if (has_rotation) {
                doc["rotation"] = rotation;
            }
            if (has_scale) {
                doc["scale"] = scale;
            }
            if (has_translation) {
                doc["translation"] = translation;
            }
        }
        if (children.size() > 0) {
            doc["children"] = children;
        }
        return 0;
    }

    void calc_stacked_matrix(const std::vector<Node> &nodes) {
        double lm[16];
        if (has_matrix) {
            matrix.fill_to(lm);
        } else if (has_rotation || has_scale || has_translation) {
            merged_matrix.fill_to(lm);
        } else {
            matrix.fill_to(lm);
        }

        int node_index = parent_node_index;
        while (node_index != -1) {
            double pm[16];
            nodes[node_index].merged_matrix.fill_to(pm);

            for (size_t i = 0; i < 4; ++i) {
                double l0 = lm[i * 4 + 0];
                double l1 = lm[i * 4 + 1];
                double l2 = lm[i * 4 + 2];

                double r0 = l0 * pm[0] + l1 * pm[4] + l2 * pm[8];
                double r1 = l0 * pm[1] + l1 * pm[5] + l2 * pm[9];
                double r2 = l0 * pm[2] + l1 * pm[6] + l2 * pm[10];

                lm[i * 4 + 0] = r0;
                lm[i * 4 + 1] = r1;
                lm[i * 4 + 2] = r2;
            }

            lm[12] += pm[12];
            lm[13] += pm[13];
            lm[14] += pm[14];

            node_index = nodes[node_index].parent_node_index;
        }
        stacked_matrix = lm;
    }

    void _calc_merged_matrix() {
        double lm[16];
        double qx = rotation[0];
        double qy = rotation[1];
        double qz = rotation[2];
        double qw = rotation[3];

        double sx = scale[0];
        double sy = scale[1];
        double sz = scale[2];

        lm[0] = (1 - 2 * qy * qy - 2 * qz * qz) * sx;
        lm[1] = (2 * qx * qy + 2 * qz * qw) * sx;
        lm[2] = (2 * qx * qz - 2 * qy * qw) * sx;
        lm[3] = 0.f;

        lm[4] = (2 * qx * qy - 2 * qz * qw) * sy;
        lm[5] = (1 - 2 * qx * qx - 2 * qz * qz) * sy;
        lm[6] = (2 * qy * qz + 2 * qx * qw) * sy;
        lm[7] = 0.f;

        lm[8] = (2 * qx * qz + 2 * qy * qw) * sz;
        lm[9] = (2 * qy * qz - 2 * qx * qw) * sz;
        lm[10] = (1 - 2 * qx * qx - 2 * qy * qy) * sz;
        lm[11] = 0.0f;

        lm[12] = translation[0];
        lm[13] = translation[1];
        lm[14] = translation[2];
        lm[15] = 1.0f;
        merged_matrix = lm;
    }
};
}

#endif