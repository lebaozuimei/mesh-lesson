#ifndef  __GLTF_MATERIAL_H_
#define  __GLTF_MATERIAL_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

namespace mesh {
using json = nlohmann::json;

/* demo here
"materials":[{"alphaMode":"OPAQUE","doubleSided":false,"emissiveFactor":[0,0,0],"pbrMetallicRoughness":{"baseColorFactor":[0.64,0.64,0.64,1],"metallicFactor":0,"roughnessFactor":1}},{"alphaMode":"OPAQUE","doubleSided":false,"emissiveFactor":[0,0,0],"pbrMetallicRoughness":{"baseColorFactor":[0.64,0.64,0.64,1],"metallicFactor":0,"roughnessFactor":1}}]
*/

class Material {

public:

    struct PBRMetallicRoughness {

        std::vector<double> base_color_factor = { 1.0f, 1.0f, 1.0f, 1.0f }; 

        struct BaseColorTexture {
            int index = -1;         // index in textures
            int tex_coord = -1;     // 0->TEXCOORD_0  1->TEXCOORD_1 
            json extensions;
        } base_color_texture;

        float metallic_factor = 0.0f;
        float roughness_factor = 0.5f;

    } pbr_metallic_roughness;

    bool double_sided = false;
    bool has_double_sided = false;

    // TODO: implement it
    std::string alpha_mode;
    std::vector<double> emissive_factor;

    json extensions;

public:

    int read(json &doc) {
        if (doc.contains("doubleSided")) {
            double_sided = json_get_bool(doc, "doubleSided", false);
            has_double_sided = true;
        }

        json &pbr_doc = doc["pbrMetallicRoughness"];
        if (pbr_doc.is_object()) {
            json &bcf_doc = pbr_doc["baseColorFactor"];
            if (bcf_doc.is_array() && bcf_doc.size() == 4) {
                for (size_t i = 0; i < 4; ++i) {
                    pbr_metallic_roughness.base_color_factor[i] = bcf_doc[i];
                }
            }

            json &bct_doc = pbr_doc["baseColorTexture"];
            if (bct_doc.is_object()) {
                pbr_metallic_roughness.base_color_texture.index = json_get_int(bct_doc, "index", -1);
                pbr_metallic_roughness.base_color_texture.tex_coord = json_get_int(bct_doc, "texCoord", -1);
                if (bct_doc.contains("extensions")) {
                    pbr_metallic_roughness.base_color_texture.extensions = bct_doc["extensions"];
                }
            }
            pbr_metallic_roughness.metallic_factor = json_get_double(pbr_doc, "metallicFactor", 0.0f);
            pbr_metallic_roughness.roughness_factor = json_get_double(pbr_doc, "roughnessFactor", 0.5f);

        }

        if (doc.contains("extensions")) {
            extensions = doc["extensions"];
        }
        return 0;
    }

    int write(json &doc) {
        if (has_double_sided) {
            doc["doubleSided"] = double_sided;
        }
        json pbr_doc;
        pbr_doc["metallicFactor"] = pbr_metallic_roughness.metallic_factor;
        pbr_doc["roughnessFactor"] = pbr_metallic_roughness.roughness_factor;
        json bcf_doc(pbr_metallic_roughness.base_color_factor);
        pbr_doc["baseColorFactor"] = bcf_doc;

        if (pbr_metallic_roughness.base_color_texture.index != -1 || 
            pbr_metallic_roughness.base_color_texture.tex_coord != -1) {
            json bct_doc;
            if (pbr_metallic_roughness.base_color_texture.index != -1) {
                bct_doc["index"] = pbr_metallic_roughness.base_color_texture.index;
            }
            if (pbr_metallic_roughness.base_color_texture.tex_coord != -1) {
                bct_doc["texCoord"] = pbr_metallic_roughness.base_color_texture.tex_coord;
            }
            if (!pbr_metallic_roughness.base_color_texture.extensions.is_null()) {
                bct_doc["extensions"] = pbr_metallic_roughness.base_color_texture.extensions;
            }
            pbr_doc["baseColorTexture"] = bct_doc;
        }
        doc["pbrMetallicRoughness"] = pbr_doc;

        // TODO: use real extensions
        // rapidjson::Value ext_doc(rapidjson::kObjectType);
        // rapidjson::Value khr_doc(rapidjson::kObjectType);
        // ext_doc.AddMember("KHR_materials_unlit", khr_doc, allocator);
        // doc.AddMember("extensions", ext_doc, allocator);

        if (!extensions.is_null()) {
            doc["extensions"] = extensions;
        }

        return 0;
    }

    void set_double_sided(bool value) {
        has_double_sided = true;
        double_sided = value;
    }

    void clear_double_sided() {
        has_double_sided = false;
        double_sided = false;
    }

};
}

#endif