#include <assert.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
				TODO: Add ability to sell stocks and split Game_State.money into net
	 worth and actual Game_State.money. -Game_State.money => net worth
								-Game_State.money <= actual Game_State.money based on stocks
	 sold and starting deposit(currently 1000)
				TODO: GOING WITH STOCK TRADING GAME. Plans in gpt. general idea is stock
	 trading clicker where the end goal is to retire rich. DONE: Figure out how to
	 have a variable autoincrement every second. The current plan is to have each
	 manastone autoincrement the click_counter by the total number of manastones
	 every second. ex: 1 manastone   = 1  clicks/sec 2 manastones  = 2  clicks/sec
												39 manastones = 39 clicks/sec
*/

#define MAX_STOCKS 5
#define MAX_TICKER_MESSAGES 5
#define GOAL_MONEY 1000000

typedef struct {
	char *name;
	int base_cost;
	int price;
	int income_per_sec;
	int owned;
} Stock;

typedef struct {
	char message[100];
	Color color;
} TickerMessage;

typedef struct {
	int money;
	int net_worth;
	int passive_income;
	float timer;
	float price_update_timer;

	Stock stocks[MAX_STOCKS];
	TickerMessage ticker_messages[MAX_TICKER_MESSAGES];
	int ticker_x; // ticker starting x position

	// TODO: RNG state
} Game_State;

void game_init(Game_State *g_state);
void UpdateGame(Game_State *g_state, float deltaTime);
void UpdateStockPrices(Game_State *g_state);
void DrawStockMarket(Game_State *g_state);
void DrawStockTicker(Game_State *g_state);
void AddTickerMessage(Game_State *g_state, const char *message, Color color);

int click_counter = 0;
int mana_stonecost = 25;
int mana_stones = 0;
int max_click_profit = 20;
int min_click_profit = -5;

bool game_already_running = false;

void game_init(Game_State *g_state) {
	assert(g_state != NULL && "g_state should not be null");
	memset(g_state, 0, sizeof(Game_State));

	g_state->money = 1000;
	g_state->net_worth = 1000;
	g_state->passive_income = 0;
	g_state->timer = 0.0;
	g_state->price_update_timer = 0.0;
	g_state->ticker_x = 800;

	g_state->stocks[0] = (Stock){"Tech Co", 100, 100, 5, 0};
	g_state->stocks[1] = (Stock){"Auto Inc", 250, 250, 15, 0};
	g_state->stocks[2] = (Stock){"Bank Corp", 500, 500, 30, 0};
	g_state->stocks[3] = (Stock){"Textio", 750, 750, 45, 0};
	g_state->stocks[4] = (Stock){"Boot.dev", 1000, 1000, 60, 0};
}

void UpdateGame(Game_State *g_state, float deltaTime) {
	assert(g_state != NULL && "g_state should not be null");
	g_state->timer += deltaTime;
	g_state->price_update_timer += deltaTime;

	if (g_state->timer >= 1.0) {
		g_state->money += g_state->passive_income;
		g_state->timer = 0.0;
	}

	if (!game_already_running) {
		UpdateStockPrices(g_state);
		game_already_running = true;
	}

	if (g_state->price_update_timer >= 5.0) {
		UpdateStockPrices(g_state);
		g_state->price_update_timer = 0.0;
	}

	assert(g_state->price_update_timer >= 0.0 && g_state->timer >= 0.0 && "Timers should never be negative");
}

void UpdateStockPrices(Game_State *g_state) {
	assert(g_state != NULL && "g_state should not be null");
	for (int i = 0; i < MAX_STOCKS; i++) {
		// fluctuate stock price
		int flux = (rand() % 21) - 10;
		int price_change = (g_state->stocks[i].base_cost * flux) / 100;

		// small % chance of major event maybe?
		//...

		g_state->stocks[i].price += price_change;

		// clamp price to make sure it doesn't drop too low
		if (g_state->stocks[i].price < (g_state->stocks[i].base_cost * 0.75)) {
			g_state->stocks[i].price = g_state->stocks[i].base_cost * 0.75;
		}

		/*//clamp price to make sure it doesn't get too high
		if (stocks[i].price > (stocks[i].base_cost * 1.25)) {
						stocks[i].price = stocks[i].base_cost * 1.25;
		}
		*/
		if (rand() % 100 < 10) { //% chance to have a massive event
			g_state->stocks[i].price *= 4;
		}
		if (rand() % 100 < 50) {
			g_state->stocks[i].price /= 2;
		}
		g_state->stocks[i].income_per_sec = (int)(g_state->stocks[i].price * 0.1);

		// add to ticker
		char message[100];
		snprintf(message, 100, "%s price %s by $%d", g_state->stocks[i].name,
				 (price_change >= 0) ? "up" : "down", abs(price_change));
		Color message_color = (price_change >= 0) ? GREEN : RED;
		AddTickerMessage(g_state, message, message_color);

		assert(g_state->stocks[i].price >= 0.0);
	}
}

void DrawStockMarket(Game_State *g_state) {
	int x = 10; // starting pos x, y for stock listings
	int y = 70;
	char stock_title[100];
	snprintf(stock_title, 100, "Stock Market - Money: $%d Net Worth: $%d",
			 g_state->money, g_state->net_worth);
	DrawText(stock_title, x, y - 30, 20, GOLD);
	DrawRectangle(x - 10, y - 10, 605, 40 * MAX_STOCKS, Fade(DARKGRAY, 0.5f));
	Vector2 mouse_pos = GetMousePosition();

	for (int i = 0; i < MAX_STOCKS; i++) {
		char stockInfo[100];
		Color price_color;
		if (g_state->stocks[i].price >=
			g_state->stocks[i]
				.base_cost) { // stock price is green if above base cost
			price_color = GREEN;
		} else {
			price_color = RED; // red if below base cost
		}
		snprintf(stockInfo, 100, "%s - $%d | +$%d/sec | Owned: %d",
				 g_state->stocks[i].name, g_state->stocks[i].price,
				 g_state->stocks[i].income_per_sec, g_state->stocks[i].owned);
		DrawText(stockInfo, x, y, 20, price_color);

		// Draw Buy Button
		Rectangle buyButton = {x + 410, y - 5, 80, 30};

		// change color if moused over
		if (CheckCollisionPointRec(mouse_pos, buyButton)) {
			DrawRectangleRec(buyButton, LIGHTGRAY);
		} else {
			DrawRectangleRec(buyButton, DARKGRAY);
		}

		DrawText("BUY", x + 430, y + 5, 20, WHITE);
		// Draw Sell Button
		Rectangle sellButton = {x + 510, y - 5, 80, 30};

		// change color if moused over
		if (CheckCollisionPointRec(mouse_pos, sellButton)) {
			DrawRectangleRec(sellButton, LIGHTGRAY);
		} else {
			DrawRectangleRec(sellButton, DARKGRAY);
		}

		DrawText("SELL", x + 530, y + 5, 20, WHITE);

		// Check buy button click
		if (CheckCollisionPointRec(GetMousePosition(), buyButton) &&
			IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (g_state->money >= g_state->stocks[i].price) {
				g_state->money -= g_state->stocks[i].price;
				g_state->stocks[i].owned++;
				// passive_income += stocks[i].income_per_sec;
			}
		}
		// Check sell button click
		if (CheckCollisionPointRec(GetMousePosition(), sellButton) &&
			IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (g_state->stocks[i].owned >= 1) {
				g_state->stocks[i].owned -= 1;
				g_state->money += g_state->stocks[i].price;
				// passive_income -= stocks[i].income_per_sec;
			}
		}

		y += 40; // Next stock gets drawn lower down
	}
	int new_passive_income = 0;
	int new_net_worth = 0;
	for (int i = 0; i < MAX_STOCKS; i++) {
		new_passive_income +=
			g_state->stocks[i].income_per_sec * g_state->stocks[i].owned;
		new_net_worth += g_state->stocks[i].price * g_state->stocks[i].owned;
	}
	if (g_state->net_worth < GOAL_MONEY) {
		g_state->passive_income = new_passive_income;
		g_state->net_worth = new_net_worth + g_state->money;
	}
}

void AddTickerMessage(Game_State *g_state, const char *message, Color color) {
	// shift existing messages over to open up space for the new one
	for (int i = MAX_TICKER_MESSAGES - 1; i > 0; i--) { // starting from the end
		g_state->ticker_messages[i] = g_state->ticker_messages[i - 1];
	}
	// add new message to beginning
	strncpy(g_state->ticker_messages[0].message, message,
			sizeof(g_state->ticker_messages[0].message));
	g_state->ticker_messages[0].color = color;
}

void DrawStockTicker(Game_State *g_state) {
	int y = 20;
	g_state->ticker_x -= 2;

	// reset ticker pos when text goes off screen
	if (g_state->ticker_x < -1000) {
		g_state->ticker_x = 800;
	}

	for (int i = 0; i < MAX_TICKER_MESSAGES; i++) {
		if (strlen(g_state->ticker_messages[i].message) > 0) {
			DrawText(g_state->ticker_messages[i].message, g_state->ticker_x + i * 325,
					 y, 20, g_state->ticker_messages[i].color);
		}
	}
}
int main() {
	// init game state
	Game_State g_state;
	game_init(&g_state);

	// init window
	const int screen_width = 800;
	const int screen_height = 600;

	InitWindow(screen_width, screen_height, "test window");

	SetTargetFPS(60); // Cap/Limit FPS

	srand((int)GetTime());

	// Rectangle clicker_rect = (Rectangle) { .height = 50, .width = 100, .x =
	// screen_width - 110, .y = screen_height - 60}; Rectangle manastone_rect =
	// (Rectangle) {.height = 50, .width = 100, .x = screen_width - 110, .y =
	// screen_height - 120};
	Rectangle tradeButton = {300, 300, 200, 50}; // stock trading button
	char *fps_str = malloc(30 * sizeof(char));
	// char *counter_txt = malloc(20 * sizeof(char));
	// char *counter_manastones = malloc(20 * sizeof(char));
	// char *counter_money = malloc(20 * sizeof(char));

	double old_time = GetTime();

	// run window
	while (!WindowShouldClose()) {

		// ESC to close window
		if (IsKeyPressed(KEY_Q) && IsKeyPressed(KEY_LEFT_SHIFT)) {
			break;
		}

		float deltaTime = GetFrameTime();
		UpdateGame(&g_state, deltaTime);

		Vector2 mouse_pos = GetMousePosition();

		// double current_time = GetTime();

		BeginDrawing();
		// draw stuff
		ClearBackground(BLACK);

		// stock trade start
		if (CheckCollisionPointRec(mouse_pos, tradeButton)) {
			DrawRectangleRounded(tradeButton, 0.2f, 10, LIGHTGRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				g_state.money += (rand() % (max_click_profit - min_click_profit + 1)) +
								 min_click_profit;
			}
		} else {
			DrawRectangleRounded(tradeButton, 0.2f, 10, GRAY);
		}
		DrawText("TRADE", 360, 310, 20, BLACK);
		// stock trade end

		snprintf(fps_str, 30, "FPS: %.3f", 1 / GetFrameTime());
		DrawText(fps_str, 10, 10, 20, DARKGRAY); // Draw FPS Counter

		if (IsKeyDown(KEY_LEFT)) {
			DrawCircle(0, screen_height / 2, 50, BLACK);
		}
		if (IsKeyDown(KEY_RIGHT)) {
			DrawCircle(screen_width, screen_height / 2, 50, BLACK);
		}

		DrawStockTicker(&g_state);
		DrawStockMarket(&g_state); // Draw stock listings

		if (g_state.net_worth >= GOAL_MONEY) {
			DrawText("You became a millionaire!", 100, 300, 50, GOLD);
		}

		EndDrawing();
	}
	free(fps_str);
	// free(counter_txt);
	// free(counter_manastones);

	// close window
	CloseWindow();

	return 0;
}
