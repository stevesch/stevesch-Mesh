/**
 * @file simpleRenderer.h
 * @author Stephen Schlueter, github: stevesch
 * @brief Simple ESP wireframe 3D renderer
 * @version 0.1
 * @date 2021-05-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */
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
