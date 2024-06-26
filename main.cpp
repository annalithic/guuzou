#include "raylib.h"
#include "raymath.h"
#include <string>
#include "collision.h"
#include "level.h"

//settings
const int screenWidth = 1280;
const int screenHeight = 720;
const int frameRate = 144;

const int tileSize = 64;
const int tileRadius = tileSize / 2;

int playerRadius = 16;
float playerMaxSpeed = 400;
float jumpSpeed = 800;
float gravity = 2000;
float coyoteTime = 0.05;
float jumpQueueWindow = 0.1;

float cameraFollowConstant = 100;
float cameraFollowFraction = 10;

//initial state
float playerPosX = 1920;
float playerPosY = 135;
int horizontalDirection = 0;
int verticalDirection = 0;
bool grounded = false;
float horizontalVelocity = 0;
float verticalVelocity = 0;
float jumpQueueTimer = 0;
float coyoteTimer = 0;
Color playerColor = YELLOW;


//uninitialized
//int mapWidth = 21;
//int mapHeight = 17;
//int* tiles;
Camera2D camera;
Vector2 cameraPos;
Level level;

//delete later
float testPosX;
float testPosY;

void CreateLevel() {
	level = ("E:\\A\\A\\Visual Studio\\game-premake-main\\game\\level.txt");
	for (int i = 0; i < level.sizeX * level.sizeY; i++) {
		if (level.tiles[i] == 2) {
			playerPosX = i % level.sizeX * tileSize;
			playerPosY = i / level.sizeX * tileSize;
			TraceLog(LOG_INFO, "%d %d PLAYER POS", playerPosX, playerPosY);
			break;
		}
	}

}

void Init() {
	//std::string level = "111111111111111111111110000000111000000011100000000000000000001101001110000000111001100001110000000111001101001111001100011101101111111001111001101100111111111111001111110111111111100011111110000000111110000011100000000000000010001100001110000000111001100001110000000111001110000000000000111101111111111111111111101111111111111111111111111111111111111111111";	
	//tiles = new int[level.size()];
	//for (int i = 0; i < level.size(); i++) { tiles[i] = level[i] - '0'; }

	camera = Camera2D{ 0 };
	camera.offset = Vector2{ screenWidth / 2, screenHeight / 2 };
	camera.rotation = 0;
	camera.zoom = 1;

	CreateLevel();
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
	const float delta = GetFrameTime();
	std::string text = std::to_string(GetFPS());

	if (IsKeyPressed(KEY_R)) CreateLevel();

	if (IsKeyPressed(KEY_LEFT) || (IsKeyReleased(KEY_RIGHT) && IsKeyDown(KEY_LEFT))) horizontalDirection = -1;
	else if (IsKeyPressed(KEY_RIGHT) || (IsKeyReleased(KEY_LEFT) && IsKeyDown(KEY_RIGHT))) horizontalDirection = 1;

	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
		horizontalVelocity = playerMaxSpeed * horizontalDirection;
	}
	else {
		horizontalVelocity = 0;
	}
	if (grounded) {
		coyoteTimer = coyoteTime;
	}
	else {
		coyoteTimer -= delta;
	}

	jumpQueueTimer -= delta;
	if (IsKeyPressed(KEY_Z)) {
		jumpQueueTimer = jumpQueueWindow;
	}
	if (jumpQueueTimer > 0 && coyoteTimer > 0) {
		verticalVelocity = jumpSpeed * -1;
	} else {
		verticalVelocity += gravity * delta;
	}
	
	/*
	if (IsKeyPressed(KEY_UP) || IsKeyReleased(KEY_DOWN)) verticalDirection = -1;
	else if (IsKeyPressed(KEY_DOWN) || IsKeyReleased(KEY_UP)) verticalDirection = 1;
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
		verticalVelocity = playerMaxSpeed * verticalDirection;
	}
	else {
		verticalVelocity = 0;
	}
	*/

	float moveX = horizontalVelocity * delta;
	float moveY = verticalVelocity * delta;

	auto result = Collision::Move(playerPosX, playerPosY, playerRadius, moveX, moveY, level.sizeX, level.sizeY, level.tiles, tileRadius);

	playerPosY += moveY * result.fraction;
	playerPosX += moveX * result.fraction;

	grounded = false;
	//hit wall, continue vertically
	if (result.result == 1 || result.result == 3) { 
		float move2Y = moveY * (1 - result.fraction);
		result = Collision::Move(playerPosX, playerPosY, playerRadius, 0, move2Y, level.sizeX, level.sizeY, level.tiles, tileRadius);
		playerPosY += move2Y * result.fraction;
	}
	//hit ceil/floor, continue horizontally
	else if (result.result == 2 || result.result == 4) {
		if (result.result == 2) grounded = true;
		verticalVelocity = 0;
		float move2X = moveX * (1 - result.fraction);
		result = Collision::Move(playerPosX, playerPosY, playerRadius, move2X, 0, level.sizeX, level.sizeY, level.tiles, tileRadius);
		playerPosX += move2X * result.fraction;
	}

	int playerX = (int)(playerPosX + 0.5f);
	int playerY = (int)(playerPosY + 0.5f);

	if (level.tiles[(playerX + tileRadius) / tileSize + (playerY + tileRadius) / tileSize * level.sizeX] == 3) {
		CreateLevel();
	}

	//camera handling
	{
		float cameraMinX = screenWidth / 2 - tileRadius;
		float cameraMaxX = level.sizeX * tileSize - screenWidth / 2 - tileRadius;
		float cameraMinY = screenHeight / 2 - tileRadius;
		float cameraMaxY = level.sizeY * tileSize - screenHeight / 2 - tileRadius;

		Vector2 newCameraPos = Vector2{
			std::min(std::max(cameraMinX, playerPosX), cameraMaxX),
			std::min(std::max(cameraMinY, playerPosY), cameraMaxY)
		};

		float cameraDistance = Vector2Length(Vector2Subtract(newCameraPos, cameraPos));

		//testPosX = newCameraPos.x;
		//testPosY = newCameraPos.y;

		//text = std::to_string(cameraDistance);

		cameraPos = Vector2Lerp(cameraPos, newCameraPos,  std::min(1.f, (cameraFollowFraction + cameraFollowConstant / cameraDistance) * delta));

		int distanceX = (int)(cameraPos.x - playerPosX + 0.5);
		int distanceY = (int)(cameraPos.y - playerPosY + 0.5);

		//camera.target = cameraPos;
		camera.target = Vector2{ (float)(playerX + distanceX), (float)(playerY + distanceY) };
	}

	//can't move around a corner in a single frame. how do we feel about this?

	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode2D(camera);

	//playerColor = coyoteTimer > 0 ? GREEN : YELLOW;

	for (int y = 0; y < level.sizeY; y++) {
		for (int x = 0; x < level.sizeX; x++) {
			if (level.tiles[y * level.sizeX + x] == 1) {
				DrawRectangle(x * tileSize - tileRadius, y * tileSize - tileRadius, tileSize, tileSize, GRAY);
			}
			else if (level.tiles[y * level.sizeX + x] == 2) {
				DrawRectangle(x* tileSize - tileRadius, y* tileSize - tileRadius, tileSize, tileSize, BLUE);
			}
			else if (level.tiles[y * level.sizeX + x] == 3) {
				DrawRectangle(x * tileSize - tileRadius, y * tileSize - tileRadius, tileSize, tileSize, RED);
			}
			//char number[7];
			//sprintf(number, "%d,%d", x, y);
			//DrawText(number, x * tileSize - 7, y * tileSize - 4, 8, WHITE);
		}
	}

	//DrawRectangle(x1, y1, x2 - x1, y2 - y1, GREEN);
	
	DrawRectangle(playerX - playerRadius, playerY - playerRadius, playerRadius * 2, playerRadius * 2, playerColor);
	

	//DrawCircle(testPosX, testPosY, 8, RED);

	EndMode2D();

	//DrawRectangle(0, 500 + playerSize, 1280, 800, GRAY);
	
	DrawText(text.c_str(), 4, 4, 20, WHITE);


	DrawText(std::to_string(playerPosX).c_str(), 128, 4, 20, WHITE);
	DrawText(std::to_string(playerPosY).c_str(), 128, 32, 20, WHITE);
	DrawText(std::to_string(cameraPos.x - playerPosX).c_str(), 128, 60, 20, WHITE);
	DrawText(std::to_string(cameraPos.y - playerPosY).c_str(), 128, 88, 20, WHITE);
	DrawText(std::to_string(camera.target.x - playerX).c_str(), 256, 60, 20, WHITE);
	DrawText(std::to_string(camera.target.y - playerY).c_str(), 256, 88, 20, WHITE);


	if (horizontalDirection == 1) DrawText("RIGHT", 4, 50, 20, RED);
	if (horizontalDirection == -1) DrawText("LEFT", 4, 50, 20, GREEN);


	EndDrawing();
}

int main ()
{
	Init();

	if(frameRate == 144) SetConfigFlags(FLAG_VSYNC_HINT);

	// set up the window
	InitWindow(screenWidth, screenHeight, "Hello Raylib");
	if (frameRate != 0 && frameRate != 144) SetTargetFPS(frameRate);

	// game loop
	while (!WindowShouldClose())
	{
		Loop();
	}

	// cleanup
	CloseWindow();
	return 0;
}

