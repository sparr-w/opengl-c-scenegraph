#include "stdafx.h"
#include "week6app.h"
#include "SceneVariables.h"

extern Node* Scene(Physics *phys);

void Week6App::Init() {
	// setup the camera
	eye = glm::vec3(0.0f, 250.0f, -250.0f);
	float lookX = eye.x, lookY = eye.y - 30.0f, lookZ = eye.z + 50.0f;
	lookAt = glm::vec3(lookX, lookY, lookZ);
	up = glm::vec3(0.0f, 1.0f, 0.0); // y is up!
	camera = new FlyCam(eye, lookAt);

	physics = new Physics();

	sceneGraphRoot = Scene(physics);
	// scene graph for dynamic objects 
	dynamicObjectNode = new GroupNode();
	sceneGraphRoot->AddChild(dynamicObjectNode);

	// projectile
	projectileNode = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.2f, 0.2f, 0.2f)));
	SphereGeometry geom(20);
	TexturedLit* mesh = new TexturedLit(geom, "textures/grid.jpg");
	projectileNode->AddChild(new GeometryNode(mesh));

	// render states
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Week6App::Draw() {
	// camera
	glm::mat4 view;
	glm::mat4 proj;

	eye = camera->GetPos();
	lookAt = camera->GetLookAt();
	up = camera->GetUp();
	view = glm::lookAt(eye, lookAt, up);
	proj = glm::perspective(glm::radians(60.0f), (float)SOF::Window::GetWidth() / (float)SOF::Window::GetHeight(), 0.1f, 1000.0f);
	// Render the scene graph
	RenderVisitor rv(view, proj);
	rv.Traverse(sceneGraphRoot);
}

void Week6App::Shoot() {
	TransformNode* node = new TransformNode(glm::mat4());
	dynamicObjectNode->AddChild(node);
	node->AddChild(projectileNode);

	float speed = 20.0f;
	glm::vec3 vel = glm::normalize(camera->GetLookAt() - camera->GetPos()) * speed;
	Projectile* proj = new Projectile(camera->GetPos(), vel, node);

	physics->AddProjectile(proj);
}

void Week6App::Update(float deltaT) {
	physics->Update(deltaT);
	// Upadate the scene graph
	UpdateVisitor uv(deltaT);
	uv.Traverse(sceneGraphRoot);

	if (SOF::Window::IsKeyDown(GLFW_KEY_W))
		camera->Move(cameraSpeed * 1.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_S))
		camera->Move(cameraSpeed * -1.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_A))
		camera->Strafe(cameraSpeed * -1.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_D))
		camera->Strafe(cameraSpeed * deltaT);

	if (SOF::Window::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		cameraSpeed += 1.0f;
	if (SOF::Window::IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
		cameraSpeed -= 1.0f;
		if (cameraSpeed <= 5.0f) cameraSpeed = 5.0f;
	}

	if (SOF::Window::IsKeyDown(GLFW_KEY_LEFT))
		camera->Pan(40.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_RIGHT))
		camera->Pan(-40.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_UP))
		camera->LookUpDown(40.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_DOWN))
		camera->LookUpDown(-40.0f * deltaT);

	if (!CatapultMoving) {
		if (SOF::Window::IsKeyDown(GLFW_KEY_1) && CatapultCurrentLocation != 0) {
			CatapultTargetLocation = 0;
		}
		if (SOF::Window::IsKeyDown(GLFW_KEY_2) && CatapultCurrentLocation != 1) {
			CatapultTargetLocation = 1;
		}
		if (SOF::Window::IsKeyDown(GLFW_KEY_3) && CatapultCurrentLocation != 2) {
			CatapultTargetLocation = 2;
		}
		if (SOF::Window::IsKeyDown(GLFW_KEY_4) && CatapultCurrentLocation != 3) {
			CatapultTargetLocation = 3;
		}
	}

	if (SOF::Window::IsKeyDown(GLFW_KEY_F)) Shoot();
}