#include "raylib.h"
#include <stdbool.h>

#include "emu.c"

// cycles per scanline: 456
// scanlines per frame: 154 (144 visible + 10 vblank lines)
#define CYCLES_PER_SCANLINE 456 * 154 // 70224
// CPU clock: 4.194304 MHz
// 4194304 / 70224 = 59.7275Hz (Vertical sync)
#define TARGET_FRAMETIME 1.0 / (4194304 / 70224)

void PlatformInitWindow() {
	InitWindow(960, 540, "Gameboy Emu");
}

void PlatformToggleFullscreen() {
	if (IsWindowFullscreen()) {
		SetWindowSize(960, 540);
		ToggleFullscreen();
	} else {
		SetWindowSize(GetScreenWidth(), GetScreenHeight());
		ToggleFullscreen();
	}
}

int main() {
	PlatformInitWindow();

	Texture2D test_texture = LoadTexture("test.jpg");

	bool running = true;
	while (!WindowShouldClose() && running) {
		if (IsKeyPressed(KEY_Q))
			running = false;
		if (IsKeyPressed(KEY_F11))
			PlatformToggleFullscreen();

		const double frame_start = GetTime();

		// ============================== DRAWING ==============================

		BeginDrawing();
		ClearBackground(BLACK);

		DrawTexture(test_texture, 0, 0, WHITE);

		EndDrawing();

		const double elapsed = GetTime() - frame_start;
		if (elapsed < TARGET_FRAMETIME)
			WaitTime(TARGET_FRAMETIME - elapsed);
	}
}
