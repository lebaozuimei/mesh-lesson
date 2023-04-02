#ifndef  __GLTF_IMAGE_H_
#define  __GLTF_IMAGE_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

#include "buffer_view.h"

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

class Image {

public:
    int buffer_view_index = -1;
    std::string mime_type;
    std::string name;

    Binary image_data;

public:

    int read(json &doc,
        const std::vector<Binary> &buffer_list, 
        const std::vector<BufferView> &buffer_views) {
        buffer_view_index = json_get_int(doc, "bufferView", -1);
        mime_type = json_get_string(doc, "mimeType");
        name = json_get_string(doc, "name");

        if (buffer_view_index != -1) {
            assert(buffer_view_index >= 0 && buffer_view_index < (int)buffer_views.size());
            const BufferView &buffer_view = buffer_views[buffer_view_index];
            image_data.resize(buffer_view.byte_length);
            memcpy(raw_ptr(image_data), raw_ptr(buffer_list[buffer_view.buffer_index]) + buffer_view.byte_offset, buffer_view.byte_length);
        }
        return 0;
    }

    int write(json &doc) {
        doc["bufferView"] = buffer_view_index;
        doc["mimeType"] = mime_type;
        if (name.size() > 0) {
            doc["name"] = name;
        }
        return 0;
    }

    std::string get_suffix_from_mime_type() {
        if (mime_type == "image/jpeg") {
            return "jpeg";
        } else if (mime_type == "image/png") {
            return "png";
        }
        return "png";
    }

    int write_to_file(const std::string &path) {
        Binary &output = image_data;
        std::ofstream fs(path, std::ios::binary);
        fs.write((char *)&output[0], output.size());
        return fs.good() ? 0 : -2;
    }

    int set_mime_type_from_path(const std::string &fpath) {
        size_t pos = fpath.rfind(".");
        if (pos != std::string::npos) {
            std::string suffix = lower_string(fpath.substr(pos + 1));
            // DEBUG(suffix);
            if (suffix == "jpeg" || suffix == "jpg") {
                mime_type = "image/jpeg";
                return 0;
            } else if (suffix == "png") {
                mime_type = "image/png";
                return 0;
            }
        }
        return -1;
    }

    void set_binary(const Binary &bin) {
        image_data = bin;
    }
};
}

#endif