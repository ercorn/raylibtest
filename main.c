#include <stdio.h>
#include <raylib.h>

int main() {
	//init window
	const int screen_width = 600;
	const int screen_height = 400;

	InitWindow(screen_width, screen_height, "test window");

	Rectangle rect = (Rectangle) { .height = 10, .width = 10, .x = 100, .y = 100};

	//run window
	while(!WindowShouldClose()) {
		BeginDrawing();
		//draw stuff
		ClearBackground(SKYBLUE);
		DrawRectangleRec(rect, RAYWHITE);
		DrawText("Raylib test!", 10, 10, 20, DARKGRAY);
		

		if (IsKeyDown(KEY_LEFT)) {
			DrawCircle(0, screen_height / 2, 50, BLACK);
		}
		if (IsKeyDown(KEY_RIGHT)) {
			DrawCircle(screen_width, screen_height / 2, 50, BLACK);
		}

		EndDrawing();
	}
	//close window
	CloseWindow();

	return 0;
}
