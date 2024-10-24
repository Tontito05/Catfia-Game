#pragma once

#include "Entity.h"
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

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void SetParameters(pugi::xml_node parameters) {

		this->parameters = parameters;

	}

public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;

	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 2; // The force to apply when jumping
	bool Jumping = false; // Flag to check if the player is currently jumping

	States state;
	b2Vec2 TerminalVelocity = b2Vec2(0, 10);

	bool isDashingL = false;
	bool isDashingR = false;
	bool CanDash = true;
	float DashForce = 3;
	float DashSlower = 0;
	float JumpMinus = 1;



	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation jumping;
	Animation falling;

	bool Godmode = false;


};