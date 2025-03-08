#include <stdio.h>
#include <raylib.h>

int main() {
	//init window
	InitWindow(600, 400, "test window");

	//run window
	while(!WindowShouldClose()) {
		BeginDrawing();
		//draw stuff
		ClearBackground(SKYBLUE);

		EndDrawing();
	}
	//close window

	return 0;
}
