// aimlock.gba - GBA homebrew demo aimlock
#include <gba.h>
#include <stdlib.h>
#include <math.h>

#define MAX_TARGETS 8
#define SCREEN_W 240
#define SCREEN_H 160
#define CENTER_X 120
#define CENTER_Y 80
#define CROSSHAIR_SIZE 4

typedef struct {
    int x, y;
    int vx, vy;
    int active;
    u16 color;
} Target;

static Target targets[MAX_TARGETS];
static int locked_index = -1;
static int aimlock_on = 0;
static int frame_count = 0;

static inline void put_pixel(int x, int y, u16 color) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
    ((u16*)VRAM)[y * SCREEN_W + x] = color;
}

static void draw_rect(int x, int y, int w, int h, u16 color) {
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            put_pixel(x + i, y + j, color);
}

static void draw_crosshair(int cx, int cy, u16 color) {
    draw_rect(cx - CROSSHAIR_SIZE, cy, CROSSHAIR_SIZE * 2 + 1, 1, color);
    draw_rect(cx, cy - CROSSHAIR_SIZE, 1, CROSSHAIR_SIZE * 2 + 1, color);
}

static void draw_target(const Target* t, u16 color) {
    draw_rect(t->x - 3, t->y - 3, 7, 7, color);
    put_pixel(t->x, t->y, RGB15(31, 31, 31));
}

static void init_targets(void) {
    for (int i = 0; i < MAX_TARGETS; i++) {
        targets[i].x = 20 + rand() % 200;
        targets[i].y = 20 + rand() % 120;
        targets[i].vx = (rand() % 3) - 1;
        targets[i].vy = (rand() % 3) - 1;
        if (targets[i].vx == 0) targets[i].vx = 1;
        if (targets[i].vy == 0) targets[i].vy = 1;
        targets[i].active = 1;
        targets[i].color = RGB15(31, 0, 0);
    }
    locked_index = -1;
}

static void update_targets(void) {
    for (int i = 0; i < MAX_TARGETS; i++) {
        if (!targets[i].active) continue;
        targets[i].x += targets[i].vx;
        targets[i].y += targets[i].vy;
        if (targets[i].x < 10 || targets[i].x > SCREEN_W - 10)
            targets[i].vx = -targets[i].vx;
        if (targets[i].y < 10 || targets[i].y > SCREEN_H - 10)
            targets[i].vy = -targets[i].vy;
        if (targets[i].x < 10) targets[i].x = 10;
        if (targets[i].x > SCREEN_W - 10) targets[i].x = SCREEN_W - 10;
        if (targets[i].y < 10) targets[i].y = 10;
        if (targets[i].y > SCREEN_H - 10) targets[i].y = SCREEN_H - 10;
    }
}

static int find_best_target(void) {
    int best = -1;
    int best_dist = 99999;
    for (int i = 0; i < MAX_TARGETS; i++) {
        if (!targets[i].active) continue;
        int dx = targets[i].x - CENTER_X;
        int dy = targets[i].y - CENTER_Y;
        int d = dx*dx + dy*dy;
        if (d < best_dist) {
            best_dist = d;
            best = i;
        }
    }
    return best;
}

static void update_aimlock(void) {
    if (!aimlock_on) { locked_index = -1; return; }
    if (locked_index < 0 || !targets[locked_index].active)
        locked_index = find_best_target();
}

static void handle_input(void) {
    scanKeys();
    u16 keys = keysDown();
    if (keys & KEY_A) aimlock_on = !aimlock_on;
    if (keys & KEY_B) init_targets();
    if (keys & KEY_START) {
        for (int i = 0; i < MAX_TARGETS; i++) {
            if (targets[i].active) { targets[i].active = 0; break; }
        }
    }
}

static void draw_hud(void) {
    draw_rect(0, 0, SCREEN_W, 8, RGB15(0, 0, 10));
    if (aimlock_on) draw_rect(2, 2, 4, 4, RGB15(0, 31, 0));
    else            draw_rect(2, 2, 4, 4, RGB15(31, 0, 0));
}

int main(void) {
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    srand(12345);
    init_targets();
    while (1) {
        VBlankIntrWait();
        handle_input();
        update_targets();
        update_aimlock();
        draw_rect(0, 0, SCREEN_W, SCREEN_H, RGB15(0, 0, 0));
        for (int i = 0; i < MAX_TARGETS; i++) {
            if (!targets[i].active) continue;
            if (i == locked_index) draw_target(&targets[i], RGB15(31, 31, 0));
            else draw_target(&targets[i], targets[i].color);
        }
        if (aimlock_on && locked_index >= 0)
            draw_crosshair(targets[locked_index].x, targets[locked_index].y, RGB15(31,31,31));
        else
            draw_crosshair(CENTER_X, CENTER_Y, RGB15(31,31,31));
        draw_hud();
        frame_count++;
    }
    return 0;
}
