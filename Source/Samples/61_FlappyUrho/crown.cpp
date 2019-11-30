#include "crown.h"


Crown::Crown(Context* context) : LogicComponent(context)
{
}

void Crown::RegisterObject(Context* context)
{
    context->RegisterFactory<Crown>();
}

void Crown::OnNodeSet(Node* node)
{ if (!node) return;

    node_->SetPosition(Vector3::RIGHT * 19.0f);

    node_->SetRotation(Quaternion(23.5f, Vector3::FORWARD));

    StaticModel* crownModel{ node_->CreateComponent<StaticModel>() };
    crownModel->SetModel(CACHE->GetResource<Model>("Models/Crown.mdl"));
    crownModel->SetCastShadows(true);
    crownModel->ApplyMaterialList();
}

void Crown::Update(float timeStep)
{
//    node_->SetEnabled(GLOBAL->GetHighscore() > 0);

    if (GLOBAL->GetScore() > GLOBAL->GetHighscore()){

        node_->SetPosition(node_->GetPosition().Lerp(CAMERA_DEFAULT_POS, Clamp(2.0f * timeStep, 0.0f, 1.0f)));
        node_->SetRotation(node_->GetRotation().Slerp(Quaternion(90.0f, Vector3::RIGHT), Clamp(3.0f * timeStep, 0.0f, 1.0f)));
        node_->Rotate(Quaternion(235.0f * timeStep, Vector3::UP), TS_LOCAL);

    } else {

        node_->Rotate(Quaternion(timeStep * 23.0f, Vector3::UP), TS_WORLD);
        node_->Rotate(Quaternion(timeStep * 23.0f, Vector3::UP), TS_LOCAL);

        Vector3 targetPos{ 2.3f + (10.0f * GLOBAL->GetHighscore() == 0) +
                    + 25.0f * (GLOBAL->GetHighscore() - GLOBAL->GetScore()) / (float)Max(GLOBAL->GetHighscore(), 1),
                    node_->GetScene()->GetChild("Urho")->GetPosition().y_ - node_->GetPosition().y_,
                    node_->GetPosition().z_
                         };
        node_->SetPosition(Vector3(0.01f * ( targetPos + 99.0f * node_->GetPosition() )));
    }
}

void Crown::Reset()
{
    node_->SetPosition(Vector3::RIGHT * 19.0f);
    node_->SetRotation(Quaternion(23.5f, Vector3::FORWARD));
}
