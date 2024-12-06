#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Box2D/Box2D.h"
#include "Timer.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy(EntityType type_);

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

	b2Vec2 FollowPlayer();

	void resetEnemy();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void ResetPath();

	bool IsAlive() {

		if (state != States::DYING) {

			return true;
		}
	}

	void SetAlive() {

		state = States::IDLE_R;
	}

public:

	int SightDistance = 0;
	Vector2D OGposition;
	States state;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation walkingright;
	Animation walkingleft;
	Animation dying;
	int enemyMovementSfx;
	int enemydamage;
	Animation walkingright2;
	
	Animation dying2;


	bool isDead = false;
	PhysBody* pbody;

private:

	States stat = States::WALKING_R;
	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Pathfinding* pathfinding;
	int DestDistance;
	Timer aggroTimer;


	

};
