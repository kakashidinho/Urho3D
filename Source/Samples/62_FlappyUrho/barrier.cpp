#include "barrier.h"
#include "crown.h"

StaticModelGroup* Barrier::netGroup_{};

Barrier::Barrier(Context* context) :
    LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Barrier::RegisterObject(Context* context)
{
    context->RegisterFactory<Barrier>();
}

void Barrier::OnNodeSet(Node *node)
{ (void)node;

    GetNode()->SetRotation(Quaternion(Random(2) ? 180.0f : 0.0f,
                                      Random(2) ? 180.0f + Random(-5.0f, 5.0f) : 0.0f + Random(-5.0f, 5.0f) ,
                                      Random(2) ? 180.0f + Random(-5.0f, 5.0f) : 0.0f + Random(-5.0f, 5.0f) ));

    GetNode()->CreateComponent<RigidBody>();
    CollisionShape* shape{GetNode()->CreateComponent<CollisionShape>()};
    shape->SetShapeType(SHAPE_BOX);
    shape->SetSize(Vector3(1.0f, BAR_GAP, 7.8f));

    Node* netNode{GetNode()->CreateChild("Net")};

    if (!netGroup_) {
        netGroup_ = node_->GetScene()->CreateComponent<StaticModelGroup>();
        netGroup_->SetModel(CACHE->GetResource<Model>("Models/Net.mdl"));
        netGroup_->SetCastShadows(true);
        netGroup_->ApplyMaterialList();
    }
    netGroup_->AddInstanceNode(netNode);

    for (float y : {15.0f, -15.0f}){
        netNode->CreateComponent<RigidBody>();
        CollisionShape* shape{netNode->CreateComponent<CollisionShape>()};
        shape->SetShapeType(SHAPE_BOX);
        shape->SetSize(Vector3(0.23f, 30.0f, 64.0f));
        shape->SetPosition(Vector3(0.0f, y + Sign(y)*(BAR_GAP / 2), 0.0f));
    }
}

void Barrier::Update(float timeStep)
{
    if (GLOBAL->gameState_ != GS_PLAY)
        return;

    Vector3 pos{node_->GetPosition()};
    pos += Vector3::LEFT * timeStep * BAR_SPEED;

    if (pos.x_ < -BAR_OUTSIDE_X)
    {
        pos.x_ += NUM_BARRIERS * BAR_INTERVAL;
        pos.y_ = BAR_RANDOM_Y;

        node_->SetRotation(Quaternion(Random(2) ? 180.0f : 0.0f,
                                      Random(2) ? 180.0f + Random(-5.0f, 5.0f) : 0.0f + Random(-5.0f, 5.0f) ,
                                      Random(2) ? 180.0f + Random(-5.0f, 5.0f) : 0.0f + Random(-5.0f, 5.0f) ));
    }

    node_->SetPosition(pos);
}
