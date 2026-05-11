#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "barrel.h"

enum class GameState;

class Graphics {
public:
	Graphics();

	sf::RenderWindow window;

	void moveScream(float dx, float dy);
	bool ScreamonGround() const;
	bool ScreamonLadder() const;
	bool ScreamhitbyBarrel() const;
	bool ScreamReachedTop() const;
	void setScreamFrame(int frame, bool moving);

	void spawnBarrel();
	void updateBarrels(float dt);

	void reset();

	void draw(int score,int bestScore, const std::string& bestScoreName, const GameState currentGameState);
	void drawNameInput(const sf::Text& namePromptText, const sf::Text& nameInputText);
	void drawLeaderboard(const std::vector<std::pair<std::string, int>>& scores);
	void loadAssets();

	sf::Texture screamTexture;
	sf::Sprite screamSprite;

	sf::Texture barrelTexture;
	std::vector<Barrel> barrels;

	std::vector<sf::RectangleShape> platforms;

	sf::Texture ladderTexture;
	std::vector<sf::Sprite> ladders;
	
	sf::Texture goalTexture;
	sf::Sprite goalSprite;

	sf::Font font;
	sf::Text titleText;
	sf::Text instructionText;
	sf::Text gameOverText;
	sf::Text winText;
	sf::Text scoreText;
	sf::Text bestScoreText;
	sf::Text bestScoreNameText;
	sf::Text leaderboardText;
	std::vector<sf::Text> leaderboardEntries;
	// Toolbar
	sf::RectangleShape toolbar;
	sf::Text saveButton;
	sf::Text loadButton;
	sf::Text menuButton;
	sf::Text HelpButton;

	// Menu buttons
	sf::Text startButton;
	sf::Text leaderboardButton;
	sf::Text exitButton;

	int frameWidth;
	int frameHeight;
};

