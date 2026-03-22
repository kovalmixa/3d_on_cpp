#ifndef FILE_FUNCTIONS_H
#define FILE_FUNCTIONS_H

#include <string>
#include <fstream>
#include <sstream>

#pragma once

inline std::string read_file(const std::string& path)
{
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
#endif