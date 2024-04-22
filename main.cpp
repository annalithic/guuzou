/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and run premake 

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event 
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial 
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you 
--  wrote the original software. If you use this software in a product, an acknowledgment 
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include "raylib.h"
#include <string>

//settings
const int tileSize = 64;
const int tileRadius = tileSize / 2;

int playerRadius = 16;
float playerMaxSpeed = 1280;
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

//temp
float distToWall = 0;

//uninitialized
int mapWidth = 21;
int mapHeight = 17;
int* tiles;

void Init() {
	std::string level = "111111111111111111111110000000111000000011100000000000000000001101001110000000111001100001110000000111001111001111001100011101111111111001111001101111111111111111001111111111111111100011111110000000111110000011100000000000000010001100001110000000111001100001110000000111001111001111001100111101111111111001111111101111111111111111111111111111111111111111111";	
	tiles = new int[level.size()];
	for (int i = 0; i < level.size(); i++) { tiles[i] = level[i] - '0'; }
}

bool Blocked(int x, int y) {
	return tiles[y * mapWidth + x] != 0;
}

bool Blocked(int direction) {
	int horizontalDirection = (direction == 0 || direction == 2) ? -1 : 1;
	int verticalDirection = direction < 2 ? -1 : 1;

	return Blocked(
		(int)((playerPosX + playerRadius * horizontalDirection) / tileSize),
		(int)((playerPosY + playerRadius * verticalDirection) / tileSize)
	);
}



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

	//wait we don't need to check the left end
	float x1 = horizontalDirection == -1 ? playerPosX + moveX - playerRadius : playerPosX - playerRadius;
	float x2 = horizontalDirection ==  1 ? playerPosX + moveX + playerRadius : playerPosX + playerRadius;
	float y1 = verticalDirection   == -1 ? playerPosY + moveY - playerRadius : playerPosY - playerRadius;
	float y2 = verticalDirection   ==  1 ? playerPosY + moveY + playerRadius : playerPosY + playerRadius;


	float moveFrac = 1;
	if (horizontalVelocity != 0 || verticalVelocity != 0) { //remove later
		for (int y = (int)((y1 + tileRadius) / tileSize); y <= (int)((y2 + tileRadius) / tileSize); y++) {
			for (int x = (int)((x1 + tileRadius) / tileSize); x <= (int)((x2 + tileRadius) / tileSize); x++) {
				if (Blocked(x, y)) {
					float fracX1 = 9999999;
					float fracX2 = 9999999;

					float fracY1 = 9999999;
					float fracY2 = 9999999;

					if (moveX == 0) {
						//x overlap
						if (x * tileSize - tileRadius < playerPosX + playerRadius && x * tileSize + tileRadius > playerPosX - playerRadius) {
							fracX1 = -999999;
							//TraceLog(LOG_INFO, "%d %d X: STATIC OVERLAP", x, y);

						}
					} else {
						//TraceLog(LOG_INFO, "CHECK WALL %d %d", x, y);
						float distX = (x * tileSize - playerPosX - (playerRadius + tileRadius) * horizontalDirection) * horizontalDirection;
						fracX1 = distX / moveX * horizontalDirection;
						fracX2 = (distX + playerRadius + playerRadius + tileSize) / moveX * horizontalDirection;
						//TraceLog(LOG_INFO, "%d %d X: DISTANCE %f MOVING %f FRACTION %f %f", x, y, distX, moveX, fracX1, fracX2);
					}

					if (moveY == 0) {
						//y overlap
						if (y * tileSize - tileRadius < playerPosY + playerRadius && y * tileSize + tileRadius > playerPosY - playerRadius) {
							fracY1 = -999999;
							//TraceLog(LOG_INFO, "%d %d Y: STATIC OVERLAP", x, y);
						}
					} else {
						distToWall = (y * tileSize - playerPosY - (playerRadius + tileRadius) * verticalDirection) * verticalDirection;
						fracY1 = distToWall / moveY * verticalDirection;
						fracY2 = (distToWall + playerRadius + playerRadius + tileSize) / moveY * verticalDirection;
						//TraceLog(LOG_INFO, "%d %d Y: DISTANCE %f MOVING %f FRACTION %f %f", x, y, distToWall, moveY, fracY1, fracY2);
					}




					if (fracX1 < 1 && fracY1 < 1 && (fracX1 >= 0 || fracY1 >= 0)) {
						//enter x in between entering and exiting y, means hit horizontal wall
						if (fracX1 > fracY1) {
							//enter y before leaving x
							if (fracY1 < fracX2) {
								//TraceLog(LOG_INFO, "HIT WALL %d %d AT %f", x, y, fracX1);
								if (fracX1 < moveFrac) moveFrac = fracX1;
							}

						}
						else {
							//enter x before leaving y
							if (fracX1 < fracY2) {
								//TraceLog(LOG_INFO, "HIT CEIL %d %d AT %f", x, y, fracY1);
								if (fracY1 < moveFrac) moveFrac = fracY1;
							}
						}
					}
					


					//if (fracY1 > 1 || fracY1 < 0) continue;


					//if (fracToWall > 1 || fracToWall < 0) continue;
					//if (fracToWall < horizontalFrac) horizontalFrac = fracToWall;
					//if (fracToWall < 1) {
					//	TraceLog(LOG_INFO, "HIT WALL %d %d DISTANCE %f FRACTION %f", x, y, distToWall, fracToWall);
					//}
				}
			}
		}
	}

	//if (moveFrac < 1) {
	//	TraceLog(LOG_INFO, "MOVE CUT TO %f", moveFrac);
	//}

	/*
	float extremePosX = playerPosX + playerRadius * horizontalDirection;
	float distanceToNextTile = tileSize - fmod(extremePosX, tileSize);
	TraceLog(LOG_INFO, "%f", distanceToNextTile);

	if (distanceToNextTile < moveX) TraceLog(LOG_INFO, "moving to new tile");
	*/
	//float moveY = verticalVelocity * delta;
	


	playerPosY += moveY * moveFrac;
	playerPosX += moveX * moveFrac;
	/*
	//12
	//34
	int direction = verticalVelocity > 0 ? horizontalVelocity > 0 ? 1 : 2 : horizontalVelocity > 0 ? 3 : 4;

	playerColor = YELLOW;
	for (int i = 0; i < 4 + 4; i++) {
		int checkDirection = (direction + i) % 4;
		if (Blocked(checkDirection)) {
			TraceLog(LOG_INFO, "collision in direction %d", checkDirection);

			//playerColor = RED;
		}
	}
	*/

	//learnings:
	//can't move left/right and up/down without checking collision in between

	//if top left only is blocked, it means either we walked left into a wall above or jumped up into a wall on the left. what's difference?

	//do vertical movement, check if blocked, move out, shrink horizontal movement by reverse move proportion?
	//do horizontal movemtnt, check if blocked, move out, vertical movement independent?
	
	//stupid jump requires to get right going around corners


	/*
	int playerTileUp = (int)(playerPosY / tileSize);
	int playerTileDown = (int)((playerPosY + playerSize) / tileSize);

	int playerTileLeft = (int)(playerPosX / tileSize);
	int playerTileRight = (int)((playerPosX + playerSize) / tileSize);
	
	bool blockLeft = Blocked(playerTileLeft, playerTileUp);
	bool blockRight = Blocked(playerTileRight, playerTileUp);

	if (blockLeft && !blockRight) {
		playerTileLeft++;
		playerPosX = (playerTileLeft) * tileSize;
	} else if (blockRight && !blockLeft) {
		playerPosX = (playerTileRight) * tileSize - playerSize;
		playerTileRight--;
	}

	if (IsKeyPressed(KEY_Z)) {
		jumpQueueTimer = jumpQueueWindow;
	}


	if (Blocked(playerTileLeft, playerTileUp) || Blocked(playerTileRight, playerTileUp)) {
		playerColor = RED;
		playerPosY = playerTileUp * tileSize;
		verticalVelocity = 0;
	} else if (Blocked(playerTileLeft, playerTileDown) || Blocked(playerTileRight, playerTileDown)) {
		//grounded
		playerPosY = playerTileDown * tileSize - playerSize;


		if (jumpQueueTimer > 0) {
			verticalVelocity = jumpSpeed;
			playerPosY -= verticalVelocity * delta;
			jumpQueueTimer = -1;
		}

	} else {
		verticalVelocity -= gravity * delta;
		jumpQueueTimer -= delta;
	}

	*/





	//playerColor = jumpQueueTimer > 0 ? GREEN : YELLOW;

	// drawing
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


	DrawText(std::to_string(distToWall).c_str(), 4, 32, 20, WHITE);
	if (horizontalDirection == 1) DrawText("RIGHT", 4, 50, 20, RED);
	if (horizontalDirection == -1) DrawText("LEFT", 4, 50, 20, GREEN);


	EndDrawing();
}

int main ()
{
	Init();

	SetConfigFlags(FLAG_VSYNC_HINT);

	// set up the window
	InitWindow(1280, 1024, "Hello Raylib");
	//SetTargetFPS(6);

	// game loop
	while (!WindowShouldClose())
	{
		Loop();
	}

	// cleanup
	CloseWindow();
	return 0;
}

