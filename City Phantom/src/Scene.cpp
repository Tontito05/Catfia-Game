#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	
	player->SetParameters(configParameters.child("entities").child("player"));
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->position = Vector2D(200, 672);
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "Map.tmx");
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 2;
	
	//Camera movement
	int mapLimitX = 3328;
	int mapLimitY = 1184;

	if (player->position.getY() > Engine::GetInstance().window.get()->height / (camSpeed*2) &&
		player->position.getY() < mapLimitY - Engine::GetInstance().window.get()->height / (camSpeed*2))
	{
		Engine::GetInstance().render.get()->camera.y = (-player->position.getY() * camSpeed) + Engine::GetInstance().window.get()->height / 2;
	}
	if (player->position.getX() > Engine::GetInstance().window.get()->width / (camSpeed * 2) &&
		player->position.getX() < mapLimitX - Engine::GetInstance().window.get()->width / (camSpeed*2))
	{
		Engine::GetInstance().render.get()->camera.x = (-player->position.getX() * camSpeed) + Engine::GetInstance().window.get()->width / 2;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_G) == KEY_DOWN) {

		LOG("%d", Engine::GetInstance().window.get()->width);
		LOG("%d", Engine::GetInstance().window.get()->height);

	}
	return true;



}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	SDL_DestroyTexture(img);
	return true;
}
