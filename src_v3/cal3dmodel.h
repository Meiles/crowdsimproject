//The source files cal3dmodel.h and cal3dmodel.cpp both come (almost directly)
//from the cally-2.11.0 demo program. It's mainly just used to draw the
//meshes.
//
//----------------------------------------------------------------------------//
// model.h                                                                    //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MODEL_H
#define MODEL_H

#define CAL3D
//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

//#include "global.h"

//#include "Defines.h"

#ifdef CAL3D
#include "cal3d/cal3d.h"
#endif

#ifndef NOGUI
#include <GL/glut.h>
#else
#include <GL/gl.h>
#endif

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class Cal3dModel {
    // misc
  public:
    static const int STATE_IDLE;
    static const int STATE_FANCY;
    static const int STATE_MOTION;

    // member variables
  protected:
    int m_state;
#ifdef CAL3D
    CalCoreModel* m_calCoreModel;
    CalModel* m_calModel;
#endif
    int m_animationId[16];
    int m_animationCount;
    int m_meshId[32];
    int m_meshCount;
    GLuint m_textureId[32];
    int m_textureCount;
    float m_motionBlend[3];
    float m_renderScale;
    float m_lodLevel;
    std::string m_path;

    // constructors/destructor
  public:
    Cal3dModel();
    virtual ~Cal3dModel();

    // member functions
  public:
    void ExecuteAction(int action);
    float GetLodLevel();
    void GetMotionBlend(float* pMotionBlend);
    float GetRenderScale();
    int GetState();
    bool OnInit(const std::string& strFilename);
    void OnRender();
    void OnShutdown();
    void OnUpdate(float elapsedSeconds);
    void SetLodLevel(float lodLevel);
    void SetMotionBlend(float* pMotionBlend, float delay);
    void SetState(int state, float delay);
    void SetPath( const std::string& strPath );

    /* DEBUG-CODE
       struct
       {
       float x, y, z;
       } Sphere;
       void adjustSphere(float x, float y, float z) { Sphere.x += x; Sphere.y += y; Sphere.z += z; };
       */

  protected:
    GLuint LoadTexture(const std::string& strFilename);
    void RenderMesh(bool bWireframe, bool bLight);
    void RenderSkeleton();
    void RenderBoundingBox();
};

#endif

//----------------------------------------------------------------------------//
