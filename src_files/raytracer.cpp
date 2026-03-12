
//
// Created by Imari on 25/2/26.
//

#include "raytracer.h"
#include "application.h"
#include <algorithm>
#include <cmath>
#include <cstring> // For memcpy
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <numeric>

// main raytracer and RK4 solver (per ray)
glm::vec3 raytracer(const ray &ray, const glm::vec3 &camera_pos) {

  // 1. Define basis vectors for 2D Cartesian plane
  glm::vec3 N = glm::cross(camera_pos, ray.d);

  // SAFETY CHECK: If ray points directly at the origin (BH), cross product is
  // 0. It falls straight in. Return black immediately to avoid divide-by-zero.
  if (length(N) < 0.000001f) {
    return glm::vec3(0, 0, 0);
  }

  N = normalize(N);
  glm::vec3 e1 = normalize(camera_pos);   // X-axis (points from BH to camera)
  glm::vec3 e2 = normalize(cross(N, e1)); // Y-axis (orthogonal in the plane)

  // 2. Get initial velocities in x and y direction
  // NOTE: These are scalars (double), not glm::vec3!
  double vx = dot(ray.d, e1);
  double vy = dot(ray.d, e2);

  // 3. Define polar coordinates and initial values
  double r_init = length(camera_pos); // Ensure this is length/magnitude
  double phi = 0.0;
  double u = 1.0 / r_init;

  // 4. Get initial velocity du/dphi
  double v = -u * (vx / vy);

  // 5. Setup Loop Variables
  glm::vec3 prev_pos3D = camera_pos;
  double d_phi = 0.05; // Step size
  int max_steps = 1000;
  int curr_steps = 0;

  // 6. RK4 step process:
  static int ray_count = 0;
  bool is_debug_ray = (ray_count == 0);
  glm::vec3 result = glm::vec3(0, 0, 0); // for color

  while (curr_steps < max_steps) {
    if (is_debug_ray && curr_steps % 200 == 0) {
      std::cout << "[Debug Ray] RK4 Step: " << curr_steps << "/" << max_steps
                << " | r = " << (1.0 / u) << std::endl;
    }

    // RK4 Integration Step (Solving: v' = 1.5 * u^2 - u)
    double k1_u = v;
    double k1_v = 1.5 * u * u - u;

    double k2_u = v + 0.5 * d_phi * k1_v;
    double k2_v =
        1.5 * pow(u + 0.5 * d_phi * k1_u, 2) - (u + 0.5 * d_phi * k1_u);

    double k3_u = v + 0.5 * d_phi * k2_v;
    double k3_v =
        1.5 * pow(u + 0.5 * d_phi * k2_u, 2) - (u + 0.5 * d_phi * k2_u);

    double k4_u = v + d_phi * k3_v;
    double k4_v = 1.5 * pow(u + d_phi * k3_u, 2) - (u + d_phi * k3_u);

    // Update u and v
    u += (d_phi / 6.0) * (k1_u + 2 * k2_u + 2 * k3_u + k4_u);
    v += (d_phi / 6.0) * (k1_v + 2 * k2_v + 2 * k3_v + k4_v);
    phi += d_phi;

    // Map back to 3D to check for collisions
    double r = 1.0 / u; // get radius

    // Local 2D coordinates
    double local_x = r * cos(phi);
    double local_y = r * sin(phi);

    // Convert back to 3D cartesian coordinates using our basis vectors
    glm::vec3 curr_pos3D = e1 * (float)local_x + e2 * (float)local_y;

    // Termination Conditions

    // A. Fell into the Event Horizon
    if (r <= 1.0) { // < shwarzchild radius, light cant escape
      result = glm::vec3(0, 0, 0);
      break;
    }

    // B. Escaped to infinity (Hit the skybox)
    if (r > 100.0) {
      glm::vec3 final_dir = normalize(curr_pos3D - prev_pos3D);
      result = sample_skybox(final_dir); // sample checkerboard for debugging
      break;
    }

    // C. Intersected the Accretion Disk (Y=0 plane)
    if (prev_pos3D.y * curr_pos3D.y < 0.0) {
      float t = (float)(std::abs(prev_pos3D.y) /
                        (std::abs(prev_pos3D.y) + std::abs(curr_pos3D.y)));
      glm::vec3 hit_point = prev_pos3D + (curr_pos3D - prev_pos3D) * t;
      float dist_to_center = glm::length(hit_point);

      if (dist_to_center >= 3.0f && dist_to_center <= 10.0f) {
        float blend = (dist_to_center - 3.0f) / 7.0f;
        result = glm::vec3(1.0f, 0.6f * (1.0f - blend), 0.1f * (1.0f - blend));
        break;
      }
    }

    prev_pos3D = curr_pos3D;
    curr_steps++;
  }

  ray_count++;
  return result;
}