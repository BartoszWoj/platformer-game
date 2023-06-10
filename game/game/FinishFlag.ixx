export module FinishFlag;
import MyText;


export class FinishFlag final : public GameObject
{
	sf::Clock animationTimer;
	sf::IntRect currentFrame;
	bool wasReached = 0;
	sf::Text congratulations;

public:

	FinishFlag(double x, double y, ResourceHolder& resources)
	{
		this->objectType = ObjectType::levelFinish;
		this->wasReached = 0;
		MyText myCongratulations;
		myCongratulations.setText(x - 45, y - 30, "CONGRATULATIONS", resources);
		congratulations = myCongratulations.getText();
		this->textures = resources.getTexture("finishFlag");
		this->sprite.setTexture(this->textures);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
		this->sprite.setPosition(sf::Vector2f(x, y));
		this->currentFrame = sf::IntRect(0, 0, 32, 32);
	}
	void animate()
	{
		if (this->animationTimer.getElapsedTime().asSeconds() > 0.25)
		{
			if (this->currentFrame.left != 32)
				this->currentFrame.left += 32;
			else
				currentFrame.left = 0;
			this->animationTimer.restart();
		}
		this->sprite.setTextureRect(this->currentFrame);
	}
	void updateObject(std::list<std::shared_ptr<GameObject>>&)
	{
		animate();
	}
	void collision(GameObject* other, CollisionSide side)override
	{
		if (other->getObjectType() == ObjectType::player)
			wasReached = 1;
	}
	bool finished()
	{
		return wasReached;
	}
	void draw(sf::RenderTarget& target) override
	{
		target.draw(this->sprite);
		if (wasReached)
			target.draw(congratulations);
	}

};