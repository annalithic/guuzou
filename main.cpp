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
int playerSize = 32;
float playerMaxSpeed = 256;
float jumpSpeed = 512;
float gravity = 1200;

//initial state
float playerPosX = 20;
float playerPosY = 500;
int direction = 0;
bool grounded = true;
float verticalVelocity = 0;

void Init() {
}

void Loop() {
	float delta = GetFrameTime();
	std::string text = std::to_string(1.f/delta);

	if (IsKeyPressed(KEY_LEFT) || IsKeyReleased(KEY_RIGHT)) direction = -1;
	else if (IsKeyPressed(KEY_RIGHT) || IsKeyReleased(KEY_LEFT)) direction = 1;

	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
		playerPosX += playerMaxSpeed * delta * direction;
	}

	if (IsKeyPressed(KEY_Z) && grounded) {
		grounded = false;
		verticalVelocity = jumpSpeed;
	}

	if (!grounded) {
		playerPosY -= verticalVelocity * delta;
		if (playerPosY > 500) {
			grounded = true;
			playerPosY = 500;
		}
		verticalVelocity -= gravity * delta;
	}


	// drawing
	BeginDrawing();
	ClearBackground(BLACK);
	DrawRectangle(playerPosX, playerPosY, playerSize, playerSize, YELLOW);
	
	DrawRectangle(0, 500 + playerSize, 1280, 800, GRAY);

	DrawText(text.c_str(), 4, 4, 20, WHITE);

	EndDrawing();
}

int main ()
{
	Init();

	// set up the window
	InitWindow(1280, 800, "Hello Raylib");


	// game loop
	while (!WindowShouldClose())
	{
		Loop();
	}

	// cleanup
	CloseWindow();
	return 0;
}

