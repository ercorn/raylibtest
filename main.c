#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>
#include <stdbool.h>

/*
	TODO: Add ability to sell stocks and split money into net worth and actual money.
		-money => net worth
		-money <= actual money based on stocks sold and starting deposit(currently 1000)
	TODO: GOING WITH STOCK TRADING GAME. Plans in gpt. general idea is stock trading clicker where the end goal is to retire rich.
	DONE: Figure out how to have a variable autoincrement every second. The current plan is to have each manastone autoincrement the click_counter
	by the total number of manastones every second.
		ex: 1 manastone   = 1  clicks/sec
			2 manastones  = 2  clicks/sec
			39 manastones = 39 clicks/sec
*/

void UpdateStockPrices(void);
void UpdateGame(float);
void DrawStockMarket(void);
void AddTickerMessage(const char *message, Color color);

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

Stock stocks[MAX_STOCKS] = {
	{"Tech Co", 100, 0, 5, 0},
	{"Auto Inc", 250, 0, 15, 0},
	{"Bank Corp", 500, 0, 30, 0},
	{"Textio", 750, 0, 45, 0},
	{"Boot.dev", 1000, 0, 60, 0}
};

typedef struct {
	char message[100];
	Color color;
} TickerMessage;

TickerMessage ticker_messages[MAX_TICKER_MESSAGES];

int ticker_x = 800; //ticker starting x position
int click_counter = 0;
int mana_stonecost = 25;
int mana_stones = 0;
int money = 1000;
int net_worth = 1000;
int passive_income = 0;
int max_click_profit = 20;
int min_click_profit = -5;
float timer = 0.0;
float price_update_timer = 0.0;
bool game_already_running = false;

void UpdateGame(float deltaTime) {
	timer += deltaTime;
	price_update_timer += deltaTime;

	if (timer >= 1.0) {
		money += passive_income;
		timer = 0.0;
	}

	if (!game_already_running) {
		UpdateStockPrices();
		game_already_running = true;
	}

	if (price_update_timer >= 5.0) {
		UpdateStockPrices();
		price_update_timer = 0.0;
	}
}

void UpdateStockPrices() {
	for (int i = 0; i < MAX_STOCKS; i++) {
		//fluctuate stock price
		int flux = (rand() % 21) - 10;
		int price_change = (stocks[i].base_cost * flux) / 100;

		//small % chance of major event maybe? 
		//...

		stocks[i].price += price_change;

		//clamp price to make sure it doesn't drop too low
		if (stocks[i].price < (stocks[i].base_cost * 0.75)) {
			stocks[i].price = stocks[i].base_cost * 0.75;
		}

		/*//clamp price to make sure it doesn't get too high
		if (stocks[i].price > (stocks[i].base_cost * 1.25)) {
			stocks[i].price = stocks[i].base_cost * 1.25;
		}
		*/
		if (rand() % 100 < 10) { //% chance to have a massive event
			stocks[i].price *= 4;
		}
		if (rand() % 100 < 50) {
			stocks[i].price /= 2;
		}
		stocks[i].income_per_sec = (int)(stocks[i].price * 0.1);

		//add to ticker
		char message[100];
		snprintf(message, 100, "%s price %s by $%d", stocks[i].name, (price_change >= 0) ? "up" : "down", abs(price_change));
		Color message_color = (price_change >= 0) ? GREEN : RED;
		AddTickerMessage(message, message_color);
	}
}

void DrawStockMarket() {
	int x = 10; //starting pos x, y for stock listings
	int y = 70;
	char stock_title[100];
	snprintf(stock_title, 100, "Stock Market - Money: $%d Net Worth: $%d", money, net_worth);
	DrawText(stock_title, x, y - 30, 20, GOLD);
	DrawRectangle(x - 10, y - 10, 605, 40 * MAX_STOCKS, Fade(DARKGRAY, 0.5f));

	for (int i = 0; i < MAX_STOCKS; i++) {
		char stockInfo[100];
		Color price_color;
		if (stocks[i].price >= stocks[i].base_cost) { //stock price is green if above base cost
			price_color = GREEN;
		} else {
			price_color = RED; //red if below base cost
		}
		snprintf(stockInfo, 100, "%s - $%d | +$%d/sec | Owned: %d",
			     stocks[i].name, stocks[i].price, stocks[i].income_per_sec, stocks[i].owned);
		DrawText(stockInfo, x, y, 20, price_color);

		//Draw Buy Button
		Rectangle buyButton = {x + 410, y - 5, 80, 30};
		DrawRectangleRec(buyButton, DARKGRAY);
		DrawText("BUY", x + 430, y + 5, 20, WHITE);
		//Draw Sell Button
		Rectangle sellButton = {x + 510, y - 5, 80, 30};
		DrawRectangleRec(sellButton, DARKGRAY);
		DrawText("SELL", x + 530, y + 5, 20, WHITE);

		//Check buy button click
		if (CheckCollisionPointRec(GetMousePosition(), buyButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (money >= stocks[i].price) {
				money -= stocks[i].price;
				stocks[i].owned++;
				//passive_income += stocks[i].income_per_sec;
			}
		}
		//Check sell button click
		if (CheckCollisionPointRec(GetMousePosition(), sellButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (stocks[i].owned >= 1) {
				stocks[i].owned -= 1;
				money += stocks[i].price;
				//passive_income -= stocks[i].income_per_sec;
			}
		}

		y += 40; //Next stock gets drawn lower down
	}
	int new_passive_income = 0;
	int new_net_worth = 0;
	for (int i = 0; i < MAX_STOCKS; i++) {
		new_passive_income += stocks[i].income_per_sec * stocks[i].owned;
		new_net_worth += stocks[i].price * stocks[i].owned;
	}
	passive_income = new_passive_income;
	net_worth = new_net_worth + money;
}

void AddTickerMessage(const char *message, Color color) {
	//shift existing messages over to open up space for the new one
	for (int i = MAX_TICKER_MESSAGES - 1; i > 0; i--) { //starting from the end
		ticker_messages[i] = ticker_messages[i - 1];
	}
	//add new message to beginning
	strncpy(ticker_messages[0].message, message, sizeof(ticker_messages[0].message));
	ticker_messages[0].color = color;
}

void DrawStockTicker() {
	int y = 20;
	ticker_x -= 2;

	//reset ticker pos when text goes off screen
	if (ticker_x < -1000) {
		ticker_x = 800;
	}

	for (int i = 0;i < MAX_TICKER_MESSAGES; i++) {
		if(strlen(ticker_messages[i].message) > 0) {
			DrawText(ticker_messages[i].message, ticker_x + i * 325, y, 20, ticker_messages[i].color);
		}
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
		ClearBackground(BLACK);

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

		DrawStockTicker();
		DrawStockMarket(); //Draw stock listings

		if (net_worth >= GOAL_MONEY) {
			DrawText("You became a millionaire!", 100, 300, 50, GOLD);
		}

		EndDrawing();
	}
	free(fps_str);
	//free(counter_txt);
	//free(counter_manastones);

	//close window
	CloseWindow();

	return 0;
}
