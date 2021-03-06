#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/sony_controller_input_manager.h>
#include <input/input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/scene.h>
#include <input/keyboard.h>

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	menuinited(false),
	startupinited(false),
	gameinited(false),
	trackNum_(1),
	game_state(STARTUP)
{
}

void SceneApp::Init()
{
	// create our helpers!
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	input_manager_ = gef::InputManager::Create(platform_);

	// set up lights camera and ACTION
	InitFont();
	SetupLights();
	SetupCamera();

	// start up the game
}

void SceneApp::CleanUp()
{
	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;
}

bool SceneApp::Update(float frame_time)
{

	// check the current game state and update the appropriate things, also check to see if the state has been initialised yet!
	switch (game_state)
	{
	case STARTUP:
		if (startupinited == false)
		{
			StartInit();
			startupinited = true;
		}
		StartUpdate();
		break;

	case MENU:
		if (menuinited == false)
		{
			MenuInit();
			menuinited = true;
		}
		MenuUpdate();
		break;

	case GAME:
		if (gameinited == false)
		{
			GameInit();
			gameinited = true;
		}

		GameUpdate(frame_time);
		break;

	case GAMEOVER:
		GameOverUpdate();
		break;

	case EXIT:
		return false; // exit the application
		break;
	}

	return true;
}

void SceneApp::Render()
{


#if 0
	gef::Matrix44 view_matrix;

	projection_matrix = platform_.PerspectiveProjectionFov(camera_fov, (float)platform_.width() / (float)platform_.height(), near_plane, far_plane);
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_projection_matrix(projection_matrix);
	renderer_3d_->set_view_matrix(view_matrix);

	// draw meshes here
	renderer_3d_->Begin();
	renderer_3d_->End();
#endif


	// check to see what the game state is and render the appropriate things
	switch (game_state)
	{
	case STARTUP:
		if (startupinited)
			StartRender();
		break;

	case MENU:
		if (menuinited)
			MenuRender();
		break;

	case GAME:
		if (gameinited)
			GameRender();
		break;

	case GAMEOVER:
		GameOverRender();
		break;

	case EXIT:
		break;

	}


}
void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

// not used anymore
void SceneApp::DrawHUD()
{
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	if (font_)
	{
		// display frame rate
		//car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2)
	}
}

void SceneApp::SetupLights()
{
	// we need to see the game
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void SceneApp::SetupCamera()
{
	// initialise the camera settings
	camera_eye = gef::Vector4(300.0f, 100.0f, 50.0f);
	camera_lookat = gef::Vector4(0.0f, 0.0f, 0.0f);
	camera_up = gef::Vector4(0.0f, 1.0f, 0.0f);
	camera_fov = gef::DegToRad(45.0f);
	near_plane = 0.01f;
	far_plane = 100.f;
}


void SceneApp::Keyboard(unsigned char key)
{
	// this is for the control system. only used in the game, could be cleaner!
	switch (key) {
	case 'a': controlState |= TDC_LEFT; break;
	case 'd': controlState |= TDC_RIGHT; break;
	case 'w': controlState |= TDC_UP; break;
	case 's': controlState |= TDC_DOWN; break;
	default: break;
	}
}

void SceneApp::KeyboardUp(unsigned char key)
{
	// this is for the control system. only used in the game, could be cleaner!
	switch (key) {
	case 'a': controlState &= ~TDC_LEFT; break;
	case 'd': controlState &= ~TDC_RIGHT; break;
	case 'w': controlState &= ~TDC_UP; break;
	case 's': controlState &= ~TDC_DOWN; break;
	default: break;
	}
}

void SceneApp::handleContact(b2Contact* contact, bool began)
{
	// contact handler for our tires
	b2Fixture* a = contact->GetFixtureA();
	b2Fixture* b = contact->GetFixtureB();
	FixtureUserData* fudA = (FixtureUserData*)a->GetUserData();
	FixtureUserData* fudB = (FixtureUserData*)b->GetUserData();

	if (!fudA || !fudB)
		return;

	if (fudA->getType() == FUD_CAR_TIRE || fudB->getType() == FUD_GROUND_AREA)
		tire_vs_groundArea(a, b, began);
	else if (fudA->getType() == FUD_GROUND_AREA || fudB->getType() == FUD_CAR_TIRE)
		tire_vs_groundArea(b, a, began);
}


void SceneApp::tire_vs_groundArea(b2Fixture* tireFixture, b2Fixture* groundAreaFixture, bool began)
{
	// make sure there is ground under the tires!
	Tire* tire = (Tire*)tireFixture->GetBody()->GetUserData();
	GroundAreaFUD* gaFud = (GroundAreaFUD*)groundAreaFixture->GetUserData();
	if (began)
		tire->addGroundArea(gaFud);
	else
		tire->removeGroundArea(gaFud);
}

void SceneApp::StartInit()
{
	// set up our initial game, load the splash screen along with loading any big files, nothing to do here though!
}

void SceneApp::StartUpdate()
{
	StartInput();
}

void SceneApp::StartRender()
{
	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here

	gef::Matrix44 projection_matrix;

	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	projection_matrix = platform_.OrthographicFrustum(0, (screen_width), 0, (screen_height), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	if (font_)
	{
		// show the title and the press enter to start message
		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) - 10, -0.9f), 0.4f, 0xff00ff00, gef::TJ_CENTRE, "MULTI-LAYER PERCEPTRON RACING!");

		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) + 10, -0.9f), 0.3f, 0xff00ff00, gef::TJ_CENTRE, "Press enter to start!");
	}


	sprite_renderer_->End();
}

void SceneApp::StartInput()
{
	input_manager_->Update();

	const gef::Keyboard* keyboard = input_manager_->keyboard();
	if (keyboard)
	{
		// go to the main menu
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_RETURN))
			game_state = MENU;
	}

}

void SceneApp::MenuInit()
{
	// load the menu assests
	trainnetworks = false;
	practice = false;
	time_trial = false;
	num_laps = 3;
	net_type = EBP;
}

void SceneApp::MenuUpdate()
{
	MenuInput();
}

void SceneApp::MenuRender()
{
	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here

	gef::Matrix44 projection_matrix;

	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;


	projection_matrix = platform_.OrthographicFrustum(0, (screen_width), 0, (screen_height), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	if (font_)
	{
		// display what AI is being raced against
		switch (net_type)
		{
		case EBP:
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) - 10.0f, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "YOU ARE RACING AGAINST EBP!");
			break;
		case RPROP:
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) - 10.0f, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "YOU ARE RACING AGAINST RPROP!");
			break;
		case RMGSN:
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) - 10.0f, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "YOU ARE RACING AGAINST RMGS!");
			break;
		}
		// display the controls to switch the AI
		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2), -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "Use W and S to change your opponent!");

		// display the start message
		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) + 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "Press enter to start!");

		// display other options
		if (trainnetworks)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 100, (screen_height / 2) + 20, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "Training Networks: ON   [T]");
		}
		else
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 100, (screen_height / 2) + 20, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "Training Networks: OFF  [T]");
		}
		if (time_trial)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 100, (screen_height / 2) + 40, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "TIME TRIAL: ON    [O]");
		}
		else
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 100, (screen_height / 2) + 40, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "TIME TRIAL: OFF   [O]");
		}
		if (practice)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 100, (screen_height / 2) + 60, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "PRACTICE: ON    [P]");
		}
		else
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 100, (screen_height / 2) + 60, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "PRACTICE: OFF   [P]");
		}
		// display number of laps
		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) + 30, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "Number of laps:%i", num_laps - 1);
		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) + 40, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "Use R and F to change the number of laps");

		//car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2)
	}
	sprite_renderer_->End();
}

void SceneApp::MenuInput()
{
	input_manager_->Update();

	const gef::Keyboard* keyboard = input_manager_->keyboard();
	if (keyboard)
	{
		// check to see if the game needs to be started
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_RETURN))
			game_state = GAME;

		// change the AI opponent
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_S))
		{
			if (net_type == EBP)
			{
				net_type = RPROP;
			}
			else if (net_type == RPROP)
			{
				net_type = RMGSN;
			}
		}
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_W))
		{
			if (net_type == RMGSN)
			{
				net_type = RPROP;
			}
			else if (net_type == RPROP)
			{
				net_type = EBP;
			}
		}

		// this below can be used for track selection
		/*
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_A))
		{
		if (net_type == EBP)
		{
		net_type = RPROP;
		}
		else if (net_type == RPROP)
		{
		net_type == RMGSN;
		}
		}
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_D))
		{
		if (net_type == RMGSN)
		{
		net_type = RPROP;
		}
		else if (net_type == RPROP)
		{
		net_type == EBP;
		}
		}*/

		// set the number of laps
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_R))
		{
			if (num_laps < 6)
			{
				num_laps++;
			}
		}
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_F))
		{
			if (num_laps > 2)
			{
				num_laps--;
			}
		}

		// set training, practice or time trial modes
		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_T))
		{
			if (trainnetworks == true)
			{
				trainnetworks = false;
			}
			else if (trainnetworks == false)
			{
				trainnetworks = true;
			}
		}

		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_P))
		{
			if (practice == true)
			{
				practice = false;
			}
			else if (practice == false)
			{
				practice = true;
			}
		}

		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_O))
		{
			if (time_trial == true)
			{
				time_trial = false;
			}
			else if (time_trial == false)
			{
				time_trial = true;
			}
		}

		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_ESCAPE))
		{
			game_state = EXIT;
		}

	}
}

void SceneApp::GameInit()
{
	// define the data size, this could be improved with something to check how many lines are in the dataset but for now this works perfectly
	dataSize = 10000;


	// create the world
	world = new b2World(b2Vec2(0, 0));


	// set up box 2d related 
	world->SetGravity(b2Vec2(0, 0));
	world->SetDestructionListener(&destructionListener);
	world->SetContactListener(&wayListener);

	//set up ground areas
	{
		b2BodyDef bodyDef;
		groundBody = world->CreateBody(&bodyDef);

		b2PolygonShape polygonShape;
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &polygonShape;
		fixtureDef.isSensor = true;

		polygonShape.SetAsBox(9, 7, b2Vec2(-10, 15), 20 * DEGTORAD);
		b2Fixture* groundAreaFixture = groundBody->CreateFixture(&fixtureDef);
		groundAreaFixture->SetUserData(new GroundAreaFUD(0.5f, false));

		polygonShape.SetAsBox(9, 5, b2Vec2(5, 20), -40 * DEGTORAD);
		groundAreaFixture = groundBody->CreateFixture(&fixtureDef);
		groundAreaFixture->SetUserData(new GroundAreaFUD(0.2f, false));
	}

	controlState = 0;

	//load the car sprite, this is done here to avoid complications in the constructors of the car class a bit messy but
	// it is fast enough to avoid any time issues

	gef::ImageData image_data;
	gef::Texture* car_texture;
	bool successful_load = true;
	// Load image
	png_loader.Load("Car.png", platform_, image_data);

	if (image_data.image() == NULL)
	{
		std::cout << "Image failed to load." << std::endl;
		successful_load = false;
	}

	car_texture = gef::Texture::Create(platform_, image_data);


	int trackways;
	// build the track, this has been set up so multiple tracks can be added later.
	switch (trackNum_)
	{
	case 1:
		level_ = new Track("racetrack4.txt", world, 1);
		trackways = 59;
		break;
	case 2:
		level_ = new Track("racetrack2.txt", world, 2);
		trackways = 59;
		break;
	case 3:
		level_ = new Track("racetrack3.txt", world, 3);
		trackways = 59;
		break;
	}

	//figure out the first waypoint
	float starting_angle = 0;
	b2Vec2 starting_point;
	// find the starting waypoint in order to spawn the racing cars next to it
	for (std::vector<Waypoint*>::size_type it = 0; it != level_->WayPoints.size(); it++)
	{
		if (level_->WayPoints[it]->WaypointOrderVal == 0)
		{
			starting_point = level_->WayPoints[it]->body->GetPosition();
			starting_angle = level_->WayPoints[it]->body->GetAngle();
		}
	}

	if (practice == false)
	{
		// decide what AI to spawn
		switch (net_type)
		{
		case EBP:
			_aiCar = new AICar(world, EBP, dataSize, CARCAT, BARRIERCAT | WAYPOINTCAT, TIRECAT, BARRIERCAT, trackways, num_laps);
			break;

		case RPROP:
			_aiCar = new AICar(world, RPROP, dataSize, CARCAT, BARRIERCAT | WAYPOINTCAT, TIRECAT, BARRIERCAT, trackways, num_laps);
			break;

		case RMGSN:
			_aiCar = new AICar(world, RMGSN, dataSize, CARCAT, BARRIERCAT | WAYPOINTCAT, TIRECAT, BARRIERCAT, trackways, num_laps);
			break;

		}

		// if the network is to be trained, train it, if not load the weights from the text file
		if (trainnetworks)
		{
			_aiCar->Train("traindat42.txt");
			_aiCar->SaveWeights();
		}
		_aiCar->LoadWeights();


		// spawn the ai car and set it to the correct position
		_aiCar->body->SetTransform(b2Vec2(starting_point.x + 10, starting_point.y + 20), starting_angle + b2_pi);// (DEGTORAD * 180));
		for (std::vector<Tire*>::size_type it = 0; it < 4; it++)
		{
			_aiCar->tires[it]->body->SetTransform(b2Vec2(starting_point.x + 10, starting_point.y + 20), starting_angle + b2_pi);
		}

		//ugly values, but not another way to do this without editing the original pngs
		if (successful_load)
		{
			_aiCar->carBodySprite.set_texture(car_texture);
			_aiCar->carBodySprite.set_height(12);
			_aiCar->carBodySprite.set_uv_position(gef::Vector2(0.3125, -0.09));
			_aiCar->carBodySprite.set_uv_height(-0.91);
			_aiCar->carBodySprite.set_uv_width(0.35);
			_aiCar->carBodySprite.set_colour(0xffff00ff);
		}

	}

	if (time_trial == false)
	{
		// spawn the car and set it to the correct position
		car = new Car(world, CARCAT, BARRIERCAT | WAYPOINTCAT, TIRECAT, BARRIERCAT, trackways, num_laps);
		car->body->SetTransform(b2Vec2(starting_point.x + 10, starting_point.y), starting_angle + b2_pi);
		for (std::vector<Tire*>::size_type it = 0; it < 4; it++)
		{
			car->tires[it]->body->SetTransform(b2Vec2(starting_point.x + 10, starting_point.y), starting_angle + b2_pi);
		}


		//ugly values, but not another way to do this without editing the original pngs
		if (successful_load)
		{
			car->carBodySprite.set_texture(car_texture);
			car->carBodySprite.set_height(12);
			car->carBodySprite.set_uv_position(gef::Vector2(0.3125, -0.09));
			car->carBodySprite.set_uv_height(-0.91);
			car->carBodySprite.set_uv_width(0.35);
			car->carBodySprite.set_colour(0xffffffff);
		}

	}


	// make sure the time is zero the countdown is ready and the race hasn't started
	startRace = false;
	countdown = 3.0;
	PlayerTime = 0.0;



}

void SceneApp::GameUpdate(float frame_time)
{
	fps_ = 1.0f / frame_time;
	// update the world
	world->Step(frame_time, 4, 6);
	// check for input
	GameInput();

	// make sure the race hasn't started before the countdown ends
	if (countdown > 0 && (startRace == false))
	{
		countdown -= frame_time;
	}
	else
	{
		startRace = true;
	}

	// RACE
	if (startRace)
	{
		// make sure we are only updating appropriate cars depending on the race type
		if (time_trial == false)
		{
			// update the car
			car->Update(controlState, level_->getWaypoints());

			// make sure the race hasn't ended!
			if (car->currentlap == num_laps)
			{
				// game ends!
				if (practice)
				{
					winner = true;
				}
				else
				{
					if (_aiCar->currentlap == num_laps)
					{
						winner = false;
					}
					else
					{
						winner = true;
					}
				}
				// calculate the time
				for (int i = 1; i < num_laps; i++)
				{
					PlayerTime += car->laptime[i];
				}
				// end the game
				game_state = GAMEOVER;
			}
		}
		else
		{
			if (practice == false)
			{
				if (_aiCar->currentlap == num_laps)
				{
					winner = true;
					// calculate the race time
					for (int i = 1; i < num_laps; i++)
					{
						PlayerTime += _aiCar->laptime[i];
					}
					// end the game
					game_state = GAMEOVER;
				}
			}

		}
		if (practice == false)
		{
			// update the AI
			_aiCar->Update(level_->getWaypoints(), level_->Barriers, world);
		}
		// update the Level sprites, this was initially here because the barriers could be moved, could be removed with current implementation
		// however, it may be useful if other tracks have movable barriers
		level_->UpdateSprites();
	}



}

void SceneApp::GameRender()
{
	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here

	gef::Matrix44 projection_matrix;

	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	// make our camera follow the correct car
	if (time_trial)
	{
		projection_matrix = platform_.OrthographicFrustum(_aiCar->getXPosition() - (screen_width / 2), _aiCar->getXPosition() + (screen_width / 2), _aiCar->getYPosition() - (screen_height / 2), _aiCar->getYPosition() + (screen_height / 2), -1.0f, 10.0f);
	}
	else
	{
		projection_matrix = platform_.OrthographicFrustum(car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2), -1.0f, 10.0f);
	}

	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	// redner the cars
	if (time_trial == false)
		car->draw(sprite_renderer_);

	if (practice == false)
		_aiCar->draw(sprite_renderer_);

	level_->DrawTrack(sprite_renderer_, true);

	// render the lap time
	if (startRace == false)
	{
		if (time_trial == false)
			font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition(), car->getYPosition(), -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "%f", countdown);
		else
			font_->RenderText(sprite_renderer_, gef::Vector4(_aiCar->getXPosition(), _aiCar->getYPosition(), -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "%f", countdown);

	}
	else
	{

		if (time_trial)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4(_aiCar->getXPosition() - (screen_width / 2) + 10, _aiCar->getYPosition() - (screen_height / 2) + 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "time: %.1f", _aiCar->currenttime);
			font_->RenderText(sprite_renderer_, gef::Vector4(_aiCar->getXPosition() - (screen_width / 2) + 10, _aiCar->getYPosition() - (screen_height / 2) + 20, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "lap: %i / %i", _aiCar->currentlap, num_laps - 1);

		}
		else
		{
			font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition() - (screen_width / 2) + 10, car->getYPosition() - (screen_height / 2) + 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "time: %.1f", car->currenttime);
			font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition() - (screen_width / 2) + 10, car->getYPosition() - (screen_height / 2) + 20, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "lap: %i / %i", car->currentlap, num_laps - 1);
			// decide who is in first
			if (car->currentlap >= _aiCar->currentlap)
			{
				if (car->currentlap > _aiCar->currentlap)
				{
					font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition() - (screen_width / 2) + 10, car->getYPosition() - (screen_height / 2) + 30, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "position: 1st");
				}
				else
				{
					if (car->currentWaypoint > _aiCar->currentWaypoint || car->currentWaypoint == 0 && _aiCar->currentWaypoint != 0)
					{
						font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition() - (screen_width / 2) + 10, car->getYPosition() - (screen_height / 2) + 30, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "position: 1st");
					}
					else if (car->currentWaypoint < _aiCar->currentWaypoint)
					{
						font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition() - (screen_width / 2) + 10, car->getYPosition() - (screen_height / 2) + 30, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "position: 2nd");
					}
					else // do this later maybe it will get quite complex quickly due to having to find the position of the waypoint
					{
						// distance calculation
						int carway;
						int aiway;
						for (int it = 0; it < level_->WayPoints.size(); it++)
						{
							if (level_->WayPoints[it]->WaypointOrderVal == car->currentWaypoint)
								carway = it;
							if (level_->WayPoints[it]->WaypointOrderVal == _aiCar->currentWaypoint)
								aiway = it;
						}


						float diffx = abs(level_->WayPoints[carway]->barrierSprite.position().x() - car->carBodySprite.position().x());
						float diffy = abs(level_->WayPoints[carway]->barrierSprite.position().y() - car->carBodySprite.position().y());
						float diffx2 = abs(level_->WayPoints[aiway]->barrierSprite.position().x() - _aiCar->carBodySprite.position().x());
						float diffy2 = abs(level_->WayPoints[aiway]->barrierSprite.position().y() - _aiCar->carBodySprite.position().y());

						float dist = sqrtf((diffy * diffy) + (diffx * diffx));
						float dist2 = sqrtf((diffy2 * diffy2) + (diffx2 * diffx2));

						if (dist <= dist2)
						{
							font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition() - (screen_width / 2) + 10, car->getYPosition() - (screen_height / 2) + 30, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "position: 1st");
						}
						else
						{
							font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition() - (screen_width / 2) + 10, car->getYPosition() - (screen_height / 2) + 30, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "position: 2nd");
						}

					}

				}
			}
			else
			{
				font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition() - (screen_width / 2) + 10, car->getYPosition() - (screen_height / 2) + 30, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "position: 2nd");

			}

		}
	}

	sprite_renderer_->End();
}

void SceneApp::GameInput()
{
	input_manager_->Update();
	// deal with the controls for the player car
	const gef::Keyboard* keyboard = input_manager_->keyboard();
	if (keyboard)
	{

		if (keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_A))
			controlState |= TDC_LEFT;
		else if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_A))
			controlState &= ~TDC_LEFT;

		if (keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_D))
			controlState |= TDC_RIGHT;
		else if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_D))
			controlState &= ~TDC_RIGHT;

		if (keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_W))
			controlState |= TDC_UP;
		else if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_W))
			controlState &= ~TDC_UP;

		if (keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_S))
			controlState |= TDC_DOWN;
		else if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_S))
			controlState &= ~TDC_DOWN;
	}
}

void SceneApp::GameOverUpdate()
{
	GameOverInput();
}

void SceneApp::GameOverRender()
{
	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here

	gef::Matrix44 projection_matrix;

	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	// make sure the camera is on the right car
	if (time_trial)
	{
		projection_matrix = platform_.OrthographicFrustum(_aiCar->getXPosition() - (screen_width / 2), _aiCar->getXPosition() + (screen_width / 2), _aiCar->getYPosition() - (screen_height / 2), _aiCar->getYPosition() + (screen_height / 2), -1.0f, 10.0f);
	}
	else
	{
		projection_matrix = platform_.OrthographicFrustum(car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2), -1.0f, 10.0f);
	}

	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	// draw the cars
	if (time_trial == false)
		car->draw(sprite_renderer_);

	if (practice == false)
		_aiCar->draw(sprite_renderer_);

	level_->DrawTrack(sprite_renderer_, true);


	// draw the text showing the score and your time
	if (time_trial)
		font_->RenderText(sprite_renderer_, gef::Vector4(_aiCar->getXPosition(), _aiCar->getYPosition(), -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "Total Time: %.1f", PlayerTime);
	else
	{
		if (winner)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition(), car->getYPosition() - 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "YOU WON!");
		}
		else
		{
			font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition(), car->getYPosition() - 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "YOU LOST!");
		}
		font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition(), car->getYPosition(), -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "Total Time: %.1f", PlayerTime);

	}

	// and press enter to exit

	if (time_trial == false)
		font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition(), car->getYPosition() + 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "Press enter to go back to the main menu");
	else
		font_->RenderText(sprite_renderer_, gef::Vector4(_aiCar->getXPosition(), _aiCar->getYPosition() + 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "Press enter to go back to the main menu");

	sprite_renderer_->End();
}

void SceneApp::GameOverInput()
{
	input_manager_->Update();

	// check the exit condition
	const gef::Keyboard* keyboard = input_manager_->keyboard();
	if (keyboard)
	{

		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_RETURN))
		{
			GameCleanUp();
			game_state = MENU;
		}

	}
}

void SceneApp::GameCleanUp()
{
	// cleanup the game
	level_->cleanUpTrack(world);
	delete level_;
	level_ = NULL;

	if (time_trial == false)
	{
		//car->~Car();
		delete car;
		car = NULL;
	}

	if (practice == false)
	{
		//_aiCar->~AICar();
		delete _aiCar;
		_aiCar = NULL;

	}


	world->~b2World();
	//delete world;
	world = NULL;

	gameinited = false;

}