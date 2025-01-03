#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "Vector2D.h"
#include <vector>

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void LoadState();

	void SaveState();

	// Return the player position
	Vector2D GetPlayerPosition();

	float Slower(float ogPos, float goalPos, float time);

	void CreateEnemyes();
	MapLayer* layerNav;
	Map* map;
	int enemyCounter = 0;
	std::vector<Enemy*> enemyList;
	bool in = false;

private:

	SDL_Texture* img;
	bool once = false;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	int enemySky = 560;
	int enemyGround = 561;
	bool enemyesIn = false;

	int WWidth;
	int WHeight;

};