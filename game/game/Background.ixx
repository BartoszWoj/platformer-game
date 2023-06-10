export module Background;

import <SFML/Graphics.hpp>;

export void drawBackground(sf::RenderTarget& target)
{
	sf::VertexArray sky;
	sf::VertexArray ground;
	int leftBorder = -500;
	int rightBorder = 2000;
	int upperBorder = 2000;
	int bottomBorder = -200;

	sky = sf::VertexArray(sf::Quads, 4);
	sky[0].position = sf::Vector2f(leftBorder, -upperBorder);
	sky[0].color = sf::Color::Blue;
	sky[1].position = sf::Vector2f(rightBorder, -upperBorder);
	sky[1].color = sf::Color::Blue;
	sky[2].position = sf::Vector2f(rightBorder, 0);
	sky[2].color = sf::Color::Cyan;
	sky[3].position = sf::Vector2f(leftBorder, 0);
	sky[3].color = sf::Color::Cyan;
	ground = sf::VertexArray(sf::Quads, 4);
	ground[0].position = sf::Vector2f(leftBorder, 0);
	ground[0].color = sf::Color::Green;
	ground[1].position = sf::Vector2f(rightBorder, 0);
	ground[1].color = sf::Color::Green;
	ground[2].position = sf::Vector2f(rightBorder, -bottomBorder);
	ground[2].color = sf::Color(0, 64, 0);
	ground[3].position = sf::Vector2f(leftBorder, -bottomBorder);
	ground[3].color = sf::Color(0, 64, 0);

	target.draw(sky);
	target.draw(ground);
}