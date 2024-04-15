#include <raylib.h>
#include <iostream>
#include <utility>
using namespace std;
#include <vector>
//Rules of the game
//1. a live cell with fewer than two live neighbours die
//2. live cell with two or three live neighbours lives on to the next generation
//3. live cell with more than three live neighbours dies
//4. dead cell with exactly three live neighbours becomes a live cell
class Grid
{
public:
	Grid(int width, int height, int cellSize) :
		rows(height / cellSize), columns(width / cellSize),
		cellSize(cellSize), cells(rows, std::vector<int>(columns, 0)) {};
	void Draw();
	void SetValue(int row, int column, int value);
	int GetValue(int row, int column);
	bool IsWithinBounds(int row, int column);
	int GetRows() { 
		return rows;
	}

	int GetColumns() {
		return columns;
	}

	void FillRandom();
	void Clear();
	void ToggleCell(int row, int column);

private:
	int rows;
	int columns;
	int cellSize;
	std::vector<std::vector<int>> cells;
};

class Simulation
{
public:
	Simulation(int width, int height, int cellSize) :
		grid(width, height, cellSize), tempGrid(width, height, cellSize), run(false) {};
	void Draw();
	void SetCellValue(int row, int column, int value);
	int CountLiveNeighbours(int row, int column);
	void Update();
	bool IsRunning() const {
		return run;
	}

	void Start() { run = true; }
	void Stop() { run = false; }
	void ClearGrid();
	void CreateRandomState();
	void ToggleCell(int row, int column);

private:
	Grid grid;
	Grid tempGrid;
	bool run;
};

void Simulation::Draw()
{
	grid.Draw();
}

void Simulation::SetCellValue(int row, int column, int value) {
	grid.SetValue(row, column, value);
}

int Simulation::CountLiveNeighbours(int row, int column)
{
	int liveNeighbours = 0;
	std::vector<std::pair<int, int>> neighbourOffsets = {
		{-1, 0 }, //above
		{1, 0 }, //below
		{0, -1}, //left
		{0, 1}, //right
		{-1, -1}, //upper left
		{-1, 1}, //upper right
		{1,-1}, //lower left
		{1, 1} //lower right
	};

	for (const auto& offset : neighbourOffsets)
	{
		int neighbourRow = (row + offset.first + grid.GetRows()) % grid.GetRows();
		int neighbourColumn = (column + offset.second + grid.GetColumns()) % grid.GetColumns();
		liveNeighbours += grid.GetValue(neighbourRow, neighbourColumn);
	}
	return liveNeighbours;
}

void Simulation::Update()
{
	if (IsRunning())
	{
		for (int row = 0; row < grid.GetRows(); row++)
		{
			for (int column = 0; column < grid.GetColumns(); column++) {

				int liveNeighbours = CountLiveNeighbours(row, column);
				int cellValue = grid.GetValue(row, column);

				if (cellValue == 1)
				{
					if (liveNeighbours > 3 || liveNeighbours < 2) {
						tempGrid.SetValue(row, column, 0);
					}
					else {
						tempGrid.SetValue(row, column, 1);
					}
				}
				else {
					if (liveNeighbours == 3) {
						tempGrid.SetValue(row, column, 1);
					}
					else {
						tempGrid.SetValue(row, column, 0);
					}
				}
			}
		}
		grid = tempGrid;
	}
}

void Simulation::ClearGrid()
{
	if (!IsRunning())
	{
		grid.Clear();
	}
}

void Simulation::CreateRandomState()
{
	if (!IsRunning())
	{
		grid.FillRandom();
	}
}

void Simulation::ToggleCell(int row, int column)
{
	if (!IsRunning())
	{
		grid.ToggleCell(row, column);
	}
}


void Grid::Draw() {
	for (int row = 0; row < rows; row++)
	{
		for (int column = 0; column < columns; column++) {
			Color color = cells[row][column] ? Color{ 255,0,0,255 } : Color{ 0,55,55,255 };
			DrawRectangle(column * cellSize, row * cellSize, cellSize - 1, cellSize - 1, color);
		}
	}
}

void Grid::SetValue(int row, int column, int value) {
	if (IsWithinBounds(row,column))
	{
		cells[row][column] = value;
	}
}

int Grid::GetValue(int row, int column)
{
	if (IsWithinBounds(row, column))
	{
		return cells[row][column];
	}
	return 0;
}

bool Grid::IsWithinBounds(int row, int column)
{
	if (row >= 0 && row < rows && column >= 0 && column < columns)
	{
		return true;
	}
	return false;
}

void Grid::FillRandom()
{
	for (int row = 0; row < rows; row++)
	{
		for (int column = 0; column < columns; column++) {

			int randomValue = GetRandomValue(0, 4);
			cells[row][column] = (randomValue == 4) ? 1 : 0;
		}
	}
}

void Grid::Clear()
{
	for (int row = 0; row < rows; row++)
	{
		for (int column = 0; column < columns; column++) {
			cells[row][column] = 0;
		}
	}
}

void Grid::ToggleCell(int row, int column)
{
	if (IsWithinBounds(row,column))
	{
		cells[row][column] = !cells[row][column];

	}
}

int main() {
	Color GREY = { 29,29,29,255 };
	const int WINDOW_WIDTH = 1600;
	const int WINDOW_HEIGHT = 1000;
	const int CELL_SIZE = 4;
	int fps = 12;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Conway");
	InitAudioDevice();

	Music music = LoadMusicStream("C:/Users/ajha4/Downloads/neon-gaming-128925.mp3");

	PlayMusicStream(music);

	SetTargetFPS(fps);
	Simulation simulation{ WINDOW_WIDTH,WINDOW_HEIGHT,CELL_SIZE };


	while (WindowShouldClose() == false)
	{
		//1. Event Handling
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			Vector2 mousePosition = GetMousePosition();
			int row = mousePosition.y / CELL_SIZE;
			int column = mousePosition.x / CELL_SIZE;
			simulation.ToggleCell(row,column);

		}
		if (IsKeyPressed(KEY_ENTER))
		{
			simulation.Start();
			SetWindowTitle("Conway's Game of Life is Running...");
		}
		else if (IsKeyPressed(KEY_SPACE)) {
			simulation.Stop();
			SetWindowTitle("Conway's Game of Life has stopped running...");
		}
		else if (IsKeyPressed(KEY_F))
		{
			fps += 2;
			SetTargetFPS(fps);
		}
		else if (IsKeyPressed(KEY_S))
		{
			if (fps > 5) {
				fps -= 2;
				SetTargetFPS(fps);
			}
		}
		else if(IsKeyPressed(KEY_R))
		{
			simulation.CreateRandomState();
		}
		else if (IsKeyPressed(KEY_C))
		{
			simulation.ClearGrid();
		}
		
		//2. Music
		UpdateMusicStream(music);

		//3. Updating state
		simulation.Update();

		//4. Drawing
		BeginDrawing();
		ClearBackground(GREY);
		simulation.Draw();
		EndDrawing();
	}
	UnloadMusicStream(music);
	CloseAudioDevice();
	CloseWindow();
}