#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "player.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    
    // 1. สร้างหน้าต่างขนาดเริ่มต้น 640x480
    // เพิ่ม SDL_WINDOW_RESIZABLE เพื่อให้ผู้ใช้กดปุ่ม Maximize (สี่เหลี่ยมมุมขวาบน) ได้
    SDL_Window* window = SDL_CreateWindow("RPG Pixel Art 24px", 640, 480, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // 2. [หัวใจสำคัญ] ตั้งค่า Logical Resolution เป็น 320x240
    // SDL_LOGICAL_PRESENTATION_LETTERBOX: จะขยายภาพให้เต็มจอที่สุดโดยรักษาอัตราส่วนเดิมไว้ (อาจมีขอบดำถ้าสัดส่วนจอไม่ตรง)
    // SDL_LOGICAL_PRESENTATION_STRETCH: ถ้าอยากให้ยืดเต็มจอแบบไม่มีขอบดำ (แต่ภาพอาจจะอ้วน/ผอม) ให้ใช้ตัวนี้แทน
    SDL_SetRenderLogicalPresentation(renderer, 320, 240, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // สร้างตัวละคร
    Player player;
    Player_Init(&player, renderer);
    
    Uint64 lastTime = SDL_GetTicks();
    int running = 1;
    SDL_Event event;

    while (running) {
        Uint64 now = SDL_GetTicks();
        float deltaTime = (now - lastTime) / 1000.0f; 
        lastTime = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = 0;
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) running = 0;
            
            // ส่ง event ไปให้ player
            Player_HandleEvent(&player, &event);
        }

        int numKeys;
        const bool* keys = SDL_GetKeyboardState(&numKeys);

        Player_Update(&player, keys, deltaTime);

        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 50, 100, 50, 255);
        SDL_RenderClear(renderer);
        
        // วาดกรอบเช็คขอบเขต (Visual Debug)
        // กรอบนี้จะขยายตามหน้าจออัตโนมัติ เพราะเราวาดบน Logical 320x240
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_FRect border = {40, 40, 240, 120}; // (x, y, w, h)
        SDL_RenderRect(renderer, &border);

        Player_Render(&player, renderer);

        SDL_RenderPresent(renderer);
    }

    Player_Cleanup(&player);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}