export module PlayerAttack;
import GameObject;

export class PlayerAttack : public GameObject
{

public:

	PlayerAttack(sf::FloatRect playerPosition, bool playerFacingLeft)
	{
		this->objectType = ObjectType::playerAttack;
		if (playerFacingLeft)
			this->sprite.setPosition(playerPosition.left - 16, playerPosition.top);
		else
			this->sprite.setPosition(playerPosition.left + 32, playerPosition.top);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 16, 32));
	}

	virtual void updateObject(std::list<std::shared_ptr<GameObject>>&)override {}
	virtual void collision(GameObject*, CollisionSide)override {};

	bool toRemoval() override
	{
		return 1;
	}
};