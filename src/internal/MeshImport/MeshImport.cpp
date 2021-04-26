#include "../MeshTypes.h"
#include "../faceMesh.h"
#include "meshImport.h"
#include "Tokenizer.h"

#include <ESP.h>

#include <FS.h>

#define HAVE_SPIFFS 1
#if HAVE_SPIFFS
#include <SPIFFS.h>
#endif

//#include <c_types.h>
#include <set>

#define DEBUG_CLASS Serial
static int8_t sDebugLevel = 0;

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR
#endif

using namespace stevesch;

class MeshImport
{
  //Tokenizer mLine;
  Tokenizer mWords;

  //static constexpr uint kLineBufLength = 255;
  //char lineBuf[kLineBufLength + 1];
  //void processLine(stevesch::FaceMesh& mesh, uint bytesRead);
  void processLine(stevesch::FaceMesh &mesh, Stream &f);

public:
  ICACHE_FLASH_ATTR MeshImport();

  bool importObj(stevesch::FaceMesh &mesh, Stream &f);
};

ICACHE_FLASH_ATTR MeshImport::MeshImport()
{
  //lineBuf[kLineBufLength] = '\0';

  //mLine.setDelimiters("\n\r");
  mWords.setDelimiters(" \t\n\r");
}

void ICACHE_FLASH_ATTR fixupIndex(int &index, int currentArraySize)
{
  if (index < 0)
  {
    index += currentArraySize; // input index is offset from end of current array.  since our internal arrays are 0-based, this is just (index + size)
  }
  else if (index > 0)
  {
    --index; // make 0-based
  }
  else
  {
    index = -1; // mark invalid?
  }
}

//void ICACHE_FLASH_ATTR MeshImport::processLine(stevesch::FaceMesh& mesh, uint bytesRead)
void ICACHE_FLASH_ATTR MeshImport::processLine(stevesch::FaceMesh &mesh, Stream &f)
{
  //DEBUG_CLASS.printf("Processing line <%s>\n", lineBuf);
  if (f.available())
  {
    //DEBUG_CLASS.printf("Processing line.\n");
    mWords.begin(f);
    if (mWords.nextToken())
    {
      //DEBUG_CLASS.printf("  token <%s>\n", mWords.getToken().c_str());
      const tokenstr_t token = mWords.getToken();
      if (token == "v")
      {
        // vertex
        float x, y, z;
        if (mWords.nextToken())
        {
          //DEBUG_CLASS.printf("  token <%s>\n", mWords.getToken().c_str());
          x = (float)atof(mWords.getToken().c_str());
          if (mWords.nextToken())
          {
            //DEBUG_CLASS.printf("  token <%s>\n", mWords.getToken().c_str());
            y = (float)atof(mWords.getToken().c_str());
            if (mWords.nextToken())
            {
              //DEBUG_CLASS.printf("  token <%s>\n", mWords.getToken().c_str());
              z = (float)atof(mWords.getToken().c_str());
              vector3 v(x, y, z);
              mesh.addPosition(v);
              //DEBUG_CLASS.println("  Adding vertex");
            }
          }
        }
      }
      else if (token == "f")
      {
        // commonly all verts are specified before faces, so
        // take this moment to compact the vert array
        if (mesh.faceCount() == 0)
        {
          mesh.compactMemory();
        }

        // face
        stevesch::indexBuffer_t &indices = mesh.refPositionIndices();
        index_t posIndex0 = indices.size();

        //PlanarFace face;
        IndexedFace face;
        face.iNormal = -1;
        face.iFirst = posIndex0;

        int i0;
        index_t index;

        while (mWords.nextToken())
        {
          //DEBUG_CLASS.printf("  token <%s>\n", mWords.getToken().c_str());
          i0 = atoi(mWords.getToken().c_str());
          fixupIndex(i0, mesh.positionCount());
          index = (index_t)i0;

          //face.iPosition.push_back(index);
          indices.push_back(index);
        }

        //size_t posCount = face.iPosition.size();
        size_t posCount = indices.size() - posIndex0;
        if (posCount > 2)
        {
          face.iCount = posCount;
          mesh.addFace(face);
        }
        else
        {
          if (posCount > 0)
          {
            auto i0 = indices.begin() + posIndex0;
            auto i1 = i0 + posCount;
            indices.erase(i0, i1);
          }
          //DEBUG_CLASS.println("  Ignoring invalid face");
        }
      }
      else if (token == "l")
      {
        // ignore line
        //DEBUG_CLASS.println("  Ignoring line tag");
      }
      else if (token == "vn")
      {
        // ignore vertex normal
        //DEBUG_CLASS.println("  Ignoring vertex normal tag");
      }
      else if (token == "o")
      {
        // new object
        //DEBUG_CLASS.println("  Ignoring object tag");
      }
      else if (token.startsWith("#"))
      {
        // ignore entire line
        //DEBUG_CLASS.println("  Ignoring comment tag");
      }
      else
      {
        if (sDebugLevel > 0)
        {
          DEBUG_CLASS.printf("Unrecognized token <%s>\n", token.c_str());
        }
      }
    }
    mWords.finishLine();
  }
}

#if USE_FACE_NORMALS
bool findGoodNormal(stevesch::vector3 &outNormal, const positionBuffer_t &positions,
                    const indexBuffer_t::const_iterator &indexBegin,
                    const indexBuffer_t::const_iterator &indexEnd)
{
  vector3 v0, v1, v2;
  vector3 e1, e2, ncontrib, n;

  //uint vc = indices.size();
  //uint i = 0;
  //uint j = 1;
  //uint k = 2;

  auto i = indexBegin;
  auto j = i + 1;
  auto k = j + 1;

  v0 = positions[*i];
  n.set(0.0f, 0.0f, 0.0f);
  do
  {
    v1 = positions[*j];
    v2 = positions[*k];
    vector3::sub(e1, v1, v0);
    vector3::sub(e2, v2, v0);
    vector3::cross(ncontrib, e1, e2);
    n += ncontrib;

    //i = j;
    j = k;
    ++k;
  } while (k != indexEnd);

  if (n.squareMag() > 1.0e-5f)
  {
    n.normalize();
    outNormal = n;
    return true;
  }

  outNormal.set(0.0f, 1.0f, 0.0f);
  return false;
}

//bool findGoodNormal(stevesch::vector3& outNormal, const positionBuffer_t& positions, const indexBuffer_t& indices)
//{
//	vector3 v0, v1, v2;
//	vector3 e1, e2, n;
//
//	uint vc = indices.size();
//	int i = 0;
//	int j = 1;
//	int k = 2;
//
//	do {
//		v0 = positions[indices[i]];
//		v1 = positions[indices[j]];
//		v2 = positions[indices[k]];
//		vector3::sub(e1, v1, v0);
//		vector3::sub(e2, v2, v0);
//		vector3::cross(n, e1, e2);
//		if (n.squareMag() > 1.0e-5f)
//		{
//			n.normalize();
//			outNormal = n;
//			return true;
//		}
//
//		i = j;
//		j = k;
//		++k;
//		if (k >= vc)
//		{
//			k = 0;
//		}
//
//	} while (i != 0);
//
//	outNormal.set(0.0f, 1.0f, 0.0f);
//	return false;
//}

void ICACHE_FLASH_ATTR indexFaceNormals(stevesch::FaceMesh &mesh)
{
  const positionBuffer_t &positions = mesh.positions();
  const indexBuffer_t &posIndices = mesh.getPositionIndices();
  vector3 faceNormal;
  const uint fc = mesh.faceCount();

  for (int iface = 0; iface < fc; ++iface)
  {
    //PlanarFace& face = mesh.refFace(iface);
    IndexedFace &face = mesh.refFace(iface);
    //if (!findGoodNormal(faceNormal, positions, face.iPosition))
    auto i0 = posIndices.begin() + face.iFirst;
    auto i1 = i0 + face.iCount;
    if (!findGoodNormal(faceNormal, positions, i0, i1))
    {
      if (sDebugLevel > 0)
      {
        DEBUG_CLASS.printf("WARNING: Using failsafe face normal for face %d (possible degenerate face)\n", iface);
      }
    }

    index_t in = mesh.findMatchingNormal(faceNormal);
    if ((index_t)(-1) == in)
    {
      in = mesh.addNormal(faceNormal);
    }
    // else
    // {
    // 	DEBUG_CLASS.printf("Reusing normal %d for face %d\n", in, iface);
    // }
    face.iNormal = in;
    //DEBUG_CLASS.printf("Face[%d] n=[%d] (%5.2f, %5.2f, %5.2f)\n", iface, in, mesh.getNormal(in).x, mesh.getNormal(in).y, mesh.getNormal(in).z);
  }
}
#endif

bool ICACHE_FLASH_ATTR MeshImport::importObj(stevesch::FaceMesh &mesh, Stream &f)
{
  int numLinesProcessed = 0;
  long t0 = micros();
  long lastYield = t0;
  constexpr long kForceYieldTime = 1000;
  while (f.available() > 0)
  {
    //uint bytesRead = f.readBytesUntil('\n', lineBuf, kLineBufLength);
    //lineBuf[bytesRead] = '\0';
    //processLine(mesh, bytesRead);
    processLine(mesh, f);

    ++numLinesProcessed;
    long now = micros();
    if ((now - lastYield) > kForceYieldTime)
    {
      if (sDebugLevel > 0)
      {
        DEBUG_CLASS.printf("  processed %d lines (%d verts, "
#if USE_FACE_NORMALS
                           "%d normals, "
#endif
                           "%d indices, %d faces) heap:%d t=%d\n",
                           numLinesProcessed,
                           mesh.positionCount(),
#if USE_FACE_NORMALS
                           mesh.normalCount(),
#endif
                           (uint)mesh.getPositionIndices().size(),
                           mesh.faceCount(), ESP.getFreeHeap(), (int)(now - t0));
      }
      //lastYield = now;
      yield();
      lastYield = micros();
    }
  }

  mesh.compactMemory();

#if USE_FACE_NORMALS
  indexFaceNormals(mesh);
  if (sDebugLevel > 0)
  {
    DEBUG_CLASS.printf("Indexed %d face normals\n", mesh.normalCount());
  }
#endif

  mesh.compactMemory();

  bool bSuccess = true;
  return bSuccess;
}

namespace stevesch
{

  int8_t meshImportDebugLevel(int8_t level)
  {
    if (level >= 0)
    {
      sDebugLevel = level;
    }
    return sDebugLevel;
  }

  bool ICACHE_FLASH_ATTR importObj(stevesch::FaceMesh &mesh, const char *path)
  {
    bool bOk = false;

#if HAVE_SPIFFS
    SPIFFS.begin();
    //if (SPIFFS.exists(path))
    if (true)
    {
      MeshImport importer;
      File f = SPIFFS.open(path, "r");
      bOk = importer.importObj(mesh, f);
      DEBUG_CLASS.printf("Model load success: <%s>\n", (bOk ? "true" : "false"));
      DEBUG_CLASS.printf("vertex count: %d\n", mesh.positionCount());
      DEBUG_CLASS.printf("face count: %d\n", mesh.faceCount());
    }
    else
    {
      DEBUG_CLASS.printf("File missing: <%s>\n", path);
    }
    SPIFFS.end();
    yield();
#endif

    return bOk;
  }

  bool ICACHE_FLASH_ATTR importObj(stevesch::FaceMesh &mesh, Stream &s)
  {
    bool bOk = false;

    MeshImport importer;
    bOk = importer.importObj(mesh, s);
    DEBUG_CLASS.printf("Model load success: <%s>\n", (bOk ? "true" : "false"));
    DEBUG_CLASS.printf("vertex count: %d\n", mesh.positionCount());
    DEBUG_CLASS.printf("face count: %d\n", mesh.faceCount());

    return bOk;
  }

}
