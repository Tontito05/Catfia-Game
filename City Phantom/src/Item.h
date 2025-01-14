#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"

struct SDL_Texture;
enum class ItemType
{
	COIN,
	HEART,
	DASH,
	KEY
};
class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void SetPosition(Vector2D pos) {
		position.setX(pos.getX());
		position.setY(pos.getY());
	}

	void SetParameters(pugi::xml_node parameters, ItemType type_) {
		this->parameters = parameters;
		type = type_;
	}

public:

	bool isPicked = false;
	PhysBody* pbody;
	ItemType type;
	int h;
	int w;

	Animation* currentAnim = nullptr;

	Animation idleAnim;

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;

	//L08 TODO 4: Add a physics to an item

};
