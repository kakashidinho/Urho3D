#ifndef CAMERALOGIC_H
#define CAMERALOGIC_H

#include "global.h"

class FlappyCam : public LogicComponent
{
    URHO3D_OBJECT(FlappyCam, LogicComponent);

public:
    FlappyCam(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep);
    //    void OnNodeSet(Node* node);
    void OnNodeSet(Node* node);

private:
    RenderPath* effectRenderPath_;
};

#endif // CAMERALOGIC_H
