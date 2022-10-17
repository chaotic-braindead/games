#include "SFML\Window.hpp"
#include "SFML\Graphics.hpp"
#include "SFML\Audio.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>


using namespace sf;

const int W = 400;
const int H = 800;
const int ROWS = 10;
const int COLS = 20;
float gapX = (W / ROWS) * 0.75;
float gapY = (H / COLS) * 0.75;

std::map<unsigned int, float> score_milestones = { {1500, 0.5}, {6500, 0.4}, {14500, 0.3}, { 20000, 0.2 }, {35500, 0.15} };

Music music;

Font font;
int fontSize = 30;

Color baseColor = Color::Black;
Color outlineColor = Color::Black;


float lineWeight = 3.5;

bool paused = false;
bool stopped = false;

bool done = true;
int dx, dy = 1, random;
float timer = 0, delay;

unsigned int score = 0;

Color color;

std::map<int, bool> filledRows;

std::vector<std::vector<int>> tetriminos = {
											{0, 3}, {0, 4}, {0, 5}, {1, 4}, // T
											{0, 4}, {1, 4}, {1, 5}, {2, 5}, // S
											{0, 4}, {1, 4}, {2, 4}, {3, 4}, // I
											{0, 4}, {0, 5}, {1, 4}, {1, 5}, // O
											{0, 4}, {1, 4}, {2, 4}, {2, 5}, // L
											{0, 5}, {1, 5}, {2, 5}, {2, 4}, // Inverted L
											{0, 5}, {1, 5}, {1, 4}, {2, 4}  // Inverted S
																		  };

std::vector<std::vector<RectangleShape>> makeGrid() {
	std::vector<std::vector<RectangleShape>> grid;
	for (unsigned int i = 0; i < COLS; i++) {
		grid.push_back({});
		for (unsigned int j = 0; j < ROWS; j++) {
			RectangleShape rect(Vector2f(gapX, gapX));
			rect.setPosition(Vector2f(j * gapX, i * gapY));
			rect.setOutlineColor(Color(255, 255, 255, 128));
			rect.setOutlineThickness(2);
			rect.setFillColor(baseColor);
			grid[i].push_back(rect);
		}
	}
	return grid;
}

void placedBlocks(std::vector<std::vector<int>>& current, std::vector<std::vector<RectangleShape>> &grid) {
	for (int i = 0; i < current.size(); i++) {
		grid[current[i][0]][current[i][1]].setFillColor(color);
		grid[current[i][0]][current[i][1]].setOutlineThickness(lineWeight);
		grid[current[i][0]][current[i][1]].setOutlineColor(outlineColor);
	}
}

int c = 0;
bool check(std::vector<std::vector<RectangleShape>>& grid) {
	for (int j = 0; j < grid[0].size(); j++) {
		if (grid[0][j].getFillColor() != baseColor) {
			return false;
		}
	}

	for (int i = 0; i < grid.size(); i++) {
		bool isRowFilled = true;
		for (int j = 0; j < grid[i].size(); j++) {
			if (grid[i][j].getFillColor() == baseColor) {
				isRowFilled = false;
			}
		}
		filledRows[i] = isRowFilled;
	}

	for (int i = 0; i < grid.size(); i++) {
		if (filledRows[i] && !stopped) {
			score += 375;
		}
	}


	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size(); j++) {
			if (filledRows[i]) {
				grid[i][j].setFillColor(baseColor);
				grid[i][j].setOutlineColor(Color(255, 255, 255, 128));
				grid[i][j].setOutlineThickness(2);
				int k = i;
				while (k > 0) {
					if (grid[k - 1][j].getFillColor() != baseColor) {
						grid[k][j].setFillColor(grid[k - 1][j].getFillColor());
						grid[k][j].setOutlineThickness(grid[k - 1][j].getOutlineThickness());
						grid[k - 1][j].setFillColor(baseColor);
						grid[k][j].setOutlineColor(grid[k - 1][j].getOutlineColor());
						grid[k-1][j].setOutlineColor(Color(255, 255, 255, 128));
						grid[k - 1][j].setOutlineThickness(2);

					}
					k--;
				}

			}
		}
	}

	return true;
}

void moveHorizontally(std::vector<std::vector<int>> &current, std::vector<std::vector<RectangleShape>>& grid) {
	for (int i = 0; i < current.size(); i++) {
		if (current[i][1] + dx < 0 || current[i][1] + dx > ROWS-1) {
			return;
		}
		if (grid[current[i][0]][current[i][1] + dx].getFillColor() != baseColor) {
			return;
		}
	}	

	for (int i = 0; i < current.size(); i++) {
		current[i][1] += dx; 
	}
}

void moveDown(std::vector<std::vector<int>>& current, std::vector<std::vector<RectangleShape>>& grid) {
	if (!paused) {
		for (int i = 0; i < current.size(); i++) {
			if (current[i][0] == COLS - 1 || grid[current[i][0] + dy][current[i][1]].getFillColor() != baseColor) {
				done = true;
				placedBlocks(current, grid);
				if (!stopped) {
					score += 17;
				}
				return;
			}
		}
		for (int i = 0; i < current.size(); i++) {
			current[i][0] += dy;
		}
	}

}

void rotate(std::vector<std::vector<int>>& current, std::vector<std::vector<RectangleShape>>& grid) {
	bool isRotatable = false;
	for (int i = 0; i < current.size(); i++) {
		isRotatable = true;
		int y = current[i][0] - current[1][0];
		int x = current[i][1] - current[1][1];
		
		if (current[1][1] - y < 0 || current[1][1] - y > ROWS - 1 || current[1][0] + x < 0 || grid[current[1][0] + x][current[1][1] - y].getFillColor() != baseColor) {
			isRotatable = false;
			break;
		}
	}

	if (isRotatable) {
		for (int i = 0; i < current.size(); i++) {
			int y = current[i][0] - current[1][0];
			int x = current[i][1] - current[1][1];
			current[i][0] = current[1][0] + x;
			current[i][1] = current[1][1] - y;
		}
	}

}

int rng(int low, int high) {
	if (low > high) return high;
	return low + (std::rand() % (high - low + 1));
}

int main() {
	RenderWindow window(VideoMode(W, H), "Tetris");
	window.setFramerateLimit(60);
	if (!music.openFromFile("Audio/Original Tetris theme (Tetris Soundtrack).ogg")) {
		return 1;
	}
	music.setLoop(true);
	//music.play();
	font.loadFromFile("Font/arial.ttf");

	Text pausedMsg("Paused", font, fontSize);
	pausedMsg.setPosition(Vector2f((W / 2) - fontSize - 20, (H / 2) - 50));

	Text overMsg("Game Over", font, fontSize);
	overMsg.setPosition(Vector2f((W / 2) - fontSize - 50, (H / 2) - 50));

	Text nextMsg("Next", font, 15);
	nextMsg.setPosition(Vector2f(W - 2.25 * gapX, 3 * gapY));

	Text holdMsg("Hold", font, 15);
	holdMsg.setPosition(Vector2f(W - 2.25 * gapX, 10 * gapY));


	RectangleShape transparent(Vector2f(W, H));
	transparent.setFillColor(Color(0, 0, 0, 128));

	std::vector<std::vector<RectangleShape>> grid = makeGrid();
	std::vector<std::vector<int>> current;

	std::vector<std::vector<std::vector<int>>> placed;
	std::vector<Color> colors = { Color::Red, Color::Blue, Color::Green, Color::Magenta, Color::Yellow, Color::Cyan, Color::Color(255, 102, 0) };


	std::srand(std::time(0));
	int numColor;
	int counter = 0;
	int next = -1;
	int nextColor = -1;
	int hold = -1;
	bool held = false;
	int holdColor = -1;


	Clock clock;


	while (window.isOpen()) {
		std::string scoreString = "Score: " + std::to_string(score);
		Text scoreText(scoreString, font, 20);
		scoreText.setPosition(Vector2f(gapX / 2, (H / 2) + 7 * gapY));
		delay = 0.55;
		dx = 0;

		for (auto& e : score_milestones) {
			if (score >= e.first) {
				delay = e.second;
			}
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			if (event.type == Event::KeyPressed) {
				switch (event.key.code) {
				case Keyboard::Left:
					dx = -1;
					break;
				case Keyboard::Right:
					dx = 1;
					break;
				case Keyboard::Down:
					delay = 0.01f;
					break;
				case Keyboard::Up:
					rotate(current, grid);
					break;
				case Keyboard::Escape:
					paused = !paused;
					break;
				case Keyboard::PageUp:
					if (!held) {
						hold = random;
						holdColor = numColor;
						held = true;
						done = true;
					}
					else {
						done = true;
						next = hold;
						nextColor = holdColor;
						held = false;
						hold = -1;
						holdColor = -1;
					}
					break;
				}

			}
		}
		int counter = 0;
		if (done) {
			std::srand((unsigned int)std::time(nullptr));
			random = (next == -1) ? rng(0, 6) * 4 : next;
			numColor = rng(0, 6);
			color = (nextColor == -1) ? colors[numColor] : colors[nextColor];
			done = false;
			for (int i = random; i < random + 4; i++) {
				if (current.size() != 4) {
					current.push_back({});
					current[counter].push_back(tetriminos[i][0]);
					current[counter].push_back(tetriminos[i][1]);
				}
				else {
					current[counter][0] = tetriminos[i][0];
					current[counter][1] = tetriminos[i][1];
				}
				counter++;
			}
			next = rng(0, 6) * 4;
			nextColor = rng(0, 6);
		}

		if (!paused) {
			moveHorizontally(current, grid);
		}

		if (timer > delay) {
			moveDown(current, grid);
			timer = 0;
		}


		window.clear(baseColor);


		for (unsigned int i = 0; i < COLS; i++) {
			for (unsigned int j = 0; j < ROWS; j++) {
				window.draw(grid[i][j]);
			}
		}

		for (int a = 0; a < current.size(); a++) {
			RectangleShape square(Vector2f(gapX, gapX));
			square.setPosition(Vector2f(current[a][1] * gapX, current[a][0] * gapX));
			square.setFillColor(color);
			square.setOutlineColor(outlineColor);
			square.setOutlineThickness(lineWeight);
			window.draw(square);
		}
		if (next > -1) {
			for (int a = next; a < next + 4; a++) {
				int copy1 = tetriminos[a][1];
				int copy2 = tetriminos[a][0];
				RectangleShape square(Vector2f(gapX * 0.65, gapX * 0.65));
				square.setPosition(Vector2f((copy1 + 13) * gapX * 0.65, (copy2 + 7) * gapX * 0.65));
				square.setFillColor(colors[nextColor]);
				square.setOutlineColor(outlineColor);
				square.setOutlineThickness(1);
				window.draw(square);
			}
		}

		if (hold > -1) {

			for (int a = hold; a < hold + 4; a++) {
				int copy1 = tetriminos[a][1];
				int copy2 = tetriminos[a][0];
				RectangleShape square(Vector2f(gapX * 0.65, gapX * 0.65));
				square.setPosition(Vector2f((copy1 + 13) * gapX * 0.65, (copy2 + 17) * gapX * 0.65));
				square.setFillColor(colors[holdColor]);
				square.setOutlineColor(outlineColor);
				square.setOutlineThickness(1);
				window.draw(square);
			}


		}

		window.draw(scoreText);
		window.draw(nextMsg);
		window.draw(holdMsg);

		if (paused) {
			window.draw(transparent);
			window.draw(pausedMsg);
		}


		if (!check(grid)) {
			window.draw(transparent);
			window.draw(overMsg);
			done = false;
			stopped = true;
			next = -1;
			hold = -1;
			holdColor = -1;
			music.stop();
			if (Keyboard::isKeyPressed(Keyboard::Enter)) {
				done = true;
				stopped = false;
				grid = makeGrid();
				//music.play();
				score = 0;
			}
			if (Keyboard::isKeyPressed(Keyboard::Escape)) {
				break;
			}
		};

		window.display();
	}

		return 0;
}