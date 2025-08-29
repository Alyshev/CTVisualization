#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "structs.h"


//default_values_by_materials - def_val_mat[Material n].typ
const std::vector<values_by_materials> DEFAULT_VALUES_BY_MATERIALS = {
    values_by_materials(0, 160, 450, 1700, Projection::Top), //material 1
    values_by_materials(1, 10, 100, 1300, Projection::Top), //material 2
    values_by_materials(2, 70, 600, 1600, Projection::Top), //material 3
    values_by_materials(3, 9, 160, 400, Projection::Top), //material 4
    values_by_materials(4, 300, 800, 1800, Projection::Front), //material 5
    values_by_materials(5, 260, 900, 1500, Projection::Right), //material 6
};

void set_materials(std::vector<std::string>& material_names, std::vector<const char*>& material_pointers, const int& N);
int get_count_materials_in_file();
int read_proc(Vector3i& size_CP, std::vector<uint16_t>& cube_pixels, const int& material_num);