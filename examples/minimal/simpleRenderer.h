#include <Arduino.h>

class TFT_eSPI;
namespace stevesch
{
  class FaceMesh;
  class matrix4;
}

void simpleRendererSetup();
void simpleRendererLoop(float dt);

void drawFaceMesh(TFT_eSPI *renderTarget, const stevesch::FaceMesh &mesh, const stevesch::matrix4 &mtxLtoW, uint16_t color);
void drawScene(TFT_eSPI *renderTarget);
void scanModels();

void nextModel();
void restartInstances();
