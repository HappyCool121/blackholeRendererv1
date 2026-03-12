//
// Created by Imari on 25/2/26.
//

#ifndef RENDERENGINEV2_RAYTRACER_H
#define RENDERENGINEV2_RAYTRACER_H

#include "dataTypes.h"
#include <glm/glm.hpp>
#include <vector>

// main raytracer function
glm::vec3 raytracer(const ray &ray, const glm::vec3 &camera_pos);

#endif // RENDERENGINEV2_RAYTRACER_H