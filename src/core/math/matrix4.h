#ifndef  __MATH_MATRIX4_H_
#define  __MATH_MATRIX4_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

namespace mesh {

using json = nlohmann::json;

// column-major double-precision matrix
class Matrix4 {

public:

    double data[16];

public:

    Matrix4() {
        for (int i = 0; i < 16; ++i) {
            data[i] = 0.0f;
        }
        data[0] = 1.0f;
        data[5] = 1.0f;
        data[10] = 1.0f;
        data[15] = 1.0f;
    }

    Matrix4(double v) {
        clear();
        data[0] = v;
        data[5] = v;
        data[10] = v;
        data[15] = v;
    }

    Matrix4(
        double v0, double v1, double v2, double v3,
        double v4, double v5, double v6, double v7,
        double v8, double v9, double v10, double v11,
        double v12, double v13, double v14, double v15) {
        data[0] = v0;
        data[1] = v1;
        data[2] = v2;
        data[3] = v3;
        data[4] = v4;
        data[5] = v5;
        data[6] = v6;
        data[7] = v7;
        data[8] = v8;
        data[9] = v9;
        data[10] = v10;
        data[11] = v11;
        data[12] = v12;
        data[13] = v13;
        data[14] = v14;
        data[15] = v15;
    }

    Matrix4(double a[16]) {
        for (size_t i = 0; i < 16; ++i) {
            data[i] = a[i];
        }
    }

    Matrix4(const json &doc) {
        if (doc.size() == 16) {
            for (size_t i = 0; i < 16; ++i) {
                data[i] = doc[i];
            }
        }
    }

    double operator[](size_t i) const {
        return data[i];
    }

    double& operator[](size_t i) {
        return data[i];
    }

    void set_with_row_major(
        double v0, double v1, double v2, double v3,
        double v4, double v5, double v6, double v7,
        double v8, double v9, double v10, double v11,
        double v12, double v13, double v14, double v15) {
        data[0] = v0;
        data[1] = v4;
        data[2] = v8;
        data[3] = v12;
        data[4] = v1;
        data[5] = v5;
        data[6] = v9;
        data[7] = v13;
        data[8] = v2;
        data[9] = v6;
        data[10] = v10;
        data[11] = v14;
        data[12] = v3;
        data[13] = v7;
        data[14] = v11;
        data[15] = v15;
    }

    Matrix4 inverse() {
        double src0 = data[0];
        double src1 = data[4];
        double src2 = data[8];
        double src3 = data[12];
        double src4 = data[1];
        double src5 = data[5];
        double src6 = data[9];
        double src7 = data[13];
        double src8 = data[2];
        double src9 = data[6];
        double src10 = data[10];
        double src11 = data[14];
        double src12 = data[3];
        double src13 = data[7];
        double src14 = data[11];
        double src15 = data[15];

        // calculate pairs for first 8 elements (cofactors)
        double tmp0 = src10 * src15;
        double tmp1 = src11 * src14;
        double tmp2 = src9 * src15;
        double tmp3 = src11 * src13;
        double tmp4 = src9 * src14;
        double tmp5 = src10 * src13;
        double tmp6 = src8 * src15;
        double tmp7 = src11 * src12;
        double tmp8 = src8 * src14;
        double tmp9 = src10 * src12;
        double tmp10 = src8 * src13;
        double tmp11 = src9 * src12;

        // calculate first 8 elements (cofactors)
        double dst0 = tmp0 * src5 + tmp3 * src6 + tmp4 * src7 - (tmp1 * src5 + tmp2 * src6 + tmp5 * src7);
        double dst1 = tmp1 * src4 + tmp6 * src6 + tmp9 * src7 - (tmp0 * src4 + tmp7 * src6 + tmp8 * src7);
        double dst2 = tmp2 * src4 + tmp7 * src5 + tmp10 * src7 - (tmp3 * src4 + tmp6 * src5 + tmp11 * src7); 
        double dst3 = tmp5 * src4 + tmp8 * src5 + tmp11 * src6 - (tmp4 * src4 + tmp9 * src5 + tmp10 * src6); 
        double dst4 = tmp1 * src1 + tmp2 * src2 + tmp5 * src3 - (tmp0 * src1 + tmp3 * src2 + tmp4 * src3); 
        double dst5 = tmp0 * src0 + tmp7 * src2 + tmp8 * src3 - (tmp1 * src0 + tmp6 * src2 + tmp9 * src3); 
        double dst6 = tmp3 * src0 + tmp6 * src1 + tmp11 * src3 - (tmp2 * src0 + tmp7 * src1 + tmp10 * src3); 
        double dst7 = tmp4 * src0 + tmp9 * src1 + tmp10 * src2 - (tmp5 * src0 + tmp8 * src1 + tmp11 * src2);

        // calculate pairs for second 8 elements (cofactors)
        tmp0 = src2 * src7;
        tmp1 = src3 * src6;
        tmp2 = src1 * src7;
        tmp3 = src3 * src5;
        tmp4 = src1 * src6;
        tmp5 = src2 * src5;
        tmp6 = src0 * src7;
        tmp7 = src3 * src4;
        tmp8 = src0 * src6;
        tmp9 = src2 * src4;
        tmp10 = src0 * src5;
        tmp11 = src1 * src4;

        // calculate second 8 elements (cofactors)
        double dst8 = tmp0 * src13 + tmp3 * src14 + tmp4 * src15 - (tmp1 * src13 + tmp2 * src14 + tmp5 * src15); 
        double dst9 = tmp1 * src12 + tmp6 * src14 + tmp9 * src15 - (tmp0 * src12 + tmp7 * src14 + tmp8 * src15); 
        double dst10 = tmp2 * src12 + tmp7 * src13 + tmp10 * src15 - (tmp3 * src12 + tmp6 * src13 + tmp11 * src15); 
        double dst11 = tmp5 * src12 + tmp8 * src13 + tmp11 * src14 - (tmp4 * src12 + tmp9 * src13 + tmp10 * src14); 
        double dst12 = tmp2 * src10 + tmp5 * src11 + tmp1 * src9 - (tmp4 * src11 + tmp0 * src9 + tmp3 * src10); 
        double dst13 = tmp8 * src11 + tmp0 * src8 + tmp7 * src10 - (tmp6 * src10 + tmp9 * src11 + tmp1 * src8); 
        double dst14 = tmp6 * src9 + tmp11 * src11 + tmp3 * src8 - (tmp10 * src11 + tmp2 * src8 + tmp7 * src9); 
        double dst15 = tmp10 * src10 + tmp4 * src8 + tmp9 * src9 - (tmp8 * src9 + tmp11 * src10 + tmp5 * src8);

        // calculate determinant
        double det = src0 * dst0 + src1 * dst1 + src2 * dst2 + src3 * dst3;

        if (std::abs(det) < 1e-6) {
            // Special case for a zero scale matrix that can occur, for example,
            // when a model's node has a [0, 0, 0] scale.
            return Matrix4(0.0f);
        }

        // calculate matrix inverse
        det = 1.0 / det;

        return Matrix4(
            dst0 * det, dst1 * det, dst2 * det, dst3 * det,
            dst4 * det, dst5 * det, dst6 * det, dst7 * det,
            dst8 * det, dst9 * det, dst10 * det, dst11 * det,
            dst12 * det, dst13 * det, dst14 * det, dst15 * det
        );
    }

    void clear() {
        for (int i = 0; i < 16; ++i) {
            data[i] = 0.0f;
        }
    }

    void fill_to(double a[16]) const {
        for (size_t i = 0; i < 16; ++i) {
            a[i] = data[i];
        }
    }

    void set_to(double a[16]) const {
        for (size_t i = 0; i < 16; ++i) {
            a[i] = data[i];
        }
    }

    void append_rotation_x(double radius) {
        Matrix4 m;
        m.set_with_row_major(
            1, 0, 0, 0,
            0, std::cos(radius), 0 - std::sin(radius), 0,
            0, std::sin(radius), std::cos(radius), 0,
            0, 0, 0, 1
        );
        *this = m * *this;
    }

    void append_rotation_y(double radius) {
        Matrix4 m;
        m.set_with_row_major(
            std::cos(radius), 0, std::sin(radius), 0,
            0, 1, 0, 0,
            0 - std::sin(radius), 0, std::cos(radius), 0,
            0, 0, 0, 1
        );
        *this = m * *this;
    }

    void append_rotation_z(double radius) {
        Matrix4 m;
        m.set_with_row_major(
            std::cos(radius), 0 - std::sin(radius), 0, 0,
            std::sin(radius), std::cos(radius), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
        *this = m * *this;
    }

    void debug(const std::string &prefix) const {
        std::cout << "------ " << prefix << " matrix4 (row-major): " << std::endl;
        std::cout << data[0] << "," << data[4] << "," << data[8] << "," << data[12] << std::endl;
        std::cout << data[1] << "," << data[5] << "," << data[9] << "," << data[13] << std::endl;
        std::cout << data[2] << "," << data[6] << "," << data[10] << "," << data[14] << std::endl;
        std::cout << data[3] << "," << data[7] << "," << data[11] << "," << data[15] << std::endl;
        std::cout << "-----------------" << std::endl;
    }

    Matrix4& operator=(const Matrix4 &mat) {
        if (this == &mat) {
            return *this;
        }

        for (int i = 0; i < 16; ++i) {
            data[i] = mat.data[i];
        }
        return *this;
    }

    friend Matrix4 operator+(const Matrix4 &a, const Matrix4 &b) {
        Matrix4 mat;
        for (int i = 0; i < 16; ++i) {
            mat.data[i] = a.data[i] + b.makedata[i];
        }
        return mat;
    }

    Matrix4 operator*(const Matrix4& mat) const {
        return Matrix4(
            // first column
            data[0] * mat.data[0] + data[4] * mat.data[1] + data[8] * mat.data[2] + data[12] * mat.data[3],
            data[1] * mat.data[0] + data[5] * mat.data[1] + data[9] * mat.data[2] + data[13] * mat.data[3],
            data[2] * mat.data[0] + data[6] * mat.data[1] + data[10] * mat.data[2] + data[14] * mat.data[3],
            data[3] * mat.data[0] + data[7] * mat.data[1] + data[11] * mat.data[2] + data[15] * mat.data[3],
            // second column
            data[0] * mat.data[4] + data[4] * mat.data[5] + data[8] * mat.data[6] + data[12] * mat.data[7],
            data[1] * mat.data[4] + data[5] * mat.data[5] + data[9] * mat.data[6] + data[13] * mat.data[7],
            data[2] * mat.data[4] + data[6] * mat.data[5] + data[10] * mat.data[6] + data[14] * mat.data[7],
            data[3] * mat.data[4] + data[7] * mat.data[5] + data[11] * mat.data[6] + data[15] * mat.data[7],
            // third column
            data[0] * mat.data[8] + data[4] * mat.data[9] + data[8] * mat.data[10] + data[12] * mat.data[11],
            data[1] * mat.data[8] + data[5] * mat.data[9] + data[9] * mat.data[10] + data[13] * mat.data[11],
            data[2] * mat.data[8] + data[6] * mat.data[9] + data[10] * mat.data[10] + data[14] * mat.data[11],
            data[3] * mat.data[8] + data[7] * mat.data[9] + data[11] * mat.data[10] + data[15] * mat.data[11],
            // fourth column
            data[0] * mat.data[12] + data[4] * mat.data[13] + data[8] * mat.data[14] + data[12] * mat.data[15],
            data[1] * mat.data[12] + data[5] * mat.data[13] + data[9] * mat.data[14] + data[13] * mat.data[15],
            data[2] * mat.data[12] + data[6] * mat.data[13] + data[10] * mat.data[14] + data[14] * mat.data[15],
            data[3] * mat.data[12] + data[7] * mat.data[13] + data[11] * mat.data[14] + data[15] * mat.data[15]
        );
    }

    void operator+=(const Matrix4 &mat) {
        for (int i = 0; i < 16; ++i) {
            data[i] += mat.data[i];
        }
    }

    void set(int i, int j, double x) {
        data[i * 4 + j] = x;
    }

    double get(int i, int j) const {
        return data[i * 4 + j];
    }

};
}

#endif