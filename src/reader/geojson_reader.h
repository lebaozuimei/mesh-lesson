#ifndef  __GEOJSON_READER_H_
#define  __GEOJSON_READER_H_

#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <assert.h>
#include <nlohmann/json.hpp>

#include "../core/basetypes.h"
#include "../core/math/point3.h"
#include "../utils/io_util.h"
#include "../utils/geo_util.h"

namespace mesh {

using json = nlohmann::json;

class GeojsonReader {

public:

    std::vector<std::vector<Point3>> line_list;
    std::vector<json> line_property_list;

    std::vector<std::vector<Point3>> polygon_list;
    std::vector<json> polygon_property_list;

    std::vector<Point3> point_list;
    std::vector<json> point_property_list;

public:

    int read_from_stream(std::istream &fs) {
        json data = json::parse(fs);
        for (size_t i = 0; i < data["features"].size(); ++i) {
            json &feature_doc = data["features"][i];
            json &geometry_doc = feature_doc["geometry"];
            if (geometry_doc["type"] == "LineString") {
                std::vector<Point3> line;
                for (size_t k = 0; k < geometry_doc["coordinates"].size(); ++k) {
                    json &coord_doc = geometry_doc["coordinates"][k];
                    line.push_back({ coord_doc[0], coord_doc[1], coord_doc[2] });
                }
                line_list.emplace_back(line);
                line_property_list.emplace_back(feature_doc["properties"]);
            } else if (geometry_doc["type"] == "Polygon") {
                for (size_t k = 0; k < geometry_doc["coordinates"].size(); ++k) {
                    std::vector<Point3> polygon;
                    json &coord_doc = geometry_doc["coordinates"][k];
                    for (size_t j = 1; j < coord_doc.size(); ++j) {
                        json &sub_coord_doc = coord_doc[j - 1];
                        polygon.push_back({ sub_coord_doc[0], sub_coord_doc[1], sub_coord_doc[2] });
                    }
                    polygon_list.emplace_back(polygon);
                }
                // polygon_property_list.emplace_back(feature_doc["properties"]);
            } else if (geometry_doc["type"] == "Point") {
                json &coord_doc = geometry_doc["coordinates"];
                point_list.push_back({coord_doc[0], coord_doc[1], coord_doc[2]});
                // TODO: remove it!
                // feature_doc["HEIGHT"] = 8;
                point_property_list.emplace_back(feature_doc["properties"]);
            }
        }
        return 0;
    }

    int read_from_file(const std::string &path) {
        line_list.clear();
        line_property_list.clear();
        std::ifstream fs(path);
        if (!fs) {
            return -1;
        }
        return read_from_stream(fs);
    }

    // void convert_from_wgs84_to_ecef() {
    //     for (auto &item : line_list) {
    //         double ex, ey, ez;
    //         wgs84_to_ecef
    //     }
    // }

};
}

#endif