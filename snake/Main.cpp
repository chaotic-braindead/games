#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>

using namespace sf;

const unsigned int W = 800, H = 800, rows = (H / 100) * 3, cols = (W / 100) * 3;
int gap = W / cols;

int rng(int low, int high) {
	if (low > high) return high;
	return low + (std::rand() % (high - low + 1));
}

class Spot {
public:
	int x;
	int y;
	int row;
	int col;

	RectangleShape square;
	Spot(int x, int y) {
		this->x = x;
		this->y = y;
		this->row = x / gap;
		this->col = y / gap;
		this->square = RectangleShape(Vector2f((float)gap, (float)gap));
		this->square.setFillColor(Color::Black);
		this->square.setPosition((float)x, (float)y);
	}
	
};

class Snake : public Spot {
public:
	std::vector<RectangleShape> tail = {};
	Snake(int x, int y) : Spot(x, y) {};

	bool move(int dx, int dy) {
		if (this->col + dx >= 0 && this->col + dx <= cols - 1) {
			this->square.move(dx * gap, 0);
			this->col += dx;
		}
		else { return true; }
		if (this->row + dy >= 0 && this->row + dy <= rows - 1) {
			this->square.move(0, dy * gap);
			this->row += dy;
		}
		else { return true; }
		RectangleShape square(Vector2f(gap, gap));
		square.setPosition(this->square.getPosition().x, this->square.getPosition().y);
		square.setOutlineThickness(2);
		square.setOutlineColor(Color::Black);

		this->tail.push_back(square);
		return false;
	}
	
	bool eat(Spot& food) {
		if (this->square.getGlobalBounds().contains(Vector2f(food.square.getPosition().x, food.square.getPosition().y))) {
			food.square.setPosition(Vector2f(gap * rng(0, 23), gap * rng(0, 23)));
			return true;
		}
		return false;
	}
	void update(Spot& food, int dx, int dy) {
		if (eat(food)) {
			RectangleShape square(Vector2f(gap, gap));
			square.setPosition(this->square.getPosition().x, this->square.getPosition().y);
			square.setOutlineThickness(2);
			square.setOutlineColor(Color::Black);
			this->tail.push_back(square);
		}
		this->tail.erase(this->tail.begin());
	}
	bool checkCollision() {
		for (int i = 2; i < this->tail.size();i++) {
			if (this->tail[0].getPosition().x == this->tail[i].getPosition().x && this->tail[0].getPosition(). y == this->tail[i].getPosition().y ) {
				return true;
			}
		}
		return false;
	}

};
std::vector<std::vector<Spot>> makeGrid() {
	std::vector<std::vector<Spot>> grid;
	for (int i = 0; i < rows; i++) {
		grid.push_back({});
		for(int j = 0; j < cols; j++){
			grid[i].push_back(Spot(i * gap, j * gap));
		}
	}
	return grid;
}


int main() {
	RenderWindow window(VideoMode(W, H), "Snake");
	window.setFramerateLimit(10);
	std::vector<std::vector<Spot>> grid = makeGrid();
	Snake player = Snake(gap * rows / 2, gap * cols / 2);
	Spot food = Spot(gap * rng(0, rows - 1), gap * (0, cols - 1));
	food.square.setFillColor(Color::Red);
	player.square.setFillColor(Color::White);
	player.square.setOutlineColor(Color::Black);
	int dx =  1, dy = 0;
	std::srand(std::time(0));
	RectangleShape first(Vector2f(gap, gap));
	first.setPosition(player.square.getPosition().x, player.square.getPosition().y);
	first.setOutlineColor(Color::Black);
	first.setOutlineThickness(2);
	player.tail.push_back(first);
	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::A) {
					if (dx == 1 && player.tail.size() > 1) { continue; }
					dy = 0; 
					dx = -1;
				}
				if (event.key.code == Keyboard::D) {
					if (dx == -1 && player.tail.size() > 1) { continue; }
					dy = 0; 
					dx = 1;
				}
				if (event.key.code == Keyboard::W) {
					if (dy == 1 && player.tail.size() > 1) { continue; }
					dx = 0; 
					dy = -1;
				}
				if (event.key.code == Keyboard::S) {
					if (dy == -1 && player.tail.size() > 1) { continue; }
					dx = 0;
					dy = 1;
				}
			}
		}
		
		if (player.move(dx, dy) || player.checkCollision()) { return 0; }
		player.update(food, dx, dy);

		window.clear();
		for (int i = 0; i < grid.size(); i++) {
			for (int j = 0; j < grid[i].size(); j++) {
				window.draw(grid[i][j].square);
			}
		}

		for (auto& e : player.tail) {
			window.draw(e);
		}
		window.draw(player.square);
		window.draw(food.square);
		window.display();

	}

	return 0;
}