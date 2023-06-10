export module BrickBlock;
import GameObject;

export class BrickBlock final : public GameObject
{
public:

	BrickBlock(double x, double y, ResourceHolder& resources)
	{
		this->objectType = ObjectType::staticBlock;
		this->textures = resources.getTexture("brickBlock");

		this->sprite.setTexture(this->textures);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
		this->sprite.setPosition(sf::Vector2f(x, y));
	}
	virtual void updateObject(std::list<std::shared_ptr<GameObject>>&)override {}
	virtual void collision(GameObject*, CollisionSide)override {}
};