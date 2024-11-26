#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Vector2D.h"
#include "Map.h"

Enemy::Enemy(EntityType type_) : Entity(type_)
{
	name = "enemy";
}

Enemy::~Enemy() {
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();


	//Load animations
	
	walkingright.LoadAnimations(parameters.child("animations").child("walkingright"));
	walkingleft.LoadAnimations(parameters.child("animations").child("walkingleft"));
	dying.LoadAnimations(parameters.child("animations").child("dying"));
	currentAnimation = &idle;
	
	//Add a physics to an item - initialize the physics body
	if (type == EntityType::FYING_ENEMY)
	{
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 3, texW / 3, bodyType::DYNAMIC);
		DestDistance = 10;
	}
	else if (type == EntityType::WALKING_ENEMY)
	{
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, texW / 1.5, bodyType::DYNAMIC);
		DestDistance = 5;
	}	

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetFixedRotation(true);

	// Set the gravity of the body

	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding(type);
	ResetPath();

	return true;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
	

	if (physB->ctype == ColliderType::PLAYER) {

	}
	else if (physB->ctype == ColliderType::DEATH) {
		isDead = true;
	}
	else if (physB->ctype == ColliderType::WALL) {

	}
	else if (physB->ctype == ColliderType::PLATFORM) {
		if (isDead == true)
		{
			pbody->body->SetLinearVelocity(b2Vec2(0, 0));
			pbody->body->DestroyFixture(pbody->body->GetFixtureList());
		}
	}
	else if (physB->ctype == ColliderType::ITEM) {

	}
	else if (physB->ctype == ColliderType::ENEMY) {

	}
	else if (physB->ctype == ColliderType::UNKNOWN) {


	}
}

bool Enemy::Update(float dt)
{
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

			SetPosition(OGposition);
			pathfinding->foundDestination = false;
			resetEnemy();

			}

			//GENERALS

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F8) == KEY_REPEAT)
			{
				pathfinding->DrawPath();
			}
			if (SightDistance <= DestDistance)
			{

				pathfinding->PropagateAStar(MANHATTAN);

				SightDistance++;
			}
			else
			{
				Vector2D pos = GetPosition();
				Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
				pathfinding->ResetPath(tilePos);
				SightDistance = 0;
			}

		if (isDead == false)
		{


			Vector2D PosInMap = Engine::GetInstance().map->WorldToMap(position.getX(), position.getY());
			b2Vec2 velocity;

			switch (type)
			{
			case EntityType::FYING_ENEMY:

				velocity = b2Vec2(0, -0.315);

				if (pathfinding->foundDestination == true)
				{
					if (pathfinding->pathTiles.size() > 0) {

						Vector2D TileOG = pathfinding->pathTiles.front();
						Vector2D Tile = Engine::GetInstance().map->MapToWorld(TileOG.getX(), TileOG.getY());
						Vector2D pos = Tile - position;
						pos.normalized();
						float velocityReducer = 0.01f;
						velocity = b2Vec2(pos.getX() * velocityReducer, pos.getY() * velocityReducer);
						if (pos.getX() >= 0)
						{
							state = States::WALKING_R;
						}
						else
						{
							state = States::WALKING_L;
						}
					}
				}


				pbody->body->SetLinearVelocity(velocity);


				break;

			case EntityType::WALKING_ENEMY://______________________________________________________________________________________________

				velocity = b2Vec2(0, -GRAVITY_Y);

				if (pathfinding->foundDestination != true)
				{
					if ((pathfinding->IsWalkable(PosInMap.getX() + 1, PosInMap.getY()) == true)
						&& (stat == States::WALKING_R))
					{
						velocity.x = 0.4 * 16;
					}
					else if ((pathfinding->IsWalkable(PosInMap.getX() + 1, PosInMap.getY()) == false)
						&& (stat == States::WALKING_R))
					{
						stat = States::WALKING_L;
					}

					if ((pathfinding->IsWalkable(PosInMap.getX(), PosInMap.getY()) == true)
						&& (stat == States::WALKING_L))
					{
						velocity.x = -0.4 * 16;
					}
					else if ((pathfinding->IsWalkable(PosInMap.getX(), PosInMap.getY()) == false)
						&& (stat == States::WALKING_L))
					{
						stat = States::WALKING_R;
					}

				}
				if (pathfinding->foundDestination == true)
				{
					if (pathfinding->pathTiles.size() > 0) {

						Vector2D TileOG = pathfinding->pathTiles.front();
						Vector2D Tile = Engine::GetInstance().map->MapToWorld(TileOG.getX(), TileOG.getY());
						Vector2D pos = Tile - position;
						pos.normalized();
						float velocityReducer = 0.01f;
						velocity = b2Vec2(pos.getX() * velocityReducer, pos.getY() * velocityReducer);
						if (pos.getX() >= 0)
						{
							state = States::WALKING_R;
						}
						else
						{
							state = States::WALKING_L;
						}
					}
				}
				pbody->body->SetLinearVelocity(velocity);

				break;
			default:
				break;
			}



		}
		else
		{
			state = States::DYING;
		}


		//POST GENERALS

		// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();
	

	return true;
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	if (pbody == nullptr || pbody->body == nullptr) {
		// Handle the error, e.g., return a default position or log an error
		return Vector2D(0, 0);
	}
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::resetEnemy() {

	isDead = false;
	state = States::IDLE_L;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}