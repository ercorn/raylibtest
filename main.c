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

#define MAX_STOCKS 5

typedef struct {
	char *name;
	int cost;
	int income_per_sec;
	int owned;
} Stock;

Stock stocks[MAX_STOCKS] = {
	{"Tech Co", 100, 5, 0},
	{"Auto Inc", 250, 15, 0},
	{"Bank Corp", 500, 30, 0},
	{"Textio", 750, 45, 0},
	{"Boot.dev", 1000, 60, 0}
};

int click_counter = 0;
int mana_stonecost = 25;
int mana_stones = 0;
int money = 1000;
int passive_income = 0;
int max_click_profit = 20;
int min_click_profit = -5;
float timer = 0.0;

void UpdateGame(float deltaTime) {
	timer += deltaTime;
	if (timer >= 1.0) {
		money += passive_income;
		timer = 0.0;
	}
}

void DrawStockMarket() {
	int x = 10; //starting pos x, y for stock listings
	int y = 70;

	DrawText("Stock Market", x, y - 30, 20, GOLD);

	for (int i = 0; i < MAX_STOCKS; i++) {
		char stockInfo[100];
		snprintf(stockInfo, 100, "%s - $%d | +$%d/sec | Owned: %d",
			     stocks[i].name, stocks[i].cost, stocks[i].income_per_sec, stocks[i].owned);
		
		DrawText(stockInfo, x, y, 20, WHITE);

		//Draw Buy Button
		Rectangle buyButton = {x + 410, y - 5, 80, 30};
		DrawRectangleRec(buyButton, DARKGRAY);
		DrawText("BUY", x + 430, y + 5, 20, WHITE);

		//Check click
		if (CheckCollisionPointRec(GetMousePosition(), buyButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (money >= stocks[i].cost) {
				money -= stocks[i].cost;
				stocks[i].owned++;
				passive_income += stocks[i].income_per_sec;
			}
		}

		y += 40; //Next stock gets drawn lower down
	}
}

int main() {
	//init window
	const int screen_width = 800;
	const int screen_height = 600;

	InitWindow(screen_width, screen_height, "test window");

	SetTargetFPS(60); // Cap/Limit FPS

	srand((int)GetTime());

	//Rectangle clicker_rect = (Rectangle) { .height = 50, .width = 100, .x = screen_width - 110, .y = screen_height - 60};
	//Rectangle manastone_rect = (Rectangle) {.height = 50, .width = 100, .x = screen_width - 110, .y = screen_height - 120};
	Rectangle tradeButton = {300, 300, 200, 50}; //stock trading button
	char *fps_str = malloc(30 * sizeof(char));
	//char *counter_txt = malloc(20 * sizeof(char));
	//char *counter_manastones = malloc(20 * sizeof(char));
	//char *counter_money = malloc(20 * sizeof(char));

	double old_time = GetTime();

	//run window
	while(!WindowShouldClose()) {

		//ESC to close window
		if (IsKeyPressed(KEY_Q) && IsKeyPressed(KEY_LEFT_SHIFT)) {
			break;
		}

		float deltaTime = GetFrameTime();
		UpdateGame(deltaTime);


		Vector2 mouse_pos = GetMousePosition();
		
		//double current_time = GetTime();

		BeginDrawing();
		//draw stuff
		ClearBackground(SKYBLUE);

		//stock trade start
		if (CheckCollisionPointRec(mouse_pos, tradeButton)) {
			DrawRectangleRounded(tradeButton, 0.2f, 10, LIGHTGRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				money += (rand() % (max_click_profit - min_click_profit + 1)) + min_click_profit;
			}
		} else {
			DrawRectangleRounded(tradeButton, 0.2f, 10, GRAY);
		}
		DrawText("TRADE", 360, 310, 20, BLACK);
		//stock trade end

		snprintf(fps_str, 30, "FPS: %.3f", 1 / GetFrameTime()); 
		DrawText(fps_str, 10, 10, 20, DARKGRAY); //Draw FPS Counter

		

		if (IsKeyDown(KEY_LEFT)) {
			DrawCircle(0, screen_height / 2, 50, BLACK);
		}
		if (IsKeyDown(KEY_RIGHT)) {
			DrawCircle(screen_width, screen_height / 2, 50, BLACK);
		}

		DrawStockMarket(); //Draw stock listings

		EndDrawing();
	}
	free(fps_str);
	//free(counter_txt);
	//free(counter_manastones);

	//close window
	CloseWindow();

	return 0;
}
