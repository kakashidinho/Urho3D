/* heXon
// Copyright (C) 2018 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "settings.h"
#include "effectmaster.h"

#include "hexocam.h"

void heXoCam::RegisterObject(Context *context)
{
    context->RegisterFactory<heXoCam>();
}

heXoCam::heXoCam(Context* context):
    LogicComponent(context)
{
}

void heXoCam::OnNodeSet(Node *node)
{ if (!node) return;

    /* Ready for VR :)
    Node* leftEye{ node_->CreateChild("Left Eye") };
    leftEye->SetPosition(Vector3::LEFT);
    stereoCam_.first_ = leftEye->CreateComponent<Camera>();
    Node* rightEye{ node_->CreateChild("Right Eye") };
    rightEye->SetPosition(Vector3::RIGHT);
    stereoCam_.second_ = rightEye->CreateComponent<Camera>();
    */

//    Node* mike{ GetScene()->CreateChild("Microphone") };
//    mike->SetPosition(Vector3(0.0f, 23.0f, -10.0f));
//    mike->LookAt(Vector3::ZERO);
    AUDIO->SetListener(node_->CreateComponent<SoundListener>());

    camera_ = node_->CreateComponent<Camera>();
    camera_->SetFarClip(128.0f);
    node_->SetPosition(Vector3(0.0f, 42.0f, -23.0f));
    node_->SetRotation(Quaternion(65.0f, 0.0f, 0.0f));

    SetupViewport();

    SubscribeToEvent(E_ENTERLOBBY, URHO3D_HANDLER(heXoCam, EnterLobby));
    SubscribeToEvent(E_ENTERPLAY,  URHO3D_HANDLER(heXoCam, EnterPlay));
}

void heXoCam::SetupViewport()
{
    SharedPtr<Viewport> viewport{ new Viewport(MC->GetContext(), MC->scene_, camera_) };
    viewport_ = viewport;

    //Add anti-asliasing, bloom and a greyscale effects
//    XMLFile* file{ new XMLFile(context_) };
//    file->LoadFile("RenderPaths/Deferred.xml");
//    viewport_->SetRenderPath(file);

    effectRenderPath_ = viewport_->GetRenderPath()->Clone();
    // TBD elix22 effectRenderPath_->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    // TBD elix22  effectRenderPath_->SetEnabled("FXAA3", GetSubsystem<Settings>()->GetAntiAliasing());
    effectRenderPath_->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
    effectRenderPath_->SetShaderParameter("BloomHDRThreshold", 0.42f);
    effectRenderPath_->SetShaderParameter("BloomHDRMix", Vector2(2.0f, 2.25f));
    effectRenderPath_->SetEnabled("BloomHDR", true);
    effectRenderPath_->Append(CACHE->GetResource<XMLFile>("PostProcess/GreyScale.xml"));
    SetGreyScale(false);
    viewport_->SetRenderPath(effectRenderPath_);
    RENDERER->SetViewport(0, viewport_);
}

void heXoCam::Update(float timeStep)
{
    node_->SetPosition(node_->GetPosition().Lerp(closeUp_ ?
                                     Vector3(0.0f, 13.0f, -6.55f):
                                     Vector3(0.0f, 43.0f, -24.0f),
                                                 Clamp(5.0f * timeStep, 0.0f, 1.0f)));
}

void heXoCam::SetGreyScale(const bool enabled)
{
    effectRenderPath_->SetEnabled("GreyScale", enabled);
}

void heXoCam::EnterLobby(StringHash eventType, VariantMap &eventData)
{
    closeUp_ = true;
    effectRenderPath_->SetShaderParameter("BloomHDRThreshold", 0.42f);
//    GetSubsystem<EffectMaster>()->TranslateTo(node_, Vector3(0.0f, 43.0f, -24.0f), 2.3f);
}
void heXoCam::EnterPlay(StringHash eventType, VariantMap &eventData)
{
    closeUp_ = false;
    effectRenderPath_->SetShaderParameter("BloomHDRThreshold", 0.28f);
//    GetSubsystem<EffectMaster>()->TranslateTo(node_, Vector3(0.0f, 43.0f, -24.0f), 2.3f);

}
