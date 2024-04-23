#include "raylib.h"
#include <string>
#include "collision.h"

//settings
const int tileSize = 64;
const int tileRadius = tileSize / 2;

int playerRadius = 16;
float playerMaxSpeed = 256;
float jumpSpeed = 700;
float gravity = 2000;
float jumpQueueWindow = 0.1;

//initial state
float playerPosX = 200;
float playerPosY = 135;
int horizontalDirection = 0;
int verticalDirection = 0;
bool grounded = true;
float horizontalVelocity = 0;
float verticalVelocity = 0;
float jumpQueueTimer = 0;
Color playerColor = YELLOW;

//uninitialized
int mapWidth = 21;
int mapHeight = 17;
int* tiles;

void Init() {
	std::string level = "111111111111111111111110000000111000000011100000000000000000001101001110000000111001100001110000000111001111001111001100011101111111111001111001101111111111111111001111111111111111100011111110000000111110000011100000000000000010001100001110000000111001100001110000000111001111001111001100111101111111111001111111101111111111111111111111111111111111111111111";	
	tiles = new int[level.size()];
	for (int i = 0; i < level.size(); i++) { tiles[i] = level[i] - '0'; }
}


/*
bool Blocked(int direction) {
	int horizontalDirection = (direction == 0 || direction == 2) ? -1 : 1;
	int verticalDirection = direction < 2 ? -1 : 1;

	return Blocked(
		(int)((playerPosX + playerRadius * horizontalDirection) / tileSize),
		(int)((playerPosY + playerRadius * verticalDirection) / tileSize)
	);
}
*/


void TileCollide(int x, int y) {

}

void Loop() {
	float delta = GetFrameTime();
	std::string text = std::to_string(GetFPS());

	if (IsKeyPressed(KEY_LEFT) || (IsKeyReleased(KEY_RIGHT) && IsKeyDown(KEY_LEFT))) horizontalDirection = -1;
	else if (IsKeyPressed(KEY_RIGHT) || (IsKeyReleased(KEY_LEFT) && IsKeyDown(KEY_RIGHT))) horizontalDirection = 1;

	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
		horizontalVelocity = playerMaxSpeed * horizontalDirection;
	}
	else {
		horizontalVelocity = 0;
	}
	
	if (IsKeyPressed(KEY_UP) || IsKeyReleased(KEY_DOWN)) verticalDirection = -1;
	else if (IsKeyPressed(KEY_DOWN) || IsKeyReleased(KEY_UP)) verticalDirection = 1;
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
		verticalVelocity = playerMaxSpeed * verticalDirection;
	}
	else {
		verticalVelocity = 0;
	}

	float moveX = horizontalVelocity * delta;
	float moveY = verticalVelocity * delta;

	auto result = Collision::Move(playerPosX, playerPosY, playerRadius, moveX, moveY, mapWidth, mapHeight, tiles, tileRadius);

	playerPosY += moveY * result.fraction;
	playerPosX += moveX * result.fraction;

	//hit wall, continue vertically
	if (result.result == 1) { 
		float move2Y = moveY * (1 - result.fraction);
		result = Collision::Move(playerPosX, playerPosY, playerRadius, 0, move2Y, mapWidth, mapHeight, tiles, tileRadius);
		playerPosY += move2Y * result.fraction;
	}
	//hit ceil/floor, continue horizontally
	else if (result.result == 2) {
		float move2X = moveX * (1 - result.fraction);
		result = Collision::Move(playerPosX, playerPosY, playerRadius, move2X, 0, mapWidth, mapHeight, tiles, tileRadius);
		playerPosX += move2X * result.fraction;
	}

	//can't move around a corner in a single frame. how do we feel about this?

	BeginDrawing();
	ClearBackground(BLACK);


	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			if (tiles[y * mapWidth + x] != 0) {
				DrawRectangle(x * tileSize - tileRadius, y * tileSize - tileRadius, tileSize, tileSize, GRAY);
			}
			char number[7];
			sprintf(number, "%d,%d", x, y);
			DrawText(number, x * tileSize - 7, y * tileSize - 4, 8, WHITE);
		}
	}

	//DrawRectangle(x1, y1, x2 - x1, y2 - y1, GREEN);

	DrawRectangle(playerPosX - playerRadius, playerPosY - playerRadius, playerRadius * 2, playerRadius * 2, playerColor);


	//DrawRectangle(0, 500 + playerSize, 1280, 800, GRAY);
	
	DrawText(std::to_string(playerPosX).c_str(), 128, 4, 20, WHITE);
	DrawText(std::to_string(playerPosY).c_str(), 128, 32, 20, WHITE);

	DrawText(text.c_str(), 64, 4, 20, WHITE);

	if (horizontalDirection == 1) DrawText("RIGHT", 4, 50, 20, RED);
	if (horizontalDirection == -1) DrawText("LEFT", 4, 50, 20, GREEN);


	EndDrawing();
}

int main ()
{
	Init();

	//SetConfigFlags(FLAG_VSYNC_HINT);

	// set up the window
	InitWindow(1280, 1024, "Hello Raylib");
	//SetTargetFPS(30);

	// game loop
	while (!WindowShouldClose())
	{
		Loop();
	}

	// cleanup
	CloseWindow();
	return 0;
}

