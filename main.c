#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

int main() {
	//init window
	const int screen_width = 600;
	const int screen_height = 400;

	InitWindow(screen_width, screen_height, "test window");

	SetTargetFPS(60); // Cap/Limit FPS

	Rectangle rect = (Rectangle) { .height = 50, .width = 100, .x = 100, .y = 100};
	char *fps_str = malloc(30 * sizeof(char));
	char *counter_txt = malloc(20 * sizeof(char));
	int click_counter = 0;

	//run window
	while(!WindowShouldClose()) {
		Vector2 mouse_pos = GetMousePosition();
		
		BeginDrawing();
		//draw stuff
		ClearBackground(SKYBLUE);

		if (CheckCollisionPointRec(mouse_pos, rect)) { //TODO: HANDLE CLICK, HOVER, and NEITHER CASES
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //check mouse left click
				click_counter++;
			}
			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {    //check if still held
				DrawRectangleRec(rect, RED);
			} else {									   //if hovering but not clicked/held
				DrawRectangleRec(rect, LIGHTGRAY);
			}
		} else {
			DrawRectangleRec(rect, DARKGRAY);			   //mouse not over rectangle at all
		}

		snprintf(fps_str, 30, "Raylib test FPS: %f", 1 / GetFrameTime()); 
		snprintf(counter_txt, 20, "Clicks: %d", click_counter);
		DrawText(fps_str, 10, 10, 20, DARKGRAY); //Draw FPS Counter
		DrawText(counter_txt, 10, 50, 20, DARKGRAY); //Draw FPS Counter

		

		if (IsKeyDown(KEY_LEFT)) {
			DrawCircle(0, screen_height / 2, 50, BLACK);
		}
		if (IsKeyDown(KEY_RIGHT)) {
			DrawCircle(screen_width, screen_height / 2, 50, BLACK);
		}

		EndDrawing();
	}
	free(fps_str);
	//close window
	CloseWindow();

	return 0;
}
