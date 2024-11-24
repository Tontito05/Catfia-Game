#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"

EntityManager::EntityManager() : Module()
{
	name = "entitymanager";
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake()
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;

	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->CleanUp();
	}

	entities.clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	//L04: TODO 3a: Instantiate entity according to the type and add the new entity to the list of Entities
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::ENEMY:
		entity = new Enemy();
		break;
	default:
		break;
	}

	entities.push_back(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		if (*it == entity) {
			(*it)->CleanUp();
			delete* it; // Free the allocated memory
			entities.erase(it); // Remove the entity from the list
			break; // Exit the loop after removing the entity
		}
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.push_back(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Update(dt);
		SetAnimation( entity);
	}
	
	return ret;

}

void EntityManager::SetAnimation(Entity* entity) {


	if (entity->type == EntityType::PLAYER) {

		Player* player = dynamic_cast<Player*>(entity);

		switch (player->state)
		{
		case States::IDLE_L:
			player->currentAnimation = &player->idle;
			break;
		case States::IDLE_R:
			player->currentAnimation = &player->idle;
			break;
		case States::WALKING_L:
			player->currentAnimation = &player->walkingleft;
			break;
		case States::WALKING_R:
			player->currentAnimation = &player->walkingright;
			break;
		case States::DASH_L:
			player->currentAnimation = &player->walkingleft;
			break;
		case States::DASH_R:
			player->currentAnimation = &player->walkingright;
			break;
		case States::JUMPING_L:
			player->currentAnimation = &player->jumpingleft;
			break;
		case States::JUMPING_R:
			player->currentAnimation = &player->jumpingright;
			break;
		case States::FALLING_L:
			player->currentAnimation = &player->jumpingleft;
			break;
		case States::FALLING_R:
			player->currentAnimation = &player->jumpingright;
			break;

		case States::DYING:
			player->currentAnimation = &player->dying;
			break;
		default:
			LOG("Unknown state. No animation set.");
			break;
		}




	}



}

