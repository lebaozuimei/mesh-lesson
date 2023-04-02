#ifndef  __GEO_UTIL_H_
#define  __GEO_UTIL_H_

#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <assert.h>

#include "../core/basetypes.h"
#include "../core/math/point3.h"

void enu_to_ecef(
    double east, double north, double up, 
    double lon, double lat, double h,
    double &ret_x, double &ret_y, double &ret_z) {

    double a = 6378137;
    double b = 6356752.3142;
    double f = (a - b) / a;
    double e_sq = f * (2 - f);

    double lamb = lat * M_PI / 180.0;
    double phi = lon * M_PI / 180.0;
    double s = std::sin(lamb);
    double N = a / std::sqrt(1 - e_sq * s * s);
 
    double sin_lambda = std::sin(lamb);
    double cos_lambda = std::cos(lamb);
    double sin_phi = std::sin(phi);
    double cos_phi = std::cos(phi);
 
    double x0 = (h + N) * cos_lambda * cos_phi;
    double y0 = (h + N) * cos_lambda * sin_phi;
    double z0 = (h + (1 - e_sq) * N) * sin_lambda;
 
    double t = cos_lambda * up - sin_lambda * north;
 
    double zd = sin_lambda * up + cos_lambda * north;
    double xd = cos_phi * t - sin_phi * east;
    double yd = sin_phi * t + cos_phi * east;
 
    ret_x = xd + x0;
    ret_y = yd + y0;
    ret_z = zd + z0;
}

mesh::Point3 enu_to_ecef(const mesh::Point3 &local, const mesh::Point3 &global) {
    double ret_x, ret_y, ret_z;
    enu_to_ecef(local.x, local.y, local.z, global.x, global.y, global.z, ret_x, ret_y, ret_z);
    return mesh::Point3(ret_x, ret_y, ret_z);
}

void wgs84_to_ecef(double lon, double lat, double h, 
    double &ret_x, double &ret_y, double &ret_z) {
    double a = 6378137;
    double b = 6356752.314245; // b=(1-f)
    double e = (a * a - b * b) / (a * a);
    double cos_lat = cos(lat * M_PI / 180.0f);
    double sin_lat = sin(lat * M_PI / 180.0f);
    double cos_lon = cos(lon * M_PI / 180.0f);
    double sin_lon = sin(lon * M_PI / 180.0f);

    double n = a / (sqrt(1 - e * sin_lat * sin_lat));
    double nh = n + h;
    ret_x = nh * cos_lat * cos_lon;
    ret_y = nh * cos_lat * sin_lon;
    ret_z = (b * b * n / (a * a) + h) * sin_lat;
}

mesh::Point3 wgs84_to_ecef(const mesh::Point3 &p) {
    double ret_x, ret_y, ret_z;
    wgs84_to_ecef(p.x, p.y, p.z, ret_x, ret_y, ret_z);
    return mesh::Point3(ret_x, ret_y, ret_z);
}

void ecef_to_wgs84(double x, double y, double z,
    double &ret_lon, double &ret_lat, double &ret_h) { 
    double a = 6378137.0;
    double b = 6356752.31424518;
    // double f = 1.0 / (double)298.257223563;
    double e = sqrtl((a * a - b * b) / (a * a));
    double e1 = sqrtl((a * a - b * b) / (b * b));
    double p = sqrtl(x * x + y * y);
    double q = atan2l(z * a, p * b);
    double Longitude = atan2l(y, x);
    double Latitude = atan2l((z + (e1 * e1) * b * powl(sinl(q), 3)), (p - (e * e) * a * powl(cosl(q), 3)));
    double N = a / sqrtl(1 - ((e * e) * powl(sinl(Latitude), 2)));

    ret_h = (p / cosl(Latitude)) - N;
    ret_lon = Longitude * 180.0 / M_PI;
    ret_lat = Latitude * 180.0 / M_PI;
}

mesh::Point3 ecef_to_wgs84(const mesh::Point3 &p) {
    double ret_lon, ret_lat, ret_h;
    ecef_to_wgs84(p.x, p.y, p.z, ret_lon, ret_lat, ret_h);
    return mesh::Point3(ret_lon, ret_lat, ret_h);
}

void wgs84_to_mercator(double lon, double lat, double &ret_x, double &ret_y) {
    ret_x = lon * 20037508.34 / 180.0;  
    ret_y = logl(tanl((90 + lat) * M_PI / 360.0)) / (M_PI / 180.0) * 20037508.34 / 180; 
}

mesh::Point3 wgs84_to_mercator(const mesh::Point3 &p) {
    return mesh::Point3(
        p.x * 20037508.34 / 180.0,
        logl(tanl((90 + p.y) * M_PI / 360.0)) / (M_PI / 180.0) * 20037508.34 / 180,
        p.z
    );
}

void ecef_to_mercator(double x, double y, double z, double &ret_x, double &ret_y, double &ret_h) {
    double lon, lat;
    ecef_to_wgs84(x, y, z, lon, lat, ret_h);
    wgs84_to_mercator(lon, lat, ret_x, ret_y);
}

mesh::Point3 ecef_to_mercator(const mesh::Point3 &p) {
    double lon, lat;
    double ret_x, ret_y, ret_h;
    ecef_to_wgs84(p.x, p.y, p.z, lon, lat, ret_h);
    wgs84_to_mercator(lon, lat, ret_x, ret_y);
    return mesh::Point3(ret_x, ret_y, ret_h);
}

void mercator_to_wgs84(double x, double y, double &ret_lon, double &ret_lat) {  
    x = x / 20037508.34 * 180;  
    y = y / 20037508.34 * 180;  
    y = 180 / M_PI * (2 * atan(exp(y * M_PI / 180)) - M_PI / 2);  
    ret_lon = x;  
    ret_lat = y;  
}

mesh::Point3 mercator_to_wgs84(const mesh::Point3 &p) {  
    double x = p.x / 20037508.34 * 180;  
    double y = p.y / 20037508.34 * 180;  
    y = 180 / M_PI * (2 * atan(exp(y * M_PI / 180)) - M_PI / 2);
    return mesh::Point3(x, y, p.z);
}

mesh::Point3 enu_to_mercator(const mesh::Point3 &local, const mesh::Point3 &global) {
    mesh::Point3 ecef = enu_to_ecef(local, global);
    return ecef_to_mercator(ecef);
}

double math_sinh(double x) {
    return (std::exp(x) - std::exp(-x)) / 2;
}

void tile_to_wgs84(int tx, int ty, int level, double &ret_lon, double &ret_lat) { 
    ret_lon = (double)tx / (1 << level) * 360 - 180;
    ret_lat = std::atan(math_sinh(M_PI * (1 - 2 * (double)ty / (1 << level)))) * 180.0 / M_PI;
}

// void pixel_to_wgs84(int tx, int px, int ty, int py, int level, double &ret_lon, double &ret_lat) { 
//     ret_lon = ((double)tx + (double)px / 256) / (1 << level) * 360 - 180;
//     ret_lat = std::atan(math_sinh(M_PI * (1 - 2 * ((double)ty + (double)py / 256) / (1 << level)))) * 180.0 / M_PI;
// }

void pixel_to_wgs84(int tx, double px, int ty, double py, int level, double &ret_lon, double &ret_lat) { 
    ret_lon = (tx + px / 256) / (1 << level) * 360 - 180;
    ret_lat = std::atan(math_sinh(M_PI * (1 - 2 * (ty + py / 256) / (1 << level)))) * 180.0 / M_PI;
}

// void pixel_to_mercator(int tx, int px, int ty, int py, int level, double &ret_x, double &ret_y) {
//     double lon = ((double)tx + (double)px / 256) / (1 << level) * 360 - 180;
//     double lat = std::atan(math_sinh(M_PI * (1 - 2 * ((double)ty + (double)py / 256) / (1 << level)))) * 180.0 / M_PI;
//     wgs84_to_mercator(lon, lat, ret_x, ret_y);
// }

void pixel_to_mercator(int tx, double px, int ty, double py, int level, double &ret_x, double &ret_y) {
    double lon = (tx + px / 256) / (1 << level) * 360 - 180;
    double lat = std::atan(math_sinh(M_PI * (1 - 2 * (ty + py / 256) / (1 << level)))) * 180.0 / M_PI;
    wgs84_to_mercator(lon, lat, ret_x, ret_y);
}

void pixel_to_ecef(int tx, double px, int ty, double py, int level, double h, double &ret_x, double &ret_y, double &ret_z) {
    double lon = (tx + px / 256) / (1 << level) * 360 - 180;
    double lat = std::atan(math_sinh(M_PI * (1 - 2 * (ty + py / 256) / (1 << level)))) * 180.0 / M_PI;
    wgs84_to_ecef(lon, lat, h, ret_x, ret_y, ret_z);
}

// void mercator_to_pixel(double mx, double my, int tx, int ty, int level, double &ret_x, double &ret_y) {
void mercator_to_pixel(double mx, double my, int tx, int ty, int level, double &ret_x, double &ret_y) {
    double lon, lat;
    mercator_to_wgs84(mx, my, lon, lat);
    // DEBUG2P(lon, lat);
    double s = std::sin(lat * M_PI / 180);
    // ret_x = (lon + 180) / 360 * (1 << level) * 256 % 256;
    // ret_y = (0.5 - std::log((1 + s) / (1 - s)) / (4 * M_PI)) * (1 << level) * 256 % 256;

    // double x = (lon + 180) / 360 * (1 << level);
    // double y = (0.5 - std::log((1 + s) / (1 - s)) / (4 * M_PI)) * (1 << level);
    // ret_x = (x - (int)x) * 256.0f;
    // ret_y = (y - (int)y) * 256.0f;

    ret_x = ((lon + 180) / 360 * (1 << level) - tx) * 256;
    ret_y = ((0.5 - std::log((1 + s) / (1 - s)) / (4 * M_PI)) * (1 << level) - ty) * 256;
}

void wgs84_to_tile(double lon, double lat, int level, int &ret_x, int &ret_y) {
    double s = std::sin(lat * M_PI / 180);
    ret_x = int((lon + 180) / 360 * (1 << level));
    ret_y = int((0.5 - std::log((1 + s) / (1 - s)) / (4 * M_PI)) * (1 << level));
}

void wgs84_to_tile_and_pixel(double lon, double lat, int level, 
    int &ret_tx, int &ret_ty, double &ret_px, double &ret_py) {
    double s = std::sin(lat * M_PI / 180);
    double tmp_x = (lon + 180) / 360 * (1 << level);
    double tmp_y = (0.5 - std::log((1 + s) / (1 - s)) / (4 * M_PI)) * (1 << level);
    ret_tx = int(tmp_x);
    ret_ty = int(tmp_y);
    ret_px = (tmp_x - ret_tx) * 256;
    ret_py = (tmp_y - ret_ty) * 256;
}

void mercator_to_tile(double mx, double my, int level, int &ret_x, int &ret_y) {
    double lon, lat;
    mercator_to_wgs84(mx, my, lon, lat);
    wgs84_to_tile(lon, lat, level, ret_x, ret_y);
}

#endif