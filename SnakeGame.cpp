#include <raylib.h>
#include <iostream>
#include <deque>
#include <raymath.h>

using namespace std;

// define variables and create game objects
// game loop to update positions and to check fro collisions
//colors in raylib defined as struct Color{R, G, B, Alpha}
//Constructor used to intialize an object when it is created and has same name as class
//Destructor to unload objects
//deque is a data structure that allows you to add and remove elements from both front and back

Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43,51,24,255 };

int cellSize = 20;
int cellCount = 40;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
	for (unsigned int i = 0; i < deque.size(); i++)
	{
		if (Vector2Equals(deque[i], element)) {
			return true;
		}
		return false;
	}
}

bool eventTriggered(double interval) {
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

class Snake {
public:
	deque<Vector2> body = { Vector2{6,9}, Vector2{5,9},Vector2{4,9} }; 
	Vector2 direction = { 1,0 };
	bool addSegment = false;

	void DrawSnake() {
		for (unsigned int i = 0; i < body.size(); i++)
		{
			float x = body[i].x;
			float y = body[i].y;
			Rectangle bodyParts = Rectangle{offset+ x * cellSize,offset+ y * cellSize, (float)cellSize, (float)cellSize };
			DrawRectangleRounded(bodyParts, 0.5, 6, darkGreen);
		}
	}

	void Update() {
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment == true)
		{
			addSegment = false;
		}
		else {
			body.pop_back();
		}
	}

	void Reset() {
		body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
		direction = { 1,0 };
	}
};

class FoodForSnake {
public:
	Vector2 position;
	Texture2D texture;

	FoodForSnake(deque<Vector2> snakeBody) {
		Image image = LoadImage("C:/Users/ajha4/Downloads/applenew.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenerateRandomPos(snakeBody);
	}

	~FoodForSnake() {
		UnloadTexture(texture);
	}

	void DrawFood() {
		DrawTexture(texture, offset+position.x * cellSize, offset + position.y * cellSize, WHITE);
	}

	Vector2 GenerateRandomCell() {
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{ x,y };
	}

	Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
		
		Vector2 position = GenerateRandomCell();
		while (ElementInDeque(position, snakeBody))
		{
			position = GenerateRandomCell();
		}
		return position;
	}
};

class Game {
public:
	Snake snake = Snake();
	FoodForSnake food = FoodForSnake(snake.body);
	bool running = true;
	int score = 0;

	void Draw() {
		food.DrawFood();
		snake.DrawSnake();
	}

	void Update() {
		if (running)
		{
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdge();
			CheckCollisionWithTail();
		}
		
	}

	void CheckCollisionWithFood() {
		if (Vector2Equals(snake.body[0], food.position))
		{
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score++;
		}
	}

	void CheckCollisionWithEdge() {
		if (snake.body[0].x == cellCount || snake.body[0].x == -1)
		{
			GameOver();
		}
		else if (snake.body[0].y == cellCount || snake.body[0].y == -1)
		{
			GameOver();

		}
	}

	void GameOver() {
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);
		running = false;
	}

	void CheckCollisionWithTail() {
		for (unsigned int i = 1; i < snake.body.size(); ++i) {
			if (Vector2Equals(snake.body[0], snake.body[i])) {
				snake.Reset();
				return;
			}
		}
	}


};

int main() {
	InitWindow(2*offset + cellSize*cellCount, 2 * offset+cellCount*cellSize, "Snake Game");
	SetTargetFPS(60);

	Game game = Game();

	while (WindowShouldClose()==false) {
		BeginDrawing();
		if (eventTriggered(0.1))
		{
			game.Update();
		}

		if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
		{
			game.snake.direction = { 0,-1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
		{
			game.snake.direction = { 0,1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
		{
			game.snake.direction = { -1,0 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
		{
			game.snake.direction = { 1,0 };
			game.running = true;
		}
		ClearBackground(green);
		DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10 }, 5, darkGreen);
		DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
		DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
		game.Draw();

		EndDrawing();
	}
	CloseWindow();
}