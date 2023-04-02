#ifndef  __OBJ_WRITER_H_
#define  __OBJ_WRITER_H_

#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <assert.h>
#include "../core/basetypes.h"
#include "../utils/io_util.h"


namespace mesh {
class ObjWriter {

public:

    std::vector<std::string> prefix_lines;
    std::vector<std::string> suffix_lines;

public:

    int write_to_file(const Primitive &in, const std::string &path) {
        std::ofstream fs(path);
        if (!fs) {
            return -1;
        }

        const std::vector<PositionType> &positions = in.positions;
        const std::vector<NormalType> &normals = in.normals;
        const std::vector<IndexType> &indices = in.indices;
        const std::vector<TexcoordType> &texcoords = in.texcoords0;

        fs << "# vertex count: " << positions.size() / 3 << std::endl;
        fs << "# triangle count: " << indices.size() / 3 << std::endl;

        for (size_t k = 0; k < prefix_lines.size(); ++k) {
            fs << prefix_lines[k] << std::endl;
        }

        fs << "# Start of Vertices" << std::endl;
        for (size_t k = 0; k < positions.size(); k += 3) {
            fs << "v " << positions[k] << " " << positions[k + 1] << " " << positions[k + 2] << std::endl;
        }

        fs << "# Start of Normals" << std::endl;
        for (size_t k = 0; k < normals.size(); k += 3) {
            fs << "vn " << normals[k] << " " << normals[k + 1] << " " << normals[k + 2] << std::endl;
        }

        fs << "# Start of Textures" << std::endl;
        for (size_t k = 0; k < texcoords.size(); k += 2) {
            fs << "vt " << texcoords[k] << " " << texcoords[k + 1] << std::endl;
        }

        for (size_t k = 0; k < indices.size(); k += 3) {
            int a = indices[k + 0] + 1;
            int b = indices[k + 1] + 1;
            int c = indices[k + 2] + 1;
            fs << "f " << a << "/" << a << "/" << a << " "
                << b << "/" << b << "/" << b << " "
                << c << "/" << c << "/" << c << " " << std::endl;
        }

        for (size_t k = 0; k < suffix_lines.size(); ++k) {
            fs << suffix_lines[k] << std::endl;
        }

        return 0;
    }

};
}

#endif