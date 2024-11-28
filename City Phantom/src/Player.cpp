#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Engine.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(100, 300);
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	menu = Engine::GetInstance().textures.get()->Load("Assets/Textures/controllsotherversion.png");
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	jumpingright.LoadAnimations(parameters.child("animations").child("jumpingright"));
	jumpingleft.LoadAnimations(parameters.child("animations").child("jumpingleft"));
	walkingleft.LoadAnimations(parameters.child("animations").child("walkingleft"));
	walkingright.LoadAnimations(parameters.child("animations").child("walkingright"));
	dying.LoadAnimations(parameters.child("animations").child("dying"));
	dash.LoadAnimations(parameters.child("animations").child("dash"));
	attack.LoadAnimations(parameters.child("animations").child("attack"));

	currentAnimation = &idle;

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW/1.5,texH/1.5, bodyType::DYNAMIC);
	pbody->body->GetFixtureList()[0].SetFriction(0);
	pbody->body->SetFixedRotation(true);

	b2Filter filter;
	filter.categoryBits = Engine::GetInstance().physics->PLAYER_LAYER;

	pbody->body->GetFixtureList()[0].SetFilterData(filter);
	
	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	

	return true;
}

bool Player::Update(float dt)
{

	// L08 TODO 5: Add physics to the  player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);

	//God Mode Controll
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && Godmode==false)
	{
		Godmode = true;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && Godmode == true)
	{
		Godmode = false;
	}

	//Controlls Menu Controll
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN && inMenu == false)
	{
		inMenu = true;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN && inMenu == true)
	{
		inMenu = false;
	}

	if (inMenu == false && isDead == false)
	{
		if (Godmode == false)
		{

			// Move left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocity.x = -0.6 * 8;

				//Set the dash so the player can use it on the LEFT
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && CanDash == true) {
					// Apply an initial Left force
					pbody->body->SetLinearVelocity({ 0,0 });
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-DashForce, 0), true);
					isDashingL = true;
					CanDash = false;
				}
				state = States::WALKING_L;

			}
		

			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocity.x = 0.6 * 8;

				//Set the dash so the player can use it on the RIGHT
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && CanDash==true) {
					// Apply an initial Right force
					pbody->body->SetLinearVelocity({ 0,0 });
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashForce, 0), true);
					isDashingR = true;
					CanDash = false;
				}

				state = States::WALKING_R;

			}
			/*else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) != KEY_REPEAT && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) != KEY_REPEAT && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) != KEY_REPEAT) {


				state = States::IDLE_R;

			}*/
			

			//Jump
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Jumping == false) {
				// Apply an initial upward force
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
				Jumping = true;
				

			}


			// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
			if (Jumping == true)
			{

				velocity = pbody->body->GetLinearVelocity();
				//We insert this here so the player camn move during the jump, so we dont limit the movement
				//Move Left
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && pbody->body->GetLinearVelocity().x > -5 && isDashingR == false)
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-0.05, 0), true);
					velocity = pbody->body->GetLinearVelocity();
					state = States::JUMPING_L;
					JumpingLeft = true;

				}

				// Move right
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && pbody->body->GetLinearVelocity().x < 5 && isDashingL == false)
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0.05, 0), true);
					velocity = pbody->body->GetLinearVelocity();

					state = States::JUMPING_R;
					JumpingRight = true;

				}

				if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_UP) && (JumpMinus > 0))
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, JumpMinus), true);
					velocity = pbody->body->GetLinearVelocity();
				}
				else
				{
					JumpMinus -= 0.1;

				}

			}

				if (isDashingR == true)
				{
					//The parameter that creates the slowing sensation of the dash
					DashSlower -= 0.01f;
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
					velocity = pbody->body->GetLinearVelocity();
					velocity.y = -1;
					//where we look if the dash has finished or not
					DashForce += DashSlower;
					if (DashForce <= 0)
					{
						ResetDash();
					}

					state = States::DASH_L;
				}
				//Left Dash
				else if (isDashingL == true)
				{
					//Same as on top
					DashSlower += 0.01f;
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
					velocity = pbody->body->GetLinearVelocity();
					velocity.y = -1;
					DashForce -= DashSlower;
					if (DashForce <= 0)
					{
						ResetDash();
					}
					state = States::DASH_R;
				}
			

			if (pbody->body->GetLinearVelocity().y > 10)
			{
				velocity.y = TerminalVelocity.y;
			}
			pbody->body->SetLinearVelocity(velocity);
		}
		else // GOD MODE 
		{
			b2Vec2 velocityGodMode = b2Vec2(0, 0);
			// Move left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocityGodMode.x = -0.5 * 16;
				state = States::WALKING_L;
			}

			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocityGodMode.x = 0.5 * 16;
				state = States::WALKING_R;
			}

			//Fly
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {

				velocityGodMode.y = -0.5 * 16;
			}
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {

				velocityGodMode.y = 0.5 * 16;
			}

			pbody->body->SetLinearVelocity(velocityGodMode);
			
		}


		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) != KEY_REPEAT && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) != KEY_REPEAT && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) != KEY_REPEAT&&Jumping==false) {

			state = States::IDLE_R;
		}
	}


	//Reset
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

		ResetPlayer();
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
	
	/*if (pbody->body->GetLinearVelocity().y < 0 && Jumping == true && state == States::JUMPING_L) {
		// Use jump animation
		currentAnimation = &jumpingleft;

	}
	else if (Jumping == true && state == States::JUMPING_R) {

		currentAnimation = &jumpingright;
	}
	else if (Jumping == true) {

		currentAnimation = &jumpingright;
	}

	/*else if (pbody->body->GetLinearVelocity().y > 5) {

		currentAnimation = &jumpingright;
	}

	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT&&state == States::WALKING_L) {
		currentAnimation = &walkingleft;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT&&state == States::WALKING_R) {
		currentAnimation = &walkingright;
	}
	else if (isDead == true) {


		currentAnimation = &dying;
	}
	else {
		currentAnimation = &idle;  // Only set to idle if no other state is active
	}*/

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();

	//Menu Controll
	if (inMenu == true)
	{
			Engine::GetInstance().render.get()->DrawTexture(menu,(- Engine::GetInstance().render.get()->camera.x / 2)+20, (- Engine::GetInstance().render.get()->camera.y / 2)+20);
	}


	return true;
}





bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

		switch (physB->ctype)
		{
		case ColliderType::PLATFORM:
			LOG("Collision PLATFORM");
			//reset the jump flag when touching the ground

			Jumping = false;
			JumpMinus = 1;
			CanDash = true;

			break;
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		case ColliderType::ENEMY:
			LOG("Collision ENEMY");

			for (int i = 0; i < Engine::GetInstance().scene->enemyList.size(); i++)
			{
				if (physB == Engine::GetInstance().scene->enemyList[i]->pbody)
				{
					if ((Godmode == false) && (state != States::DYING) && (state != States::DASH_L) && (state != States::DASH_R) && (Engine::GetInstance().scene->enemyList[i]->isDead == false))
					{
						checkLife();
					}
					else if ((Godmode == false) && (state != States::DYING) && ((state == States::DASH_L) || (state == States::DASH_R)) && (Engine::GetInstance().scene->enemyList[i]->isDead == false))
					{

						Engine::GetInstance().scene->enemyList[i]->isDead = true;
						if (state == States::DASH_L)
						{
							if (Engine::GetInstance().scene->enemyList[i]->type == EntityType::FYING_ENEMY)
							{
								Engine::GetInstance().scene->enemyList[i]->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(enemyKillImpact, -0.1), true);
							}
							else if(Engine::GetInstance().scene->enemyList[i]->type == EntityType::WALKING_ENEMY)
							{
								Engine::GetInstance().scene->enemyList[i]->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(enemyKillImpact, -1), true);
							}
							
						}
						else
						{
							if (Engine::GetInstance().scene->enemyList[i]->type == EntityType::FYING_ENEMY)
							{
								Engine::GetInstance().scene->enemyList[i]->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-enemyKillImpact, -0.1), true);
							}
							else if (Engine::GetInstance().scene->enemyList[i]->type == EntityType::WALKING_ENEMY)
							{
								Engine::GetInstance().scene->enemyList[i]->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-enemyKillImpact, -1), true);
							}
						}
					}
				}
			}

			break;

		case ColliderType::WALL:
			ResetDash();
			LOG("Collision WALL");
			
			break;
		case ColliderType::DEATH:
			LOG("Collision DEATH");
			
			if (Godmode == false)
			{
				checkLife();
			}

			break;
		}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{

		switch (physB->ctype)
		{
		case ColliderType::PLATFORM:
			LOG("End Collision PLATFORM");
			CanDash = true;
			break;
		case ColliderType::ITEM:
			LOG("End Collision ITEM");
			break;
		case ColliderType::UNKNOWN:
			LOG("End Collision UNKNOWN");
			break;
		case ColliderType::WALL:
			LOG("End Collision WALL");
			
			break;
		case ColliderType::DEATH:
			LOG("End Collision DEATH");
			break;
		default:
			break;
		}
}

void Player::ResetDash()
{
	DashForce = 3;
	DashSlower = 0;
	isDashingL = false;
	isDashingR = false;
	attacking = false;
}

void Player::ResetPlayer()
{
	CleanUp();
	pbody->body->DestroyFixture(&pbody->body->GetFixtureList()[0]);
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;
	Awake();
	Start();
	isDead = false;
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Player::checkLife()
{
	if (life <= 0)
	{
		isDead = true;
		state = States::DYING;
	}
	else
	{
		life--;
	}
}