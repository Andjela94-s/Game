#include "igrica.h"
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "Graphics.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

Game::Game() : graphics(), playerNameInput(""), namePromptText(graphics.font), nameInputText(graphics.font),
gravity(0.6f), velocityY(0.0f), onGround(false), moving(false), gameOver(false),
onLadder(false), win(false), barrelTimer(0.0f), barrelSpawnInterval(2.0f),
climbSpeed(2.0f), score(0), frame(0), animationTimer(0.0f),
animationSpeed(0.15f), scoreTimer(0.0f), timeElapsed(0.0f) {
};

GameState currentGameState = GameState::Menu;

static std::vector<std::pair<std::string, int>> readAllHighscores(const std::string& path = "highscores.txt") {
    std::ifstream in(path);
	std::vector<std::pair<std::string, int>> scores;

	if (!in.is_open()) return scores;

    std::string name;
    int score;

    while (in >> name >> score) {
		scores.push_back({ name, score });
    }

    return scores;
}
static void writeAllHighscores(std::vector<std::pair<std::string, int>>& scores,
    const std::string& path = "highscores.txt") {
    std::sort(scores.begin(), scores.end(),
        [](auto& a, auto& b) {
            return a.second > b.second;
		});
    if (scores.size() > 5)
        scores.resize(5);

	std::ofstream out(path, std::ofstream::trunc);
	if (!out.is_open()) return;

    for (auto& entry : scores) {
        out << entry.first << " " << entry.second << "\n";
    }
}

void Game::run() {
    // Load assets once before the main loop so sprites/shapes are initialized
    try {
        graphics.loadAssets();
    }
    catch (const std::exception& ex) {
        std::cerr << "Failed to load assets: " << ex.what() << '\n';
        return;
    }

    initNameInputUI();

	auto allScores = readAllHighscores();
	int bestScore = 0;
	std::string bestScoreName;

	for (const auto& entry : allScores) {
		if (entry.second > bestScore) {
			bestScore = entry.second;
			bestScoreName = entry.first;
		}
	}

	bool highScoreRecorded = false;

    sf::Clock clock;

    while (graphics.window.isOpen()) {
        float dt = clock.restart().asSeconds();

        processInput();
		//Help state
        if (currentGameState == GameState::Help) {
            graphics.window.clear();
            graphics.window.draw(graphics.instructionText);
            graphics.window.display();
            continue;
        }
		//Leaderboard state
		if (currentGameState == GameState::Leaderboard) {
			auto scores = readAllHighscores();
			graphics.drawLeaderboard(scores);
            continue;
		}
        if (currentGameState == GameState::EnterName) {
            graphics.window.clear();
            graphics.drawNameInput(namePromptText, nameInputText);
            graphics.window.display();
            continue;
        }

        if (currentGameState == GameState::Playing) {
            update(dt);
        }

        graphics.draw(score, bestScore, bestScoreName, currentGameState);

        if (currentGameState == GameState::Win && !highScoreRecorded) {
			auto scores = readAllHighscores();

            if(scores.size()<5)
            {
                currentGameState = GameState::EnterName;
                //highScoreRecorded = true;
                //return;
			}
            else {
				int lowestScore = scores.back().second;
				if(score > lowestScore) {
                    currentGameState = GameState::EnterName;
            }
            else {
                std::cout << "You Win! Your score: " << score << ". Best score: " << bestScore << " by " << bestScoreName << "." << std::endl;
            }
			}
            highScoreRecorded = true;
        }
    }
}

void Game::processInput() {
    while (auto event = graphics.window.pollEvent()) {
        if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse->button == sf::Mouse::Button::Right) {
                graphics.window.close();
                return;
            }
        }
        if (event->is<sf::Event::Closed>()) {
            graphics.window.close();
            return;
        }
        // ================= MOUSE CLICK HANDLING (MENU + TOOLBAR) =================
        if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            sf::Vector2i pos = sf::Mouse::getPosition(graphics.window);
            if (graphics.HelpButton.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)))) {
                currentGameState = GameState::Help;
                continue;
            }
            // ===== MAIN MENU BUTTONS =====
            if (currentGameState == GameState::Menu) {
                if (graphics.startButton.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)))) {
                    reset();
                    currentGameState = GameState::Playing;
                    continue;
                }
                if (graphics.leaderboardButton.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)))) {
                    currentGameState = GameState::Leaderboard;
                    continue;
                }
                if (graphics.exitButton.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)))) {
                    graphics.window.close();
                    return;
                }
            }

            // ===== TOOLBAR BUTTONS (visible in all states except Menu) =====
            if (currentGameState != GameState::Menu) {
                if (graphics.saveButton.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)))) {
                    auto scores = readAllHighscores();
                    writeAllHighscores(scores);
                    continue;
                }
                if (graphics.loadButton.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)))) {
                    auto scores = readAllHighscores();
                    continue;
                }
                if (graphics.menuButton.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)))) {
                    currentGameState = GameState::Menu;
                    continue;
                }
            }
        }
        // ================= LEADERBOARD =================
        if (currentGameState == GameState::Leaderboard) {
            if (auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    currentGameState = GameState::Menu;
                }
            }
            continue; // ⬅️ VAŽNO
        }
        // ================= HELP =================
        if (currentGameState == GameState::Help) {
            if (auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    currentGameState = GameState::Menu;
                }
            }
            continue;
        }

        // ================= MENU =================
        if (currentGameState == GameState::Menu) {
            if (auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::L) {
                    currentGameState = GameState::Leaderboard;
                }
                else if (key->code == sf::Keyboard::Key::Enter) {
                    reset();
                    currentGameState = GameState::Playing;
                }
            }
            continue;
        }

        // ================= ENTER NAME =================
        if (currentGameState == GameState::EnterName) {
            if (auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Enter) {
                    if (playerNameInput.empty())
                        playerNameInput = "Player";

                    auto scores = readAllHighscores();
                    scores.push_back({ playerNameInput, score });
                    writeAllHighscores(scores);

                    playerNameInput.clear();
                    currentGameState = GameState::Menu;
                }
                else if (key->code == sf::Keyboard::Key::Backspace && !playerNameInput.empty()) {
                    playerNameInput.pop_back();
                }
            }

            if (auto text = event->getIf<sf::Event::TextEntered>()) {
                char c = static_cast<char>(text->unicode);
                if (std::isprint(c) && playerNameInput.size() < 12)
                    playerNameInput.push_back(c);
            }

            nameInputText.setString(playerNameInput);
            continue;
        }

        // ================= GAME OVER / WIN =================
        if (currentGameState == GameState::GameOver ||
            currentGameState == GameState::Win) {

            if (auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::R) {
                    reset();
                    currentGameState = GameState::Playing;
                }
            }
        }
    }
}


void Game::initNameInputUI() {
    namePromptText.setFont(graphics.font);
    namePromptText.setString("New High Score! Enter your name:");
    namePromptText.setCharacterSize(32);
    namePromptText.setFillColor(sf::Color::White);
    namePromptText.setPosition({100.f, 200.f});

    nameInputText.setFont(graphics.font);
    nameInputText.setCharacterSize(32);
    nameInputText.setFillColor(sf::Color::Yellow);
    nameInputText.setPosition({100.f, 260.f});
}
void Game::update(float dt) {
    if (gameOver || win)
        return;

    // Ladder check
    onLadder = graphics.ScreamonLadder();

    // Handle real-time movement (frame-rate independent)
    const float horizontalSpeed = 200.f; // pixels per second
    const float climbMultiplier = 60.f;  // converts your climbSpeed to pixels/sec-like feel

    moving = false;
    if (currentGameState == GameState::Playing) {
		timeElapsed += dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            graphics.moveScream(-horizontalSpeed * dt, 0.f);
            moving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            graphics.moveScream(horizontalSpeed * dt, 0.f);
            moving = true;
        }

        // Climb / descend when overlapping a ladder
        float climbAmount = climbSpeed * climbMultiplier * dt;
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) && onLadder) {
            graphics.moveScream(0.f, -climbAmount);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) && onLadder) {
            graphics.moveScream(0.f, climbAmount);
        }

        // Jump (real-time)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && onGround) {
            velocityY = -12.f;
            onGround = false;
        }
    }

    // Gravity
    if (!onGround && !onLadder) {
        velocityY += gravity * dt;
        graphics.moveScream(0.f, velocityY * dt);
    }

    // Ground
    if (graphics.ScreamonGround()) {
        onGround = true;
        velocityY = 0.f;
    }
    else {
        onGround = false;
    }

    // Barrel spawn/update
    barrelTimer += dt;
    if (barrelTimer >= barrelSpawnInterval) {
        graphics.spawnBarrel();
        barrelTimer = 0.f;
    }
    graphics.updateBarrels(dt);

    if (graphics.ScreamhitbyBarrel()) {
        currentGameState = GameState::GameOver;
        gameOver = true;
    }
    // Score update
    scoreTimer += dt;
    if (scoreTimer >= 1.0f){
        int increment = static_cast<int>(scoreTimer);
    score += increment;
    scoreTimer -= static_cast<float>(increment);
}

    // Win condition
    if (graphics.ScreamReachedTop()) {
        if(currentGameState != GameState::Win) {
            currentGameState = GameState::Win;
            win = true;
			float timeBonus = 0.f;
			if (timeElapsed > 0.f) {
				timeBonus = 500.f / timeElapsed;
			}
			int bonusPoints = static_cast<int>(timeBonus);
			score += bonusPoints;
			std::cout << "Time Bonus: " << bonusPoints << " points!" << std::endl;
			velocityY = 0.f;
			moving = false;
		}
    }

    // Animation
    animationTimer += dt;
    if (animationTimer >= animationSpeed) {
        frame = (frame + 1) % 3;
        graphics.setScreamFrame(frame, moving);
        animationTimer = 0.f;
    }
}

void Game::reset() {
    score = 0;
	scoreTimer = 0.f;
	timeElapsed = 0.f;
    velocityY = 0.f;
    onGround = false;
    onLadder = false;
    moving = false;
    gameOver = false;
    win = false;
    barrelTimer = 0.f;
    frame = 0;
    animationTimer = 0.f;
    graphics.reset();
    graphics.barrels.clear();
}
