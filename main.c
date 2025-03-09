#include <stdio.h>
#include <raylib.h>

int main() {
	//init window
	InitWindow(600, 400, "test window");

	Rectangle rect = (Rectangle) { .height = 10, .width = 10, .x = 100, .y = 100};

	//run window
	while(!WindowShouldClose()) {
		BeginDrawing();
		//draw stuff
		ClearBackground(SKYBLUE);
		DrawRectangleRec(rect, RAYWHITE);

		EndDrawing();
	}
	//close window

	return 0;
}
