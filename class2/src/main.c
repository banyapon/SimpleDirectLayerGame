#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char* argv[]) {
    // 1. Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // 2. สร้างหน้าต่าง (เพิ่ม Flag SDL_WINDOW_RESIZABLE เพื่อให้ขยายจอได้)
    SDL_Window* window = SDL_CreateWindow("Lesson 1: Load BMP & Events", 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. สร้าง Renderer (ตัววาดภาพลงหน้าจอ)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 4. โหลดภาพ BMP
    // SDL_LoadBMP จะโหลดภาพมาเป็น Surface (ข้อมูลดิบใน RAM)
    // เราต้องแปลงเป็น Texture (ข้อมูลใน GPU) เพื่อให้วาดได้เร็ว
    SDL_Surface* surface = SDL_LoadBMP("logo.bmp");
    if (!surface) {
        SDL_Log("Failed to load BMP: %s", SDL_GetError());
        // ถ้าโหลดไม่เจอ จะรันต่อก็ได้แต่จะไม่มีรูปโชว์
    }

    SDL_Texture* texture = NULL;
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface); // แปลงเสร็จแล้ว ลบ Surface ทิ้งได้เลยเพื่อคืน RAM
    }

    // 5. Game Loop (หัวใจหลักของเกม)
    int running = 1; // ตัวแปรเช็คว่าเกมยังรันอยู่ไหม
    SDL_Event event;

    while (running) {
        // --- Event Processing (รับคำสั่งจากเมาส์/คีย์บอร์ด) ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                // กดกากบาทปิดหน้าต่าง
                running = 0;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                // เช็คปุ่มกด
                if (event.key.key == SDLK_ESCAPE) {
                    running = 0; // กด ESC แล้วจบการทำงาน
                }
            }
        }

        /*
        // --- Rendering (วาดภาพ) ---
        // 1. ล้างหน้าจอให้เป็นสีดำ (หรือสีที่กำหนด)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // R, G, B, Alpha
        SDL_RenderClear(renderer);

        // 2. วาดรูป (Texture) ลงไป
        if (texture) {
            // NULL ตัวแรก = เอามาทั้งรูป, NULL ตัวสอง = วาดเต็มหน้าจอ
            // ถ้าอยากกำหนดขนาด ให้สร้าง SDL_FRect dstRect; แล้วส่งเข้าไปแทน NULL ตัวหลัง
            SDL_RenderTexture(renderer, texture, NULL, NULL); 
        }

        // 3. แสดงผลขึ้นหน้าจอ
        SDL_RenderPresent(renderer);*/

        // --- Rendering (วาดภาพ) ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (texture) {
            // กำหนดขนาดและตำแหน่ง
            SDL_FRect destRect;
            destRect.x = 100;  // ตำแหน่งแนวนอน (เริ่มจากซ้าย)
            destRect.y = 100;  // ตำแหน่งแนวตั้ง (เริ่มจากบน)
            destRect.w = 290;  // ความกว้าง (ตามที่ต้องการ)
            destRect.h = 125;  // ความสูง (ตามที่ต้องการ)

            // ส่ง &destRect เข้าไปในพารามิเตอร์ตัวสุดท้าย
            SDL_RenderTexture(renderer, texture, NULL, &destRect);
        }

        SDL_RenderPresent(renderer);
    }

    // 6. Cleanup (คืนค่าหน่วยความจำ)
    if (texture) SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}