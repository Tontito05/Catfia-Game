#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

public:

	int SightDistance = 0;
	Vector2D OGposition;
	States state;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation walkingright;
	Animation walkingleft;
	Animation dying;

private:

	States stat = States::WALKING_R;

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;

	PhysBody* pbody;
	Pathfinding* pathfinding;
};
