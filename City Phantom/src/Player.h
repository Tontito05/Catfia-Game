#pragma once

#include "Entity.h"
#include "Vector2D.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void ResetDash();

	void ResetPlayer();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void SetParameters(pugi::xml_node parameters) {

		this->parameters = parameters;

	}

	void SetPosition(Vector2D pos);
	Vector2D GetPosition();

public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	SDL_Texture* menu = NULL;
	bool inMenu = false;
	Vector2D menusize = { 606,332 };

	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 2; // The force to apply when jumping
	bool Jumping = false; // Flag to check if the player is currently jumping
	bool JumpingRight = false;
	bool JumpingLeft = false; 

	States state;
	b2Vec2 TerminalVelocity = b2Vec2(0, 10);

	bool isDashingL = false;
	bool isDashingR = false;
	bool CanDash = true;
	float DashForce = 3;
	float DashSlower = 0;
	float JumpMinus = 1;
	bool attacking = false;
	int enemyKillImpact = 1;

	int life = 3;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation jumpingright;
	Animation jumpingleft;
	Animation walkingleft;
	Animation walkingright;
	Animation dying;

	bool Godmode = false;


};