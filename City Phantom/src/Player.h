#pragma once

#include "Entity.h"
#include "Vector2D.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Timer.h"

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

	void checkLife();

	bool Fxplayed = false;

	void SetPosition(Vector2D pos);
	Vector2D GetPosition();

public:

	//Declare player parameters
	Vector2D OGPosition = { 80,80 };
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	SDL_Texture* menu = NULL;
	bool inMenu = false;
	Vector2D menusize = { 606,332 };


	float fxTimer = 0.0f;

	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 2; // The force to apply when jumping
	bool Jumping = false; // Flag to check if the player is currently jumping
	bool JumpingReset = false;//A flag to indicate that the player guets an impulse when an enemy is killed
	bool JumpingRight = false;// A way to tell where the player is going when jumping
	bool JumpingLeft = false; 

	States state;//the states of the player
	float TerminalVelocity = 0;//The variable that we controll to make the player fal slowly

	bool isDashingL = false;//Directions of the dash
	bool isDashingR = false;
	bool CanDash = true;//To tell if the player can or not dash
	float DashForce = 4;//The force aplyed to the player on the dash
	float DashSlower = 0;//The Force that goes againts the player on the dah (aka air friction)
	float JumpMinus = 1;//The variable that modifies the player jump depending on the time pressing the space
	bool falling = false; //A flag to indicate the terminal velocity

	//Life and damage management
	int life = 3;
	Timer damageTimer;
	int dashPlayer;
	int damagePlayer;
	int jumpPlayer;
	bool damaged = false;

	Timer walksoundTimer;

	//Flag to activate the other booleans and states for the player when it kills an enemy
	bool KillReset = false;

	//Animations
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation jumpingright;
	Animation jumpingleft;
	Animation walkingleft;
	Animation walkingright;
	Animation dying;
	Animation dash;
	Animation attack;
	Animation damage;



	//fx
	int walkingplayer;

	//For debbugguing pourposes
	int counter = 0;

	//Godmode bool
	bool Godmode = false;


};