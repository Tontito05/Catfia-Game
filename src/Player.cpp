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
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	jumping.LoadAnimations(parameters.child("animations").child("jumping"));
	falling.LoadAnimations(parameters.child("animations").child("falling"));
	currentAnimation = &idle;

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW/1.5,texH/1.5, bodyType::DYNAMIC);
	pbody->body->GetFixtureList()[0].SetFriction(0);
	pbody->body->SetFixedRotation(0);
	
	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	

	return true;
}


bool Player::Update(float dt)
{
	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && Godmode==false)
	{
		Godmode = true;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && Godmode == true)
	{
		Godmode = false;
	}

	if (Godmode == false)
	{

		// Move left
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -0.4 * 16;

			//Set the dash so the player can use it on the LEFT
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && isDashingL == false) {
				// Apply an initial Left force
				pbody->body->SetLinearVelocity({ 0,0 });
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-DashForce, 0), true);
				isDashingL = true;
			}
			state = States::WALKING_L;
		}

		// Move right
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = 0.4 * 16;

			//Set the dash so the player can use it on the RIGHT
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && isDashingR == false) {
				// Apply an initial Right force
				pbody->body->SetLinearVelocity({ 0,0 });
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashForce, 0), true);
				isDashingR = true;
			}
			state = States::WALKING_R;
		}

		//Reset
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

			CleanUp();
			pbody->body->DestroyFixture(&pbody->body->GetFixtureList()[0]);
			Engine::GetInstance().render.get()->camera.x = 0;
			Engine::GetInstance().render.get()->camera.y = 0;
			Awake();
			Start();
		}

		//Jump
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Jumping == false) {
			// Apply an initial upward force
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			Jumping = true;
			

			currentAnimation = &jumping;
		

		}
		if (Jumping == false) {

			currentAnimation = &idle;
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
			}
			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && pbody->body->GetLinearVelocity().x < 5 && isDashingL == false)
			{
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0.05, 0), true);
				velocity = pbody->body->GetLinearVelocity();

				state = States::JUMPING_R;
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

		//Glovals to add --> DashForce / DashSlower / PlayerVelocity 

		//Right Dash
		if (CanDash == true)
		{

			if (isDashingR == true)
			{
				//The parameter that creates the slowing sensation of the dash
				DashSlower -= 0.01f;
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
				velocity = pbody->body->GetLinearVelocity();
				velocity.y = 0;
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
				velocity.y = 0;
				DashForce -= DashSlower;
				if (DashForce <= 0)
				{
					ResetDash();
				}
				state = States::DASH_R;
			}
		}

		//Stop the acceleration



		if (pbody->body->GetLinearVelocity().y > 10)
		{
			velocity.y = TerminalVelocity.y;
		}
		pbody->body->SetLinearVelocity(velocity);
		// Apply the velocity to the player

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

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

			CleanUp();
			pbody->body->DestroyFixture(&pbody->body->GetFixtureList()[0]);
			Engine::GetInstance().render.get()->camera.x = 0;
			Engine::GetInstance().render.get()->camera.y = 0;
			Awake();
			Start();
		}

		pbody->body->SetLinearVelocity(velocityGodMode);
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
	

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();

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
			isDashingL = false;
			isDashingR = false;
			CanDash = true;

			break;
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		case ColliderType::WALL:
			LOG("Collision WALL");
			isDashingL = false;
			isDashingR = false;
			break;
		default:
			break;
		}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{

		switch (physB->ctype)
		{
		case ColliderType::PLATFORM:
			LOG("End Collision PLATFORM");
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
		default:
			break;
		}
}

void Player::ResetDash()
{

	DashForce = 3;
	DashSlower = 0;
	CanDash = false;
}