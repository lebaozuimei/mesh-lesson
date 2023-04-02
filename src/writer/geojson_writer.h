#ifndef  __GEOJSON_WRITER_H_
#define  __GEOJSON_WRITER_H_

#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <assert.h>
#include <nlohmann/json.hpp>

#include "../core/basetypes.h"
#include "../core/math/point3.h"
#include "../utils/io_util.h"

namespace mesh {

using json = nlohmann::json;

class GeojsonWriter {

public:

    std::vector<std::vector<Point3>> line_list;
    std::vector<json> line_property_list;

public:

    int write_to_file(const std::string &path) const {
        json data;
        data["type"] = "FeatureCollection";
        for (size_t i = 0; i < line_list.size(); ++i) {
            json geometry_doc;
            geometry_doc["type"] = "LineString";
            const std::vector<Point3> &line = line_list[i];
            for (const Point3 &p : line) {
                json point_doc;
                point_doc.push_back(p.x);
                point_doc.push_back(p.y);
                point_doc.push_back(p.z);
                geometry_doc["coordinates"].emplace_back(point_doc);
            }
            json feature_doc;
            feature_doc["geometry"] = geometry_doc;
            if (i < line_property_list.size()) {
                feature_doc["properties"] = line_property_list[i];
            }
            data["features"].emplace_back(feature_doc);
        }

        std::ofstream fs(path);
        if (!fs) {
            return -1;
        }
        fs << std::setw(4) << data << std::endl;
        return 0;
    }

};
}

#endif