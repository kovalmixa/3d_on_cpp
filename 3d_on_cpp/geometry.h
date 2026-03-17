#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

struct Geometry {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    Geometry() = default;
    Geometry(std::vector<glm::vec3> vertices, std::vector<unsigned int> lines) :
        vertices(vertices), indices(lines) { }
};

inline void to_json(nlohmann::json& j, const Geometry& g) {
    j = nlohmann::json{ {"vertice", g.vertices}, {"lines", g.indices} };
}

inline void from_json(const nlohmann::json& j, Geometry& g) {
    j.at("vertices").get_to(g.vertices);
    j.at("indices").get_to(g.indices);
}