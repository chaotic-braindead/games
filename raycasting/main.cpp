#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#define PI 3.141592
using namespace sf;

const unsigned int W = 800, H = 800, rows = (W / 100) * 2, cols = (H / 100) * 2, gapX = W / rows, gapY = H / cols;
float speed = 3.5f, pSize = 10.f, moveX, moveY, angle, dx, dy;

typedef struct {
	int w;
	int a;
	int s;
	int d;
} PressedKeys;

PressedKeys keys;

std::vector<std::string> map = { "################",
								"#   #          #",
								"#   #          #",
								"#   #          #",
								"#              #",
								"#              #",
								"#              #",
								"#        # #   #",
								"#        ###   #",
								"#              #",
								"####           #",
								"#              #",
								"#  #           #",
								"#  #           #",
								"#  #           #",
								"################" };

		
std::vector<std::vector<RectangleShape>> createGrid() {
	std::vector<std::vector<RectangleShape>> grid;
	for (unsigned int i = 0; i < rows; i++) {
		grid.push_back({});
		for (unsigned int j = 0; j < cols; j++) {
			RectangleShape square(Vector2f(gapX, gapX));
			square.setPosition(Vector2f(j * gapX, i * gapY));
			if (map[i][j] == '#') {
				square.setFillColor(Color::White);
				square.setOutlineColor(Color::Black);
				square.setOutlineThickness(1);
			}
			else {
				square.setFillColor(Color::Black);
				square.setOutlineColor(Color(255, 255, 255, 128));
				square.setOutlineThickness(1);
			}
			grid[i].push_back(square);
		}
	}
	return grid;
}

void checkCollision(CircleShape& player, std::vector<std::vector<RectangleShape>>& grid) {
	for (unsigned int i = 0; i < grid.size(); i++ ) {
		for (unsigned int j = 0; j < grid[i].size(); j++) {
			Vector2f pPos = player.getPosition();
			float X = pPos.x, Y = pPos.y;
			if (grid[i][j].getFillColor() == Color::White && grid[i][j].getGlobalBounds().contains(Vector2f(X + dx * speed, Y + dy * speed))) {
				player.move(-moveX, -moveY);
			}
		}
	}
}

int main() {
	RenderWindow window(VideoMode(W, H), "Testing");
	window.setFramerateLimit(60);
	window.setPosition(Vector2i(VideoMode::getDesktopMode().width / 2 - (W / 2), VideoMode::getDesktopMode().height / 2 - (H / 2)));

	std::vector<std::vector<RectangleShape>> grid = createGrid();
	CircleShape player(pSize);
	player.setPosition(Vector2f((W / 2) - pSize, (H / 2) - pSize));
	player.setFillColor(Color::Blue);
	player.setOrigin(Vector2f(pSize, pSize));

	Vertex line[2];
	line[0].color = Color::Yellow;
	line[1].color = Color::Yellow;

	while (window.isOpen()) {
		window.setSize(Vector2u(W, H));
		Event event;
		while (window.pollEvent(event)){
			if(event.type == Event::Closed){
				window.close();
			}
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::W) { keys.w = 1; }
				if (event.key.code == Keyboard::A) { keys.a = 1; }
				if (event.key.code == Keyboard::S) { keys.s = 1; }
				if (event.key.code == Keyboard::D) { keys.d = 1; }
			}
			if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::W) { keys.w = 0; }
				if (event.key.code == Keyboard::A) { keys.a = 0; }
				if (event.key.code == Keyboard::S) { keys.s = 0; }
				if (event.key.code == Keyboard::D) { keys.d = 0; }
			}
			moveX = 0, moveY = 0;
		}

		if (keys.w) { 
			moveX = dx;
			moveY = dy; 
		}
		if (keys.s) {
			moveX = -dx;
			moveY = -dy;
		}
		if (keys.a) {
			angle -= 0.1;
			if (angle < 0) { angle += 2 * PI; }
			dx = std::cos(angle) * speed;
			dy = std::sin(angle) * speed;
			
		}
		if (keys.d) {
			angle += 0.1;
			if (angle > 2 * PI) { angle -= 2 * PI; }
			dx = std::cos(angle) * speed;
			dy = std::sin(angle) * speed;
			
		}
		dx = std::cos(angle) * speed;
		dy = std::sin(angle) * speed;
		player.move(moveX, moveY);
		checkCollision(player, grid);

		window.clear();
		for (unsigned int i = 0; i < rows; i++) {
			for (unsigned int j = 0; j < cols; j++) {
				window.draw(grid[i][j]);
			}
		}
		window.draw(player);

		line[0].position = Vector2f(player.getPosition().x ,player.getPosition().y);
		line[1].position = Vector2f(player.getPosition().x + dx * 5, player.getPosition().y + dy * 5);
		window.draw(line, 2, Lines);
		

		window.display();

	}

	return 0;
}