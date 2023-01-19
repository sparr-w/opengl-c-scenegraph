#pragma once
#include "stdafx.h"
#include "GeometryHelper.h"

class CrownGeometry : public GeometryHelper {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec2> uvs;

	void CreateGeometry(glm::vec3 uvScale, float height, int nDivisions, float thickness, int nSegments, float rimProportion) {
#pragma region Cylindrical Segment
		glm::vec3* outerCircle = new glm::vec3[nDivisions];
		glm::vec3* innerCircle = new glm::vec3[nDivisions];

		glm::vec2* outerUVs = new glm::vec2[nDivisions];
		glm::vec2* innerUVs = new glm::vec2[nDivisions];

		for (int i = 0; i < nDivisions; i++) {
			float theta = glm::radians(360.0f) * (float)i / (nDivisions - 1);

			outerCircle[i] = glm::vec3(cos(theta), 0.0f, sin(theta));
			innerCircle[i] = outerCircle[i] * (1.0f - thickness);

			outerUVs[i] = (glm::vec2(outerCircle[i].x, outerCircle[i].z) + glm::vec2(1.0f, 1.0f)) * 0.5f;
			innerUVs[i] = (glm::vec2(innerCircle[i].x, innerCircle[i].z) + glm::vec2(1.0f, 1.0f)) * 0.5f;
		}

		// top
		float topYpos = ((height * 2) * rimProportion) - height; // whole height multiplied by proportional value to get total height, minus half of height to get pos

		glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 tangent = glm::vec3(1.0f, 0.0f, 0.0f);

		for (int i = 0; i < nDivisions - 1; i++) {
			glm::vec3 v0 = innerCircle[i]; glm::vec3 v1 = outerCircle[i];
			glm::vec3 v2 = outerCircle[(i + 1) % nDivisions]; glm::vec3 v3 = innerCircle[(i + 1) % nDivisions];

			positions.push_back(glm::vec3(v0.x, topYpos, v0.z)); positions.push_back(glm::vec3(v1.x, topYpos, v1.z)); positions.push_back(glm::vec3(v2.x, topYpos, v2.z));
			positions.push_back(glm::vec3(v0.x, topYpos, v0.z)); positions.push_back(glm::vec3(v2.x, topYpos, v2.z)); positions.push_back(glm::vec3(v3.x, topYpos, v3.z));
			
			glm::vec2 uv0 = glm::vec2(innerUVs[i].x * uvScale.x, innerUVs[i].y * uvScale.z); glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.z);
			glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % nDivisions].x * uvScale.x, outerUVs[(i + 1) % nDivisions].y * uvScale.z); glm::vec2 uv3 = glm::vec2(innerUVs[(i + 1) % nDivisions].x * uvScale.x, innerUVs[(i + 1) % nDivisions].y * uvScale.z);

			uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);
			uvs.push_back(uv0); uvs.push_back(uv2); uvs.push_back(uv3);

			for (int j = 0; j < 6; j++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}
		}

		// bottom
		normal = glm::vec3(0.0f, -1.0f, 0.0f);
		tangent = glm::vec3(1.0f, 0.0f, 0.0f);

		for (int i = 0; i < nDivisions - 1; i++) {
			glm::vec3 v0 = innerCircle[i]; glm::vec3 v1 = outerCircle[i];
			glm::vec3 v2 = outerCircle[(i + 1) % nDivisions]; glm::vec3 v3 = innerCircle[(i + 1) % nDivisions];

			positions.push_back(glm::vec3(v0.x, -height, v0.z)); positions.push_back(glm::vec3(v1.x, -height, v1.z)); positions.push_back(glm::vec3(v2.x, -height, v2.z));
			positions.push_back(glm::vec3(v0.x, -height, v0.z)); positions.push_back(glm::vec3(v2.x, -height, v2.z)); positions.push_back(glm::vec3(v3.x, -height, v3.z));

			glm::vec2 uv0 = glm::vec2(innerUVs[i].x * uvScale.x, innerUVs[i].y * uvScale.z); glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.z);
			glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % nDivisions].x * uvScale.x, outerUVs[(i + 1) % nDivisions].y * uvScale.z); glm::vec2 uv3 = glm::vec2(innerUVs[(i + 1) % nDivisions].x * uvScale.x, innerUVs[(i + 1) % nDivisions].y * uvScale.z);

			uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);
			uvs.push_back(uv0); uvs.push_back(uv2); uvs.push_back(uv3);

			for (int j = 0; j < 6; j++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}
		}

		// outer faces
		for (int i = 0; i < nDivisions; i++) {
			glm::vec3 pos[4]; glm::vec3 norm[4]; glm::vec3 tang[4]; glm::vec2 uv[4];

			// positions
			pos[0] = outerCircle[i] + glm::vec3(0.0f, -height, 0.0f);
			pos[1] = outerCircle[i] + glm::vec3(0.0f, topYpos, 0.0f);
			pos[2] = outerCircle[(i + 1) % nDivisions] + glm::vec3(0.0f, topYpos, 0.0f);
			pos[3] = outerCircle[(i + 1) % nDivisions] + glm::vec3(0.0f, -height, 0.0f);

			// normals
			norm[0] = outerCircle[i];
			norm[1] = outerCircle[i];
			norm[2] = outerCircle[(i + 1) % nDivisions];
			norm[3] = outerCircle[(i + 1) % nDivisions];

			// tangents
			tang[0] = glm::cross(norm[0], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[1] = glm::cross(norm[1], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[2] = glm::cross(norm[2], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[3] = glm::cross(norm[3], glm::vec3(0.0f, 1.0f, 0.0f));

			// uv
			uv[0] = glm::vec2(((float)i / nDivisions) * uvScale.x, 0.0f);
			uv[1] = glm::vec2(((float)i / nDivisions) * uvScale.x, uvScale.y * rimProportion);
			uv[2] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, uvScale.y * rimProportion);
			uv[3] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, 0.0f);

			int triangles[] = { 0, 1, 2, 0, 2, 3 };
			for (int j = 0; j < 6; j++) {
				positions.push_back(pos[triangles[j]]);
				normals.push_back(norm[triangles[j]]);
				tangents.push_back(tang[triangles[j]]);
				uvs.push_back(uv[triangles[j]]);
			}
		}

		// inner faces
		for (int i = 0; i < nDivisions; i++) {
			glm::vec3 pos[4]; glm::vec3 norm[4]; glm::vec3 tang[4]; glm::vec2 uv[4];

			// positions
			pos[0] = innerCircle[i] + glm::vec3(0.0f, -height, 0.0f);
			pos[1] = innerCircle[i] + glm::vec3(0.0f, topYpos, 0.0f);
			pos[2] = innerCircle[(i + 1) % nDivisions] + glm::vec3(0.0f, topYpos, 0.0f);
			pos[3] = innerCircle[(i + 1) % nDivisions] + glm::vec3(0.0f, -height, 0.0f);

			// normals
			norm[0] = -innerCircle[i];
			norm[1] = -innerCircle[i];
			norm[2] = -innerCircle[(i + 1) % nDivisions];
			norm[3] = -innerCircle[(i + 1) % nDivisions];

			// tangents
			tang[0] = glm::cross(norm[0], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[1] = glm::cross(norm[1], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[2] = glm::cross(norm[2], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[3] = glm::cross(norm[3], glm::vec3(0.0f, 1.0f, 0.0f));

			// uv
			uv[0] = glm::vec2(((float)i / nDivisions) * uvScale.x, 0.0f);
			uv[1] = glm::vec2(((float)i / nDivisions) * uvScale.x, uvScale.y * rimProportion);
			uv[2] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, uvScale.y * rimProportion);
			uv[3] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, 0.0f);

			int triangles[] = { 0, 1, 2, 0, 2, 3 };
			for (int j = 0; j < 6; j++) {
				positions.push_back(pos[triangles[j]]);
				normals.push_back(norm[triangles[j]]);
				tangents.push_back(tang[triangles[j]]);
				uvs.push_back(uv[triangles[j]]);
			}
		}
#pragma endregion Cylindrical Segment

#pragma region Top Segments
		int sDivisions = (nSegments * 2) + 1;
		outerCircle = new glm::vec3[sDivisions];
		innerCircle = new glm::vec3[sDivisions];

		outerUVs = new glm::vec2[nDivisions];
		innerUVs = new glm::vec2[nDivisions];

		for (int i = 0; i < sDivisions; i++) {
			float theta = glm::radians(360.0f) * (float)i / (sDivisions - 1);

			outerCircle[i] = glm::vec3(cos(theta), 0.0f, sin(theta));
			innerCircle[i] = outerCircle[i] * (1.0f - thickness);

			outerUVs[i] = (glm::vec2(outerCircle[i].x, outerCircle[i].z) + glm::vec2(1.0f, 1.0f)) * 0.5f;
			innerUVs[i] = (glm::vec2(innerCircle[i].x, innerCircle[i].z) + glm::vec2(1.0f, 1.0f)) * 0.5f;
		}

		// top faces
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
		tangent = glm::vec3(1.0f, 0.0f, 0.0f);

		for (int i = 0; i < nSegments; i++) {
			int it = i * 2;
			
			glm::vec3 v0 = innerCircle[it]; glm::vec3 v1 = outerCircle[it];
			glm::vec3 v2 = outerCircle[(it + 1) % sDivisions]; glm::vec3 v3 = innerCircle[(it + 1) % sDivisions];

			positions.push_back(glm::vec3(v0.x, height, v0.z)); positions.push_back(glm::vec3(v1.x, height, v1.z)); positions.push_back(glm::vec3(v2.x, height, v2.z));
			positions.push_back(glm::vec3(v0.x, height, v0.z)); positions.push_back(glm::vec3(v2.x, height, v2.z)); positions.push_back(glm::vec3(v3.x, height, v3.z));
		
			glm::vec2 uv0 = glm::vec2(innerUVs[i].x * uvScale.x, innerUVs[i].y * uvScale.z); glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.z);
			glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % nDivisions].x * uvScale.x, outerUVs[(i + 1) % nDivisions].y * uvScale.z); glm::vec2 uv3 = glm::vec2(innerUVs[(i + 1) % nDivisions].x * uvScale.x, innerUVs[(i + 1) % nDivisions].y * uvScale.z);

			uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);
			uvs.push_back(uv0); uvs.push_back(uv2); uvs.push_back(uv3);

			for (int j = 0; j < 6; j++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}
		}

		// bottom faces
		normal = glm::vec3(0.0f, -1.0f, 0.0f);
		tangent = glm::vec3(1.0f, 0.0f, 0.0f);

		for (int i = 0; i < nSegments; i++) {
			int it = i * 2;

			glm::vec3 v0 = innerCircle[it]; glm::vec3 v1 = outerCircle[it];
			glm::vec3 v2 = outerCircle[(it + 1) % nDivisions]; glm::vec3 v3 = innerCircle[(it + 1) % nDivisions];

			positions.push_back(glm::vec3(v0.x, topYpos, v0.z)); positions.push_back(glm::vec3(v1.x, topYpos, v1.z)); positions.push_back(glm::vec3(v2.x, topYpos, v2.z));
			positions.push_back(glm::vec3(v0.x, topYpos, v0.z)); positions.push_back(glm::vec3(v2.x, topYpos, v2.z)); positions.push_back(glm::vec3(v3.x, topYpos, v3.z));

			glm::vec2 uv0 = glm::vec2(innerUVs[i].x * uvScale.x, innerUVs[i].y * uvScale.z); glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.z);
			glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % nDivisions].x * uvScale.x, outerUVs[(i + 1) % nDivisions].y * uvScale.z); glm::vec2 uv3 = glm::vec2(innerUVs[(i + 1) % nDivisions].x * uvScale.x, innerUVs[(i + 1) % nDivisions].y * uvScale.z);

			uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);
			uvs.push_back(uv0); uvs.push_back(uv2); uvs.push_back(uv3);

			for (int j = 0; j < 6; j++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}
		}

		// outer faces
		for (int i = 0; i < nSegments; i++) {
			int it = i * 2;

			glm::vec3 pos[4]; glm::vec3 norm[4]; glm::vec3 tang[4]; glm::vec2 uv[4];

			// positions
			pos[0] = outerCircle[it] + glm::vec3(0.0f, topYpos, 0.0f);
			pos[1] = outerCircle[it] + glm::vec3(0.0f, height, 0.0f);
			pos[2] = outerCircle[(it + 1) % sDivisions] + glm::vec3(0.0f, height, 0.0f);
			pos[3] = outerCircle[(it + 1) % sDivisions] + glm::vec3(0.0f, topYpos, 0.0f);

			// normals
			norm[0] = outerCircle[it];
			norm[1] = outerCircle[it];
			norm[2] = outerCircle[(it + 1) % sDivisions];
			norm[3] = outerCircle[(it + 1) % sDivisions];

			// tangents
			tang[0] = glm::cross(norm[0], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[1] = glm::cross(norm[1], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[2] = glm::cross(norm[2], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[3] = glm::cross(norm[3], glm::vec3(0.0f, 1.0f, 0.0f));

			// uv
			uv[0] = glm::vec2(((float)i / nDivisions) * uvScale.x, uvScale.y * rimProportion);
			uv[1] = glm::vec2(((float)i / nDivisions) * uvScale.x, (uvScale.y * rimProportion) + (uvScale.y * (1.0f - rimProportion)));
			uv[2] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, (uvScale.y * rimProportion) + (uvScale.y * (1.0f - rimProportion)));
			uv[3] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, uvScale.y * rimProportion);

			int triangles[] = { 0, 1, 2, 0, 2, 3 };
			for (int j = 0; j < 6; j++) {
				positions.push_back(pos[triangles[j]]);
				normals.push_back(norm[triangles[j]]);
				tangents.push_back(tang[triangles[j]]);
				uvs.push_back(uv[triangles[j]]);
			}
		}

		// inner faces
		for (int i = 0; i < nSegments; i++) {
			int it = i * 2;

			glm::vec3 pos[4]; glm::vec3 norm[4]; glm::vec3 tang[4]; glm::vec2 uv[4];

			// positions
			pos[0] = innerCircle[it] + glm::vec3(0.0f, topYpos, 0.0f);
			pos[1] = innerCircle[it] + glm::vec3(0.0f, height, 0.0f);
			pos[2] = innerCircle[(it + 1) % sDivisions] + glm::vec3(0.0f, height, 0.0f);
			pos[3] = innerCircle[(it + 1) % sDivisions] + glm::vec3(0.0f, topYpos, 0.0f);

			// normals
			norm[0] = -innerCircle[it];
			norm[1] = -innerCircle[it];
			norm[2] = -innerCircle[(it + 1) % sDivisions];
			norm[3] = -innerCircle[(it + 1) % sDivisions];

			// tangents
			tang[0] = glm::cross(norm[0], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[1] = glm::cross(norm[1], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[2] = glm::cross(norm[2], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[3] = glm::cross(norm[3], glm::vec3(0.0f, 1.0f, 0.0f));

			// uv
			uv[0] = glm::vec2(((float)i / nDivisions) * uvScale.x, uvScale.y * rimProportion);
			uv[1] = glm::vec2(((float)i / nDivisions) * uvScale.x, (uvScale.y * rimProportion) + (uvScale.y * (1.0f - rimProportion)));
			uv[2] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, (uvScale.y * rimProportion) + (uvScale.y * (1.0f - rimProportion)));
			uv[3] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, uvScale.y * rimProportion);

			int triangles[] = { 0, 1, 2, 0, 2, 3 };
			for (int j = 0; j < 6; j++) {
				positions.push_back(pos[triangles[j]]);
				normals.push_back(norm[triangles[j]]);
				tangents.push_back(tang[triangles[j]]);
				uvs.push_back(uv[triangles[j]]);
			}
		}

		// inside faces
		for (int i = 0; i < nSegments; i++) {
			int it = i * 2;

			glm::vec3 pos[4]; glm::vec3 norm[4]; glm::vec3 tang[4]; glm::vec2 uv[4];

			// positions
			for (int k = 0; k < 2; k++) {
				pos[0] = innerCircle[it + k] + glm::vec3(0.0f, topYpos, 0.0f);
				pos[1] = innerCircle[it + k] + glm::vec3(0.0f, height, 0.0f);
				pos[2] = outerCircle[it + k] + glm::vec3(0.0f, height, 0.0f);
				pos[3] = outerCircle[it + k] + glm::vec3(0.0f, topYpos, 0.0f);

				// normals
				norm[0] = glm::cross((pos[1] - pos[0]), (pos[3] - pos[0]));
				norm[1] = glm::cross((pos[2] - pos[1]), (pos[0] - pos[1]));
				norm[2] = glm::cross((pos[3] - pos[2]), (pos[1] - pos[2]));
				norm[3] = glm::cross((pos[0] - pos[3]), (pos[2] - pos[3]));

				if (k == 1) {
					for (int l = 0; l < 4; l++)
						norm[l] = -norm[l];
				}

				// tangents
				tang[0] = glm::cross(norm[0], glm::vec3(0.0f, 1.0f, 0.0f));
				tang[1] = glm::cross(norm[1], glm::vec3(0.0f, 1.0f, 0.0f));
				tang[2] = glm::cross(norm[2], glm::vec3(0.0f, 1.0f, 0.0f));
				tang[3] = glm::cross(norm[3], glm::vec3(0.0f, 1.0f, 0.0f));

				// uv
				uv[0] = glm::vec2(((float)i / nDivisions) * uvScale.x, uvScale.y * rimProportion);
				uv[1] = glm::vec2(((float)i / nDivisions) * uvScale.x, (uvScale.y * rimProportion) + (uvScale.y * (1.0f - rimProportion)));
				uv[2] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, (uvScale.y * rimProportion) + (uvScale.y * (1.0f - rimProportion)));
				uv[3] = glm::vec2(((float)(i + 1) / nDivisions) * uvScale.x, uvScale.y * rimProportion);

				int triangles[] = { 0, 1, 2, 0, 2, 3 };
				for (int j = 0; j < 6; j++) {
					positions.push_back(pos[triangles[j]]);
					normals.push_back(norm[triangles[j]]);
					tangents.push_back(tang[triangles[j]]);
					uvs.push_back(uv[triangles[j]]);
				}
			}
		}

		/*
		for (int i = 0; i < nSegments; i++) {
			int it = i * 2;

			glm::vec3 segmentVpos[] = {
				glm::vec3(innerCircle[it].x, topYpos, innerCircle[it].z),
				glm::vec3(outerCircle[it].x, topYpos, outerCircle[it].z),
				glm::vec3(outerCircle[(it + 1) % sDivisions].x, topYpos, outerCircle[(it + 1) % sDivisions].z),
				glm::vec3(innerCircle[(it + 1) % sDivisions].x, topYpos, innerCircle[(it + 1) % sDivisions].z),
				glm::vec3(innerCircle[it].x, height, innerCircle[it].z),
				glm::vec3(outerCircle[it].x, height, outerCircle[it].z),
				glm::vec3(outerCircle[(it + 1) % sDivisions].x, height, outerCircle[(it + 1) % sDivisions].z),
				glm::vec3(innerCircle[(it + 1) % sDivisions].x, height, innerCircle[(it + 1) % sDivisions].z),
			};

			glm::vec3 segmentTan[] = {
				glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
				glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)
			};

			glm::vec3 segmentNorm[] = {
				glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)
			};

			glm::vec2 segmentUVs[] = {
				glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
			};

			int triangles[] = { 0,4,5, 0,5,1, // left
				1,5,6, 1,6,2, // front
				2,6,7, 2,7,3, // right
				3,7,4, 3,4,0, // back
				4,7,6, 4,6,5, // top
				3,0,1, 3,1,2  // bottom
			}; 

			for (int j = 0; j < 36; j++) {
				positions.push_back(segmentVpos[triangles[j]]);
				normals.push_back(segmentNorm[j / 6]);
				tangents.push_back(segmentTan[j / 6]);
				uvs.push_back(segmentUVs[j % 6]);
			}
		}
		*/
#pragma endregion Top Segments
	}
public:
	CrownGeometry(glm::vec3 uvScale, float height = 1.0f, int nDivisions = 17, float thickness = 0.3f, int nSegments = 8, float rimProportion = 0.5f) { CreateGeometry(uvScale, height, nDivisions, thickness, nSegments, rimProportion); }
	const std::vector<glm::vec3>& GetPositionVector() const { return positions; }
	const std::vector<glm::vec3>& GetNormalVector() const { return normals; }
	const std::vector<glm::vec2>& GetUVVector() const { return uvs; }
	const std::vector<glm::vec3>& GetTangentVector() const { return tangents; }
	int GetNumVertices() const { return positions.size(); }
};