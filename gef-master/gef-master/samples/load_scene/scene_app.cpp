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
	trackNum_(1)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	input_manager_ = gef::InputManager::Create(platform_);

	InitFont();

	// create a new scene object and read in the data from the file
	// no meshes or materials are created yet
	// we're not making any assumptions about what the data may be loaded in for
	// we do want to render the data stored in the scene file so lets create the materials from the material data present in the scene file

	SetupLights();
	SetupCamera();


	

	game_state = STARTUP;



}

void SceneApp::CleanUp()
{
	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	//delete car;
	//car = NULL;
	//world->DestroyBody(groundBody);
}

bool SceneApp::Update(float frame_time)
{
	
	HandleInput();

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



	switch (game_state)
	{
	case STARTUP:
		if(startupinited)
			StartRender();
		break;

	case MENU:
		if(menuinited)
			MenuRender();
		break;

	case GAME:
		if(gameinited)
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

void SceneApp::DrawHUD()
{
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	if(font_)
	{
		// display frame rate
		//car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2)
	}
}

void SceneApp::SetupLights()
{
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
	Tire* tire = (Tire*)tireFixture->GetBody()->GetUserData();
	GroundAreaFUD* gaFud = (GroundAreaFUD*)groundAreaFixture->GetUserData();
	if (began)
		tire->addGroundArea(gaFud);
	else
		tire->removeGroundArea(gaFud);
}

void SceneApp::HandleInput()
{
	
	

}

void SceneApp::StartInit()
{
	// set up our initial game, load the splash screen along with loading any big files
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

	projection_matrix = platform_.OrthographicFrustum(0, (screen_width), 0, (screen_height ), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	if (font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) ,  (screen_height / 2) , -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "MULTI-LAYER PERCEPTRON RACING!");

		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) + 10 , -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "Press enter to start!");
		//car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2)
	}


	sprite_renderer_->End();
}

void SceneApp::StartInput()
{
	input_manager_->Update();

	const gef::Keyboard* keyboard = input_manager_->keyboard();
	if (keyboard)
	{

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


	projection_matrix = platform_.OrthographicFrustum(0, (screen_width ),0, (screen_height ), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	if (font_)
	{
		// display frame rate
		switch (net_type)
		{
		case EBP:
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2), -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "YOU ARE RACING AGAINST EBP!");
			break;
		case RPROP:
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2), -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "YOU ARE RACING AGAINST RPROP!");
			break;
		case RMGSN:
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2), -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "YOU ARE RACING AGAINST RMGS!");
			break;
		}
		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) + 10 , -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "use w and s to change your opponent!");


		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2), (screen_height / 2) + 20, -0.9f), 0.25f, 0xff00ff00, gef::TJ_CENTRE, "Press enter to start!");

		if (trainnetworks)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 40, (screen_height / 2) + 20, -0.9f), 0.10f, 0xff00ff00, gef::TJ_CENTRE, "Training Networks!");
		}
		if (time_trial)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 40, (screen_height / 2) + 30, -0.9f), 0.10f, 0xff00ff00, gef::TJ_CENTRE, "TIME TRIAL!");
		}
		if (practice)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 40, (screen_height / 2) + 40, -0.9f), 0.10f, 0xff00ff00, gef::TJ_CENTRE, "PRACTICE!");
		}
		font_->RenderText(sprite_renderer_, gef::Vector4((screen_width / 2) + 20, (screen_height / 2) + 30, -0.9f), 0.10f, 0xff00ff00, gef::TJ_CENTRE, "Number of laps:%i",num_laps);
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

		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_RETURN))
			game_state = GAME;

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

	}
}

void SceneApp::GameInit()
{
	trainnetworks = false;
	practice = false;
	time_trial = false;
	num_laps = 3;

	net_type = EBP;
	dataSize = 10000;
	// FOR TESTING
	// RPROP; //EBP is great, dat 25 = 21% error and going lower, needs more than 10000 iterations

	//dataSize = 14641; //10600 dat1 2425 dat2 500 dat3 180 dat4 2420 dat5 20 trainingData 500 dat6 500 dat7 500 dat8 11737 dat9 625 dat10 1375 dat11 625 dat12 3025 dat13 26620 dat14 26620 dat15 160000 dat16 160000 dat17 14641 dat18 14641 dat 19 53240 dat20 26620 dat21 34606 dat22 26620 dat24 14641 dat25
	//dataSize = 26620;
	//dataSize = 5500;


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
	
	
	

	int trackways;

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
		switch (net_type)
		{
		case EBP:
			_aiCar = new AICar(world, EBP, dataSize, CARCAT, BARRIERCAT | CARCAT | WAYPOINTCAT, TIRECAT, BARRIERCAT, trackways, num_laps);
			//_aiCar = new AICar(world, EBP, dataSize, CARCAT, CARCAT | WAYPOINTCAT, TIRECAT, 0, 77);
			break;

		case RPROP:
			_aiCar = new AICar(world, RPROP, dataSize, CARCAT, BARRIERCAT | CARCAT | WAYPOINTCAT, TIRECAT, BARRIERCAT, trackways, num_laps);
			break;

		case RMGSN:
			_aiCar = new AICar(world, RMGSN, dataSize, CARCAT, BARRIERCAT | CARCAT | WAYPOINTCAT, TIRECAT, BARRIERCAT, trackways, num_laps);
			break;

		}

		if (trainnetworks)
		{
			//_aiCar->Train("traindat34.txt");
			//_aiCar->Train("traindat39.txt");
			_aiCar->Train("traindat42.txt"); // 40 was best for rprop, 42 for rmgs, 35/37 for ebp i believe
			_aiCar->SaveWeights();
		}
		_aiCar->LoadWeights();

		_aiCar->body->SetTransform(b2Vec2(starting_point.x + 10, starting_point.y + 20), starting_angle + b2_pi);// (DEGTORAD * 180));
		for (std::vector<Tire*>::size_type it = 0; it < 4; it++)
		{
			_aiCar->tires[it]->body->SetTransform(b2Vec2(starting_point.x + 10, starting_point.y + 20), starting_angle + b2_pi);
		}

	}

	if (time_trial == false)
	{
		car = new Car(world, CARCAT, BARRIERCAT | CARCAT | WAYPOINTCAT, TIRECAT, BARRIERCAT, trackways, num_laps);
		car->body->SetTransform(b2Vec2(starting_point.x + 10, starting_point.y), starting_angle + b2_pi);
		for (std::vector<Tire*>::size_type it = 0; it < 4; it++)
		{
			car->tires[it]->body->SetTransform(b2Vec2(starting_point.x + 10, starting_point.y), starting_angle + b2_pi);
		}
	}
	
	startRace = false;
	countdown = 3.0;
	PlayerTime = 0.0;
	

		
}

void SceneApp::GameUpdate(float frame_time)
{
	fps_ = 1.0f / frame_time;

	world->Step(frame_time, 4, 6);

	GameInput();

	if (countdown > 0 && (startRace==false))
	{
		countdown -= frame_time;
	}
	else
	{
		startRace = true;
	}

	if (startRace)
	{
		if (time_trial == false)
		{
			car->Update(controlState, level_->getWaypoints());

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
				for (int i = 1; i < num_laps; i++)
				{
					PlayerTime += car->laptime[i];
				}
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
					for (int i = 1; i < num_laps; i++)
					{
						PlayerTime += _aiCar->laptime[i];
					}
					game_state = GAMEOVER;
				}
			}
			
		}
		if (practice == false)
		{
			_aiCar->Update(level_->getWaypoints(), level_->Barriers, world);
		}
		level_->UpdateSprites();

		//check for victory!
		
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

	if(time_trial == false)
		car->draw(sprite_renderer_);

	if(practice == false)
		_aiCar->draw(sprite_renderer_);

	level_->DrawTrack(sprite_renderer_, true);

	if (startRace == false)
	{
		font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition(), car->getYPosition() , -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "%f", countdown);
	}
	else
	{

		if(time_trial)
			font_->RenderText(sprite_renderer_, gef::Vector4(_aiCar->getXPosition() - (screen_width / 2) + 10, _aiCar->getYPosition() - (screen_height / 2) + 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "time: %.1f", _aiCar->currenttime);
		else
			font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition()-(screen_width/2) + 10, car->getYPosition() - (screen_height / 2) + 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "time: %.1f", car->currenttime);
	}

	sprite_renderer_->End();
}

void SceneApp::GameInput()
{
	input_manager_->Update();

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

	if (time_trial == false)
		car->draw(sprite_renderer_);

	if (practice == false)
		_aiCar->draw(sprite_renderer_);

	level_->DrawTrack(sprite_renderer_, true);


	// draw the text showing the score and your time
	if(time_trial)
		font_->RenderText(sprite_renderer_, gef::Vector4(_aiCar->getXPosition() , _aiCar->getYPosition(), -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "Total Time: %.1f", PlayerTime);
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
		font_->RenderText(sprite_renderer_, gef::Vector4(car->getXPosition(), car->getYPosition() , -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "Total Time: %.1f", PlayerTime);

	}
		
	// and press enter to exit
	font_->RenderText(sprite_renderer_, gef::Vector4(_aiCar->getXPosition(), _aiCar->getYPosition() + 10, -0.9f), 0.25f, 0xff00ff00, gef::TJ_LEFT, "Press enter to go back to the main menu");

	sprite_renderer_->End();
}

void SceneApp::GameOverInput()
{
	input_manager_->Update();

	const gef::Keyboard* keyboard = input_manager_->keyboard();
	if (keyboard)
	{

		if (keyboard->IsKeyReleased(gef::Keyboard::KeyCode::KC_RETURN))
			game_state = MENU;
	}
}