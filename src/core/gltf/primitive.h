#ifndef  __GLTF_PRIMITIVE_H_
#define  __GLTF_PRIMITIVE_H_

#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"
#include "buffer_view.h"
#include "accessor.h"
// #include "extension.h"

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

class Primitive {

public:

    typedef uint16_t IndicesType16;
    typedef uint32_t IndicesType32;
    // const uint32_t MAX_INDICES_16 = 65535;

    std::vector<PositionType> positions; 
    std::vector<TexcoordType> texcoords0; 
    std::vector<TexcoordType> texcoords1; 
    std::vector<NormalType> normals; 
    std::vector<IndexType> indices;

    int material_index = -1;
    int mode = -1;

public:

    void clear() {
        positions.clear(); 
        texcoords0.clear(); 
        texcoords1.clear(); 
        normals.clear(); 
        indices.clear();

        material_index = -1;
        mode = -1;
    }

    void swap(Primitive other) {
        positions.swap(other.positions);
        texcoords0.swap(other.texcoords0);
        texcoords1.swap(other.texcoords1);
        normals.swap(other.normals);
        indices.swap(other.indices);
        std::swap(material_index, other.material_index);
        std::swap(mode, other.mode);
    }

    bool empty() const {
        return positions.size() == 0;
    }

    int read(json &doc, 
        const std::vector<Binary> &buffer_list, 
        const std::vector<BufferView> &buffer_views,
        const std::vector<Accessor> &accessors) {
        material_index = json_get_int(doc, "material", -1);
        mode = json_get_int(doc, "mode", -1);

        json &attributes = doc["attributes"];
        _read_block<PositionType>(json_get_int(attributes, "POSITION", -1), buffer_list, buffer_views, accessors, positions);
        _read_block<TexcoordType>(json_get_int(attributes, "TEXCOORD_0", -1), buffer_list, buffer_views, accessors, texcoords0);
        _read_block<TexcoordType>(json_get_int(attributes, "TEXCOORD_1", -1), buffer_list, buffer_views, accessors, texcoords1);
        _read_block<NormalType>(json_get_int(attributes, "NORMAL", -1), buffer_list, buffer_views, accessors, normals);

        std::vector<IndicesType16> indices16; 
        std::vector<IndicesType32> indices32;
        _read_block<IndicesType16>(json_get_int(doc, "indices", -1), buffer_list, buffer_views, accessors, indices16);
        _read_block<IndicesType32>(json_get_int(doc, "indices", -1), buffer_list, buffer_views, accessors, indices32);

        if (indices16.size() > 0) {
            set_indices(indices16);
        } else if (indices32.size() > 0) {
            set_indices(indices32);
        }

        _shrink(positions, texcoords0, normals, indices);

        // for (size_t i = 0; i < std::min(10, int(positions.size() / 3)); ++i) {
        //     std::cout << "positions: " << positions[i * 3] << "," << positions[i * 3 + 1] << "," << positions[i * 3 + 2] << std::endl;
        // }
        // for (size_t i = 0; i < std::min(10, int(texcoords0.size() / 2)); ++i) {
        //     std::cout << "texcoords0: " << texcoords0[i * 3] << "," << texcoords0[i * 3 + 1] << std::endl;
        // }
        // for (size_t i = 0; i < std::min(10, int(indices16.size() / 3)); ++i) {
        //     std::cout << "indices16: " << indices16[i * 3] << "," << indices16[i * 3 + 1] << "," << indices16[i * 3 + 2] << std::endl;
        // }
        // for (size_t i = 0; i < std::min(10, int(indices32.size() / 3)); ++i) {
        //     std::cout << "indices32: " << indices32[i * 3] << "," << indices32[i * 3 + 1] << "," << indices32[i * 3 + 2] << std::endl;
        // }
        return 0;
    }

    int write(json &root, json &doc, Binary &output,
        std::vector<Binary> &buffer_list, 
        std::vector<BufferView> &buffer_views,
        std::vector<Accessor> &accessors) {
        json attr_doc;

        bool has_position_data = positions.size() > 0;
        bool has_texcoord_data = texcoords0.size() + texcoords1.size() > 0;
        bool has_normal_data = normals.size() > 0;
        bool has_index_data = indices.size() > 0;
        bool has_texture_data = material_index != -1;

        if (mode == -1 || mode == gltf_const::PRIMITIVE_MODE_TRIANGLES) {
            assert(has_position_data && has_index_data);
        }

        int accessors_index = accessors.size();

        // if (has_texcoord_data) {
        //     json_add_int(attr_doc, allocator, "TEXCOORD_0", accessors_index++);
        // }
        // if (has_normal_data) {
        //     json_add_int(attr_doc, allocator, "NORMAL", accessors_index++);
        // }
        // json_add_int(doc, allocator, "indices", accessors_index++);
        // json_add_int(doc, allocator, "material", gltf.output_material_count++);

        const int buffer_index = 0;

        // ----------- for POSITION/TEXCOORD_0 -------------
        if (has_position_data) {
            Accessor accessor;
            accessor.buffer_view_index = buffer_views.size();
            accessor.byte_offset = 0;
            accessor.count = positions.size() / 3;
            accessor.component_type = gltf_const::COMPONENT_TYPE_FLOAT;
            accessor.type = gltf_const::TYPE_VEC3;
            accessors.emplace_back(accessor);

            json accessor_doc;
            accessor.write(accessor_doc);
            std::vector<PositionType> min_list, max_list;
            // _calc_max_min<PositionType, 3>(positions, min_list, max_list);
            _calc_max_min(3, positions, min_list, max_list);
            accessor_doc["min"] = min_list;
            accessor_doc["max"] = max_list;
            root["accessors"].push_back(accessor_doc);

            attr_doc["POSITION"] = accessors_index++;

            // {
            //     const PositionType *p = &positions[0];
            //     size_t count = positions.size() / 3;
            //     std::vector<PositionType> min_list, max_list;
            //     min_list.resize(3, std::numeric_limits<PositionType>::max());
            //     max_list.resize(3, std::numeric_limits<PositionType>::min());
            //     for (size_t i = 0; i < count; ++i) {
            //         if (*(p + 1) >= -1.0f) {
            //             DEBUG3(*p, *(p + 1), *(p + 2));
            //         }
            //         for (int k = 0; k < 3; ++k) {
            //             PositionType v = *(p++);
            //             min_list[k] = std::min(v, min_list[k]);
            //             max_list[k] = std::max(v, max_list[k]);
            //         }
            //     }
            // }
        }

        if (has_texcoord_data) {
            Accessor accessor;
            accessor.buffer_view_index = buffer_views.size();
            accessor.byte_offset = sizeof(PositionType) * 3;        // ATTENTION: offset is relative to position, 12
            accessor.count = texcoords0.size() / 2;
            accessor.component_type = gltf_const::COMPONENT_TYPE_FLOAT;
            accessor.type = gltf_const::TYPE_VEC2;
            accessors.emplace_back(accessor);

            json accessor_doc;
            accessor.write(accessor_doc);
            std::vector<TexcoordType> min_list, max_list;
            // _calc_max_min<TexcoordType, 2>(texcoords0, min_list, max_list);
            // accessor_doc["min"] = min_list;
            // accessor_doc["max"] = max_list;
            root["accessors"].push_back(accessor_doc);

            attr_doc["TEXCOORD_0"] = accessors_index++;
        }

        if (has_position_data) {
            BufferView buffer_view;
            buffer_view.buffer_index = buffer_index;
            buffer_view.byte_length = byte_length(positions) + byte_length(texcoords0);
            buffer_view.byte_offset = output.size();
            buffer_view.byte_stride = sizeof(PositionType) * 3 + (has_texcoord_data ? sizeof(TexcoordType) * 2 : 0);
            buffer_view.target = gltf_const::TARGET_VERTEX_ATTRIBUTES;
            buffer_views.emplace_back(buffer_view);

            json buffer_view_doc;
            buffer_view.write(buffer_view_doc);
            root["bufferViews"].push_back(buffer_view_doc);

            Binary buf(buffer_view.byte_length);
            size_t point_count = positions.size() / 3;
            size_t positionbyte_length = sizeof(PositionType) * 3;
            size_t texcoordbyte_length = sizeof(TexcoordType) * 2;
            // DEBUG(point_count);
            // DEBUG(positionbyte_length);
            // DEBUG(texcoordbyte_length);
            // DEBUG(positions.size() / 3);
            // DEBUG(byte_length(positions));
            // DEBUG(texcoords0.size() / 2);
            // DEBUG(byte_length(texcoords0));
            // DEBUG(buffer_view.byte_length);
            const char *position_src = raw_ptr(positions);
            const char *texcoord_src = raw_ptr(texcoords0);
            char *dst = raw_ptr(buf);

            for (size_t k = 0; k < point_count; ++k) {
                memcpy(dst, position_src, positionbyte_length);
                position_src += positionbyte_length;
                dst += positionbyte_length;

                if (has_texcoord_data) {
                    memcpy(dst, texcoord_src, texcoordbyte_length);
                    texcoord_src += texcoordbyte_length;
                    dst += texcoordbyte_length;
                }
            }
            // DEBUG(dst - raw_ptr(buf));
            // DEBUG(buf.size());
            assert(dst - raw_ptr(buf) == (int)buf.size());
            append_buffer_data(output, buf);
        }

        // ----------- for NORMAL -------------
        if (has_normal_data) {
            Accessor accessor;
            accessor.buffer_view_index = buffer_views.size();
            accessor.byte_offset = 0;
            accessor.count = normals.size() / 3;
            accessor.component_type = gltf_const::COMPONENT_TYPE_FLOAT;
            accessor.type = gltf_const::TYPE_VEC3;
            accessors.emplace_back(accessor);

            json accessor_doc;
            accessor.write(accessor_doc);
            std::vector<NormalType> min_list, max_list;
            // _calc_max_min<NormalType, 3>(normals, min_list, max_list);
            _calc_max_min(3, normals, min_list, max_list);
            accessor_doc["min"] = min_list;
            accessor_doc["max"] = max_list;
            root["accessors"].push_back(accessor_doc);

            attr_doc["NORMAL"] = accessors_index++;

            // DEBUG(join_list(normals));


            BufferView buffer_view;
            buffer_view.buffer_index = buffer_index;
            buffer_view.byte_length = byte_length(normals);
            buffer_view.byte_offset = output.size();
            buffer_view.byte_stride = 0;
            buffer_view.target = gltf_const::TARGET_VERTEX_ATTRIBUTES;
            buffer_views.emplace_back(buffer_view);
            json buffer_view_doc;
            buffer_view.write(buffer_view_doc);
            root["bufferViews"].push_back(buffer_view_doc);
            // output.insert(output.end(), raw_ptr(normals), raw_ptr(normals) + byte_length(normals));
            // _pad_0_align_4(output);
            append_buffer_data(output, normals);
        }

        // ----------- for indices -------------
        if (has_index_data) {
            std::vector<IndicesType16> indices16; 
            std::vector<IndicesType32> indices32;
            fill_index_by_range(indices16, indices32);

            Accessor accessor;
            accessor.buffer_view_index = buffer_views.size();
            accessor.byte_offset = 0;
            accessor.count = indices.size();
            if (indices16.size() > 0) {
                accessor.component_type = gltf_const::COMPONENT_TYPE_UNSIGNED_SHORT;
            } else {
                accessor.component_type = gltf_const::COMPONENT_TYPE_UNSIGNED_INT;
            }
            accessor.type = gltf_const::TYPE_SCALAR;
            accessors.emplace_back(accessor);
            json accessor_doc;
            accessor.write(accessor_doc);
            // std::vector<NormalType> min_list, max_list;
            // _calc_max_min<NormalType, 3>(normals, min_list, max_list);
            // accessor_doc["min"] = min_list;
            // accessor_doc["max"] = max_list;
            root["accessors"].push_back(accessor_doc);
            doc["indices"] = accessors_index++;

            BufferView buffer_view;
            buffer_view.buffer_index = buffer_index;
            buffer_view.byte_length = byte_length(indices16) + byte_length(indices32);
            buffer_view.byte_offset = output.size();
            buffer_view.byte_stride = 0;
            buffer_view.target = gltf_const::TARGET_VERTEX_INDICES;
            buffer_views.emplace_back(buffer_view);
            json buffer_view_doc;
            buffer_view.write(buffer_view_doc);
            root["bufferViews"].push_back(buffer_view_doc);
            append_buffer_data(output, indices16);
            append_buffer_data(output, indices32);
        }

        // ----------- for texture -------------
        if (has_texture_data) {
            doc["material"] = material_index;
        }

        if (mode != -1) {
            doc["mode"] = mode;
        }

        // TODO: may not has texture
        // BufferView bimg;
        // bimg.buffer_index = buffer_index;
        // bimg.length = image_holder.getbyte_length();
        // bimg.offset = buffer.size();
        // bimg.stride = 0;
        // bimg.target = gltf_const::TARGET_VERTEX_ATTRIBUTES;
        // // LOG(INFO) << bimg.length;
        // gltf.buffer_views.emplace_back(bimg);
        // rapidjson::Value buffer_view_img_doc(rapidjson::kObjectType);
        // bimg.save_to_glb(buffer_view_img_doc, allocator);
        // root["bufferViews"].PushBack(buffer_view_img_doc, allocator);
        // buffer.insert(buffer.end(), (const char *)image_holder.get_ptr(), (const char *)image_holder.get_ptr() + image_holder.getbyte_length());
        // pad_0_align_4(buffer);

        // rapidjson::Value sampler_doc(rapidjson::kObjectType);
        // sampler.save_to_glb(sampler_doc, allocator);   // use original
        // root["samplers"].PushBack(sampler_doc, allocator);

        // rapidjson::Value image_doc(rapidjson::kObjectType);
        // image_t image_inst;
        // image_inst.buffer_view_index = gltf.buffer_views.size() - 1; // must minus 1
        // image_inst.mime_type = image.mime_type;
        // image_inst.save_to_glb(image_doc, allocator);
        // root["images"].PushBack(image_doc, allocator);

        // rapidjson::Value texture_doc(rapidjson::kObjectType);
        // texture_t texture;
        // texture.sampler_index = gltf.output_sampler_count++;
        // texture.source_index = gltf.output_image_count++;
        // texture.save_to_glb(texture_doc, allocator);
        // root["textures"].PushBack(texture_doc, allocator);

        // rapidjson::Value material_doc(rapidjson::kObjectType);
        // material.pbr_metallic_roughness.base_color_texture.index = gltf.output_texture_count++;
        // material.pbr_metallic_roughness.base_color_texture.tex_coord = 0;   // TEXCOORD_0
        // material.save_to_glb(material_doc, allocator);
        // root["materials"].PushBack(material_doc, allocator);
        // json_add_int(doc, allocator, "material", gltf.output_material_count++);

        doc["attributes"] = attr_doc;
        return 0;
    }

    void set_positions(std::vector<PositionType> &data) {
        positions.clear();
        positions.swap(data);
    }

    void set_normals(std::vector<NormalType> &data) {
        normals.clear();
        normals.swap(data);
    }

    void set_texcoords0(std::vector<TexcoordType> &data) {
        texcoords0.clear();
        texcoords0.swap(data);
    }

    template<typename T>
    void set_indices(std::vector<T> &data) {
        indices.resize(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            indices[i] = data[i];
        }
    }

    void reverse_indices() {
        for (size_t i = 0; i < indices.size(); i += 3) {
            IndexType t = indices[i];
            indices[i] = indices[i + 2];
            indices[i + 2] = t;
        }
    }

    // template<>
    // void set_indices<IndexType>(std::vector<IndexType> &data) {
    //     indices.clear();
    //     indices.swap(data);
    // }

    void attach_material(int index) {
        material_index = index;
    }

    void copy_material_from(const Primitive &other) {
        material_index = other.material_index;
    }

    void add_position(PositionType x, PositionType y, PositionType z) {
        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
    }

    void add_index(IndexType i, IndexType j, IndexType k) {
        indices.push_back(i);
        indices.push_back(j);
        indices.push_back(k);
    }

    // void reset_index(std::vertor<int> &indices) {
    //     if (indices.size() > 0) {
    //         int max_value = indices[0];
    //         for (int index : indices) {
    //             max_value = std::max(max_value, index);
    //         }
    //     }
    // }

    // void rewrite_index(std::vector<int> &out) const {
    //     if (indices16.size() > 0) {
    //         out.resize(indices16.size());
    //         for (size_t i = 0; i < indices16.size(); ++i) {
    //             out[i] = indices16[i];
    //         }
    //     } else if (indices32.size() > 0) {
    //         out.resize(indices32.size());
    //         for (size_t i = 0; i < indices32.size(); ++i) {
    //             out[i] = indices32[i];
    //         }
    //     }
    // }

    void fill_index_by_range(std::vector<IndicesType16> &indices16, std::vector<IndicesType32> &indices32) {
        if (indices.size() > 0) {
            int max_value = indices[0];
            for (int index : indices) {
                max_value = std::max(max_value, index);
            }
            if (max_value < 65536) {
                indices16.resize(indices.size());
                indices32.resize(0);
                for (size_t k = 0; k < indices.size(); ++k) {
                    indices16[k] = indices[k];
                }
            } else {
                indices16.resize(0);
                indices32.resize(indices.size());
                for (size_t k = 0; k < indices.size(); ++k) {
                    indices32[k] = indices[k];
                }
            }
        }
    }

    void add_point(double x, double y, double z) {
        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
    }

    void add_quad(int i, int j, int k, int t) {
        indices.push_back(i);
        indices.push_back(j);
        indices.push_back(k);
        indices.push_back(i);
        indices.push_back(k);
        indices.push_back(t);
    }

protected:
    template<typename T>
    int _read_block(int index, 
        const std::vector<Binary> &buffer_list, 
        const std::vector<BufferView> &buffer_views,
        const std::vector<Accessor> &accessors,
        std::vector<T> &data) {
        if (index >= 0 && index < (int)accessors.size()) {
            const Accessor &accessor = accessors[index];
            // DEBUG(accessor.buffer_view_index);
            // DEBUG(buffer_views.size());
            assert(accessor.buffer_view_index >= 0 && accessor.buffer_view_index < (int)buffer_views.size());
            const BufferView &buffer_view = buffer_views[accessor.buffer_view_index];
            // maybe correct, when we are not sure to use indices16/indices32
            if (sizeof(T) != gltf_const::get_component_byte(accessor.component_type)) {
                // std::cerr << "component size not match: " << sizeof(T) << ", " << gltf_const::get_component_byte(accessor.component_type) << std::endl;
                return 0;
            }

            int component_count = gltf_const::get_component_count(accessor.type);
            int componentbyte_length = gltf_const::get_component_byte(accessor.component_type);
            int total_byte_length = accessor.count * component_count * componentbyte_length;
            data.resize(total_byte_length / sizeof(T));
            char *dst = raw_ptr(data);

            // DEBUG(total_byte_length);
            // DEBUG(buffer_view.byte_stride);

            const char *src = raw_ptr(buffer_list[buffer_view.buffer_index]) + buffer_view.byte_offset + accessor.byte_offset;
            if (buffer_view.byte_stride == 0) {
                memcpy(dst, src, total_byte_length);
            } else {
                // for POSITION, copy_count=12; for TEXTCOORD, copy_count=8
                int copy_count = component_count * componentbyte_length; 
                for (int k = 0; k < accessor.count; ++k) {
                    memcpy(dst, src, copy_count);
                    src += buffer_view.byte_stride;
                    dst += copy_count;
                }
            }
        } else if (index != -1) {
            std::cerr << "accessor index overflow: " << index << "/" << accessors.size() << std::endl;
            return -1;
        }
        return 0;
    }

    // template <typename T, int COMPONENT_COUNT>
    template <typename T>
    void _calc_max_min(int COMPONENT_COUNT, const std::vector<T> &data, std::vector<T> &min_list, std::vector<T> &max_list) {
        const T *p = &data[0];
        size_t count = data.size() / COMPONENT_COUNT;
        min_list.resize(COMPONENT_COUNT, std::numeric_limits<T>::max());
        max_list.resize(COMPONENT_COUNT, std::numeric_limits<T>::lowest());
        for (size_t i = 0; i < count; ++i) {
            for (int k = 0; k < COMPONENT_COUNT; ++k) {
                T v = *(p++);
                min_list[k] = std::min(v, min_list[k]);
                max_list[k] = std::max(v, max_list[k]);
            }
        }
    }

    template <typename INDICES_TYPE>
    size_t _shrink(std::vector<PositionType> &positions, std::vector<TexcoordType> &texcoords, 
        std::vector<NormalType> &normals, std::vector<INDICES_TYPE> &indices) {
        if (indices.size() == 0) {
            return 0;
        }
        assert(positions.size() % 3 == 0 && texcoords.size() % 2 == 0 && normals.size() % 3 == 0);
        assert(texcoords.size() == 0 || positions.size() / 3 == texcoords.size() / 2);
        assert(normals.size() == 0 || positions.size() == normals.size());

        size_t unused_count = 0;

        size_t value_count = positions.size() / 3;
        std::vector<bool> used(value_count, false);
        for (auto v : indices) {
            assert(v < (int)used.size());
            used[v] = true;
        }
        std::vector<INDICES_TYPE> remap(value_count);
        INDICES_TYPE cur = 0;
        for (size_t i = 0; i < used.size(); ++i) {
            if (used[i]) {
                remap[i] = cur++;
            } else {
                ++unused_count;
            }
        }
        for (size_t i = 0; i < remap.size(); ++i) {
            if (remap[i] != (int)i) {
                positions[remap[i] * 3 + 0] = positions[i * 3 + 0];
                positions[remap[i] * 3 + 1] = positions[i * 3 + 1];
                positions[remap[i] * 3 + 2] = positions[i * 3 + 2];

                if (texcoords.size() > 0) {
                    texcoords[remap[i] * 2 + 0] = texcoords[i * 2 + 0];
                    texcoords[remap[i] * 2 + 1] = texcoords[i * 2 + 1];
                }

                if (normals.size() > 0) {
                    normals[remap[i] * 3 + 0] = normals[i * 3 + 0];
                    normals[remap[i] * 3 + 1] = normals[i * 3 + 1];
                    normals[remap[i] * 3 + 2] = normals[i * 3 + 2];
                }
            }
        }

        positions.resize((value_count - unused_count) * 3);
        if (texcoords.size() > 0) {
            texcoords.resize((value_count - unused_count) * 2);
        }
        if (normals.size() > 0) {
            normals.resize((value_count - unused_count) * 3);
        }
        for (size_t i = 0; i < indices.size(); ++i) {
            indices[i] = remap[indices[i]];
        }
        return unused_count;
    }
};
}

#endif