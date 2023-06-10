export module Button;
export import MyText;

export class Button final
{
	sf::RectangleShape box;
	sf::Text text;
	float boxX;
	float boxY;
	float textX;

public:
	Button()
	{

	}
	Button(float _boxX, float _boxY, std::string _text, ResourceHolder& resources) :
		boxX(_boxX), boxY(_boxY)
	{
		MyText myText(boxX + 2, boxY + 1, _text, resources);
		this->text = myText.getText();
		this->box.setFillColor(sf::Color(128, 128, 128));
		unselect();
	}
	void select()
	{
		this->box.setPosition(this->boxX - 4, this->boxY - 1);
		this->box.setSize(sf::Vector2f(88, 22));
		this->box.setOutlineThickness(4);
		this->box.setOutlineColor(sf::Color::Red);
		this->box.setRotation(3.f);
		this->text.setRotation(3.f);
	}
	void unselect()
	{
		this->box.setPosition(this->boxX, this->boxY);
		this->box.setSize(sf::Vector2f(80, 20));
		this->box.setOutlineThickness(2);
		this->box.setOutlineColor(sf::Color::White);
		this->box.setRotation(0.f);
		this->text.setRotation(0.f);
	}
	void draw(sf::RenderTarget& target)
	{
		target.draw(this->box);
		target.draw(this->text);
	}
};