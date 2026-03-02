#include "raylib.h"
#include <stdbool.h>

#include "gameboy.c"
#include "gameboy.h"
#include "types.h"

void PlatformInitWindow() {
	InitWindow(960, 540, "Gameboy Emu");
}

void PlatformToggleFullscreen() {
	ToggleBorderlessWindowed();
}

void PlatformDraw() {
	BeginDrawing();
	ClearBackground(BLACK);

	Texture2D test_texture = LoadTexture("test.jpg");
	DrawTexture(test_texture, 0, 0, WHITE);

	EndDrawing();
}

int main() {
	PlatformInitWindow();

	bool running = true;
	while (!WindowShouldClose() && running) {
		if (IsKeyPressed(KEY_Q))
			running = false;
		if (IsKeyPressed(KEY_F11))
			PlatformToggleFullscreen();

		const f64 frame_start = GetTime();

		// ============================== DRAWING ==============================

		PlatformDraw();

		const f64 elapsed = GetTime() - frame_start;
		if (elapsed < TARGET_FRAMETIME)
			WaitTime(TARGET_FRAMETIME - elapsed);
	}
}
