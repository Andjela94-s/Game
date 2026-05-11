#pragma once
#include <SFML/Graphics.hpp>

class Barrel {
public:
	Barrel(sf::Texture& texture, sf::Vector2f Startingposition, sf::Vector2f initialVelocity = { 0.f, 0.f });
	
	void update(float dt);
	void draw(sf::RenderWindow& window);

	sf::FloatRect getGlobalBounds() const;
private:
	sf::Sprite barrelSprite;
	sf::Vector2f velocity{ 0.f, 0.f };
	float gravity = 600.0f; // pixels per second squared
	float speed = 100.0f; // pixels per second
};