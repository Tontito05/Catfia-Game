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
	position.setY(parameters.child("position").attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();


	//Load animations

	walkingright.LoadAnimations(parameters.child("animations").child("walkingright"));
	walkingleft.LoadAnimations(parameters.child("animations").child("walkingleft"));
	dying.LoadAnimations(parameters.child("animations").child("dying"));

	walkingright2.LoadAnimations(parameters.child("animations").child("walkingright2"));
	walkingleft2.LoadAnimations(parameters.child("animations").child("walkingleft2"));
	dying2.LoadAnimations(parameters.child("animations").child("dying2"));
	currentAnimation = &idle;

	enemyMovementSfx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/enemy_sfx.ogg");
	enemydamage = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/damage_sfx.ogg");
	//Add a physics to an item - initialize the physics body

	//Diferent enemyes diferent initializatons
	if (type == EntityType::FYING_ENEMY)
	{
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 1.2, texW / 1.2, bodyType::DYNAMIC);
		DestDistance = 10;
	}
	else if (type == EntityType::WALKING_ENEMY)
	{
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH, texW, bodyType::DYNAMIC);
		DestDistance = 7;
	}

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetFixedRotation(true);
	pbody->listener = this;

	//Assign a layer only for the enemyes
	b2Filter filter;
	filter.categoryBits = Engine::GetInstance().physics->ENEMY_LAYER;
	pbody->body->GetFixtureList()[0].SetFilterData(filter);

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding(type);
	ResetPath();

	return true;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB)
{

	if (physB->ctype == ColliderType::PLATFORM || physB->ctype == ColliderType::DEATH) {

		//if it collides with a platform or a death collider, it will change the layer and it will set them on a {0,0} velocity, on a death state basically
		if (pbody->body->GetFixtureList() != nullptr && pbody->body->GetFixtureList() != nullptr && isDead == true) {
			pbody->body->SetLinearVelocity(b2Vec2(0, 0));

			b2Filter filter;

			// CatBits tipo de layer, maskBits con que layers colisiona
			filter.categoryBits = 0;
			filter.maskBits = 0;

			//Ponidendo nuevo filtro
			pbody->body->GetFixtureList()[0].SetFilterData(filter);

			pbody->body->SetGravityScale(0);
		}
	}
}

bool Enemy::Update(float dt)
{
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

		resetEnemy();
	}


	//Check if the enemy is dead
	if (isDead == false)
	{
		//GENERALS FOR BOTH ENEMYES
		while (SightDistance <= DestDistance)
		{
			if (pathfinding->pathTiles.empty())
			{
				pathfinding->PropagateAStar(MANHATTAN);
			}
			SightDistance++;
			if (Engine::GetInstance().physics->debug == true && SightDistance == DestDistance)
			{
				pathfinding->DrawPath();
			}
		}

		Vector2D PosInMap = Engine::GetInstance().map->WorldToMap(position.getX(), position.getY());
		b2Vec2 velocity;

		//Check the enemy type
		switch (type)
		{
		case EntityType::FYING_ENEMY:

			//I don't really know why but the flying enemies slowly go down, so I added a little velocity to counteract that
			//i tryed to get it to work with the gravity scale to 0, but then it just floats in the air
			velocity = b2Vec2(0, -0.315);

			if (pathfinding->pathTiles.size() > 0) {

				//We get the next tile in the path and create a vetor that goes there and apoly a velocity to the enemy
				Vector2D TileOG = pathfinding->pathTiles.back();
				Vector2D Tile = Engine::GetInstance().map->MapToWorld(TileOG.getX(), TileOG.getY());
				Vector2D pos = Tile - position;
				pos = pos.normalized();
				Engine::GetInstance().audio.get()->PlayFx(enemyMovementSfx, 0);

				//The velocity is reduced to make the enemy move slower
				velocity = b2Vec2(pos.getX()*0.2, pos.getY()*0.2);
				if (pos.getX() >= 0)
				{
					state = States::WALKING_R;
				}
				else
				{
					state = States::WALKING_L;
				}

			}
			//Reset the path
			if (SightDistance > DestDistance)
			{
				ResetPath();
				SightDistance = 0;
			}
			pbody->body->SetLinearVelocity(velocity);


			break;

		case EntityType::WALKING_ENEMY://______________________________________________________________________________________________

			velocity = b2Vec2(0, -GRAVITY_Y);
			
			//Movement of the enemy
			if (pathfinding->pathTiles.size() > 0) {

				//We get the next tile in the path and create a vetor that goes there and apoly a velocity to the enemy
				Vector2D TileOG = pathfinding->pathTiles.back();
				Vector2D Tile = Engine::GetInstance().map->MapToWorld(TileOG.getX(), TileOG.getY());
				Vector2D pos = Tile - position;
				pos = pos.normalized();
				//The velocity is reduced to make the enemy move slower
				velocity = b2Vec2(pos.getX()*2, 0);
				if (pos.getX() >= 0)
				{
					state = States::WALKING_R;
				}
				else
				{
					state = States::WALKING_L;
				}
				DestDistance = 20;
			}
			//Reset the path
			if (SightDistance > DestDistance)
			{
				ResetPath();
				SightDistance = 0;
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
	CleanUp();
	pbody->body->DestroyFixture(&pbody->body->GetFixtureList()[0]);
	Awake();
	Start();
	SetPosition(OGposition);
	pathfinding->foundDestination = false;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}