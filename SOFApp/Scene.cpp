#include "stdafx.h"
#include "RenderLib.h"
#include "SceneVariables.h"
#include "Physics.h"
#include <random>

class WaveInterpolate : public UpdateCallback {
	Ocean* ocean;
	float elapsed = 0.0f, speed;
	float defaultSpeed = 0.5f;
public:
	WaveInterpolate(Ocean* nOcean, float Ispeed = 1.0f) : ocean(nOcean), speed(Ispeed) {}
	virtual void operator()(float dt) {
		elapsed = elapsed + (defaultSpeed * speed * dt);

		float height = sin(elapsed);
		height = (height + 1.0f) / 2;

		ocean->SetWaveDelta(height);
	}
};

class SunUpdateTime : public UpdateCallback {
	EmissiveSun* sun;
	float elapsed = 0.0f, speed;
public:
	SunUpdateTime(EmissiveSun* nSun, float Ispeed = 1.0f) : sun(nSun), speed(Ispeed) {}
	virtual void operator()(float dt) {
		elapsed = elapsed + (speed * dt);
		sun->UpdateElapsedTime(elapsed);
	}
};

class SplineAnimationCallback : public UpdateCallback {
	Spline* spline;
	TransformNode* trans;
	float rate;
	float t = 0;
public:
	SplineAnimationCallback(TransformNode* trans, Spline* spline, float rate) : trans(trans), spline(spline), rate(rate), t(0.0f) {}
	virtual void operator()(float dt) {
		t += dt * rate;
		if (t > spline->GetNumPoints()) {
			t -= spline->GetNumPoints();
		}
		// work out the new transform here
		glm::vec3 position = spline->GetPoint(t);
		glm::vec3 tangent = spline->GetTangent(t);
		glm::mat4 lookAt = glm::lookAt(position, position + tangent, glm::vec3(0, 1, 0));
		trans->SetTransform(glm::inverse(lookAt));
	}
};

bool CatapultMoving = false;
int CatapultCurrentLocation = 0; int CatapultTargetLocation = 0;

class CatapultMovement : public UpdateCallback {
	TransformNode* catapult;
	TransformNode* rotateNode;
	TransformNode* cataWheelRotator;

	float speedMulti, speedDefault = 0.2f;
	float progress = 0.0f;

	float wheelsAngle = 0.0f; float catapultAngle = 0.0f; float cataMaxAngle = -145.0f;

	int movementStage = 0;
	
	std::vector<glm::vec3> pathPoints;
	std::vector<glm::vec3> destinations;

	Spline* path;
public:
	CatapultMovement(TransformNode* nCatapult, std::vector<glm::vec3> nPathPoints, std::vector<glm::vec3> nDestinations, 
		TransformNode* wheelRotator, TransformNode* catapultRotator, float speed = 1.0f) : catapult(nCatapult), pathPoints(nPathPoints), 
		destinations(nDestinations), cataWheelRotator(wheelRotator), rotateNode(catapultRotator), speedMulti(speed) {}
	virtual void operator()(float dt) {
		if (CatapultMoving) {
			if (movementStage == 1) { // stage 1 is moving from point A to point B
				progress += speedDefault * speedMulti * dt;

				if (progress > path->GetNumPoints() - 1) {
					progress = path->GetNumPoints();
					CatapultCurrentLocation = CatapultTargetLocation;
					movementStage++;
				}

				// move the whole catapult to the new location
				glm::vec3 position = path->GetPoint(progress);
				glm::vec3 tangent = path->GetTangent(progress);
				glm::mat4 lookAt = glm::lookAt(position, position + tangent, glm::vec3(0.0f, 1.0f, 0.0f));
				catapult->SetTransform(glm::inverse(lookAt));
			}
			else if (movementStage > 1) {
				catapultAngle -= dt * 50.0f * speedMulti;

				if (catapultAngle <= cataMaxAngle) {
					catapultAngle = cataMaxAngle;
					movementStage = 0;
					CatapultMoving = false;
				}

				rotateNode->SetTransform(glm::rotate(glm::mat4(), glm::radians(catapultAngle), glm::vec3(0.0f, 1.0f, 0.0f)));
			}

			// rotate the wheels whilst the catapult is moving
			wheelsAngle += dt * 100.0f * speedMulti;
			if (wheelsAngle >= 360.0f) wheelsAngle -= 360.0f;

			cataWheelRotator->SetTransform(glm::rotate(glm::mat4(), glm::radians(wheelsAngle), glm::vec3(-1.0f, 0.0f, 0.0f)));
		} else {
			if (CatapultTargetLocation != CatapultCurrentLocation) {
				CatapultMoving = true;
				movementStage = 1;

				std::vector<glm::vec3> catapultRoute;
				catapultRoute.push_back(destinations[CatapultCurrentLocation]);

				int cataRouteDistance = CatapultTargetLocation - CatapultCurrentLocation;
				if (abs(cataRouteDistance) > 2) { // shortest route
					cataRouteDistance = CatapultTargetLocation + 4 - CatapultCurrentLocation;
					if (cataRouteDistance > 2) cataRouteDistance = -1;
				}

				for (int i = 0; i < abs(cataRouteDistance); i++) {
					int iCorner = CatapultCurrentLocation + (i * (cataRouteDistance / abs(cataRouteDistance))); // find the corner we are currently at with direction of travel
					iCorner = (iCorner + destinations.size()) % destinations.size(); // loop to back of array if it goes negative

					int it = iCorner * 3; // get starting position
					if (cataRouteDistance < 0) it -= 1; // direction going backwards instead
					it = (it + pathPoints.size()) % pathPoints.size(); // loop back to the end of the array if it goes negative

					for (int j = 0; j < 3; j++) {
						int jit = it + (j * (cataRouteDistance / abs(cataRouteDistance)));
						catapultRoute.push_back(pathPoints[jit]);
					}
				}

				catapultRoute.push_back(destinations[CatapultTargetLocation]);
				path = new CRSpline(catapultRoute, false, 0.5f);

				catapultAngle = 0.0f;
				rotateNode->SetTransform(glm::rotate(glm::mat4(), glm::radians(catapultAngle), glm::vec3(0.0f, 1.0f, 0.0f)));

				progress = 0.0f;
			}
		}
	}
};

Node* Scene(Physics* phys) {
	float lightPosX = 20.0f, lightPosY = 200.0f, lightPosZ = -200.0f;
	LightNode* light = new LightNode(glm::vec3(lightPosX, lightPosY, lightPosZ),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, 1.0f, 0.2f, 10.0f, 400.0f);
	
#pragma region Skybox
	// create a cubemap
	std::vector<std::string> cubeMapFiles; // up, down, front, back, right, left;
	cubeMapFiles.push_back("textures/cubemaps/miramar_up.png");
	cubeMapFiles.push_back("textures/cubemaps/miramar_dn.png");
	cubeMapFiles.push_back("textures/cubemaps/miramar_ft.png");
	cubeMapFiles.push_back("textures/cubemaps/miramar_bk.png");
	cubeMapFiles.push_back("textures/cubemaps/miramar_rt.png");
	cubeMapFiles.push_back("textures/cubemaps/miramar_lf.png");
	SOF::Cubemap* cubeMap = new SOF::Cubemap(cubeMapFiles);
	Skybox* sb = new Skybox(cubeMap);
	GeometryNode* sbNode = new GeometryNode(sb);

	light->AddChild(sbNode);
#pragma endregion Skybox

#pragma region Custom Sun Shader
	// custom sun drawable which is static to the light position so that we can see where the light is coming from
	float sunScaleFactor = 3.0f;
	glm::vec3 sunSize = glm::vec3(sunScaleFactor, sunScaleFactor, sunScaleFactor);

	EmissiveSun* sun = new EmissiveSun(sunSize);
	TransformNode* sunTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(lightPosX, lightPosY, lightPosZ)));
	TransformNode* sunScale = new TransformNode(glm::scale(glm::mat4(), sunSize));

	light->AddChild(sunTrans);
	sunTrans->AddChild(sunScale);
	sunScale->AddChild(new GeometryNode(sun));

	sunTrans->SetUpdateCallback(new SunUpdateTime(sun, 1.0f));
#pragma endregion Custom Sun Shader

#pragma region Terrain
	float terrainXZ = 1000.0f, terrainY = 100.0f;
	Terrain* terrain = new Terrain("textures/terrain/heightmap.png", "textures/splatmap.png",
								   "textures/grass.jpg", "textures/dirt.jpg",
								   "textures/stone.png", "textures/sand.jpg",
									terrainXZ, terrainY);

	TransformNode* terrainTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-terrainXZ / 2, 0.0f, -terrainXZ / 2))); // this will ensure that the bottom centre of the terrain mesh is centred on the world origin (0,0,0)
	TransformNode* terrainScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));
	light->AddChild(terrainTrans);
	terrainTrans->AddChild(terrainScale);
	terrainScale->AddChild(new GeometryNode(terrain));
#pragma endregion Terrain

#pragma region Ocean
	float oceanXZ = 480.0f, oceanY = 4.0f;
	Ocean* ocean = new Ocean("textures/terrain/water-height.png", "textures/terrain/water-height2.png", 
		"textures/terrain/water-diffuse.png", "textures/terrain/water-diffuse2.png", oceanXZ, oceanY);

	// the original ocean which only showed one tile, it didn't cover enough area
	/* 
	TransformNode* oceanTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-oceanXZ / 2, 0.0f, -oceanXZ / 2)));
	TransformNode* oceanScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));
	light->AddChild(oceanTrans);
	oceanTrans->AddChild(oceanScale);
	oceanScale->AddChild(new GeometryNode(ocean));
	*/

	// new hierarchical transforms for tiling ocean -- the textures used are seamless so these can easily be tiled together
	TransformNode* oceanTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)));
	TransformNode* oceanScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));
	light->AddChild(oceanTrans);
	oceanTrans->AddChild(oceanScale);
	
	// the origin point of an oceantile is the bottom right corner so there needs to be a offset/buffer to make up for this
	TransformNode* OtilesOffset = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-oceanXZ / 2, 0.0f, -oceanXZ / 2)));
	oceanScale->AddChild(OtilesOffset);

	int oceanTilesX = 3, oceanTilesZ = 3;
	float centreTileX = (oceanTilesX + 1) / 2, centreTileZ = (oceanTilesZ + 1) / 2;

	for (int x = 0; x < oceanTilesX; x++) {
		float offsetX = (x + 1) - centreTileX;
		for (int z = 0; z < oceanTilesZ; z++) {
			float offsetZ = (z + 1) - centreTileZ;

			TransformNode* oceanTileTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(offsetX * oceanXZ, 0.0f, offsetZ * oceanXZ)));
			TransformNode* oceanTileScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));

			OtilesOffset->AddChild(oceanTileTrans);
			oceanTileTrans->AddChild(oceanTileScale);
			oceanTileScale->AddChild(new GeometryNode(ocean));
		}
	}

	oceanTrans->SetUpdateCallback(new WaveInterpolate(ocean, 1.0f));
#pragma endregion Ocean
	
#pragma region Big Hierarchal Castle
	TransformNode* castleTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 100.0f, 0.0f)));
	TransformNode* castleScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));
	light->AddChild(castleTrans);
	castleTrans->AddChild(castleScale);

#pragma region Castle Towers
	TransformNode* castleTower = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)));

#pragma region Castle Tower Variables
	float towerCylHeight = 15.0f; float towerXZscale = 7.0f;
	int towerDivisions = 17;
	float towerCrownHeight = 2.0f;

	float towerCrownYpos = (towerCylHeight * 2) + towerCrownHeight;
	float towerXZpos = 50.0f;
#pragma endregion Castle Tower Variables

	CylinderGeometry CTcylG(glm::vec3(towerXZscale, towerCylHeight, towerXZscale), 1.0f, towerDivisions);
	TexturedLit* CTcyl = new TexturedLit(CTcylG, "textures/marble.png");
	TransformNode* castleCylTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, towerCylHeight, 0.0f)));
	TransformNode* castleCylScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(towerXZscale, towerCylHeight, towerXZscale)));
	castleCylTrans->AddChild(castleCylScale);
	castleCylScale->AddChild(new GeometryNode(CTcyl));
	
	castleTower->AddChild(castleCylTrans);
	
	CrownGeometry CTcrownG(glm::vec3(towerXZscale, towerCrownHeight, towerXZscale), 1.0f, towerDivisions, 0.3f, towerDivisions / 2, 0.5f);
	TexturedLit* CTcrown = new TexturedLit(CTcrownG, "textures/marble.png");
	TransformNode* castleCrownTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, towerCrownYpos, 0.0f)));
	TransformNode* castleCrownScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(towerXZscale, towerCrownHeight, towerXZscale)));
	castleCrownTrans->AddChild(castleCrownScale);
	castleCrownScale->AddChild(new GeometryNode(CTcrown));
	
	castleTower->AddChild(castleCrownTrans);
	
	TransformNode* castleFLtower = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-towerXZpos, 0.0f, towerXZpos)));
	TransformNode* castleFRtower = new TransformNode(glm::translate(glm::mat4(), glm::vec3(towerXZpos, 0.0f, towerXZpos)));
	TransformNode* castleBLtower = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-towerXZpos, 0.0f, -towerXZpos)));
	TransformNode* castleBRtower = new TransformNode(glm::translate(glm::mat4(), glm::vec3(towerXZpos, 0.0f, -towerXZpos)));
	
	castleFLtower->AddChild(castleTower); castleFRtower->AddChild(castleTower);
	castleBLtower->AddChild(castleTower); castleBRtower->AddChild(castleTower);

	castleScale->AddChild(castleFLtower); castleScale->AddChild(castleFRtower);
	castleScale->AddChild(castleBLtower); castleScale->AddChild(castleBRtower);
#pragma endregion Castle Towers

#pragma region Castle Walls
#pragma region Castle Wall Variables
	float wallCubeScale = 2.8f;
	int wallSegmentCubes = 5;

	float towerGap = (towerXZpos * 2) - (towerXZscale * 2);
	int wallSegmentCount = towerGap / (wallCubeScale * 2); wallSegmentCount += 1;
#pragma endregion Castle Wall Variables

	CubeGeometry cubeGeom;
	TexturedLit* CWcube = new TexturedLit(cubeGeom, "textures/marble.png");
	TransformNode* CWcubeScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(wallCubeScale, wallCubeScale, wallCubeScale)));
	CWcubeScale->AddChild(new GeometryNode(CWcube));

	TransformNode* castleLwall = new TransformNode(glm::translate(glm::mat4(), glm::vec3(towerXZpos, 0.0f, 0.0f)));
	TransformNode* castleRwall = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-towerXZpos, 0.0f, 0.0f)));
	TransformNode* castleFwall = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, towerXZpos)));
	TransformNode* castleBwall = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -towerXZpos)));

	// left and right wall, consisting of wall segments, made up of smaller cubes so that it can be destroyed and break into smaller parts
	for (int wSeg = 0; wSeg < wallSegmentCount; wSeg++) {
		float across = (-((float)wallSegmentCount / 2) + wSeg) * wallCubeScale * 2 + wallCubeScale;
		TransformNode* CWsegment = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, across)));

		for (int i = 0; i < wallSegmentCubes; i++) {
			TransformNode* CWcubeSegment = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, (wallCubeScale + (i * 2.0f * wallCubeScale)), 0.0f)));
			CWcubeSegment->AddChild(CWcubeScale);
			CWsegment->AddChild(CWcubeSegment);
		}

		castleLwall->AddChild(CWsegment);
		castleRwall->AddChild(CWsegment);
	}

	// forward wall, just the same as the left and right wall
	for (int wSeg = 0; wSeg < wallSegmentCount; wSeg++) {
		float across = (-((float)wallSegmentCount / 2) + wSeg) * wallCubeScale * 2 + wallCubeScale;
		TransformNode* CWsegment = new TransformNode(glm::translate(glm::mat4(), glm::vec3(across, 0.0f, 0.0f)));

		for (int i = 0; i < wallSegmentCubes; i++) {
			TransformNode* CWcubeSegment = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, (wallCubeScale + (i * 2.0f * wallCubeScale)), 0.0f)));
			CWcubeSegment->AddChild(CWcubeScale);
			CWsegment->AddChild(CWcubeSegment);
		}
		
		castleFwall->AddChild(CWsegment);
	}

	// adaptable door that will be even in size if the wall is made up of an even number of segments, otherwise odd if odd number of segments
	int doorSegmentWidth; int doorSegmentIgnore;
	if (wallSegmentCount % 2 == 0) doorSegmentWidth = (wallSegmentCubes % 2 == 0) ? wallSegmentCubes - 2 : wallSegmentCubes - 1;
	else doorSegmentWidth = (wallSegmentCubes % 2 == 1) ? wallSegmentCubes - 2 : wallSegmentCubes - 1;
	// door centres on the middle of the wall, it does this by using taking the width from the number of segments and dividing it by 2, this gives the number of segments that need to be either side of the door
	doorSegmentIgnore = (wallSegmentCount - doorSegmentWidth) / 2;

	// back wall, same as other walls but instead of placing cubes throughout the segments it will ignore them so a door can fit
	for (int wSeg = 0; wSeg < wallSegmentCount; wSeg++) {
		float across = (-((float)wallSegmentCount / 2) + wSeg) * wallCubeScale * 2 + wallCubeScale;
		TransformNode* CWsegment = new TransformNode(glm::translate(glm::mat4(), glm::vec3(across, 0.0f, 0.0f)));
		if (wSeg > doorSegmentIgnore - 1 && wSeg <= doorSegmentIgnore - 1 + doorSegmentWidth) {
			for (int i = doorSegmentWidth; i < wallSegmentCubes; i++) {
				TransformNode* CWcubeSegment = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, (wallCubeScale + (i * 2.0f * wallCubeScale)), 0.0f)));
				CWcubeSegment->AddChild(CWcubeScale);
				CWsegment->AddChild(CWcubeSegment);
			}
		}
		else {
			for (int i = 0; i < wallSegmentCubes; i++) {
				TransformNode* CWcubeSegment = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, (wallCubeScale + (i * 2.0f * wallCubeScale)), 0.0f)));
				CWcubeSegment->AddChild(CWcubeScale);
				CWsegment->AddChild(CWcubeSegment);
			}
		}

		castleBwall->AddChild(CWsegment);
	}

	// this is the archway/door that should fit nicely in the gap that was left for it
	ArchwayGeometry CWarch(glm::vec3(doorSegmentWidth, doorSegmentWidth, doorSegmentWidth), 0.2f, 1.0f, true, 0.7f);
	TexturedLit* CWarchRender = new TexturedLit(CWarch, "textures/marble.png");
	TransformNode* CWarchTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)));
	TransformNode* CWarchScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(doorSegmentWidth * wallCubeScale, doorSegmentWidth * wallCubeScale, wallCubeScale)));

	CWarchTrans->AddChild(CWarchScale);
	CWarchScale->AddChild(new GeometryNode(CWarchRender));
	castleBwall->AddChild(CWarchTrans);

	castleScale->AddChild(castleLwall); castleScale->AddChild(castleRwall);
	castleScale->AddChild(castleFwall); castleScale->AddChild(castleBwall);
#pragma endregion Castle Walls

#pragma region Castle Colliders
	float castleCollHeight = wallCubeScale * wallSegmentCubes;
	float castleCollWidth = wallCubeScale * wallSegmentCount;

	CubeGeometry castlePlaneGeom;
	TexturedLit* castlePlaneLit = new TexturedLit(castlePlaneGeom, "textures/grid.jpg");
	TransformNode* castleCollTrans;
	const bool CASTLE_COLL_DEBUG = false;

	// wall colliders
	for (int j = 0; j < 2; j++) {
		float innerDiff = -(wallCubeScale * 2) * j;
		for (int i = 0; i < 4; i++) {
			float xDir = (i + 1) % 2 == 1 ? 1 : 0; float zDir = (i + 1) % 2 == 1 ? 0 : 1;
			float dire = i + 1 <= 2 ? -1 : 1;
			xDir = xDir * dire; zDir = zDir * dire;
			glm::vec3 castleCollPos = glm::vec3((wallCubeScale + 50.0f) * xDir, castleCollHeight + 100.0f, (wallCubeScale + 50.0f) * zDir);
			float castleCollYrot = 90.0f * xDir;

			castleCollPos += innerDiff * glm::vec3(xDir, 0.0f, zDir);

			glm::mat4 castleColl = glm::mat4();
			castleColl = glm::translate(castleColl, castleCollPos);
			castleColl = glm::rotate(castleColl, glm::radians(castleCollYrot), glm::vec3(0.0f, 1.0f, 0.0f));
			castleColl = glm::rotate(castleColl, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			castleColl = glm::scale(castleColl, glm::vec3(castleCollWidth, 0.1f, castleCollHeight));
			
			if (CASTLE_COLL_DEBUG) {
				castleCollTrans = new TransformNode(castleColl);
				light->AddChild(castleCollTrans);
				castleCollTrans->AddChild(new GeometryNode(castlePlaneLit));
			}

			QuadCollider* castleWallCollider = new QuadCollider(castleColl);
			phys->AddCollider(castleWallCollider);

			castleCollPos = glm::vec3(xDir * 50.0f, 100.0f + castleCollHeight * 2, zDir * 50.0f);

			castleColl = glm::mat4();
			castleColl = glm::translate(castleColl, castleCollPos);
			castleColl = glm::rotate(castleColl, glm::radians(castleCollYrot), glm::vec3(0.0f, 1.0f, 0.0f));
			castleColl = glm::scale(castleColl, glm::vec3(castleCollWidth, 0.1f, wallCubeScale));

			if (CASTLE_COLL_DEBUG) {
				castleCollTrans = new TransformNode(castleColl);
				light->AddChild(castleCollTrans);
				castleCollTrans->AddChild(new GeometryNode(castlePlaneLit));
			}

			castleWallCollider = new QuadCollider(castleColl);
			phys->AddCollider(castleWallCollider);
		}
	}

	std::vector<glm::vec3> towerVertices;

	for (int i = 0; i < towerDivisions; i++) {
		float theta = glm::radians(360.0f) * (float)i / (towerDivisions - 1);
		towerVertices.push_back(glm::vec3(cos(theta), 0.0f, sin(theta)));
	}

	castleCollWidth = (towerXZscale / (towerDivisions - 1)) * 3.0f;
	for (int i = 0; i < 4; i++) {
		float centreX = towerXZpos; float centreZ = towerXZpos;
		if (i < 2) centreX = centreX * -1;
		if (i < 3 && i > 0) centreZ = centreZ * -1;

		glm::vec3 centreOfTower = glm::vec3(centreX, 100.0f, centreZ);
		for (int i = 0; i < towerDivisions - 1; i++) {
			glm::vec3 vDiff = towerVertices[i + 1] - towerVertices[i];
			glm::vec3 castleCollPos = towerVertices[i] + vDiff * 0.5f;
			float castleCollYrot = 360.0f - (360.0f / (towerDivisions - 1));
			castleCollYrot = castleCollYrot * i;

			glm::mat4 castleColl = glm::mat4();
			castleColl = glm::translate(castleColl, (castleCollPos * towerXZscale + centreOfTower) + glm::vec3(0.0f, towerCylHeight, 0.0f));
			castleColl = glm::rotate(castleColl, glm::radians(castleCollYrot + 78.7f), glm::vec3(0.0f, 1.0f, 0.0f));
			castleColl = glm::rotate(castleColl, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			castleColl = glm::scale(castleColl, glm::vec3(castleCollWidth, 0.1f, towerCylHeight));

			if (CASTLE_COLL_DEBUG) {
				castleCollTrans = new TransformNode(castleColl);
				light->AddChild(castleCollTrans);
				castleCollTrans->AddChild(new GeometryNode(castlePlaneLit));
			}

			QuadCollider* castleWallCollider = new QuadCollider(castleColl);
			phys->AddCollider(castleWallCollider);
		}
	}

#pragma endregion Castle Colliders
#pragma endregion Big Hierarchal Castle

#pragma region Showcase Custom Geometry
	TransformNode* showcaseTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 101.0f, 0.0f)));
	TransformNode* showcaseScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));
	light->AddChild(showcaseTrans);
	showcaseTrans->AddChild(showcaseScale);

	std::vector<TransformNode*> showcaseGeometry;
	int showcaseGeometryCount = 0;

	// demonstrating the intended look of the archway for the castle wall
	ArchwayGeometry Sarch0(glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, 1.0f, true, 0.3f);
	TexturedLit* SarchRender0 = new TexturedLit(Sarch0, "textures/grid.jpg");
	TransformNode* SarchOffset0 = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f)));
	TransformNode* SarchScale0 = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));

	SarchOffset0->AddChild(SarchScale0);
	SarchScale0->AddChild(new GeometryNode(SarchRender0));
	showcaseGeometry.push_back(SarchOffset0); showcaseGeometryCount++;

	// demonstrating an alternative look of the archway, no extruding arch and longer side walls without increasing size of whole object
	ArchwayGeometry Sarch1(glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, 2.0f, true, 0.0f);
	TexturedLit* SarchRender1 = new TexturedLit(Sarch1, "textures/grid.jpg");
	TransformNode* SarchOffset1 = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f)));
	TransformNode* SarchScale1 = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));

	SarchOffset1->AddChild(SarchScale1);
	SarchScale1->AddChild(new GeometryNode(SarchRender1));
	showcaseGeometry.push_back(SarchOffset1); showcaseGeometryCount++;

	// demonstrating another alternative look of the archway, shorter side walls, also a thicker archway and no flat top, the flat top is intended so a cube can cleanly be placed ontop
	ArchwayGeometry Sarch2(glm::vec3(1.0f, 1.0f, 1.0f), 0.4f, 0.75f, false, 0.0f);
	TexturedLit* SarchRender2 = new TexturedLit(Sarch2, "textures/grid.jpg");
	TransformNode* SarchOffset2 = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f)));
	TransformNode* SarchScale2 = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));

	SarchOffset2->AddChild(SarchScale2);
	SarchScale2->AddChild(new GeometryNode(SarchRender2));
	showcaseGeometry.push_back(SarchOffset2); showcaseGeometryCount++;

	// demonstrating how the default constructor variables for the crown, which is a shape used to top cylinders, making them look like castle towers
	CrownGeometry Scrown0(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 17, 0.3f, 8, 0.5f);
	TexturedLit* ScrownRender0 = new TexturedLit(Scrown0, "textures/grid.jpg");
	TransformNode* ScrownScale0 = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));

	ScrownScale0->AddChild(new GeometryNode(ScrownRender0));
	showcaseGeometry.push_back(ScrownScale0); showcaseGeometryCount++;

	// demonstrating the crown how it is intended to look, squashed down rather than being stretched, demonstrated some other variable changes
	CrownGeometry Scrown1(glm::vec3(1.0f, 0.4f, 1.0f), 1.0f, 21, 0.2f, 10, 0.6f);
	TexturedLit* ScrownRender1 = new TexturedLit(Scrown1, "textures/grid.jpg");
	TransformNode* ScrownOffset1 = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, -0.6f, 0.0f)));
	TransformNode* ScrownScale1 = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 0.4f, 1.0f)));

	ScrownOffset1->AddChild(ScrownScale1);
	ScrownScale1->AddChild(new GeometryNode(ScrownRender1));
	showcaseGeometry.push_back(ScrownOffset1); showcaseGeometryCount++;

	// demonstrating what can be achieved when feeding in different variables
	CrownGeometry Scrown2(glm::vec3(1.0f, 0.6f, 1.0f), 1.0f, 21, 1.0f, 10, 0.2f);
	TexturedLit* ScrownRender2 = new TexturedLit(Scrown2, "textures/grid.jpg");
	TransformNode* ScrownOffset2 = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, -0.4f, 0.0f)));
	TransformNode* ScrownScale2 = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 0.6f, 1.0f)));

	ScrownOffset2->AddChild(ScrownScale2);
	ScrownScale2->AddChild(new GeometryNode(ScrownRender2));
	showcaseGeometry.push_back(ScrownOffset2); showcaseGeometryCount++;

	for (int i = 0; i < showcaseGeometryCount; i++) {
		float across = (-((float)showcaseGeometryCount / 2) + i) * 3.0f;

		TransformNode* newGeom = new TransformNode(glm::translate(glm::mat4(), glm::vec3(across, 0.0f, 0.0f)));
		newGeom->AddChild(showcaseGeometry[i]);
		showcaseScale->AddChild(newGeom);
	}
#pragma endregion Showcase Custom Geometry

#pragma region Catapult
#pragma region Catapult Model and Transforms
	float cataScaleXYZ = 4.0f;

	MeshObject* catapultBody = new MeshObject("models/catapult-body.obj");
	TransformNode* catapultTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.0f, 200.0f, 0.0f)));
	TransformNode* catapultScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(cataScaleXYZ, cataScaleXYZ, cataScaleXYZ)));
	TransformNode* catapultRotator = new TransformNode(glm::rotate(glm::mat4(), -glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	catapultRotator->AddChild(new GeometryNode(catapultBody));
	catapultScale->AddChild(catapultRotator);
	catapultTrans->AddChild(catapultScale);
	light->AddChild(catapultTrans);
	
	MeshObject* catapultWheels = new MeshObject("models/catapult-wheels.obj");
	TransformNode* catapultFWtrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.001843f, -0.18546f, 1.7875f)));
	TransformNode* catapultBWtrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.001843f, -0.18546f, -1.9737f)));
	TransformNode* catapultWheelScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));
	TransformNode* catapultWheelRotator = new TransformNode(glm::rotate(glm::mat4(), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));

	catapultWheelRotator->AddChild(new GeometryNode(catapultWheels));
	catapultWheelScale->AddChild(catapultWheelRotator);
	catapultFWtrans->AddChild(catapultWheelScale);
	catapultBWtrans->AddChild(catapultWheelScale);
	catapultRotator->AddChild(catapultFWtrans);
	catapultRotator->AddChild(catapultBWtrans);

	MeshObject* catapultSpinner = new MeshObject("models/catapult-spinner.obj");
	TransformNode* catapultSpinnerTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.025895f, -0.034912f, 2.7524f)));
	TransformNode* catapultSpinnerScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));

	catapultSpinnerScale->AddChild(new GeometryNode(catapultSpinner));
	catapultSpinnerTrans->AddChild(catapultSpinnerScale);
	catapultRotator->AddChild(catapultSpinnerTrans);

	MeshObject* catapultSpoon = new MeshObject("models/catapult-spoon.obj");
	TransformNode* catapultSpoonTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.00281f, -0.034912f, -0.30006f)));
	TransformNode* catapultSpoonScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));

	catapultSpoonScale->AddChild(new GeometryNode(catapultSpoon));
	catapultSpoonTrans->AddChild(catapultSpoonScale);
	catapultRotator->AddChild(catapultSpoonTrans);

	MeshObject* catapultString = new MeshObject("models/catapult-string.obj");
	TransformNode* catapultStringTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(0.011025f, 1.1875f, 2.1226f)));
	TransformNode* catapultStringScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)));
	TransformNode* catapultStringRot = new TransformNode(glm::rotate(glm::mat4(), -glm::radians(21.7f), glm::vec3(1.0f, 0.0f, 0.0f)));

	catapultStringRot->AddChild(new GeometryNode(catapultString));
	catapultStringScale->AddChild(catapultStringRot);
	catapultStringTrans->AddChild(catapultStringScale);
	catapultRotator->AddChild(catapultStringTrans);
#pragma endregion Catapult Model and Transforms

#pragma region Catapult Path
	std::vector<glm::vec3> cataPathVerts;
	std::vector<glm::vec3> cataPathPoints;

	float bodyCompensation = 0.62f; float pathPeakY = bodyCompensation * cataScaleXYZ + 100.0f;

	glm::vec3 backLeftPos = glm::vec3(-90.0f, pathPeakY, 90.0f); glm::vec3 backRightPos = glm::vec3(90.0f, pathPeakY, 90.0f);
	glm::vec3 frontLeftPos = glm::vec3(-90.0f, pathPeakY, -90.0f); glm::vec3 frontRightPos = glm::vec3(90.0f, pathPeakY, -90.0f);
	std::vector<glm::vec3> catapultLocations;
	catapultLocations.push_back(backLeftPos); catapultLocations.push_back(frontLeftPos);
	catapultLocations.push_back(frontRightPos); catapultLocations.push_back(backRightPos);

	std::vector<glm::vec3> cataBLtoFLpath;
	cataBLtoFLpath.push_back(glm::vec3(-80.0f, pathPeakY, 50.0f));
	cataBLtoFLpath.push_back(glm::vec3(-80.0f, pathPeakY, 0.0f));
	cataBLtoFLpath.push_back(glm::vec3(-80.0f, pathPeakY, -50.0f));
	
	std::vector<glm::vec3> cataFLtoFRpath;
	cataFLtoFRpath.push_back(glm::vec3(-50.0f, pathPeakY, -80.0f)); 
	cataFLtoFRpath.push_back(glm::vec3(0.0f, pathPeakY, -80.0f));
	cataFLtoFRpath.push_back(glm::vec3(50.0f, pathPeakY, -80.0f));

	std::vector<glm::vec3> cataFRtoBRpath;
	cataFRtoBRpath.push_back(glm::vec3(80.0f, pathPeakY, -50.0f));
	cataFRtoBRpath.push_back(glm::vec3(80.0f, pathPeakY, 0.0f));
	cataFRtoBRpath.push_back(glm::vec3(80.0f, pathPeakY, 50.0f));

	std::vector<glm::vec3> cataBRtoBLpath;
	cataBRtoBLpath.push_back(glm::vec3(50.0f, pathPeakY, 80.0f));
	cataBRtoBLpath.push_back(glm::vec3(0.0f, pathPeakY, 80.0f));
	cataBRtoBLpath.push_back(glm::vec3(-50.0f, pathPeakY, 80.0f));

	for (int i = 0; i < 3; i++) cataPathPoints.push_back(cataBLtoFLpath[i]);
	for (int i = 0; i < 3; i++) cataPathPoints.push_back(cataFLtoFRpath[i]);
	for (int i = 0; i < 3; i++) cataPathPoints.push_back(cataFRtoBRpath[i]);
	for (int i = 0; i < 3; i++) cataPathPoints.push_back(cataBRtoBLpath[i]);
	
	/*
	int catapultCorner = 3; int catapultDestination = 2;
	std::vector<glm::vec3> catapultRoute;
	catapultRoute.push_back(catapultLocations[catapultCorner]);

	int cataRouteDistance = catapultDestination - catapultCorner;
	if (abs(cataRouteDistance) > 2) { // shortest route
		cataRouteDistance = catapultDestination + 4 - catapultCorner;
		if (cataRouteDistance > 2) cataRouteDistance = -1;
	}

	for (int i = 0; i < abs(cataRouteDistance); i++) {
		int iCorner = catapultCorner + (i * (cataRouteDistance / abs(cataRouteDistance))); // find the corner we are currently at with direction of travel
		iCorner = (iCorner + catapultLocations.size()) % catapultLocations.size(); // loop to back of array if it goes negative

		int it = iCorner * 3; // get starting position
		if (cataRouteDistance < 0) it -= 1; // direction going backwards instead
		it = (it + cataPathPoints.size()) % cataPathPoints.size(); // loop back to the end of the array if it goes negative

		for (int j = 0; j < 3; j++) {
			int jit = it + (j * (cataRouteDistance / abs(cataRouteDistance)));
			catapultRoute.push_back(cataPathPoints[jit]);
		}
	}

	catapultRoute.push_back(catapultLocations[catapultDestination]);

	CRSpline* catapultPath = new CRSpline(catapultRoute, false, 0.5f);
	catapultTrans->SetUpdateCallback(new SplineAnimationCallback(catapultTrans, catapultPath, 1.0f));
	*/
	
	catapultTrans->SetTransform(glm::translate(glm::mat4(), backLeftPos));
	catapultTrans->SetUpdateCallback(new CatapultMovement(catapultTrans, cataPathPoints, catapultLocations, catapultWheelRotator, catapultRotator));

// path visualisers don't really work with the new updatecallback, uncomment the section above and comment the callback to see the paths
#pragma region Catapult Path Visualisers
#if false // change to true to visualise the points of the path
	SphereGeometry pathSphere(20);
	TexturedLit* pathSphereRender = new TexturedLit(pathSphere, "textures/grid.jpg");
	GeometryNode* pathNode = new GeometryNode(pathSphereRender);

	for (glm::vec3 pos : catapultRoute) {
		TransformNode* pathNodeTrans = new TransformNode(glm::translate(glm::mat4(), pos));
		light->AddChild(pathNodeTrans);
		pathNodeTrans->AddChild(pathNode);
	}
#endif

#if false // change to true to visual the line of the path
	for (int i = 0; i < 500; i++) {
		float u = catapultRoute.size() * i / 499.0f;
		cataPathVerts.push_back(catapultPath->GetPoint(u));
	}

	PolyLine* cataPathLine = new PolyLine(cataPathVerts, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	light->AddChild(new GeometryNode(cataPathLine));
#endif
#pragma endregion Catapult Path Visualisers
#pragma endregion Catapult Path
#pragma endregion Catapult

#pragma region Test Collider
	glm::vec3 planeCollpos = glm::vec3(0.0f, 200.0f, 0.0f);
	float planeCollYangle = 45.0f;

	glm::mat4 planeColl = glm::mat4();
	planeColl = glm::translate(planeColl, planeCollpos);
	planeColl = glm::rotate(planeColl, glm::radians(planeCollYangle), glm::vec3(0.0f, 1.0f, 0.0f));
	planeColl = glm::rotate(planeColl, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	planeColl = glm::scale(planeColl, glm::vec3(10.0f, 0.1f, 10.0f));

	CubeGeometry planeCollGeom;
	TexturedLit* planeCollLit = new TexturedLit(planeCollGeom, "textures/grid.jpg");

	TransformNode* planeCollTrans = new TransformNode(planeColl);
	light->AddChild(planeCollTrans);
	planeCollTrans->AddChild(new GeometryNode(planeCollLit));
	
	QuadCollider* planeCollider = new QuadCollider(planeColl);
	phys->AddCollider(planeCollider);
#pragma endregion Test Collider

	return light;
}