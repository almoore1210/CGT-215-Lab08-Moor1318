// CGT-215-Lab08-Moor1318.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;
using namespace sfp;

// make window dimensions
const int WINDOWWIDTH = 800;
const int WINDOWHEIGHT = 600;

// arrow speed
const float ARROWSPEED = 550.0f;

int main() {
	srand(static_cast<unsigned int>(time(0)));

	// draw window
	RenderWindow window(VideoMode(WINDOWWIDTH, WINDOWHEIGHT), "Duck Hunter");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);

	// load images
	Texture duckTexture;
	Texture crossbowTexture;
	Texture arrowTexture;

	if (!crossbowTexture.loadFromFile("images/crossbow.png")) {
		cout << "Error loading crossbow image!" << endl;
		return -1;
	}

	if (!arrowTexture.loadFromFile("images/arrow.png")) {
		cout << "Error loading arrow image!" << endl;
		return -1;
	}

	if (!duckTexture.loadFromFile("images/duck.png")) {
		cout << "Error loading duck image!" << endl;
		return -1;
	}

	// create crossbow
	Sprite crossbow;
	crossbow.setTexture(crossbowTexture);

	crossbow.setOrigin(
		crossbowTexture.getSize().x / 2.0f,
		crossbowTexture.getSize().y / 2.0f
	);

	crossbow.setPosition(WINDOWWIDTH / 2.0f, WINDOWHEIGHT - 75.0f);

	// create arrow
	Sprite arrow;
	arrow.setTexture(arrowTexture);

	arrow.setOrigin(
		arrowTexture.getSize().x / 2.0f,
		arrowTexture.getSize().y / 2.0f
	);

	bool drawingArrow = false;
	bool gameOver = false;

	int arrowsLeft = 5;

	// physics world
	Vector2f gravity(0.0f, 0.0f);
	World world(gravity);

	// list to hold ducks
	PhysicsShapeList<PhysicsSprite> ducks;

	// load font
	Font font;

	if (!font.loadFromFile("arial.ttf")) {
		cout << "Error loading font!" << endl;
		return -1;
	}

	// score text
	int score = 0;

	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(24);
	scoreText.setFillColor(Color::White);
	scoreText.setString("Score: 0");
	scoreText.setPosition(WINDOWWIDTH - 130.0f, WINDOWHEIGHT - 40.0f);

	// arrows left text
	Text arrowsText;
	arrowsText.setFont(font);
	arrowsText.setCharacterSize(24);
	arrowsText.setFillColor(Color::White);
	arrowsText.setString("Arrows: 5");
	arrowsText.setPosition(10.0f, WINDOWHEIGHT - 40.0f);

	//game over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(48);
	gameOverText.setFillColor(Color::White);
	gameOverText.setOutlineColor(Color::Black);
	gameOverText.setOutlineThickness(2);
	gameOverText.setString("GAME OVER");

	// center game over text
	FloatRect textBounds = gameOverText.getLocalBounds();

	gameOverText.setOrigin(
		textBounds.left + textBounds.width / 2.0f,
		textBounds.top + textBounds.height / 2.0f
	);

	gameOverText.setPosition(WINDOWWIDTH / 2.0f, WINDOWHEIGHT / 2.0f);

	// game clock
	Clock clock;
	Time lastTime = clock.getElapsedTime();

	// duck spawning timer
	long duckMS = 0;

	// check if game is over
	if (arrowsLeft == 0 && !drawingArrow) {
		gameOver = true;
	}

	// main game loop
	while (window.isOpen()) {

		Event event;

		// check for events
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}

			// shoot arrow when SPACE is pressed
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Space) {
					if (!drawingArrow && arrowsLeft > 0) {
						drawingArrow = true;

						arrow.setPosition(
							crossbow.getPosition().x,
							crossbow.getPosition().y - 50.0f
						);

						arrowsLeft--;

						arrowsText.setString(
							"Arrows: " + to_string(arrowsLeft)
						);
					}
				}
			}
		}

		// stop drawing if window is closed
		if (!window.isOpen()) {
			break;
		}

		// calculate elapsed time
		Time currentTime = clock.getElapsedTime();
		Time deltaTime = currentTime - lastTime;
		long deltaMS = deltaTime.asMilliseconds();

		// only update game when enough time has passed
		if (deltaMS > 9) {

			// reset frame timer
			lastTime = currentTime;

			// update physics world
			world.UpdatePhysics(deltaMS);

			// update duck timer
			duckMS += deltaMS;

			// move arrow
			if (drawingArrow) {
				arrow.move(0.0f, -ARROWSPEED * (deltaMS / 1000.0f));

				if (arrow.getPosition().y < 0) {
					drawingArrow = false;
				}
			}

			// create duck every 2 seconds
			if (duckMS >= 2000) {
				duckMS = 0;

				// create a new duck
				PhysicsSprite& duck = ducks.Create();

				duck.setTexture(duckTexture);

				// get duck dimensions
				Vector2f sz = duck.getSize();

				//smaller
				duck.setSize(Vector2f(sz.x * 0.15f, sz.y * 0.15f));

				sz = duck.getSize();

				// random vertical position
				int duckY = 400 + rand() % 70;

				// start duck off screen to the left
				duck.setCenter(Vector2f(100.0f, 430.0f));

				// make duck fly to the right
				duck.setVelocity(Vector2f(0.25f, 0.0f));

				// add duck to physics world
				world.AddPhysicsBody(duck);
			}
		}

		// draw everything
		window.clear(Color(135, 206, 235));

		window.draw(crossbow);

		// draw all ducks
		for (auto& duck : ducks) {
			window.draw(duck);
		}

		// draw arrow if it is currently active
		if (drawingArrow) {
			window.draw(arrow);
		}

		window.draw(scoreText);
		window.draw(arrowsText);

		// display game over screen
		if (gameOver) {
			window.draw(gameOverText);
		}

		// display
		window.display();
	}

	return 0;
}