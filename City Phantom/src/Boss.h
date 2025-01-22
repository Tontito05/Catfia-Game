#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Box2D/Box2D.h"
#include "Timer.h"

struct SDL_Texture;

class Boss : public Entity
{
public:

	Boss(EntityType type_);

	virtual ~Boss();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void Reset();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	bool IsAlive() {

		if (state != States::DYING) {

			return true;
		}
	}

	void SetAlive() {

		state = States::IDLE_R;
	}

	void Damaged();

public:

	Vector2D OGposition;
	States state = States::WALKING_R;
	Animation* currentAnimation = nullptr;
	Animation walkingright;
	Animation walkingleft;
	Animation attackleft;
	Animation attackright;
	Animation stunned;
	Animation dying;
	Animation damage;
	int enemyMovementSfx;
	int enemydamage;


	bool damageFx = false;
	bool isDead = false;
	PhysBody* pbody;

	//Movement
	b2Vec2 Vel;
	bool CheckIfAttack();

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Timer attTimer;
	int attDuration = 2;
	int loops = 2;
	int health = 2;
};
