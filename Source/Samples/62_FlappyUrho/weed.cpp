#include "weed.h"


Weed::Weed(Context* context) : LogicComponent(context)
{
}

void Weed::RegisterObject(Context* context)
{
    context->RegisterFactory<Weed>();
}

void Weed::OnNodeSet(Node* node)
{ (void)node;

    if (!node_)
        return;

    AnimatedModel* animatedModel{ node_->CreateComponent<AnimatedModel>() };
    animatedModel->SetModel(CACHE->GetResource<Model>("Models/Weed.mdl"));
    animatedModel->SetMaterial(CACHE->GetResource<Material>("Materials/VCol.xml")->Clone());
    animatedModel->GetMaterial()->SetShaderParameter("MatDiffColor", Color(Random(0.8f, 1.0f), Random(0.8f, 1.0f), Random(0.8f, 1.0f)));
    animatedModel->SetCastShadows(false);

    AnimationController* animCtrl{ node_->CreateComponent<AnimationController>() };
    animCtrl->PlayExclusive("Models/Wave.ani", 1, true);
    animCtrl->SetAnimationTime(Random(animCtrl->GetLength("Models/Wave.ani")));
    animCtrl->SetSpeed("Models/Wave.ani", Random(0.42f, 0.666f));

}

void Weed::Update(float timeStep)
{
    if (GLOBAL->gameState_ == GS_DEAD)
        return;

    Vector3 pos{node_->GetPosition()};
    pos += Vector3::LEFT * timeStep * BAR_SPEED;

    if (pos.x_ < -BAR_OUTSIDE_X && !node_->GetComponent<AnimatedModel>()->IsInView())
    {
        pos.x_ += NUM_BARRIERS * BAR_INTERVAL + pos.z_;
        pos.y_ = WEED_RANDOM_Y;

        node_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        node_->SetScale(Vector3(Random(0.5f, 1.23f), Random(0.8f, 2.3f), Random(0.5f, 1.23f)));
    }

    node_->SetPosition(pos);
}





