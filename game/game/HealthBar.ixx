export module HealthBar;
import Player;

export class HealthBar final
{
	sf::RectangleShape HPbar;
	sf::RectangleShape HPbarOutline;
public:
	HealthBar()
	{
		HPbar.setFillColor(sf::Color::Red);
		HPbarOutline.setFillColor(sf::Color::Black);
		HPbarOutline.setOutlineThickness(2);
		HPbarOutline.setOutlineColor(sf::Color(128, 128, 128));
		HPbarOutline.setSize(sf::Vector2f(100, 10));
	}
	void updateHealthBar(std::shared_ptr<Player>& player)
	{
		HPbar.setSize(sf::Vector2f(player->getHP(), 10));
	}
	void drawHealthBar(sf::RenderTarget& target)
	{
		sf::Vector2f windowCoords = target.mapPixelToCoords(sf::Vector2i(3, 5), target.getView());
		windowCoords.x += 5;
		windowCoords.y += 5;
		HPbar.setPosition(sf::Vector2(windowCoords.x + 5, windowCoords.y + 5));
		HPbarOutline.setPosition(sf::Vector2(windowCoords.x + 5, windowCoords.y + 5));
		target.draw(HPbarOutline);
		target.draw(HPbar);
	}
};