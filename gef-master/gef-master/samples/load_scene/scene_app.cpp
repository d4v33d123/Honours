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
	renderer_3d_(NULL)
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


	world = new b2World(b2Vec2(0, 0));
	

	// set up box 2d related 
	
	world->SetGravity(b2Vec2(0, 0));
	world->SetDestructionListener(&destructionListener);

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

	// build track
	{


	}
	
	car = new Car(world);

	controlState = 0;


}

void SceneApp::CleanUp()
{
	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete car;
	car = NULL;
	world->DestroyBody(groundBody);
}

bool SceneApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	world->Step(frame_time, 4, 6);
	HandleInput();
	car->Update(controlState);

	switch (game_state)
	{
	case STARTUP:
		break;

	case MENU:
		MenuUpdate();
		break;

	case GAME:
		GameUpdate();
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
		break;

	case MENU:
		MenuRender();
		break;

	case GAME:
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
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 50.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
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

static const char* key_names[] =
{
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"NUMPAD0",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"NUMPAD7",
	"NUMPAD8",
	"NUMPAD9",
	"NUMPADENTER",
	"NUMPADSTAR",
	"NUMPADEQUALS",
	"NUMPADMINUS",
	"NUMPADPLUS",
	"NUMPADPERIOD",
	"NUMPADSLASH",
	"ESCAPE",
	"TAB",
	"LSHIFT",
	"RSHIFT",
	"LCONTROL",
	"RCONTROL",
	"BACKSPACE",
	"RETURN",
	"LALT",
	"SPACE",
	"CAPSLOCK",
	"NUMLOCK",
	"SCROLL",
	"RALT",
	"AT",
	"COLON",
	"UNDERLINE",
	"MINUS",
	"EQUALS",
	"LBRACKET",
	"RBRACKET",
	"SEMICOLON",
	"APOSTROPHE",
	"GRAVE",
	"BACKSLASH",
	"COMMA",
	"PERIOD",
	"SLASH"
};

void SceneApp::StartUpdate()
{

}

void SceneApp::StartRender()
{
	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here

	gef::Matrix44 projection_matrix;

	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	projection_matrix = platform_.OrthographicFrustum(car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	car->draw(sprite_renderer_);


	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::StartInput()
{

}

void SceneApp::MenuUpdate()
{

}

void SceneApp::MenuRender()
{
	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here

	gef::Matrix44 projection_matrix;

	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	projection_matrix = platform_.OrthographicFrustum(car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	car->draw(sprite_renderer_);


	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::MenuInput()
{

}

void SceneApp::GameUpdate()
{

}

void SceneApp::GameRender()
{
	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here

	gef::Matrix44 projection_matrix;

	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	projection_matrix = platform_.OrthographicFrustum(car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	car->draw(sprite_renderer_);


	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::GameInput()
{

}

void SceneApp::GameOverUpdate()
{

}

void SceneApp::GameOverRender()
{
	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here

	gef::Matrix44 projection_matrix;

	float aspect_ratio = (float)platform_.width() / (float)platform_.height();

	float screen_width = 300.0f;
	float screen_height = screen_width / aspect_ratio;

	projection_matrix = platform_.OrthographicFrustum(car->getXPosition() - (screen_width / 2), car->getXPosition() + (screen_width / 2), car->getYPosition() - (screen_height / 2), car->getYPosition() + (screen_height / 2), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(projection_matrix);

	sprite_renderer_->Begin();

	car->draw(sprite_renderer_);


	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::GameOverInput()
{

}