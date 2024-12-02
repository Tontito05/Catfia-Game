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
	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
		item->SetParameters(itemNode);
	}

	// Create a enemy using the entity manager 
	//the num of enemyes in tyhe level


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

	map = Engine::GetInstance().map.get();
	layerNav = map->GetNavigationLayer();
	if (enemyesIn == false)
	{
		CreateEnemyes();
		enemyesIn = true;
	}

	return true;
}

void Scene::CreateEnemyes()
{
	for (int x = 0; x < map->GetWidth(); x++)
	{
		for (int y = 0; y < map->GetHeight(); y++)
		{
			if (layerNav != nullptr) {
				if (x >= 0 && y >= 0 && x < map->GetWidth() && y < map->GetHeight()) {

					int gid = layerNav->Get(x, y);
					if (gid == enemyGround)
					{
							pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy");
							
								Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::WALKING_ENEMY);
								enemy->SetParameters(enemyNode);
								enemy->Start();
								enemy->OGposition = Vector2D(map->MapToWorld(x, y));
								enemy->SetPosition(enemy->OGposition);
								enemyList.push_back(enemy);
								enemyCounter++;
							
					}
					else if (gid == enemySky)
					{
						pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy");
						
							Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FYING_ENEMY);
							enemy->SetParameters(enemyNode);
							enemy->Start();
							enemy->OGposition = Vector2D(map->MapToWorld(x, y));
							enemy->SetPosition(enemy->OGposition);
							enemyList.push_back(enemy);
							enemyCounter++;
							
					}
				}
			}
		}
	}

}

float Scene::Slower(float ogPos, float goalPos, float time)
{
	float acceleration = goalPos - ogPos;
	float speed = ogPos + time * acceleration;
	return speed;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 2;
	float WinWidth = Engine::GetInstance().window.get()->width;
	float WinHeight = Engine::GetInstance().window.get()->height;
	
	//Camera movement
	int mapLimitX = 3328;
	int mapLimitY = 1184;

	if (player->position.getY() > WinHeight / (camSpeed*2) &&
		player->position.getY() < mapLimitY - WinHeight / (camSpeed*2))
	{
		Engine::GetInstance().render.get()->camera.y = (-player->position.getY() * camSpeed) + WinHeight / 2;
	}
	if (player->position.getX() > (WinWidth-230) / (camSpeed * 2) &&
		player->position.getX() < mapLimitX - WinWidth / (camSpeed*2))
	{
		Engine::GetInstance().render.get()->camera.x = Slower(Engine::GetInstance().render.get()->camera.x, (- player->position.getX() * camSpeed) + WinWidth / 2, 0.2f);
	}
	

	return true;



}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		LoadState();
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveState();

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	SDL_DestroyTexture(img);
	return true;
}

Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Read XML and restore information

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("entities").child("player").attribute("x").as_int(),
		sceneNode.child("entities").child("player").attribute("y").as_int());
	player->SetPosition(playerPos);

	//enemies
	// ...

}

void Scene::SaveState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY());

	//enemies
	// ...

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");

}
