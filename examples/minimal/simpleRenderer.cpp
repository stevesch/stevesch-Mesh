/**
 * @file simpleRenderer.cpp
 * @author Stephen Schlueter, github: stevesch
 * @brief Simple ESP wireframe 3D renderer implementation file
 * @version 0.1
 * @date 2021-05-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "simpleRenderer.h"

#include <StreamString.h>

#include <stevesch-Mesh.h>
#include <stevesch-Display.h>

#include <Button2.h>

#include <SPIFFS.h>
#include <esp_vfs.h>

using namespace stevesch;

///
#ifdef BUTTON_1
Button2 button1(BUTTON_1);
#endif
#ifdef BUTTON_2
Button2 button2(BUTTON_2);
#endif
///

Display display(TFT_WIDTH, TFT_HEIGHT);

float sFOV_Vertical = degToRad(60.0f);
constexpr float kZNear = 0.1f;
constexpr float kZFar = 8.0f;

vector4 vCameraFocus(0.0f, 0.0f, -4.0f);
vector4 limita, limitb; // cheap xyz limits for objects

matrix4 mtxWtoV(1.0f);   // camera transform (inverse of the world space transform of the camera object)
matrix4 mtxVtoC(1.0f);   // view-to-clip transform (projection)
matrix4 mtxNDCtoS(1.0f); // normalized-device-coords-to-screen transform

std::vector<vector3> vertDst; // used by drawFaceMesh

// updated in updateFrustum:
matrix4 mtxVtoW;
Frustum frustum;

FaceMesh mesh1;

std::vector<String> models;

int currentModel = -1;

bool usingPlaceholder = false;

struct Inst : public SceneObj
{
  uint16_t color = TFT_GREEN;
};

std::vector<Inst> instances;
const int maxInstCount = 8;
int activeInstCount = 1;

inline void perspectiveTransform(vector3 &vOut, const vector4 &vLocal, const matrix4 &mtxLToC)
{
  vector4 dst;
  vector4::transform(dst, mtxLToC, vLocal);
  float invw = stevesch::recipf(dst.w);
  dst.x *= invw;
  dst.y *= invw;
  dst.z *= invw;
  dst.w = 1.0f;

  // Serial.printf("ptC: <%5.2f, %5.2f, %5.2f, %5.2F>\n", dst.x, dst.y, dst.z, dst.w);

  vector4::transform(dst, mtxNDCtoS, dst);
  vOut.x = dst.x;
  vOut.y = dst.y;
  vOut.z = dst.z;
  // Serial.printf("ptS: <%5.2f, %5.2f, %5.2f, %5.2F>\n", dst.x, dst.y, dst.z);
}

void drawFaceMesh(TFT_eSPI *renderTarget, const FaceMesh &mesh, const matrix4 &mtxLtoW, uint16_t color)
{
  //matrix4 mtxLtoS;	// mtxLtoW * mtxWtoS	(== mtxLtoW * mtxWtoV * mtxVtoS)
  matrix4 mtxLtoV;
  matrix4 mtxLtoC;
  vector4 faceNormal;

  //matrix4::mul(mtxLtoS, mtxWtoS, mtxLtoW);
  matrix4::mul(mtxLtoV, mtxWtoV, mtxLtoW);
  matrix4::mul(mtxLtoC, mtxVtoC, mtxLtoV);

  const stevesch::positionBuffer_t positions = mesh.positions();
  //const normalBuffer_t normals = mesh.normals();
  const stevesch::faceBuffer_t &faces = mesh.faces();
  const stevesch::indexBuffer_t &posIndices = mesh.getPositionIndices();

  const int faceCount = faces.size();
  for (int iface = 0; iface < faceCount; ++iface)
  {
    const stevesch::IndexedFace &face = faces[iface];

#if USE_FACE_NORMALS
    faceNormal.set(mesh.getNormal(face.iNormal));
    //Serial.printf("Face normal[%d] (%5.2f, %5.2f, %5.2f)\n", iface, faceNormal.x, faceNormal.y, faceNormal.z);
    //renderTarget->setCursor(0, 0);
    //renderTarget->printf("x:%5.2f\ny:%5.2f\nz:%5.2f\n", faceNormal.x, faceNormal.y, faceNormal.z);
    vector4 v0;
    //v0.Set(positions[face.iPosition[0]]);
    uint index0 = face.iFirst;
    v0.set(positions[posIndices[index0]]);

    v0.transform(mtxLtoV);
    vector4::transformSub(faceNormal, mtxLtoV, faceNormal);
    if (faceNormal.dot3(v0) < 0.0f)
#endif
    {
      vector4 src, dst;
      //uint vertCount = face.iPosition.size();
      uint vertCount = face.iCount;
      if (vertDst.size() < vertCount)
      {
        vertDst.resize(vertCount);
      }

      int j;
      for (j = 0; j < vertCount; ++j)
      {
        //uint posIndex = face.iPosition[j];
        uint posIndex = posIndices[index0 + j];
        src.set(positions[posIndex]);
        perspectiveTransform(vertDst[j], src, mtxLtoC);
      }

#if !USE_FACE_NORMALS
      // works for convex polys, but we're trying non-convex faces:
      float cw;
      {
        float x0 = vertDst[0].x;
        float y0 = vertDst[0].y;
        float x1 = vertDst[1].x;
        float y1 = vertDst[1].y;
        float x2 = vertDst[2].x;
        float y2 = vertDst[2].y;
        x1 -= x0;
        y1 -= y0;
        x2 -= x0;
        y2 -= y0;
        cw = x1 * y2 - x2 * y1;
      }
      if (cw > 0.0f)
#endif
      {
        j = vertCount - 1;
        for (int k = 0; k < vertCount; ++k)
        {
          const vector3 &v1 = vertDst[j];
          const vector3 &v2 = vertDst[k];
          if ((v1.z >= 0.0f) && (v2.z >= 0.0f))
          {
            int16_t x1 = (int16_t)v1.x;
            int16_t y1 = (int16_t)v1.y;
            int16_t x2 = (int16_t)v2.x;
            int16_t y2 = (int16_t)v2.y;
            renderTarget->drawLine(x1, y1, x2, y2, color);
          }

          j = k;
        }
      }
    }
  }
}

void drawScene(TFT_eSPI *renderTarget)
{
  matrix4 mtxLtoW;

  // for (auto&& obj : instances)
  for (int index = 0; index < activeInstCount; ++index)
  {
    const auto &obj = instances[index];
    obj.calcLtoW(mtxLtoW);
    uint16_t color = obj.color;
    drawFaceMesh(renderTarget, mesh1, mtxLtoW, color);
  }
}

void ICACHE_FLASH_ATTR scanModels()
{
  display.yieldSPI();
  SPIFFS.begin();

  String path = "/spiffs/";
  DIR *dir = opendir(path.c_str());
  if (dir)
  {
    Serial.printf("Opened root folder\n");
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
      String name = "/";
      name += ent->d_name;

      Serial.printf("File: %s\n", ent->d_name);
      if (name.endsWith(".obj") && (name.indexOf("wire-") < 0))
      {
        Serial.printf("Adding model <%s>\n", name.c_str());
        models.push_back(name);
      }
    }
  }
  else
  {
    Serial.printf("Couldn't open root folder\n");
  }
  SPIFFS.end();
  display.claimSPI();
  yield();
}

void SetScreenMatrix(int width, int height)
{
  matrix4 m;

  int nWidthPixels = width;
  int nHeightPixels = height;
  float fRadiansV = sFOV_Vertical;
  float fRadiansH = 2.0f * atanf(((float)nWidthPixels / (float)nHeightPixels) * tanf(0.5f * fRadiansV));

  if (fRadiansH > degToRad(120.0f))
  {
    fRadiansH = degToRad(120.0f);
    fRadiansV = 2.0f * atanf(((float)nHeightPixels / (float)nWidthPixels) * tanf(0.5f * fRadiansH));
    sFOV_Vertical = fRadiansV;
  }

  Serial.printf("Frustum fovH=%5.2f fovV=%5.2f>\n", radToDeg(fRadiansH), radToDeg(fRadiansV));
  // m.PerspectiveLH(fRadiansH, fRadiansV, kZNear, kZFar);
  m.perspectiveRH(fRadiansH, fRadiansV, kZNear, kZFar);

  // projection matrix (view-to-clip)
  mtxVtoC = m;

  // NDC-to-screen matrix
  m.identity();
  m.m00 = 0.5f * width;
  m.m11 = -0.5f * height;
  m.m03 = 0.5f * width;
  m.m13 = 0.5f * height;
  mtxNDCtoS = m;
}

const char *planeName[] = {
    "LEFT", "RIGHT", "BOTTOM", "TOP", "NEAR", "FAR"};

void updateFrustum()
{
  matrix4::invert(mtxVtoW, mtxWtoV);
  Serial.printf("VtoC m22: %5.2F\n", mtxVtoC.m22);
  Serial.printf("VtoC m32: %5.2F\n", mtxVtoC.m32);
  frustum = Frustum(kZNear, kZFar, mtxVtoC, &mtxVtoW);

  for (unsigned int i = 0; i < Frustum::SFRUSTUM_PLANECOUNT; ++i)
  {
    const Plane &plane = frustum.getPlane(i);
    float d = plane.getDistance();
    const vector3 &n = plane.getNormal();
    Serial.printf("Frustum plane %6s: <%5.2f, %5.2f, %5.2f; %5.2F>\n", planeName[i], n.x, n.y, n.z, d);
  }

  // DEBUG:
  // bool rh = (mtxVtoC.m32 < 0.0f);
  // float zScreenNear = rh ? -kZNear : kZNear;
  // float zScreenFar = rh ? -kZFar : kZFar;
  float zScreenNear = kZNear;
  float zScreenFar = kZFar;
  vector4 v[8]{
      vector4(-1.0f, -1.0f, zScreenNear),
      vector4(1.0f, -1.0f, zScreenNear),
      vector4(-1.0f, 1.0f, zScreenNear),
      vector4(1.0f, 1.0f, zScreenNear),
      vector4(-1.0f, -1.0f, zScreenFar),
      vector4(1.0f, -1.0f, zScreenFar),
      vector4(-1.0f, 1.0f, zScreenFar),
      vector4(1.0f, 1.0f, zScreenFar),
  };

  matrix4 mtxCtoV;
  matrix4::invert4x4(mtxCtoV, mtxVtoC);
  Serial.printf("Frustum points:\n");
  SScreenToWorld(v, v, 8, mtxVtoC, mtxCtoV, &mtxVtoW);
  for (const auto &r : v)
  {
    Serial.printf("  <%5.2f, %5.2f, %5.2f>\n", r.x, r.y, r.z);
  }

  Plane pl(v[3], v[2], v[0], true);
  {
    const Plane &plane = pl;
    float d = plane.getDistance();
    const vector3 &n = plane.getNormal();
    Serial.printf("Test plane %6s: <%5.2f, %5.2f, %5.2f; %5.2F>\n", "TEST", n.x, n.y, n.z, d);
  }

  // printPts();
}

void ICACHE_FLASH_ATTR scaleModelToCamera()
{
  //matrix4 rot;
  //rot.XMatrix(degToRad(0.0f));

  vector3 vmin, vmax, vdif, vcen;
  mesh1.computeExtents(vmin, vmax);
  vector3::sub(vdif, vmax, vmin);
  vector3::add(vcen, vmax, vmin);
  vcen *= 0.5f;

  float radius = 0.5f * vdif.abs();
  if (radius > 0.0f)
  {
    // shrink radius if possible
    radius = stevesch::minf(radius, mesh1.computeExtentsFrom(vcen));
    Serial.printf("Model radius=%5.2f\n", radius);

    vector3 vCameraPos;
    {
      matrix4 mtxVtoW;
      matrix4::invert(mtxVtoW, mtxWtoV);
      mtxVtoW.getTranslation(vCameraPos);
    }
    vector3 vToTarget;
    vector3::sub(vToTarget, vCameraFocus, vCameraPos);
    float dist = vToTarget.abs();
    constexpr float kFillFactor = 0.5; // amount of screen to occupy (vertically)
    // constexpr float kFillFactor = 0.1;	// amount of screen to occupy (vertically)
    float idealRadius = dist * kFillFactor * tanf(0.5f * sFOV_Vertical);
    // float idealRadius = 0.1f;
    // float idealRadius = 0.01f;
    Serial.printf("ideal radius=%5.2f\n", idealRadius);
    if (idealRadius > (dist - kZNear))
    {
      idealRadius = dist - kZNear;
      Serial.printf("ideal radius clamped=%5.2f\n", idealRadius);
    }
    float scale = idealRadius / radius;

    uint vc = mesh1.positionCount();
    for (uint i = 0; i < vc; ++i)
    {
      vector3 &rv = mesh1.refPosition(i);
      rv -= vcen;
      rv *= scale;
      //rv.transform(rot);
    }

    ////////////////

    limita = vCameraFocus;
    limitb = vCameraFocus;

    const float dx = 2.0f;
    const float dy = 2.0f;

    limita.x -= dx;
    limita.y -= dy;
    limita.z = kZNear + idealRadius * 1.1f;

    limitb.x += dx;
    limitb.y += dy;
    limitb.z += 10.0f;

    ////////////////

    // display.clearRenderTarget();

    // TFT_eSPI* renderTarget = display.currentRenderTarget();
    // renderTarget->setTextDatum(TL_DATUM); // restore to default
    // renderTarget->setCursor(0, 0);
    // if (currentModel <= models.size())
    // {
    // 	renderTarget->printf("%s\n", models[currentModel].c_str());
    // }
    // renderTarget->printf("verts:%d\nfaces:%d", mesh1.positionCount(), mesh1.faceCount());
    // renderTarget->printf("\nscale: %4.2f", scale);

    // display.finishRender();

    // espDelay(displayStatsDelay);
  }
}

const char *asciiCube =
    "v -20 -20 20\n\
v 20 -20 20\n\
v -20 20 20\n\
v 20 20 -20\n\
v -20 -20 -20\n\
v -20 20 -20\n\
v 20 -20 -20\n\
v 20 20 20\n\
f 3\\0\\6 8\\0\\6 4\\0\\6 6\\0\\6\n\
f 7\\0\\3 2\\0\\3 1\\0\\3 5\\0\\3\n\
f 2\\0\\5 7\\0\\5 4\\0\\5 8\\0\\5\n\
f 6\\0\\4 5\\0\\4 1\\0\\4 3\\0\\4\n\
f 8\\0\\1 3\\0\\1 1\\0\\1 2\\0\\1\n\
f 5\\0\\2 6\\0\\2 4\\0\\2 7\\0\\2\n";

const char *asciiHexag =
    "v -9 -16 14\n\
v -0.34 -21 14\n\
v -9 -6 14\n\
v -9 -6 0\n\
v -0.34 -1 0\n\
v -0.34 -1 14\n\
v 8.321 -6 0\n\
v 8.321 -6 14\n\
v 8.321 -16 0\n\
v -9 -16 0\n\
v -0.34 -21 0\n\
v 8.321 -16 14\n\
f 11\\0\\7 2\\0\\7 1\\0\\7 10\\0\\7\n\
f 12\\0\\1 8\\0\\1 6\\0\\1 3\\0\\1 1\\0\\1 2\\0\\1\n\
f 3\\0\\2 4\\0\\2 10\\0\\2 1\\0\\2\n\
f 5\\0\\6 7\\0\\6 9\\0\\6 11\\0\\6 10\\0\\6 4\\0\\6\n\
f 7\\0\\5 8\\0\\5 12\\0\\5 9\\0\\5\n\
f 12\\0\\8 2\\0\\8 11\\0\\8 9\\0\\8\n\
f 7\\0\\4 5\\0\\4 6\\0\\4 8\\0\\4\n\
f 5\\0\\3 4\\0\\3 3\\0\\3 6\\0\\3\n";

static int modelNum = 0;

bool ICACHE_FLASH_ATTR loadModel(FaceMesh &meshDst, const char *path)
{
  Serial.printf("Loading model <%s>\n", path);

  // give up this memory for now
  vertDst.clear();
  vertDst.shrink_to_fit();

  meshDst.clear();
  meshDst.compactMemory();

  display.yieldSPI();
  bool bImportSuccess = importObj(meshDst, path);
  display.claimSPI();

  constexpr size_t kExpectedMaxVertsPerFace = 64;
  vertDst.reserve(kExpectedMaxVertsPerFace);

  if (bImportSuccess && meshDst.positionCount() > 0)
  {
    usingPlaceholder = false;
    return true;
  }
  Serial.printf("Failed to load model <%s>\n", path);
  meshDst.clear();
  {
    const char *src;
    if ((modelNum % 2) == 0)
    {
      src = asciiCube;
      Serial.printf("Using placeholder: asciiCube\n");
    }
    else
    {
      src = asciiHexag;
      Serial.printf("Using placeholder: asciiHexag\n");
    }
    ++modelNum;

    StreamString ss;
    ss.write((const uint8_t *)src, strlen(src) + 1);
    display.yieldSPI();
    importObj(meshDst, ss);
    display.claimSPI();
  }
  usingPlaceholder = true;
  return false;
}


void ICACHE_FLASH_ATTR randomizeVelocity(struct Inst& obj)
{
	const float speedFactor = 2.0f*(1.0f + log2f((float)activeInstCount));
	constexpr float speedMin = degToRad(22.5f);
	constexpr float speedMax = degToRad(90.0f);
//  constexpr float speedMin = degToRad(180.0f);
//  constexpr float speedMax = degToRad(720.0f);
	randomizeAngularVelocity(obj.mAngularV, speedFactor*speedMin, speedFactor*speedMax);

	obj.mLinearV.randSpherical(S_RandGen);
	const float kRandSpeedMax = 0.5f * speedFactor;
 	// const float kRandSpeedMax = 3.5f;
	obj.mLinearV *= kRandSpeedMax;
}


constexpr uint16_t stockColors[] = {
  // 0x000f, 0x03e0, 0x03ef, 0x7800, 0x780f,
	// 0x7be0, 0xc618, 0x7bef, 0x001f, 0x07e0,
	// 0x07ff, 0xf800, 0xf81f, 0xffe0, 0xffff,
	// 0xfda0, 0xb7e0, 0xfc9f
	TFT_NAVY, TFT_DARKGREEN, TFT_DARKCYAN, TFT_MAROON, TFT_PURPLE,
	TFT_OLIVE, TFT_LIGHTGREY, TFT_DARKGREY, TFT_BLUE, TFT_GREEN,
	TFT_CYAN, TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_WHITE,
	TFT_ORANGE, TFT_GREENYELLOW, TFT_PINK
};
constexpr int stockColorCount = sizeof(stockColors) / sizeof(stockColors[0]);

void ICACHE_FLASH_ATTR randomizeInstance(struct Inst& obj)
{
	vector4 v(vCameraFocus);
	// DEBUG: start all at camera focus (disable position randomization)
	//v += vector4(
	//	S_RandGen.GetFloatAB(-2.0f, 2.0f),
	//	S_RandGen.GetFloatAB(-1.0f, 1.0f),
	//	S_RandGen.GetFloatAB(0.0f, 4.0f));
	obj.mvLtoW = v;

	randomizeVelocity(obj);

	int colorIndex = S_RandGen.getInt(stockColorCount);
	// Serial.printf("Color index chosen: %d\n", colorIndex);
	obj.color = stockColors[colorIndex];
}


void ICACHE_FLASH_ATTR randomizeInstances()
{
	for (int index=0; index<activeInstCount; ++index)
	{
		auto& obj = instances[index];
		randomizeInstance(obj);
	}

  // DEBUG: make one stationary
	// auto& o = instances[0];
	// o.mLinearV.Set(0.0f, 0.0f, 0.0f);
	// o.mLinearV.Set(1.0f, 0.0f, 0.0f);
	// o.mLinearV.Set(0.0f, 1.0f, 0.0f);
	// o.mLinearV.Set(0.0f, 0.0f, 1.0f);

	// o.mAngularV.Set(0.0f, 0.0f, 0.0f);
	// o.mAngularV.Set(1.6f, 0.0f, 0.0f);
	// o.mAngularV.Set(0.0f, 1.6f, 0.0f);
	// o.mAngularV.Set(0.0f, 0.0f, 1.6f);
	// o.mAngularV.Set(0.0f, 0.0f, 10.0f);
	// o.mAngularV.Set(11.0f, 23.0f, 7.0f);
}

void ICACHE_FLASH_ATTR restartInstances() {
  randomizeInstances();
}


void ICACHE_FLASH_ATTR nextModel()
{
  int lastModel = currentModel;
  const int modelCount = models.size();
  ++currentModel;
  if (currentModel >= modelCount)
  {
    currentModel = 0;
  }

  if ((currentModel != lastModel) && (currentModel < modelCount))
  {
    display.fullScreenMessage("Loading...");
    loadModel(mesh1, models[currentModel].c_str());
    scaleModelToCamera();
  }
  // else there must be no models

  // lastModelChange = millis();
}

void reflectAtFrustum(struct Inst &obj)
{
  Sphere s(obj.mvLtoW, obj.mRadius);
  vector3 &v = obj.mLinearV;
  std::vector<vector3> normals;

  for (unsigned int i = 0; i < Frustum::SFRUSTUM_PLANECOUNT; ++i)
  {
    const Plane &plane = frustum.getPlane(i);
    float fDistance = s.centerAbovePlane(plane); // plane.DotNormal(s.m_v) - plane.GetDistance()
    if (fDistance < s.getRadius())
    {
      // at least part of sphere is outside-- reflect velocity
      const vector3 &n = plane.getNormal();
      float vperp = v.dot(n);
      if (vperp < 0)
      {
        normals.push_back(n);
      }
    }
  }

  int normalCount = normals.size();
  if (normalCount > 0)
  {
    vector3 naverage(0.0f, 0.0f, 0.0f);
    for (const auto &n : normals)
    {
      naverage += n;
    }

    if (normalCount > 1)
    {
      naverage.div((float)normalCount);
      constexpr float eps = 1.0e-4f;
      if (naverage.squareMag() > eps)
      {
        naverage.normalize();
      }
      else
      {
        normalCount = 0;
      }
    }

    if (normalCount > 0)
    {
      float vperp = v.dot(naverage);
      if (vperp < 0)
      {
        vector3::addScaled(v, v, naverage, -2.0f * vperp);
      }
      else
      {
        Serial.printf("Reflection cancelled: velocity not impacting average normal.");
      }
    }
  }
}

void updateTransforms(float dt)
{
  for (int index = 0; index < activeInstCount; ++index)
  {
    auto &obj = instances[index];
    obj.updateTransform(dt);
    reflectAtFrustum(obj);
  }
}

void simpleRendererSetup()
{
  display.setup();

  TFT_eSPI *renderTarget = display.currentRenderTarget();
  int w = renderTarget->getViewportWidth();
  int h = renderTarget->getViewportHeight();
  SetScreenMatrix(w, h);

  updateFrustum();

	for (int i = 0; i < maxInstCount; ++i)
	{
		instances.emplace_back();
	}

  vector4 vCameraPos(0.0f, 0.0f, 0.0f);
  matrix4::lookAtRHView(mtxWtoV, vCameraPos, vCameraFocus, vector4(0.0f, 1.0f, 0.0f));

  scanModels();
  if (models.size() == 0)
  {
    display.fullScreenMessage("No models.\nOnly .obj\nsupported.");
    models.push_back("dummy1");
    models.push_back("dummy2");
  }

  nextModel();
  restartInstances();

#ifdef BUTTON_1
  // assign button 1 to advance to next model
  button1.setClickHandler([=](Button2 &btn) {
    nextModel();
    restartInstances();
  });
#endif
#ifdef BUTTON_2
  // assign button 1 to change number of models drawn
  button2.setClickHandler([=](Button2 &btn) {
    activeInstCount = (activeInstCount % maxInstCount) + 1;
    restartInstances();
  });
#endif
}


void drawFps(TFT_eSPI* target, float dt)
{
  target->setCursor(0, 0);

	float fps = (dt > 0.0f) ? 1.0f / dt : 0.0f;

#if 1
  // weighted average of fps over time:
  static float avfps = 0.0f;
  const float kAvFactor = 0.1f; // between 0 and 1.  closer to 0 does more averaging of reading over time.
  avfps = lerpf(avfps, fps, kAvFactor);
  fps = avfps;
#endif

	uint16_t color;
	if (fps < 10.0f) {
		color = TFT_RED;
	} else if (fps < 30.0f) {
		color = TFT_YELLOW;
	} else if (fps < 60.0f) {
		color = TFT_CYAN;
	} else {
		color = TFT_GREEN;
	}
	target->setTextColor(color);
	target->printf("fps: %5.1f\n", fps);
}


void simpleRendererLoop(float dt)
{
#ifdef BUTTON_1
  button1.loop();
#endif
#ifdef BUTTON_2
  button2.loop();
#endif

  updateTransforms(dt);

  display.clearRenderTarget();

  TFT_eSPI *renderTarget = display.currentRenderTarget();
  drawScene(renderTarget);

  drawFps(renderTarget, dt);

  if (usingPlaceholder) {
    renderTarget->setTextColor(TFT_YELLOW);
    renderTarget->printf("No .obj found in data folder.  Rendering placeholder model.  Use 'Upload Filesystem Image' to upload .obj files");
  }

  display.finishRender();
}
