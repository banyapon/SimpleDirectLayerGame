#include "player.h"
#include <SDL3/SDL.h>

// ฟังก์ชันช่วยโหลด Texture และตั้งค่า Pixel Art
SDL_Texture *LoadTex(SDL_Renderer *r, const char *path)
{
    SDL_Surface *surf = SDL_LoadBMP(path);
    if (!surf)
    {
        SDL_Log("Failed to load %s: %s", path, SDL_GetError());
        return NULL;
    }
    // กำหนด Color Key (สีดำ 0,0,0 ให้เป็นโปร่งใส)
    SDL_SetSurfaceColorKey(surf, true, SDL_MapRGB(SDL_GetPixelFormatDetails(surf->format), NULL, 0, 0, 0));

    SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST); // ให้ภาพคม
    SDL_DestroySurface(surf);
    return tex;
}

void Player_Init(Player *p, SDL_Renderer *renderer)
{
    // --- แก้ไข 1: ปรับขนาดตัวละครให้เป็น 24x24 ---
    p->w = 24; 
    p->h = 24;

    // จัดกึ่งกลางจอ Logical 320x240
    // (320/2) - (24/2) = 160 - 12 = 148
    // (240/2) - (24/2) = 120 - 12 = 108
    p->x = 148; 
    p->y = 108;

    p->speed = 100.0f; // ลดความเร็วลงหน่อย เพราะตัวละครเล็กลงแล้ว (เดิม 300 เร็วไปสำหรับจอเล็ก)
    p->facingRight = 1;
    p->state = STATE_IDLE;
    p->currentFrame = 0;
    p->lastFrameTime = 0;
    p->isAttacking = 0;

    // โหลดรูปทั้ง 3 ท่า
    p->texIdle = LoadTex(renderer, "idle.bmp");
    p->texWalk = LoadTex(renderer, "walk.bmp");
    p->texAttack = LoadTex(renderer, "attack.bmp");
}

void Player_HandleEvent(Player *p, SDL_Event *event)
{
    // กด P เพื่อโจมตี (ต้องไม่โจมตีอยู่)
    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        if (event->key.key == SDLK_P && !p->isAttacking)
        {
            p->isAttacking = 1;
            p->state = STATE_ATTACK;
            p->currentFrame = 0; // เริ่มเฟรมแรกใหม่
        }
    }
}

void Player_Update(Player *p, const bool *keys, float deltaTime)
{
    // --- แก้ไข 2: ปรับจำนวนเฟรม Animation (ทุกท่ามี 6 เฟรม) ---
    int totalFrames = 6;
    int animSpeed = 100; // ms

    if (p->state == STATE_ATTACK)
    {
        totalFrames = 6; // แก้จาก 8 เป็น 6 ตามโจทย์
        animSpeed = 80; 
    }
    else if (p->state == STATE_WALK)
    {
        totalFrames = 6;
        animSpeed = 100;
    }

    // คำนวณเวลาเปลี่ยนเฟรม
    if (SDL_GetTicks() - p->lastFrameTime > animSpeed)
    {
        p->currentFrame++;
        p->lastFrameTime = SDL_GetTicks();

        // ถ้าโจมตีครบเฟรม ให้กลับมาปกติ
        if (p->state == STATE_ATTACK && p->currentFrame >= totalFrames)
        {
            p->isAttacking = 0;
            p->state = STATE_IDLE;
            p->currentFrame = 0;
        }
        else
        {
            p->currentFrame %= totalFrames; // วนลูปปกติ
        }
    }

    // --- Movement Logic ---
    if (p->isAttacking)
        return;

    float moveX = 0;
    float moveY = 0;

    // รับค่าปุ่ม
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
        moveY = -1;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
        moveY = 1;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
        moveX = -1;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
        moveX = 1;

    // อัปเดตสถานะ
    if (moveX != 0 || moveY != 0)
    {
        p->state = STATE_WALK;
        if (moveX > 0) p->facingRight = 1;
        if (moveX < 0) p->facingRight = 0;
    }
    else
    {
        p->state = STATE_IDLE;
    }

    // เคลื่อนที่
    p->x += moveX * p->speed * deltaTime;
    p->y += moveY * p->speed * deltaTime;

    // --- Boundary Check ---
    // Logical Screen: 320x240
    // ขอบ: ซ้าย/ขวา/บน 40, ล่าง 80
    float minX = 40;
    float maxX = 320 - 40 - p->w;
    float minY = 40;
    float maxY = 240 - 80 - p->h;

    if (p->x < minX) p->x = minX;
    if (p->x > maxX) p->x = maxX;
    if (p->y < minY) p->y = minY;
    if (p->y > maxY) p->y = maxY;
}

void Player_Render(Player *p, SDL_Renderer *renderer)
{
    SDL_Texture *currentTex = p->texIdle;
    if (p->state == STATE_WALK)
        currentTex = p->texWalk;
    if (p->state == STATE_ATTACK)
        currentTex = p->texAttack;

    // --- แก้ไข 3: ปรับการตัดภาพ (Source Rect) เป็นขนาด 24x24 ---
    // p->currentFrame * 24 เพราะภาพเรียงต่อกันทีละ 24px
    SDL_FRect src = {p->currentFrame * 24, 0, 24, 24};
    
    // วางภาพ (Dest Rect) ใช้ขนาด p->w, p->h (ซึ่ง Init ไว้เป็น 24x24)
    SDL_FRect dst = {p->x, p->y, p->w, p->h};

    SDL_FlipMode flip = (p->facingRight) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    SDL_RenderTextureRotated(renderer, currentTex, &src, &dst, 0, NULL, flip);
}

void Player_Cleanup(Player *p)
{
    if (p->texIdle) SDL_DestroyTexture(p->texIdle);
    if (p->texWalk) SDL_DestroyTexture(p->texWalk);
    if (p->texAttack) SDL_DestroyTexture(p->texAttack);
}