#include<SFML/Graphics.hpp>
#include "Graphics.h"
#include "igrica.h"
#include <iostream>
#include <random>
#include <algorithm>

Graphics::Graphics()
    : window(sf::VideoMode({800, 600}), "Scream Game"),
      screamSprite(screamTexture),
	titleText(font), instructionText(font), gameOverText(font), winText(font), scoreText(font), bestScoreText(font), bestScoreNameText(font), leaderboardText(font), toolbar(), saveButton(font), loadButton(font), menuButton(font), HelpButton(font), startButton(font), leaderboardButton(font), exitButton(font),
	frameWidth(50), frameHeight(70), goalSprite(goalTexture), barrels()
{
    window.setFramerateLimit(60);
}

void Graphics::loadAssets() {
	if (screamTexture.loadFromFile("player.png") == false) {
		throw std::runtime_error("Failed to load player texture");
	}
	screamSprite.setTexture(screamTexture);
	screamSprite.setTextureRect(sf::IntRect({ 0, 0 }, { frameWidth, frameHeight }));
	screamSprite.setPosition({100.f, 450.f});
	if (barrelTexture.loadFromFile("barrel.png") == false) {
		throw std::runtime_error("Failed to load barrel texture");
	}
	//Load ladders
	if (ladderTexture.loadFromFile("ladder.png") == false) {
		throw std::runtime_error("Failed to load ladder texture");
	}
	//Load goal
	if (goalTexture.loadFromFile("goal.png") == false) {
		throw std::runtime_error("Failed to load goal texture");
	}
	goalSprite.setTexture(goalTexture);
	goalSprite.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(goalTexture.getSize().x), static_cast<int>(goalTexture.getSize().y) }));
	// Load platforms
	platforms.clear();

	// Load ladders
	sf::RectangleShape ground;
	ground.setSize({ 800.f, 20.f });
	ground.setPosition({ 0.f, 500.f });
	ground.setFillColor(sf::Color::Green);
	platforms.push_back(ground);

	//Elevated platform
	sf::RectangleShape plaform1;
	plaform1.setSize({ 300.f, 20.f });
	plaform1.setPosition({ 250.f, 400.f });
	plaform1.setFillColor(sf::Color::Green);
	platforms.push_back(plaform1);

	sf::RectangleShape plaform2;
	plaform2.setSize({ 300.f, 20.f });
	plaform2.setPosition({ 500.f, 300.f });
	plaform2.setFillColor(sf::Color::Green);
	platforms.push_back(plaform2);
	
	sf::RectangleShape plaform3;
	plaform3.setSize({ 400.f, 20.f });
	plaform3.setPosition({ 200.f, 200.f });
	plaform3.setFillColor(sf::Color::Green);
	platforms.push_back(plaform3);

	//Ladder Sprite
	ladders.clear();

	{
		sf::Sprite ladder(ladderTexture);
		ladder.setPosition({ 220.f, 420.f }); // x approx inside plat1, y below plat1 toward ground
		// scale ladder to visually span from ground y to plat1 y (approx)
		const float desiredHeight = 500.f - 380.f; // 120 px
		if (ladderTexture.getSize().y > 0) {
			ladder.setScale({1,1});
		}
		ladders.push_back(ladder);
	}

	// Ladder between plat1 and plat2
	{
		sf::Sprite ladder(ladderTexture);
		ladder.setPosition({ 500.f, 320.f }); // between plat1 (y=380) and plat2 (y=300)
		const float desiredHeight = 380.f - 300.f; // 80 px
		if (ladderTexture.getSize().y > 0) {
			ladder.setScale({ 1, 1 });
		}
		ladders.push_back(ladder);
	}

	// Ladder between plat2 and plat3 (to reach the top)
	{
		sf::Sprite ladder(ladderTexture);
		ladder.setPosition({ 550.f, 240.f }); // between plat2(y=300) and plat3(y=220)
		const float desiredHeight = 300.f - 220.f; // 80 px
		if (ladderTexture.getSize().y > 0) {
			ladder.setScale({ 1, 1 });
		}
		ladders.push_back(ladder);
	}

	// Position goal at the top platform
	if (!platforms.empty()) {
		const auto& topPlatform = platforms.back();
		auto platformPos = topPlatform.getPosition();
		auto platformSize = topPlatform.getSize();
		sf::Vector2u goalSize = goalTexture.getSize();
		goalSprite.setOrigin({static_cast<float>(goalSize.x) / 2.f, static_cast<float>(goalSize.y)});
		float goalX = platformPos.x + platformSize.x / 2.f;
		float goalY = platformPos.y;
		goalSprite.setPosition({ goalX, goalY });

		const float maxWidth = platformSize.x * 0.6f;
		const float maxHeight = 80.f;
		float scaleX = 1.f;
		float scaleY = 1.f;
		if(static_cast<float>(goalSize.x) > maxWidth) {
			scaleX = maxWidth / static_cast<float>(goalSize.x);
		}
		if(static_cast<float>(goalSize.y) > maxHeight) {
			scaleY = maxHeight / static_cast<float>(goalSize.y);
		}
		float finalScale = std::min(scaleX, scaleY);
		if (finalScale < 1.f) {
			goalSprite.setScale({ finalScale, finalScale });
		}
	}

	// Load font
	if (!font.openFromFile("font.ttf")) {
		throw std::runtime_error("Failed to load font");
	}
	// Setup texts
	titleText.setFont(font);
	titleText.setString("Scream Game");
	titleText.setCharacterSize(64);
	titleText.setFillColor(sf::Color::White);

	instructionText.setFont(font);
	instructionText.setString("Manual:\n-Press Enter to Start\n-W/A/S/D or Arrows to move\n-Space to jump\n-Right click to exit\n-L to view Leaderboard\n-Esc to return to Menu");
	instructionText.setCharacterSize(20);
	instructionText.setFillColor(sf::Color::Magenta);

	gameOverText.setFont(font);
	gameOverText.setString("Game Over\nPress  R to Restart");
	gameOverText.setCharacterSize(40);
	gameOverText.setFillColor(sf::Color::Red);

	winText.setFont(font);
	winText.setString("You Win!\nPress R to Play Again");
	winText.setCharacterSize(40);
	winText.setFillColor(sf::Color::Green);

	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(sf::Color::Cyan);
	scoreText.setPosition({ 10.f, 50.f });

	bestScoreText.setFont(font);
	bestScoreText.setCharacterSize(20);
	bestScoreText.setFillColor(sf::Color::Cyan);
	bestScoreText.setPosition({ 10.f, 100.f });

	bestScoreNameText.setFont(font);
	bestScoreNameText.setCharacterSize(20);
	bestScoreNameText.setFillColor(sf::Color::Cyan);
	bestScoreNameText.setPosition({ 10.f, 120.f });

	leaderboardText.setFont(font);
	leaderboardText.setString("Leaderboard - Top 5");
	leaderboardText.setFillColor(sf::Color::Cyan);
	leaderboardText.setPosition({ 200.f, 115.f });

	//leaderboardEntries.clear();
	leaderboardEntries.reserve(5);
	for (int i = 0; i < 5; i++) {
		leaderboardEntries.emplace_back(font);
		leaderboardEntries[i].setString("");
		leaderboardEntries[i].setCharacterSize(24);
		leaderboardEntries[i].setFillColor(sf::Color::White);
		leaderboardEntries[i].setPosition({ 300.f, 150.f + i * 50.f });
	}

	// ===== TOOLBAR =====
	toolbar.setSize({ 80.f, 40.f });
	toolbar.setFillColor(sf::Color(50, 50, 50));
	toolbar.setPosition({ 0.f, 0.f });

	saveButton.setFont(font);
	saveButton.setString("Save");
	saveButton.setCharacterSize(20);
	saveButton.setPosition({ 20.f, 10.f });

	loadButton.setFont(font);
	loadButton.setString("Load");
	loadButton.setCharacterSize(20);
	loadButton.setPosition({ 120.f, 10.f });

	menuButton.setFont(font);
	menuButton.setString("Menu");
	menuButton.setCharacterSize(20);
	menuButton.setPosition({ 220.f, 10.f });

	HelpButton.setFont(font);
	HelpButton.setString("Help");
	HelpButton.setCharacterSize(20);
	HelpButton.setPosition({ 320.f, 10.f });

	// ===== MAIN MENU BUTTONS =====
	startButton.setFont(font);
	startButton.setString("Start Game");
	startButton.setCharacterSize(18);
	startButton.setPosition({ 30.f, 200.f });

	leaderboardButton.setFont(font);
	leaderboardButton.setString("Leaderboard");
	leaderboardButton.setCharacterSize(18);
	leaderboardButton.setPosition({ 30.f, 260.f });

	exitButton.setFont(font);
	exitButton.setString("Exit");
	exitButton.setCharacterSize(18);
	exitButton.setPosition({ 30.f, 320.f });
}
void Graphics::moveScream(float dx, float dy) {
	screamSprite.move({dx, dy});
}
bool Graphics::ScreamonGround() const {
	for (const auto& platform : platforms) {
		if (screamSprite.getGlobalBounds().findIntersection(platform.getGlobalBounds()).has_value()) {
			return true;
		}
	}
	return false;
}
bool Graphics::ScreamonLadder() const {
	for (const auto& ladder : ladders) {
		if (screamSprite.getGlobalBounds().findIntersection(ladder.getGlobalBounds()).has_value()) {
			return true;
		}
	}
	return false;
}
bool Graphics::ScreamReachedTop() const {
	return screamSprite.getGlobalBounds().findIntersection(goalSprite.getGlobalBounds()).has_value();
}
bool Graphics::ScreamhitbyBarrel() const {
	for (const auto& barrel : barrels) {
		if (screamSprite.getGlobalBounds().findIntersection(barrel.getGlobalBounds()).has_value()) {
			return true;
		}
	}
	return false;
}
void Graphics::spawnBarrel() {
	sf::Vector2u winSize = window.getSize();
	float winW = static_cast<float>(winSize.x);

	float texW = static_cast<float>(barrelTexture.getSize().x);
	if (texW <= 0.f) texW = 16.f;

	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> distX(0.f, std::max(0.f, winW - texW));
	std::uniform_real_distribution<float> distVx(-80.f, 80.f);

	float spawnX = distX(rng);
	float spawnY = -static_cast<float>(barrelTexture.getSize().y); // start just above the window
	float initialVx = distVx(rng);
	float initialVy = 80.f; // initial downward speed

	barrels.emplace_back(barrelTexture, sf::Vector2f(spawnX, spawnY), sf::Vector2f(initialVx, initialVy));

}
void Graphics::updateBarrels(float dt) {
	for (auto& barrel : barrels) {
		barrel.update(dt);
		//Delete barrels that are out of screen
		barrels.erase(
			std::remove_if(barrels.begin(), barrels.end(), [&](const Barrel& b) {
				return b.getGlobalBounds().position.y> static_cast<float>(window.getSize().y) + 200.f;
				}),
			barrels.end()
		);
	}
}
void Graphics::setScreamFrame(int frame, bool moving) {
	if (!moving) frame = 0;
	screamSprite.setTextureRect(sf::IntRect({ frame * frameWidth, 0 }, { frameWidth, frameHeight }));
}
void Graphics::reset() {
	screamSprite.setPosition({ 100.f, 450.f });
	barrels.clear();
}
void Graphics::draw(int score, int bestScore, const std::string& bestScoreName, GameState currentGameState) {
	
	if (currentGameState == GameState::Help) {
		window.clear(sf::Color::Black);
		window.draw(instructionText);
		window.display();
		return;
	}
	window.clear(sf::Color::Black);

	// Draw toolbar
	if(currentGameState != GameState::Help){
	window.draw(toolbar);
	window.draw(saveButton);
	window.draw(loadButton);
	window.draw(menuButton);
	window.draw(HelpButton);
	}
	if (currentGameState == GameState::Menu) {
		// center title and instruction
		auto winSize = window.getSize();
		sf::FloatRect tb = titleText.getLocalBounds();
		titleText.setOrigin({25.f,25.f});
		titleText.setPosition({50.f,50.f});

		window.draw(startButton);
		window.draw(leaderboardButton);
		window.draw(exitButton);

		sf::FloatRect ib = instructionText.getLocalBounds();
		instructionText.setOrigin({25.f,25.f});
		instructionText.setPosition({200.f,200.f});

		window.draw(titleText);
		//window.draw(instructionText);

		window.display();
		return;
	}
	// Draw platforms
	for (const auto& platform : platforms) {
		window.draw(platform);
	}
	// Draw ladders
	for (const auto& ladder : ladders) {
		window.draw(ladder);
	}
	// Draw barrels
	for (auto& barrel : barrels) {
		barrel.draw(window);
	}
		float playerY = screamSprite.getPosition().y;
		float goalY = goalSprite.getPosition().y;

	// Draw player and goal
		if (playerY <= goalY) {
					window.draw(screamSprite);
					window.draw(goalSprite);
		}
		else {
			// Draw goal first, then player to appear on top
			window.draw(goalSprite);
			window.draw(screamSprite);
		}
		// Draw score
		scoreText.setString("Score: " + std::to_string(score));
		window.draw(scoreText);
		// Draw game over or win text if needed
		if (currentGameState == GameState::GameOver || currentGameState == GameState::Win) {
			// dark overlay
			sf::RectangleShape overlay;
			overlay.setSize({ static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) });
			overlay.setFillColor(sf::Color(0, 0, 0, 160));
			window.draw(overlay);

			// choose text
			sf::Text* msg = (currentGameState == GameState::Win) ? &winText : &gameOverText;

			// center text
			sf::FloatRect tb = msg->getLocalBounds();
			msg->setOrigin({25.f,25.f});
			msg->setPosition({50.f, 50.f});

			window.draw(*msg);
		}
	window.display();
}
void Graphics::drawNameInput(const sf::Text& namePromptText, const sf::Text& nameInputText) {
	window.clear(sf::Color::Black);
	window.draw(namePromptText);
	window.draw(nameInputText);
	window.display();
}
void Graphics::drawLeaderboard(const std::vector<std::pair<std::string, int>>& scores) {
	//window.clear(sf::Color::Black);

	window.draw(leaderboardText);

	int count = std::min(5, static_cast<int>(scores.size()));
	for (int i = 0; i < count; i++) {
		leaderboardEntries[i].setString(
			std::to_string(i + 1) + ". " + scores[i].first + " - " + std::to_string(scores[i].second)
		);
		window.draw(leaderboardEntries[i]);
	}

	window.display();
}