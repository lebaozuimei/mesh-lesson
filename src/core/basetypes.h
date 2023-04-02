#ifndef  __BASETYPES_H_
#define  __BASETYPES_H_

typedef bool bool_t;

#define UNUSED(x) (void)(x)

typedef std::vector<char> Binary;
typedef float PositionType;
typedef float TexcoordType;
typedef float NormalType;
typedef int IndexType;

enum CoordType {
    COORD_ECEF      = 0,
    COORD_MERCATOR  = 1,
    COORD_UTM       = 2
};

struct gltf_const {
    const static int TARGET_NONE                    = 0;
    const static int TARGET_VERTEX_ATTRIBUTES       = 34962;
    const static int TARGET_VERTEX_INDICES          = 34963;

    const static int COMPONENT_TYPE_FLOAT           = 5126;
    const static int COMPONENT_TYPE_UNSIGNED_SHORT  = 5123;
    const static int COMPONENT_TYPE_UNSIGNED_INT    = 5125;

    const static int PRIMITIVE_MODE_POINTS          = 0;
    const static int PRIMITIVE_MODE_LINES           = 1;
    const static int PRIMITIVE_MODE_LINE_LOOP       = 2;
    const static int PRIMITIVE_MODE_LINE_STRIP      = 3;
    const static int PRIMITIVE_MODE_TRIANGLES       = 4;
    const static int PRIMITIVE_MODE_TRIANGLE_STRIP  = 5;
    const static int PRIMITIVE_MODE_TRIANGLE_FAN    = 6;
    
    enum { TYPE_VEC2, TYPE_VEC3, TYPE_SCALAR } type_enum;

    static int get_component_byte(int component_type) {
        if (component_type == COMPONENT_TYPE_FLOAT) {
            return 4;
        } else if (component_type == COMPONENT_TYPE_UNSIGNED_INT) {
            return 4;
        } else if (component_type == COMPONENT_TYPE_UNSIGNED_SHORT) {
            return 2;
        } else {
            std::cerr << "unknown component type: " << component_type << std::endl;
        }
        return 0;
    }

    static int get_component_count(int type) {
        if (type == TYPE_VEC3) {
            return 3;
        } else if (type == TYPE_VEC2) {
            return 2;
        } else if (type == TYPE_SCALAR) {
            return 1;
        } else {
            std::cerr << "unknown component type: " << type << std::endl;
        }
        return 0;
    }
};

template <typename T>
char* raw_ptr(T &vec) {
    return reinterpret_cast<char *>(&vec[0]);
}

template <typename T>
const char* raw_ptr(const T &vec) {
    return reinterpret_cast<const char *>(&vec[0]);
}

template <typename T>
int byte_length(const std::vector<T> &vec) {
    return sizeof(T) * vec.size();
}

uint32_t padding(uint32_t v, uint32_t pad) {
    if (v % pad == 0) {
        return v;
    }
    return (v / pad + 1) * pad;
}

uint32_t padding4(uint32_t v) {
    if (v % 4 == 0) {
        return v;
    }
    return (v / 4 + 1) * 4;
}

uint32_t padding8(uint32_t v) {
    if (v % 8 == 0) {
        return v;
    }
    return (v / 8 + 1) * 8;
}

uint32_t pad_0_align_4(Binary &buf) {
    uint32_t padded = padding4(buf.size()) - buf.size();
    if (padded > 0) {
        buf.insert(buf.end(), padded, 0);
    }
    return padded;
}

uint32_t pad_space_align_8(uint32_t offset, std::string &str) {
    uint32_t padded = padding8(offset + str.size()) - str.size() - offset;
    for (uint32_t i = 0; i < padded; ++i) {
        str.push_back(0x20);
    }
    return padded;
}

template <typename T>
void append_buffer_data(Binary &buf, const std::vector<T> &data) {
    if (data.size() > 0) {
        buf.insert(buf.end(), raw_ptr(data), raw_ptr(data) + byte_length(data));
        pad_0_align_4(buf);
    }
}

#endif