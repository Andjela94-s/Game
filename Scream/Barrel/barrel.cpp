#include "barrel.h"

Barrel::Barrel(sf::Texture& texture, sf::Vector2f Startingposition, sf::Vector2f initialVelocity) : barrelSprite(texture), velocity(initialVelocity) {
	barrelSprite.setPosition(Startingposition);
}

void Barrel::update(float dt) {
	velocity.y += gravity * dt;
	barrelSprite.move(velocity * dt);
}
void Barrel::draw(sf::RenderWindow& window) {
	window.draw(barrelSprite);
}
sf::FloatRect Barrel::getGlobalBounds() const {
	return barrelSprite.getGlobalBounds();
}