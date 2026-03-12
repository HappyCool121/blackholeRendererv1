#include "application.h"
#include "dataTypes.h"
#include "raytracer.h"
#include <chrono>
#include <glm/glm.hpp>
#include <iostream>

const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

// global variables
// camera settings
glm::vec3 camera_pos = {0.0f, 0.0f,
                        -30.0f}; // we will be raising the camera later on!

// checkerboard texture wall
glm::vec3 skybox_pos = {0.0f, 0.0f, 50.0f};
glm::vec3 skybox_scale = {1.0f, 1.0f, 1.0f};

// black hole settings
glm::vec3 bh_pos = {0.0f, 0.0f, 0.0f};

// create for every pixel and call raytracer for each
void RenderImage() {
  auto start = std::chrono::high_resolution_clock::now();

  // 1. Calculate screen aspect ratio
  float aspect_ratio = (float)WIDTH / (float)HEIGHT;

  // 2. Loop over every pixel
  for (int y = 0; y < HEIGHT; y++) {
    // Print progress every 10%
    if (y % (HEIGHT / 10) == 0) {
      std::cout << "Rendering: " << (int)((float)y / HEIGHT * 100.0f) << "%"
                << std::endl;
    }
    for (int x = 0; x < WIDTH; x++) {

      // Map pixel coordinates (x,y) to screen space (-1 to 1)
      float screen_x = (2.0f * (x + 0.5f) / WIDTH - 1.0f) * aspect_ratio;
      float screen_y = 1.0f - 2.0f * (y + 0.5f) / HEIGHT; // Flipped so +Y is up

      // Create the ray
      ray current_ray;
      current_ray.origin = camera_pos;
      // Ray direction pointing to the virtual screen 1 unit away (Z = 1.0)
      current_ray.d = glm::normalize(glm::vec3(screen_x, screen_y, 1.0f));

      // Trace the ray
      glm::vec3 pixel_vec3 = raytracer(current_ray, camera_pos);

      // Convert glm::vec3 (0.0 to 1.0) to ARGB (0 to 255)
      uint8_t r = (uint8_t)(std::min(pixel_vec3.x, 1.0f) * 255);
      uint8_t g = (uint8_t)(std::min(pixel_vec3.y, 1.0f) * 255);
      uint8_t b = (uint8_t)(std::min(pixel_vec3.z, 1.0f) * 255);
      uint32_t argb = (255 << 24) | (r << 16) | (g << 8) | b;

      pixels[y * WIDTH + x] = argb;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;
  std::cout << "Raytracing took: " << duration.count() << "ms" << std::endl;

  // 3. Send the pixel array to the SDL Texture
  SDL_UpdateTexture(app.texture, nullptr, pixels.data(),
                    WIDTH * sizeof(uint32_t));
}

int main(int argc, char *argv[]) {
  std::cout << "--- blackholev1 start ---" << std::endl;

  // 1. INIT SDL and IMGUI
  app = initSDL();
  initIMGUI(app);

  if (app.window == nullptr) {
    return 1;
  }

  pixels.resize(WIDTH * HEIGHT);

  // 8. DISPLAY LOOP
  std::cout << "beginning render..." << std::endl;
  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        running = false;

      // start render
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
        RenderImage();
      }
    }

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Status");
    ImGui::Text("blackholev1");

    if (ImGui::Button("Render image (R)")) {
      RenderImage();
    }
    ImGui::End();

    ImGui::Render();
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);
    SDL_RenderCopy(app.renderer, app.texture, nullptr, nullptr);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), app.renderer);
    SDL_RenderPresent(app.renderer);
    SDL_Delay(30);
  }

  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyTexture(app.texture);
  SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  SDL_Quit();

  return 0;
}
