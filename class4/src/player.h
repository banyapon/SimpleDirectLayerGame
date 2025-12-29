#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>

// สถานะของตัวละคร
typedef enum {
    STATE_IDLE,
    STATE_WALK,
    STATE_ATTACK
} PlayerState;

// โครงสร้างข้อมูลตัวละคร
typedef struct {
    float x, y;          // ตำแหน่ง
    float w, h;          // ขนาดตัว
    float speed;         // ความเร็ว
    int facingRight;     // 1 = หันขวา, 0 = หันซ้าย
    PlayerState state;   // กำลังทำอะไรอยู่

    // Animation
    int currentFrame;
    Uint64 lastFrameTime; // เวลาล่าสุดที่เปลี่ยนเฟรม
    int isAttacking;      // เช็คว่ากำลังโจมตีอยู่ไหม (ห้ามเดิน)

    // Textures (เก็บรูปแยกกัน)
    SDL_Texture* texIdle;
    SDL_Texture* texWalk;
    SDL_Texture* texAttack;
} Player;

// ฟังก์ชันต่างๆ ที่เรียกใช้ได้จากข้างนอก
void Player_Init(Player* p, SDL_Renderer* renderer);
void Player_Update(Player* p, const bool* keys, float deltaTime);
void Player_HandleEvent(Player* p, SDL_Event* event);
void Player_Render(Player* p, SDL_Renderer* renderer);
void Player_Cleanup(Player* p);

#endif