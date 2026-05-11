#pragma once
#include "Graphics.h"

enum class GameState {
	Menu,
	Playing,
	GameOver,
	Win,
	EnterName,
	Leaderboard,
	Help
};
class Game {
public:
	Game();
	Graphics graphics;

	std::string playerNameInput;
	sf::Text namePromptText;
	sf::Text nameInputText;


	float gravity = 0.6f;
	float velocityY = 0.0f;
	bool onGround = false;
	bool moving = false;
	bool gameOver = false;
	bool onLadder = false;
	bool win = false;
	bool showBestScore = false;
	float barrelTimer = 0.0f;
	float barrelSpawnInterval = 2.0f; // Spawn a barrel every 2 seconds
	float climbSpeed = 2.0f;
	int score = 0;
	int frame = 0;
	float animationTimer = 0.0f;
	float animationSpeed = 0.15f; // Time per frame in seconds
	float scoreTimer = 0.0f;
	float timeElapsed = 0.0f;

	void run();
	void processInput();
	void initNameInputUI();
	void update(float dt);
	void reset();
}; // <-- Fixed: added missing semicolon and closing brace