#ifndef  __GLTF_GLTF_H_
#define  __GLTF_GLTF_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"
#include "accessor.h"
#include "buffer_view.h"
#include "mesh.h"
#include "node.h"
#include "scene.h"
#include "image.h"
#include "texture.h"
#include "material.h"
#include "sampler.h"

namespace mesh {

using json = nlohmann::json;

class Gltf {

public:

    Gltf() {
        Scene scene;
        scenes.push_back(scene);
    }

    int read_from_file(const std::string &path) {
        Binary data;
        const std::string suffix = get_path_suffix(path);
        if (suffix == "glb") {
            int ret = read_binary(path, data);
            if (ret != 0) {
                std::cerr << "read file failed: " << path << std::endl;
                return ret;
            }
        } else {
            return -1;
        }
        return read_from_byte(&data[0]);        
    }

    int read_from_byte(char *p) {
        clear();
        // uint32_t magic = *(uint32_t *)(p);
        version = *(uint32_t *)(p + 4);
        total_size = *(uint32_t *)(p + 8);

        // DEBUG(version);
        // DEBUG(total_size);

        uint32_t current = 12;
        while (current < total_size) {
            uint32_t chunk_size = *(uint32_t *)(p + current);
            char chunk_type[5] = { 0 };
            memcpy(chunk_type, p + current + 4, 4);
            Binary buf;
            buf.resize(chunk_size);
            memcpy(&buf[0], p + current + 8, chunk_size);

            if (strncmp(chunk_type, "JSON", 4) == 0) {
                json_data.swap(buf);
            } else if (strncmp(chunk_type, "BIN", 3) == 0) {
                buffer_list.emplace_back(buf);
            } else {
                // std::cerr << "unknown gltf/glb chunk type: " << chunk_type << std::endl;
                return -1;
            }
            current += 8 + chunk_size;
        }

        if (json_data.size() == 0 || buffer_list.size() == 0) {
            // std::cerr << "invalid gltf/glb, JSON size: " << json_data.size() << ", BIN size: " << buffer_list.size() << std::endl;
            return -2;
        }

        json doc = json::parse(json_data);
        // DEBUG(doc);
        // std::cout << "gltf: " << doc.dump(4) << std::endl;

        for (json &doc : doc["accessors"]) {
            Accessor accessor;
            accessor.read(doc);
            accessors.push_back(accessor);
        }
        // DEBUG(accessors.size());

        for (json &doc : doc["bufferViews"]) {
            BufferView buffer_view;
            buffer_view.read(doc);
            buffer_views.push_back(buffer_view);
        }
        // DEBUG(buffer_views.size());

        for (json &doc : doc["textures"]) {
            Texture texture;
            texture.read(doc);
            textures.push_back(texture);
        }
        // DEBUG(textures.size());

        for (json &doc : doc["samplers"]) {
            Sampler sampler;
            sampler.read(doc);
            samplers.push_back(sampler);
        }
        // DEBUG(samplers.size());

        for (json &doc : doc["images"]) {
            Image image;
            image.read(doc, buffer_list, buffer_views);
            images.push_back(image);
        }
        // DEBUG(images.size());

        for (json &doc : doc["materials"]) {
            Material material;
            material.read(doc);
            materials.push_back(material);
        }
        // DEBUG(materials.size());

        // DEBUG(doc["nodes"]);
        for (json &doc : doc["nodes"]) {
            Node node;
            node.read(doc);
            nodes.push_back(node);
        }
        // DEBUG(nodes.size());

        for (json &doc : doc["scenes"]) {
            Scene scene;
            scene.read(doc);
            scenes.push_back(scene);
        }
        // DEBUG(scenes.size());

        // must be procceed at last
        if (doc.find("meshes") != doc.end()) {
            json &meshes_doc = doc["meshes"];
            for (size_t i = 0; i < meshes_doc.size(); ++i) {
                Mesh mesh;
                mesh.read(meshes_doc[i], buffer_list, buffer_views, accessors);
                meshes.push_back(mesh);
            }
        }
        // DEBUG(meshes.size());

        extensions_used = json_get_string_array(doc, "extensionsUsed");
        extensions_required = json_get_string_array(doc, "extensionsRequired");
        _remove_extension("KHR_draco_mesh_compression");
        _build_revert_index();
        return 0;
    }

    int write_to_file(const std::string &path) {
        Binary output;
        write_to_byte(output);
        std::ofstream fs(path, std::ios::binary);
        fs.write((char *)&output[0], output.size());
        return fs.good() ? 0 : -2;
    }

    int write_to_byte(Binary &output) {
        json root;
        root["asset"]["version"] = "2.0";
        // root["extensionsRequired"] = std::vector<std::string>({ "KHR_materials_unlit" });
        // root["extensionsUsed"] = std::vector<std::string>({ "KHR_materials_unlit" });

        buffer_views.clear();
        accessors.clear();
        output_sampler_count = 0;
        output_texture_count = 0;
        output_image_count = 0;
        output_material_count = 0;

        const int buffer_index = 0;
        Binary buffer_data;

        for (size_t i = 0; i < meshes.size(); ++i) {
            json doc;
            meshes[i].write(root, doc, buffer_data, buffer_list, buffer_views, accessors);
            root["meshes"].push_back(doc);
        }

        for (size_t i = 0; i < samplers.size(); ++i) {
            Sampler &sampler = samplers[i];
            json doc;
            sampler.write(doc);
            root["samplers"].push_back(doc);
        }

        for (size_t i = 0; i < materials.size(); ++i) {
            Material &material = materials[i];
            json doc;
            material.write(doc);
            root["materials"].push_back(doc);
        }

        for (size_t i = 0; i < images.size(); ++i) {
            BufferView buffer_view;
            buffer_view.buffer_index = buffer_index;
            buffer_view.byte_length = byte_length(images[i].image_data);
            buffer_view.byte_offset = buffer_data.size();
            buffer_view.byte_stride = 0;
            buffer_view.target = gltf_const::TARGET_NONE;
            // buffer_view.target = 34962;
            buffer_views.emplace_back(buffer_view);
            json buffer_view_doc;
            buffer_view.write(buffer_view_doc);
            root["bufferViews"].push_back(buffer_view_doc);
            append_buffer_data(buffer_data, images[i].image_data);

            json doc;
            images[i].buffer_view_index = buffer_views.size() - 1; // must minus 1
            images[i].write(doc);
            root["images"].push_back(doc);

            // source_reindex[images[i].source_index] = buffer_views.size() - 1;
        }

        for (size_t i = 0; i < textures.size(); ++i) {
            Texture &texture = textures[i];
            json doc;
            texture.write(doc);
            root["textures"].push_back(doc);
        }

        root["scene"] = 0;
        for (size_t i = 0; i < scenes.size(); ++i) {
            Scene &scene = scenes[i];
            json doc;
            scene.write(doc);
            root["scenes"].push_back(doc);
        }

        for (size_t i = 0; i < nodes.size(); ++i) {
            Node &node = nodes[i];
            json doc;
            node.write(doc);
            root["nodes"].push_back(doc);
        }

        if (buffer_data.size() > 0) {
            json doc;
            doc["byteLength"] = buffer_data.size();
            root["buffers"].push_back(doc);
        }

        if (extensions_used.size() > 0) {
            root["extensionsUsed"] = extensions_used;
        }

        if (extensions_required.size() > 0) {
            root["extensionsRequired"] = extensions_required;
        }

        const std::string json_data = root.dump();

        // std::cout << root.dump(4) << std::endl;
        // DEBUG(json_data);

        uint32_t total_size = 
            4 +                                     // magic
            4 +                                     // version
            4 +                                     // total_size
            4 + 4 + padding4(json_data.size()) +    // json
            4 + 4 + padding4(buffer_data.size());   // buffer

        output.resize(total_size, 0);
        char *p = raw_ptr(output);
        *(uint32_t *)(p + 0) = 0x46546C67;
        *(uint32_t *)(p + 4) = version;
        *(uint32_t *)(p + 8) = total_size;
        p += 12;

        *(uint32_t *)p = padding4(json_data.size());
        memcpy(p + 4, "JSON", 4);
        memcpy(p + 8, raw_ptr(json_data), json_data.size());
        int padded = padding4(json_data.size()) - json_data.size();
        if (padded > 0) {
            for (int k = 0; k < padded; ++k) {
                *(p + 8 + json_data.size() + k) = 0x20;
            }
        }
        p += 4 + 4 + padding4(json_data.size());

        *(uint32_t *)p = padding4(buffer_data.size());
        memcpy(p + 4, "BIN\0", 4);
        memcpy(p + 8, raw_ptr(buffer_data), buffer_data.size());
        p += 4 + 4 + padding4(buffer_data.size());

        return 0;
    }

    int merge(const Gltf &model) {
        return 0;
    }

    void debug_json() {
        json doc = json::parse(json_data);
        std::cout << doc.dump(4) << std::endl;
    }

    void clear() {
        // version = 0;
        total_size = 0;

        json_data.clear();
        buffer_list.clear();
        meshes.clear();

        output_buffer.clear();
        output_material_count = 0;
        output_texture_count = 0;
        output_sampler_count = 0;
        output_image_count = 0;

        nodes.clear();
        scenes.clear();

        accessors.clear();
        buffer_views.clear();

        textures.clear();
        materials.clear();
        samplers.clear();
        images.clear();
    }

    void rotate_for_3dtiles() {
        for (size_t i = 0; i < nodes.size(); ++i) {
            Node &node = nodes[i];
            node.has_matrix = true;
            // node.matrix.set_with_row_major(
            //     1, 0, 0, 0, 
            //     0, 0, -1, 0, 
            //     0, 1, 0, 0, 
            //     0, 0, 0, 1
            // );
            node.matrix.set_with_row_major(
                1, 0, 0, 0, 
                0, 0, 1, 0, 
                0, -1, 0, 0, 
                0, 0, 0, 1
            );
        }
    }

    void _build_revert_index() {
        for (size_t i = 0; i < nodes.size(); ++i) {
            for (int index : nodes[i].children) {
                nodes[index].parent_node_index = i;
            }
            if (nodes[i].mesh_index != -1) {
                meshes[nodes[i].mesh_index].node_index = i;
            }
        }

        for (size_t i = 0; i < scenes.size(); ++i) {
            for (int index : scenes[i].indices) {
                nodes[index].parent_scene_index = i;
            }
        }
    }

    void _remove_extension(const std::string &ext) {
        std::vector<std::string> list;
        for (size_t i = 0; i < extensions_used.size(); ++i) {
            if (extensions_used[i] != ext) {
                list.push_back(extensions_used[i]);
            }
        }
        extensions_used.swap(list);

        list.clear();
        for (size_t i = 0; i < extensions_required.size(); ++i) {
            if (extensions_required[i] != ext) {
                list.push_back(extensions_required[i]);
            }
        }
        extensions_required.swap(list);
    }

    void add_mesh(Mesh &mesh) {
        Node node;
        node.mesh_index = meshes.size();
        nodes.push_back(node);

        scenes[0].indices.push_back(nodes.size() - 1);
        meshes.push_back(mesh);
    }

    int add_image_material(const std::string &fpath) {
        Binary bin;
        int ret = read_binary(fpath, bin);
        if (ret != 0) {
            return -1;
        }

        int image_index = images.size();
        int material_index = materials.size();
        int texture_index = textures.size();
        int sampler_index = samplers.size();

        Image image;
        image.set_mime_type_from_path(fpath);
        image.set_binary(bin);
        images.emplace_back(image);

        Material material;
        material.pbr_metallic_roughness.base_color_texture.index = texture_index;
        material.pbr_metallic_roughness.base_color_texture.tex_coord = 0;
        materials.push_back(material);

        Texture texture;
        texture.source_index = image_index;
        texture.sampler_index = sampler_index;
        textures.push_back(texture);

        Sampler sampler;
        samplers.push_back(sampler);

        return material_index;
    }

    int add_color_material(double r, double g, double b, double a = 1.0f) {
        int material_index = materials.size();

        Material material;
        material.pbr_metallic_roughness.base_color_factor = { r, g, b, a };
        materials.push_back(material);

        return material_index;
    }

    void calc_stacked_matrix() {
        for (size_t i = 0; i < nodes.size(); ++i) {
            nodes[i].calc_stacked_matrix(nodes);
        }
    }

    // int add_double_side_material() {
    //     int material_index = materials.size();

    //     Material material;
    //     material.set_double_sided(true);
    //     materials.push_back(material);

    //     return material_index;
    // }

public:
    uint32_t version = 2;
    uint32_t total_size = 0;

    Binary json_data;
    std::vector<Binary> buffer_list;

    std::vector<Mesh> meshes;

    // ------------- use when save -------------
    Binary output_buffer;
    int output_material_count = 0;
    int output_texture_count = 0;
    int output_sampler_count = 0;
    int output_image_count = 0;

    std::vector<Node> nodes;
    std::vector<Scene> scenes;

    std::vector<Accessor> accessors;
    std::vector<BufferView> buffer_views;

    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<Sampler> samplers;
    std::vector<Image> images;

    std::vector<std::string> extensions_used;
    std::vector<std::string> extensions_required;
};

}

#endif