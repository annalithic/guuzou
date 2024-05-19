#include "collision.h"

bool Blocked(int x, int y, int mapWidth, int mapHeight, int* tiles) {
	return tiles[y * mapWidth + x] == 1;
}

MoveResult Collision::Move(float posX, float posY, float radius, float moveX, float moveY, int mapWidth, int mapHeight, int* tiles, int tileRadius)
{
	//todo disable collision in direction if block has another block in that direction?

	float moveFrac = 1;
	int collisionResult = 0;

	int horizontalDirection = moveX == 0 ? 0 : moveX > 0 ? 1 : -1;
	int verticalDirection = moveY == 0 ? 0 : moveY > 0 ? 1 : -1;
	int tileSize = tileRadius * 2;

	float x1 = horizontalDirection == -1 ? posX + moveX - radius : posX - radius;
	float x2 = horizontalDirection == 1 ? posX + moveX + radius : posX + radius;
	float y1 = verticalDirection == -1 ? posY + moveY - radius : posY - radius;
	float y2 = verticalDirection == 1 ? posY + moveY + radius : posY + radius;

	if (moveX != 0 || moveY != 0) { //remove later
		for (int y = (int)((y1 + tileRadius) / tileSize); y <= (int)((y2 + tileRadius) / tileSize); y++) {
			for (int x = (int)((x1 + tileRadius) / tileSize); x <= (int)((x2 + tileRadius) / tileSize); x++) {
				if (Blocked(x, y, mapWidth, mapHeight, tiles)) {
					float fracX1 = 9999999, fracX2 = 9999999, fracY1 = 9999999, fracY2 = 9999999;

					if (moveX == 0) {
						//x overlap
						if (x * tileSize - tileRadius < posX + radius && x * tileSize + tileRadius > posX - radius) {
							fracX1 = -999999;
							//TraceLog(LOG_INFO, "%d %d X: STATIC OVERLAP", x, y);

						}
					}
					else {
						//TraceLog(LOG_INFO, "CHECK WALL %d %d", x, y);
						float distX = (x * tileSize - posX - (radius + tileRadius) * horizontalDirection) * horizontalDirection;
						fracX1 = distX / moveX * horizontalDirection;
						fracX2 = (distX + radius + radius + tileSize) / moveX * horizontalDirection;
						//TraceLog(LOG_INFO, "%d %d X: DISTANCE %f MOVING %f FRACTION %f %f", x, y, distX, moveX, fracX1, fracX2);
					}

					if (moveY == 0) {
						//y overlap
						if (y * tileSize - tileRadius < posY + radius && y * tileSize + tileRadius > posY - radius) {
							fracY1 = -999999;
							//TraceLog(LOG_INFO, "%d %d Y: STATIC OVERLAP", x, y);
						}
					}
					else {
						float distY = (y * tileSize - posY - (radius + tileRadius) * verticalDirection) * verticalDirection;
						fracY1 = distY / moveY * verticalDirection;
						fracY2 = (distY + radius + radius + tileSize) / moveY * verticalDirection;
						//TraceLog(LOG_INFO, "%d %d Y: DISTANCE %f MOVING %f FRACTION %f %f", x, y, distToWall, moveY, fracY1, fracY2);
					}




					if (fracX1 < 1 && fracY1 < 1 && (fracX1 >= 0 || fracY1 >= 0)) {
						//enter y before entering x - hit ceil
						if (fracY1 >= fracX1) {
							//enter x before leaving y
							if (fracX1 < fracY2) {
								//TraceLog(LOG_INFO, "HIT CEIL %d %d AT %f", x, y, fracY1);
								if (fracY1 <= moveFrac) {
									moveFrac = fracY1;
									collisionResult = verticalDirection == 1 ? 2 : 4;
								}
							}
						}
						//enter x before entering y - hit wall
						else {
							//enter y before leaving x
							if (fracY1 < fracX2) {
								//TraceLog(LOG_INFO, "HIT WALL %d %d AT %f", x, y, fracX1);
								if (fracX1 < moveFrac) {
									moveFrac = fracX1;
									collisionResult = horizontalDirection == 1 ? 1 : 3;
								}
							}

						}
					}
				}
			}
		}
	}

	return MoveResult{ collisionResult, moveFrac };
}
