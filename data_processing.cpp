#include "data_processing.h"

void set_materials(std::vector<std::string>& material_names, std::vector<const char*>& material_pointers, const int& N) {
    material_names.clear();
    material_pointers.clear();

    for (int i = 0; i < N; i++) {
        material_names.push_back("Material " + std::to_string(i + 1));
    }

    for (auto& name : material_names) {
        material_pointers.push_back(name.c_str());
    }
}

int get_count_materials_in_file() {
    for (int count = 1; count < 100; count++) {
        std::ifstream file("materials/material_" + std::to_string(count) + ".bin", std::ios::binary);
        if (!file) {
            return count - 1;
        }
    }

    return -1;
}

int read_proc(Vector3i& size_CP, std::vector<uint16_t>& cube_pixels, const int& material_num)
{
    std::ifstream file("materials/material_" + std::to_string(material_num + 1) + ".bin", std::ios::binary);
    if (!file) {
        std::cerr << "Ошибка открытия файла" << std::endl;
        return 1;
    }

    uint16_t shape[3];
    file.read(reinterpret_cast<char*>(shape), sizeof(shape));
    size_CP.z = shape[0], size_CP.y = shape[1], size_CP.x = shape[2];

    std::vector<uint16_t> data2(static_cast<size_t>(size_CP.z) * static_cast<size_t>(size_CP.y) * static_cast<size_t>(size_CP.x));
    cube_pixels = data2;

    file.read(reinterpret_cast<char*>(cube_pixels.data()), cube_pixels.size() * sizeof(uint16_t));
    file.close();

    return 0;
}