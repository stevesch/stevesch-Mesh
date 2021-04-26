#ifndef STEVESCH_RENDER_MESHIMPORT_MESHIMPORT_H_
#define STEVESCH_RENDER_MESHIMPORT_MESHIMPORT_H_

class Stream;

namespace stevesch
{
  class FaceMesh;

  bool importObj(FaceMesh &mesh, const char *path);
  bool importObj(FaceMesh &mesh, Stream &s);
  int8_t meshImportDebugLevel(int8_t level = -1);
}

#endif
