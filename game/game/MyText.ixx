export module MyText;
export import GameObject;
import ResourceHolder;

export class MyText final : public GameObject
{
	sf::Text text;
	sf::Font* font;

public:
	MyText()
	{
		objectType = ObjectType::nonPhysical;
		text.setScale(0.25, 0.25);
	}
	MyText(int x, int y, std::string _text, ResourceHolder& resources, int size = 50, sf::Color colour = sf::Color::Black, int rotation = 0)
	{
		objectType = ObjectType::nonPhysical;
		font = &resources.getFont("arial");
		text.setFont(*font);
		text.setPosition(x, y);
		text.setString(_text);
		text.setScale(0.25, 0.25);
		text.setCharacterSize(size);
		text.setFillColor(colour);
		text.setRotation(rotation);
	}
	void setText(int x, int y, std::string _text, ResourceHolder& resources, int size = 50, sf::Color colour = sf::Color::Black, int rotation = 0)
	{
		font = &resources.getFont("arial");
		text.setFont(*font);
		text.setPosition(x, y);
		text.setString(_text);
		text.setCharacterSize(size);
		text.setFillColor(colour);
		text.setRotation(rotation);
	}
	sf::Text getText()
	{
		return text;
	}
	void draw(sf::RenderTarget& target) override
	{
		target.draw(this->sprite);
	}
	virtual void updateObject(std::list<std::shared_ptr<GameObject>>&)override {}
	virtual void collision(GameObject*, CollisionSide)override {}
};