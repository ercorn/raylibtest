#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

/*
	TODO: GOING WITH STOCK TRADING GAME. Plans in gpt. general idea is stock trading clicker where the end goal is to retire rich.
	TODO: Decide direction for the game. Current idea is to load some 3d and have resources gained from the clicker effect it kind of like growing
	an egg or something.
	DONE: Figure out how to have a variable autoincrement every second. The current plan is to have each manastone autoincrement the click_counter
	by the total number of manastones every second.
		ex: 1 manastone   = 1  clicks/sec
			2 manastones  = 2  clicks/sec
			39 manastones = 39 clicks/sec
*/

int main() {
	//init window
	const int screen_width = 800;
	const int screen_height = 600;

	InitWindow(screen_width, screen_height, "test window");

	SetTargetFPS(60); // Cap/Limit FPS

	srand((int)GetTime());

	Rectangle clicker_rect = (Rectangle) { .height = 50, .width = 100, .x = screen_width - 110, .y = screen_height - 60};
	Rectangle manastone_rect = (Rectangle) {.height = 50, .width = 100, .x = screen_width - 110, .y = screen_height - 120};
	Rectangle tradeButton = {300, 250, 200, 50}; //stock trading button
	char *fps_str = malloc(30 * sizeof(char));
	char *counter_txt = malloc(20 * sizeof(char));
	char *counter_manastones = malloc(20 * sizeof(char));
	char *counter_money = malloc(20 * sizeof(char));
	int click_counter = 0;
	int mana_stonecost = 25;
	int mana_stones = 0;
	int money = 100;
	int max_click_profit = 20;
	int min_click_profit = -5;
	//float second_timer = 0;
	double old_time = GetTime();

	//def camera
	Camera3D camera = {0};
	camera.position = (Vector3){10.0f, 10.0f, 10.0f};
	camera.target = (Vector3){0.0f, 0.0f, 0.0f};
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	//run window
	while(!WindowShouldClose()) {
		Vector2 mouse_pos = GetMousePosition();
		
		double current_time = GetTime();
		if (mana_stones >=1) {
			if (current_time - old_time >= 1.0) { //if a second has passed
				click_counter += mana_stones;
				old_time = current_time;
			}
		}

		BeginDrawing();
		UpdateCamera(&camera, CAMERA_ORBITAL);
		//draw stuff
		ClearBackground(SKYBLUE);

		BeginMode3D(camera);

		DrawSphere((Vector3){0.0f, 0.0f, 0.0f}, 2.0f, BLACK);

		EndMode3D();

		//stock trade start
		if (CheckCollisionPointRec(mouse_pos, tradeButton)) {
			DrawRectangleRounded(tradeButton, 0.2f, 10, LIGHTGRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				money += (rand() % (max_click_profit - min_click_profit + 1)) + min_click_profit;
			}
		} else {
			DrawRectangleRounded(tradeButton, 0.2f, 10, GRAY);
		}
		DrawText("TRADE", 360, 265, 20, BLACK);
		//stock trade end

		if (CheckCollisionPointRec(mouse_pos, clicker_rect)) { //TODO: HANDLE CLICK, HOVER, and NEITHER CASES
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //check mouse left click
				click_counter++;
			}
			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {    //check if still held
				DrawRectangleRec(clicker_rect, RED);
			} else {									   //if hovering but not clicked/held
				DrawRectangleRec(clicker_rect, LIGHTGRAY);
			}
		} else {
			DrawRectangleRec(clicker_rect, DARKGRAY);			   //mouse not over rectangle at all
		}

		if ((click_counter >=  mana_stonecost) && CheckCollisionPointRec(mouse_pos, manastone_rect)) { //DRAW MANA STONE BUY BUTTON
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //check mouse left click
				if (click_counter >= mana_stonecost) { //manastone buy check
					click_counter -= mana_stonecost;
					mana_stones++;
				}
			}
			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {    //check if still held
				DrawRectangleRec(manastone_rect, RED);
			} else {									   //if hovering but not clicked/held
				DrawRectangleRec(manastone_rect, LIGHTGRAY);
			}
		} else if (click_counter >= mana_stonecost) {
			DrawRectangleRec(manastone_rect, DARKGRAY);			   //mouse not over rectangle at all
		}

		snprintf(fps_str, 30, "FPS: %f", 1 / GetFrameTime()); 
		snprintf(counter_txt, 20, "Clicks: %d", click_counter);
		snprintf(counter_manastones, 20, "Mana Stones: %d", mana_stones);
		snprintf(counter_money, 20, "MONEY: $%d", money);
		DrawText(fps_str, 10, 10, 20, DARKGRAY); //Draw FPS Counter
		DrawText(counter_txt, 10, 50, 20, DARKGRAY); //Draw Click Counter
		DrawText(counter_manastones, 10, 70, 20, DARKBLUE); //Draw Mana Stone Counter
		DrawText(counter_money, 10, 90, 20, DARKGRAY);

		

		if (IsKeyDown(KEY_LEFT)) {
			DrawCircle(0, screen_height / 2, 50, BLACK);
		}
		if (IsKeyDown(KEY_RIGHT)) {
			DrawCircle(screen_width, screen_height / 2, 50, BLACK);
		}

		EndDrawing();
	}
	free(fps_str);
	free(counter_txt);
	free(counter_manastones);
	//close window
	CloseWindow();

	return 0;
}
