#include "flappycam.h"

FlappyCam::FlappyCam(Context* context) :
    LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void FlappyCam::RegisterObject(Context* context)
{
    context->RegisterFactory<FlappyCam>();
}

void FlappyCam::OnNodeSet(Node *node)
{
    node_->SetPosition(CAMERA_DEFAULT_POS);

    Camera* camera{ node_->CreateComponent<Camera>() };
    Viewport* viewport{new Viewport(context_, GetScene(), camera)};
    RENDERER->SetViewport(0, viewport);

    effectRenderPath_ = viewport->GetRenderPath();
   // TBD ELI  effectRenderPath_->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath_->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
   // TBD ELI  effectRenderPath_->Append(CACHE->GetResource<XMLFile>("PostProcess/Distortion.xml"));
    effectRenderPath_->SetShaderParameter("BloomHDRThreshold", 0.8f);
    effectRenderPath_->SetShaderParameter("BloomHDRMix", Vector2(0.7f, 0.8f));
    effectRenderPath_->SetEnabled("BloomHDR", true);
}

void FlappyCam::Update(float timeStep)
{

    effectRenderPath_->SetShaderParameter("ElapsedTime", GetSubsystem<Time>()->GetElapsedTime() * GetScene()->GetTimeScale());
//    int wheel{INPUT->GetMouseMoveWheel()};
//    if (wheel != 0)
//    {
//        Vector3 pos{node_->GetPosition()};
//        pos.z_ += wheel * 4.0f;

//        if (pos.z_ > -5.0f)
//            pos.z_ = -5.0f;
        
//        node_->SetPosition(pos);
//    }

//    if (INPUT->GetMouseButtonPress(MOUSEB_RIGHT))
//        node_->SetPosition(CAMERA_DEFAULT_POS);
}
