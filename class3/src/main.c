#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char* argv[]) {
    // 1. Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // 2. สร้างหน้าต่างขนาด 320x240
    SDL_Window* window = SDL_CreateWindow("Lesson 2: Keep Ratio 320x240", 320, 240, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. สร้าง Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- จุดสำคัญ: ตั้งค่า Logical Size ให้เป็น 320x240 และรักษาอัตราส่วน (Letterbox) ---
    // SDL_LOGICAL_PRESENTATION_LETTERBOX: ขยายภาพให้เต็มจอโดยรักษาสัดส่วน และเติมขอบดำในส่วนที่เหลือ
    SDL_SetRenderLogicalPresentation(renderer, 320, 240, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // 4. โหลดภาพ BMP
    SDL_Surface* surface = SDL_LoadBMP("idle.bmp");
    if (!surface) {
        SDL_Log("Failed to load BMP: %s", SDL_GetError());
    }

    SDL_Texture* texture = NULL;
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        
        // --- เพิ่มบรรทัดนี้ครับ ---
        // สั่งให้ Texture นี้ขยายแบบ "Nearest" (เอาพิกเซลที่ใกล้ที่สุดมาแปะ ห้ามเบลอ)
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST); 
        
        SDL_DestroySurface(surface); 
    }

    // --- ตั้งค่าตัวแปร Animation ---
    int frameWidth = 100;
    int frameHeight = 100;
    int totalFrames = 6;
    int animationSpeed = 100;

    // 5. Game Loop
    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = 0;
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) running = 0;
        }

        Uint64 currentTime = SDL_GetTicks();
        int currentFrameIndex = (currentTime / animationSpeed) % totalFrames;

        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (texture) {
            SDL_FRect srcRect;
            srcRect.x = (float)(currentFrameIndex * frameWidth);
            srcRect.y = 0.0f;
            srcRect.w = (float)frameWidth;
            srcRect.h = (float)frameHeight;

            // คำนวณตำแหน่งกลางจอ โดยอิงจากขนาด Logical (320x240) ไม่ใช่ขนาดหน้าต่างจริง
            // กึ่งกลางแกน X = (320 - 100) / 2 = 110
            // กึ่งกลางแกน Y = (240 - 100) / 2 = 70
            SDL_FRect destRect;
            destRect.x = 110.0f; 
            destRect.y = 70.0f;
            destRect.w = 100.0f;
            destRect.h = 100.0f; 

            SDL_RenderTexture(renderer, texture, &srcRect, &destRect);
        }

        SDL_RenderPresent(renderer);
    }

    if (texture) SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}