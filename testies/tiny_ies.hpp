/*
MIT License

Copyright(c) 2021 - 2024 Lukas Lipp

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cstdint>
#include <stdio.h>
#include <iostream>

// 实现meshgrid功能，返回两个矩阵X和Y
template<typename T>
void meshgrid(const std::vector<T>& x, const std::vector<T>& y,
    std::vector<std::vector<T>>& X, std::vector<std::vector<T>>& Y,
    bool indexing = true) {
    size_t nx = x.size();
    size_t ny = y.size();

    // 初始化输出矩阵
    X.resize(ny, std::vector<T>(nx));
    Y.resize(ny, std::vector<T>(nx));

    if (indexing) {
        // 'xy' 索引：X的行是x的复制，Y的列是y的复制
        for (size_t i = 0; i < ny; ++i) {
            for (size_t j = 0; j < nx; ++j) {
                X[i][j] = x[j];
                Y[i][j] = y[i];
            }
        }
    }
    else {
        // 'ij' 索引：X的列是x的复制，Y的行是y的复制
        for (size_t i = 0; i < ny; ++i) {
            for (size_t j = 0; j < nx; ++j) {
                X[i][j] = x[i];
                Y[i][j] = y[j];
            }
        }
    }
}
// 基本版本：处理嵌套的vector
template<typename T>
std::vector<T> flatten(const std::vector<std::vector<T>>& nested) {
    std::vector<T> result;
    for (const auto& inner : nested) {
        result.insert(result.end(), inner.begin(), inner.end());
    }
    return result;
}
// 重载版本，只返回X和Y矩阵
template<typename T>
std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>>
meshgrid(const std::vector<T>& x, const std::vector<T>& y, bool indexing = true) {
    std::vector<std::vector<T>> X, Y;
    meshgrid(x, y, X, Y, indexing);
    return { X, Y };
}

// 实现NumPy的searchsorted功能
template<typename T>
std::vector<T> searchsorted(const std::vector<T>& arr,
    const std::vector<T>& values,
    const std::string& side = "left") {
    std::vector<T> indices;
    indices.reserve(values.size());

    if (side == "left") {
        for (const auto& value : values) {
            auto it = std::lower_bound(arr.begin(), arr.end(), value);
            indices.push_back(std::distance(arr.begin(), it));
        }
    }
    else {
        for (const auto& value : values) {
            auto it = std::upper_bound(arr.begin(), arr.end(), value);
            indices.push_back(std::distance(arr.begin(), it));
        }
    }

    return indices;
}
template<typename T>
std::vector<std::vector<T>> reshape_2d(const std::vector<T>& data,
    size_t rows, size_t cols) {
    if (data.size() != rows * cols) {
        throw std::invalid_argument("Total size must equal rows * cols");
    }

    std::vector<std::vector<T>> result(rows, std::vector<T>(cols));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = data[i * cols + j];
        }
    }

    return result;
}
//// 模拟Python的np.mod功能，将角度归一化到[0, 360)范围
//std::vector<double> mod_360(const std::vector<double>& angles) {
//	std::vector<double> result;
//	result.reserve(angles.size());
//
//	for (double angle : angles) {
//		double normalized = std::fmod(angle, 360.0);
//		if (normalized < 0) normalized += 360.0;
//		result.push_back(normalized);
//	}
//
//	return result;
//}

// 模拟Python的np.clip功能
template<typename T>
std::vector<T> clip_indices(const std::vector<T>& indices,
    size_t min_val, size_t max_val) {
    std::vector<T> result;
    result.reserve(indices.size());

    for (size_t idx : indices) {
        if (idx < min_val) {
            result.push_back(min_val);
        }
        else if (idx > max_val) {
            result.push_back(max_val);
        }
        else {
            result.push_back(idx);
        }
    }

    return result;
}




template <typename T>
struct tiny_ies {
    enum EIESType
    {
        eC0=0,
        eC90,
        eC180,
        eC360,
        eNotSupoort,
    };
    static_assert(std::is_floating_point<T>::value, "T must be floating point");
    struct light {
        light() :
            lamp_to_luminaire_geometry{},
            number_of_tilt_angles{},
            number_lights{},
            lumens_per_lamp{},
            multiplier{},
            number_vertical_angles{},
            number_horizontal_angles{},
            photometric_type{}, units_type{},
            width{}, length{}, height{},
            ballast_factor{},
            future_use{},
            input_watts{},
            min_vertical_angle{},
            max_vertical_angle{},
            min_horizontal_angle{},
            max_horizontal_angle{},
            max_candela{}, m_IESType{},
            header{}
        {
        }

        /*
        ** HEADER
        */
        std::string ies_version;
        std::map<std::string, std::string> properties;
        std::string tilt;

        /*
        ** DATA
        */
        /* tilt data */
        uint32_t lamp_to_luminaire_geometry;
        uint32_t number_of_tilt_angles;
        std::vector<T> tilt_angles;
        std::vector<T> tilt_multiplying_factors;

        /* data */
        uint32_t number_lights;
        int lumens_per_lamp; // in case of absolute photometry the value is -1
        T multiplier;
        uint32_t number_vertical_angles;
        uint32_t number_horizontal_angles;
        uint32_t photometric_type;
        uint32_t units_type;
        T width;
        T length;
        T height;

        T ballast_factor;
        T future_use;
        T input_watts;
        EIESType m_IESType;
        std::vector<T> vertical_angles;
        T min_vertical_angle;
        T max_vertical_angle;
        std::vector<T> horizontal_angles;
        T min_horizontal_angle;
        T max_horizontal_angle;

        std::vector<T> candela;
        std::vector<std::vector<T>> candela_hv;
        T max_candela;
        std::string header;
    };

    static bool load_ies(const std::string& file, std::string& err_out, std::string& warn_out, light& ies_out) {
        std::ifstream f(file);
        if (!f) {
            err_out = "Failed reading file: " + file;
            return false;
        }

        ies_out = {};
        std::string line;
        /*
        ** HEADER
        */
        // the first line in a valid ies file should be IESNA:
        if (f.good()) {
            std::getline(f, line);
            if (!read_property("IESNA", line, ies_out.ies_version)) {
                warn_out = "First line did not start with IESNA " + file;
            }
        }
    
        // read properties
        while (f.good() && std::getline(f, line)) {

            if (read_property("TILT=", line, ies_out.tilt))
            {        
                break;
            }
            ies_out.header += line;
            ies_out.header += "\n";
            read_property(line, ies_out.properties);
        }
        if (ies_out.tilt.empty()) {
            err_out = "TILT propertie not found: " + file;
            return false;
        }

        /*
        ** DATA
        */
        // replace comma with empty space (just in case)
        std::ostringstream oss;
        oss << f.rdbuf();
        std::string s = oss.str();
        std::replace(s.begin(), s.end(), ',', ' ');

        // go through the data
        std::stringstream stream;
        stream.str(s);

#define NEXT_VALUE(name, var) if (!store_value(stream, var)) { err_out = "Error reading <" name "> property: " + file; f.close(); return false; }

        // <lamp to luminaire geometry> <#tilt angles> <angles> <multiplying factors>
        if (ies_out.tilt == "INCLUDE") {
            NEXT_VALUE("lamp to luminaire geometry", ies_out.lamp_to_luminaire_geometry)
            NEXT_VALUE("#tilt angles", ies_out.number_of_tilt_angles)

            ies_out.tilt_angles.resize(ies_out.number_of_tilt_angles);
            ies_out.tilt_multiplying_factors.resize(ies_out.number_of_tilt_angles);
            for (uint32_t i = 0; i < ies_out.number_of_tilt_angles; i++) {
                NEXT_VALUE("angles", ies_out.tilt_angles[i])
            }
            for (uint32_t i = 0; i < ies_out.number_of_tilt_angles; i++) {
                NEXT_VALUE("angles", ies_out.tilt_multiplying_factors[i])
            }
        }

        // <#lamps> <lumen/lamp> <multiplier> <#vertical angles> <#horizontal angles> <photometric type> <units type> <width> <length> <height>
        NEXT_VALUE("#lamps", ies_out.number_lights)
        NEXT_VALUE("lumens/lamp", ies_out.lumens_per_lamp)
        NEXT_VALUE("multiplier", ies_out.multiplier)
        NEXT_VALUE("#vertical angles", ies_out.number_vertical_angles)
        NEXT_VALUE("#horizontal angles", ies_out.number_horizontal_angles)
        NEXT_VALUE("photometric type", ies_out.photometric_type)
        NEXT_VALUE("units type", ies_out.units_type)
        NEXT_VALUE("width", ies_out.width)
        NEXT_VALUE("length", ies_out.length)
        NEXT_VALUE("height", ies_out.height)
        // <ballast factor> <future use> <input watts>
        NEXT_VALUE("ballast factor", ies_out.ballast_factor)
        NEXT_VALUE("future use", ies_out.future_use)
        NEXT_VALUE("input watts", ies_out.input_watts)
        // <vertical angles>
        ies_out.min_vertical_angle = std::numeric_limits<T>::max();
        ies_out.max_vertical_angle = -std::numeric_limits<T>::max();
        ies_out.vertical_angles.resize(ies_out.number_vertical_angles);
        for (uint32_t i = 0; i < ies_out.number_vertical_angles; i++) {
            NEXT_VALUE("vertical angles", ies_out.vertical_angles[i])
        	ies_out.min_vertical_angle = std::min(ies_out.min_vertical_angle, ies_out.vertical_angles[i]);
            ies_out.max_vertical_angle = std::max(ies_out.max_vertical_angle, ies_out.vertical_angles[i]);
        }
        // <horizontal angles>
        ies_out.min_horizontal_angle = std::numeric_limits<T>::max();
        ies_out.max_horizontal_angle = -std::numeric_limits<T>::max();
        ies_out.horizontal_angles.resize(ies_out.number_horizontal_angles);
        for (uint32_t i = 0; i < ies_out.number_horizontal_angles; i++) {
            NEXT_VALUE("horizontal angles", ies_out.horizontal_angles[i])
        	ies_out.min_horizontal_angle = std::min(ies_out.min_horizontal_angle, ies_out.horizontal_angles[i]);
            ies_out.max_horizontal_angle = std::max(ies_out.max_horizontal_angle, ies_out.horizontal_angles[i]);
        }
        // <candela values for all vertical angles at first horizontal angle>
        //                                              :
        // <candela values for all vertical angles at last horizontal angle>
        ies_out.candela_hv.resize(static_cast<uint64_t>(ies_out.number_horizontal_angles));
        ies_out.candela.resize(static_cast<uint64_t>(ies_out.number_vertical_angles) * static_cast<uint64_t>(ies_out.number_horizontal_angles));
        ies_out.max_candela = 0.0;
        for (uint32_t i = 0; i < ies_out.number_vertical_angles * ies_out.number_horizontal_angles; i++) {
            NEXT_VALUE("candela values", ies_out.candela[i])
        	ies_out.max_candela = std::max(ies_out.max_candela, ies_out.candela[i]);              
        }
        for (uint32_t i = 0; i < ies_out.number_horizontal_angles; i++) {
            std::vector<T> h;
            for (uint32_t j = 0; j < ies_out.number_vertical_angles; j++)
            {
                h.push_back(ies_out.candela[i * ies_out.number_vertical_angles + j] * ies_out.multiplier);
            }
            ies_out.candela_hv[i] = h;
        }

#undef NEXT_VALUE
        f.close();

        switch (ies_out.photometric_type)
        {
        case 1:
        {
            if (ies_out.horizontal_angles.size() > 0)
            {
                int maxAngle = ies_out.horizontal_angles[ies_out.horizontal_angles.size() - 1];
                switch (maxAngle)
                {
                case 0:
                    ies_out.m_IESType = eC0;
                    break;
                case 90:
                    ies_out.m_IESType = eC90;
                    break;
                case 180:
                    ies_out.m_IESType = eC180;
                    break;
                case 360:
                    ies_out.m_IESType = eC360;
                    break;
                default:
                    break;
                }
            }
        }
        break;
        case 2:
        {

        }
            break;
        case 3:
        {

        }
            break;
        default:
            break;
        }
        return true;
    }

    static bool write_ies(const std::string& filename, const light& ies, const uint32_t precision = std::numeric_limits<T>::max_digits10) {
        std::stringstream ss;
        ss.precision(precision);

        /*
        ** HEADER
        */
        ss << "IESNA:" << ies.ies_version << std::endl;
        for (const auto& pair : ies.properties) ss << "[" << pair.first << "] " << pair.second << std::endl;

        /*
        ** DATA
        */
        ss << "TILT=" << ies.tilt << std::endl;
        if (ies.tilt == "INCLUDE") {
            // <lamp to luminaire geometry> <#tilt angles> <angles> <multiplying factors>
            ss << ies.lamp_to_luminaire_geometry << " " << ies.number_of_tilt_angles;
            for (int i = 0; i < ies.number_of_tilt_angles; i++) {
                ss << ies.tilt_angles[i];
                if (i < (ies.number_of_tilt_angles - 1)) ss << " ";
            }
            for (int i = 0; i < ies.number_of_tilt_angles; i++) {
                ss << ies.tilt_multiplying_factors[i];
                if (i < (ies.number_of_tilt_angles - 1)) ss << " ";
            }
            ss << std::endl;
        }

        // <#lamps> <lumen/lamp> <multiplier> <#vertical angles> <#horizontal angles> <photometric type> <units type> <width> <length> <height>
        ss << ies.number_lights << " " << ies.lumens_per_lamp << " " << ies.multiplier << " ";
        ss << ies.number_vertical_angles << " " << ies.number_horizontal_angles << " " << ies.photometric_type << " ";
        ss << ies.units_type << " " << ies.width << " " << ies.length << " " << ies.height << std::endl;
        // <ballast factor> <future use> <input watts>
        ss << ies.ballast_factor << " " << ies.future_use << " " << ies.input_watts << std::endl;
        // <vertical angles>
        for (int i = 0; i < ies.number_vertical_angles; i++) {
            ss << ies.vertical_angles[i];
            if (i < (ies.number_vertical_angles - 1)) ss << " ";
            else ss << std::endl;
        }
        // <horizontal angles>
        for (int i = 0; i < ies.number_horizontal_angles; i++) {
            ss << ies.horizontal_angles[i];
            if (i < (ies.number_horizontal_angles - 1)) ss << " ";
            else ss << std::endl;
        }
        // <candela values for all vertical angles at first horizontal angle>
        //                                              :
        // <candela values for all vertical angles at last horizontal angle>
        const int count = ies.number_vertical_angles * ies.number_horizontal_angles;
        for (int i = 0; i < count; i++) {
            ss << ies.candela[i];
            if (i != 0 && (i + 1) % (ies.number_vertical_angles) == 0) ss << std::endl;
            else if (i < (count - 1)) ss << " ";
        }

        std::ofstream file(filename, std::ios::out | std::ios::trunc);
        if (!file.is_open()) return false;
        file << ss.rdbuf();
        file.close();
        return true;
    }

private:
    /* read property in current line and return value */
    static bool read_property(const std::string& name, std::string& line, std::string& out) {
        size_t pos = line.find(name);
        if (pos == std::string::npos) return false;

        const std::string substring = line.substr(pos + name.size(), line.size());
        pos = substring.find_first_not_of(' ');
        if (pos == std::string::npos) return false;
        out = substring.substr(pos, substring.size());
        return true;
    }
    /* read property in current line and add it to the dict */
    static bool read_property(std::string& line, std::map<std::string, std::string>& property_map) {
        const std::string start_delim = "[";
        const std::string stop_delim = "]";
        const size_t attribute_start = line.find(start_delim) + start_delim.size();
        const size_t attribute_end = line.find_last_of(stop_delim);
        if (attribute_start == std::string::npos || attribute_end == std::string::npos) return false;

        std::string attribute = line.substr(attribute_start, attribute_end - attribute_start);
        std::string property;
        if (!read_property(attribute + stop_delim, line, property)) return false;
        property_map.insert({ attribute, property });
        return true;
    }

    template <typename U>
    static bool store_value(std::stringstream& stream, U& out) {
        return static_cast<bool>(stream >> out);
    }
};

class BMPGenerator {
#pragma pack(push, 1)
    struct BMPHeader {
        uint16_t signature = 0x4D42; // "BM"
        uint32_t fileSize;
        uint16_t reserved1 = 0;
        uint16_t reserved2 = 0;
        uint32_t dataOffset = 54; // 头部大小
    };

    struct BMPInfoHeader {
        uint32_t headerSize = 40;
        int32_t width;
        int32_t height;
        uint16_t planes = 1;
        uint16_t bitsPerPixel = 24;
        uint32_t compression = 0;
        uint32_t imageSize;
        int32_t xPixelsPerMeter = 0;
        int32_t yPixelsPerMeter = 0;
        uint32_t colorsUsed = 0;
        uint32_t colorsImportant = 0;
    };
#pragma pack(pop)

public:
    static bool generateBMP(const std::string& filename,
        const std::vector<uint8_t>& imageData,
        int width, int height) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "无法创建BMP文件: " << filename << std::endl;
            return false;
        }

        BMPHeader header;
        BMPInfoHeader infoHeader;

        infoHeader.width = width;
        infoHeader.height = height;

        // 计算每行字节数（必须是4的倍数）
        int rowSize = (width * 3 + 3) & ~3;
        infoHeader.imageSize = rowSize * height;
        header.fileSize = infoHeader.imageSize + sizeof(BMPHeader) + sizeof(BMPInfoHeader);

        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

        // 写入图像数据（BMP是从下到上存储的）
        for (int y = height - 1; y >= 0; --y) {
            int rowStart = y * width * 3;
            file.write(reinterpret_cast<const char*>(&imageData[rowStart]), width * 3);

            // 填充行对齐字节
            int padding = rowSize - width * 3;
            for (int i = 0; i < padding; ++i) {
                file.put(0);
            }
        }

        return true;
    }
};

class IESToTextureConverter {
public:
    static std::vector<uint8_t> convertToTexture( tiny_ies<float>::light iesData
        ) {

        int textureWidth = iesData.horizontal_angles.size();
        int textureHeight = iesData.vertical_angles.size();

        std::vector<uint8_t> textureData(textureWidth * textureHeight * 3, 0);

        // 找到最大光度值用于归一化
        float maxCandela = iesData.max_candela;

        if (maxCandela == 0.0f) {
            return textureData; // 避免除以零
        }

        // 将IES数据映射到纹理
        for (int y = 0; y < textureHeight; ++y) {
            for (int x = 0; x < textureWidth; ++x) {
                // 将纹理坐标转换为球面坐标
                float u = static_cast<float>(x) / textureWidth;
                float v = static_cast<float>(y) / textureHeight;

                ////// 计算对应的IES角度索引
                //int vIdx = static_cast<int>(u * iesData.vertical_angles.size()) % iesData.vertical_angles.size();
                //int hIdx = static_cast<int>(v * iesData.horizontal_angles.size()) % iesData.horizontal_angles.size();

                // 获取光度值并归一化
                float intensity = iesData.candela[x* textureHeight +y] / maxCandela;
                uint8_t pixelValue = static_cast<uint8_t>(intensity * 255);

                // 设置RGB值（创建灰度图像）
                int index = (y * textureWidth + x) * 3;
                textureData[index] = pixelValue;     // B
                textureData[index + 1] = pixelValue; // G
                textureData[index + 2] = pixelValue; // R
            }
        }

        return textureData;
    }
};