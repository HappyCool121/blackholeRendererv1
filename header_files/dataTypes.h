//
// Created by Imari on 11/2/26.
//

#ifndef RENDERENGINEV2_DATATYPES_H
#define RENDERENGINEV2_DATATYPES_H
#include "vectorMath.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

const float PI = 3.14159265359f;

struct ray {
  glm::vec3 origin = {0, 0, 0};
  glm::vec3 d = {0, 0, 0};
};

#endif // RENDERENGINEV2_DATATYPES_H