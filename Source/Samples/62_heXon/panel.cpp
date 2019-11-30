#include "effectmaster.h"
#include "razor.h"
#include "pilot.h"
#include "player.h"

#include "panel.h"

void Panel::RegisterObject(Context* context)
{
    context->RegisterFactory<Panel>();
}

Panel::Panel(Context* context) : LogicComponent(context),
    currentInfoNodeIndex_{0},
    sinceInfoChange_{0.0f},
    active_{false}
{
}

void Panel::OnNodeSet(Node* node)
{ (void)node;
}

void Panel::Initialize(int colorSet)
{
    colorSet_ = colorSet;

    CreatePanels();

    FadeOutPanel(true);

    SubscribeToEvent(E_ENTERLOBBY, URHO3D_HANDLER(Panel, EnterLobby));
    SubscribeToEvent(E_ENTERPLAY,  URHO3D_HANDLER(Panel, EnterPlay));
}


void Panel::CreatePanels()
{
    panelScene_ = new Scene(context_);
    panelScene_->CreateComponent<Octree>();

    Zone* panelZone{ panelScene_->CreateComponent<Zone>() };
    panelZone->SetFogColor(Color::WHITE);
    panelZone->SetFogStart(5.5f);
    panelZone->SetFogEnd(10.0f);

    Node* panelCamNode{ panelScene_->CreateChild("Camera") };
    panelCamNode->SetPosition(Vector3::BACK * 5.0f);
    Camera* panelCam{ panelCamNode->CreateComponent<Camera>() };


    CreateInfos();

    panelTexture_ = new Texture2D(context_);
    panelTexture_->SetSize(1024, 1024, GRAPHICS->GetRGBFormat(), TEXTURE_RENDERTARGET);

//    RenderSurface* panelSurface{ panelTexture_->GetRenderSurface() };
//    SharedPtr<Viewport> panelViewport{ new Viewport(context_, panelScene_, panelCam) };
//    panelSurface->SetViewport(0, panelViewport);

    for (bool small : {true, false}) {

        Vector3 panelPos{};
        Quaternion panelRot{};

        switch(colorSet_) {
        case 1: {
            panelPos = small ? Vector3(-0.83787f, 0.4306f, 2.01268f)
                             : Vector3(-4.49769f, 0.0f, 2.59509f);

            panelRot = small ? Quaternion(-120.0f, Vector3::UP)
                             : Quaternion( -60.0f, Vector3::UP);
        } break;
        case 2: {
            panelPos = small ? Vector3(0.83787f, 0.4306f, 2.01268f)
                             : Vector3(4.49769f, 0.0f, 2.59509f);
            panelRot = small ? Quaternion(120.0f, Vector3::UP)
                             : Quaternion( 60.0f, Vector3::UP);
        } break;
        case 3: {
            panelPos = small ? Vector3(-3.46253f, 0.4306f, -0.47611f)
                             : Vector3(-4.49767f, 0.0f, -2.59507f);
            panelRot = small ? Quaternion( 120.0f, Vector3::UP)
                             : Quaternion(-120.0f, Vector3::UP);
        } break;
        case 4: {
            panelPos = small ? Vector3(3.46253f, 0.4306f, -0.47611f)
                             : Vector3(4.49767f, 0.0f, -2.59507f);
            panelRot = small ? Quaternion(-120.0f, Vector3::UP)
                             : Quaternion( 120.0f, Vector3::UP);
        } break;
        default: break;
        }

        Node* panelNode{ node_->CreateChild(small ? "SmallPanel" : "BigPanel") };
        panelNode->SetPosition(panelPos);
        panelNode->SetRotation(panelRot);
        panelNode->SetScale(small ? 1.0f : 3.472769409f);
        panelNode->SetEnabled(small);

        StaticModel* panelModel{ panelNode->CreateComponent<StaticModel>() };
        panelModel->SetModel(MC->GetModel("Panel"));

        SharedPtr<Material> panelMaterial{};
        if (small) {

            panelMaterial = MC->colorSets_[colorSet_].panelMaterial_->Clone();
            panelMaterial->SetTexture(TU_EMISSIVE, panelTexture_);

            smallPanelNode_ = panelNode;

            panelTriggerNode_ = node_->CreateChild("PanelTrigger");
            panelTriggerNode_->SetPosition(panelPos);
            panelTriggerNode_->CreateComponent<RigidBody>()->SetTrigger(true);
            CollisionShape* panelTrigger{ panelTriggerNode_->CreateComponent<CollisionShape>() };
            panelTrigger->SetBox(Vector3(0.7f, 0.9f, 1.23f));

            SubscribeToEvent(panelTriggerNode_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Panel, ActivatePanel));
            SubscribeToEvent(panelTriggerNode_, E_NODECOLLISIONEND, URHO3D_HANDLER(Panel, DeactivatePanel));

        } else {

            panelMaterial = MC->colorSets_[colorSet_].addMaterial_->Clone();
            panelMaterial->SetTexture(TU_DIFFUSE, panelTexture_);

            bigPanelNode_ = panelNode;
        }

        panelModel->SetMaterial(panelMaterial);
    }
}
void Panel::CreateInfos()
{
    Node* infos{ panelScene_->CreateChild("Infos") };
    infos->Rotate(Quaternion(180.0f * ((colorSet_ + 1) % 2), Vector3::FORWARD));

    //Create Apple info
    Node* appleInfo{ infos->CreateChild("AppleInfo") };
    infoNodes_.Push(appleInfo);

    Node* apple{ appleInfo->CreateChild("PanelApple") };
    apple->SetPosition(Vector3::RIGHT + Vector3::FORWARD);
    apple->Rotate(Quaternion(180.0f * ((colorSet_ + 1) % 2), Vector3::FORWARD));
    apple->SetScale(0.34f);
    apple->CreateComponent<StaticModel>()->SetModel(MC->GetModel("Apple"));

    Node* appleEqualsNode{ appleInfo->CreateChild("SpireEquals") };
    appleEqualsNode->Rotate(Quaternion(90.0f, Vector3::LEFT));
    appleEqualsNode->Rotate(Quaternion(180.0f, Vector3::UP));
    appleEqualsNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("="));

    Node* appleScoreNode{ appleInfo->CreateChild("AppleScore") };
    appleScoreNode->SetPosition(Vector3::LEFT * 0.75f);
    appleScoreNode->Rotate(Quaternion(90.0f, Vector3::LEFT));
    appleScoreNode->Rotate(Quaternion(180.0f, Vector3::UP));
    appleScoreNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("2"));
    Node* appleThreeNode{ appleScoreNode->CreateChild("3") };
    appleThreeNode->SetPosition(Vector3::RIGHT * 0.5f);
    appleThreeNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("3"));

    //Create Heart info
    Node* heartInfo{ infos->CreateChild("HeartInfo") };
    infoNodes_.Push(heartInfo);

    Node* heart{ heartInfo->CreateChild("PanelHeart") };
    heart->SetPosition(Vector3::RIGHT + Vector3::FORWARD);
    heart->Rotate(Quaternion(180.0f * ((colorSet_ + 1) % 2), Vector3::FORWARD));
    heart->SetScale(0.34f);
    heart->CreateComponent<StaticModel>()->SetModel(MC->GetModel("Heart"));

    Node* heartEqualsNode{ heartInfo->CreateChild("HeartEquals") };
    heartEqualsNode->Rotate(Quaternion(90.0f, Vector3::LEFT));
    heartEqualsNode->Rotate(Quaternion(180.0f, Vector3::UP));
    heartEqualsNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("="));

    Node* heartScoreNode{ heartInfo->CreateChild("HeartScore") };
    heartScoreNode->SetPosition(Vector3::LEFT);
    heartScoreNode->Rotate(Quaternion(90.0f, Vector3::LEFT));
    heartScoreNode->Rotate(Quaternion(180.0f, Vector3::UP));
    heartScoreNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("0"));

    //Create Razor info
    Node* razorInfo{ infos->CreateChild("RazorInfo") };
    infoNodes_.Push(razorInfo);

    Node* razor{ razorInfo->CreateChild("PanelRazor") };
    razor->SetPosition(Vector3::RIGHT + Vector3::FORWARD);
    razor->SetScale(0.34f);
    razor->CreateComponent<StaticModel>()->SetModel(MC->GetModel("Core"));
    razor->CreateChild("RazorTop")->CreateComponent<StaticModel>()->SetModel(MC->GetModel("RazorHalf"));
    Node* razorBottomNode{ razor->CreateChild("RazorBottom") };
    razorBottomNode->Rotate(Quaternion(180.0f, Vector3::RIGHT));
    razorBottomNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("RazorHalf"));

    Node* razorEqualsNode{ razorInfo->CreateChild("RazorEquals") };
    razorEqualsNode->Rotate(Quaternion(90.0f, Vector3::LEFT));
    razorEqualsNode->Rotate(Quaternion(180.0f, Vector3::UP));
    razorEqualsNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("="));

    Node* razorScoreNode{ razorInfo->CreateChild("RazorScore") };
    razorScoreNode->SetPosition(Vector3::LEFT);
    razorScoreNode->Rotate(Quaternion(90.0f, Vector3::LEFT));
    razorScoreNode->Rotate(Quaternion(180.0f, Vector3::UP));
    razorScoreNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("5"));

    //Create Spire info
    Node* spireInfo{ infos->CreateChild("SpireInfo") };
    infoNodes_.Push(spireInfo);

    Node* spire{ spireInfo->CreateChild("PanelSpire") };
    spire->SetPosition(Vector3::RIGHT + Vector3::FORWARD);
    spire->Rotate(Quaternion(180.0f * ((colorSet_ + 1) % 2), Vector3::FORWARD));
    spire->SetScale(0.34f);
    spire->CreateComponent<StaticModel>()->SetModel(MC->GetModel("Core"));
    spire->CreateChild("SpireTop")->CreateComponent<StaticModel>()->SetModel(MC->GetModel("SpireTop"));
    spire->CreateChild("SpireBottom")->CreateComponent<StaticModel>()->SetModel(MC->GetModel("SpireBottom"));

    Node* spireEqualsNode{ spireInfo->CreateChild("SpireEquals") };
    spireEqualsNode->Rotate(Quaternion(90.0f, Vector3::LEFT));
    spireEqualsNode->Rotate(Quaternion(180.0f, Vector3::UP));
    spireEqualsNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("="));

    Node* spireScoreNode{ spireInfo->CreateChild("SpireScore") };
    spireScoreNode->SetPosition(Vector3::LEFT * 0.75f);
    spireScoreNode->Rotate(Quaternion(90.0f, Vector3::LEFT));
    spireScoreNode->Rotate(Quaternion(180.0f, Vector3::UP));
    spireScoreNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("1"));
    Node* spireZeroNode{ spireScoreNode->CreateChild("0") };
    spireZeroNode->SetPosition(Vector3::RIGHT * 0.5f);
    spireZeroNode->CreateComponent<StaticModel>()->SetModel(MC->GetModel("0"));

    //Set first info as current
    SetCurrentInfoNode(infoNodes_.At(currentInfoNodeIndex_));
}

void Panel::Update(float timeStep)
{
    panelScene_->GetChild("RazorTop", true)->Rotate(Quaternion(timeStep * 42.0f, Vector3::UP));
    panelScene_->GetChild("RazorBottom", true)->Rotate(Quaternion(timeStep * 23.0f, Vector3::UP));

    panelScene_->GetChild("SpireTop", true)->Rotate(Quaternion(timeStep * 42.0f, Vector3::UP));
    panelScene_->GetChild("SpireBottom", true)->Rotate(Quaternion(timeStep * 23.0f, Vector3::DOWN));

    panelScene_->GetChild("PanelApple", true)->Rotate(Quaternion(timeStep * 55.0f, Vector3::UP));
    panelScene_->GetChild("PanelHeart", true)->Rotate(Quaternion(timeStep * 55.0f, Vector3::UP));

    if (active_) {

        sinceInfoChange_ += timeStep;
        if (sinceInfoChange_ > 5.0f) {
            sinceInfoChange_ = 0.0f;

            ++currentInfoNodeIndex_;
            if (currentInfoNodeIndex_ == infoNodes_.Size())
                currentInfoNodeIndex_ = 0;

            SetCurrentInfoNode(infoNodes_.At(currentInfoNodeIndex_));
        }
    }
}
void Panel::SetCurrentInfoNode(Node* infoNode)
{
    currentInfoNode_ = infoNode;
    for (Node* n : infoNodes_)
        GetSubsystem<EffectMaster>()->TranslateTo(n, Vector3::FORWARD * 13.0f * (n != currentInfoNode_), 0.42f);
}

void Panel::EnterLobby(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

//    smallPanelNode_->SetEnabled(true);
//    bigPanelNode_->SetEnabled(true);
//    panelTriggerNode_->SetEnabled(true);
    node_->SetEnabledRecursive(true);
    SetUpdateEventMask(USE_UPDATE);
}
void Panel::EnterPlay(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    node_->SetEnabledRecursive(false);
    SetUpdateEventMask(USE_NO_EVENT);
}

void Panel::ActivatePanel(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    Node* otherNode{ static_cast<Node*>(eventData[NodeCollisionStart::P_OTHERNODE].GetPtr()) };

    if (Pilot* pilot = otherNode->GetComponent<Pilot>()) {
        int pilotColorSet{};
        Player::takenColorSets_.TryGetValue(pilot->GetPlayerId(), pilotColorSet);

        if (IsOwner(pilot->GetPlayerId()) || (!HasOwner() && !pilotColorSet)) {

            FadeInPanel();
            active_ = true;
        }
    }
}
void Panel::FadeInPanel()
{
    GetSubsystem<EffectMaster>()->FadeTo(bigPanelNode_->GetComponent<StaticModel>()->GetMaterial(),
                                         MC->colorSets_[colorSet_].addMaterial_->GetShaderParameter("MatDiffColor").GetColor(),
                                         0.23f, 0.1f);
    GetSubsystem<EffectMaster>()->FadeTo(smallPanelNode_->GetComponent<StaticModel>()->GetMaterial(),
                                         MC->colorSets_[colorSet_].glowMaterial_->GetShaderParameter("MatEmissiveColor").GetColor(),
                                         0.23f, 0.0f, "MatEmissiveColor");
}
void Panel::DeactivatePanel(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    Node* otherNode{ static_cast<Node*>(eventData[NodeCollisionStart::P_OTHERNODE].GetPtr()) };

    if (Pilot* pilot = otherNode->GetComponent<Pilot>()) {
        int pilotColorSet{};
        Player::takenColorSets_.TryGetValue(pilot->GetPlayerId(), pilotColorSet);

        PODVector<RigidBody*> bodies{};
        panelTriggerNode_->GetComponent<RigidBody>()->GetCollidingBodies(bodies);
        for (RigidBody* body : bodies) {

            if (Pilot* otherPilot = body->GetNode()->GetComponent<Pilot>()) {
                int otherPilotColorSet{};

                if (otherPilot == pilot || Player::takenColorSets_.TryGetValue(otherPilot->GetPlayerId(), otherPilotColorSet))
                    bodies.Remove(body);

            } else {

                bodies.Remove(body);
            }
        }

        if (IsOwner(pilot->GetPlayerId()) || (!HasOwner() && !bodies.Size())) {

            FadeOutPanel();
            active_ = false;
        }
    }
}
void Panel::FadeOutPanel(bool immediate)
{
    GetSubsystem<EffectMaster>()->FadeTo(bigPanelNode_->GetComponent<StaticModel>()->GetMaterial(),
                                         Color::BLACK,
                                         0.23f * !immediate, 0.1f * !immediate);
    GetSubsystem<EffectMaster>()->FadeTo(smallPanelNode_->GetComponent<StaticModel>()->GetMaterial(),
                                         Color::BLACK,
                                         0.23f * !immediate, 0.1f * !immediate, "MatEmissiveColor");
}

bool Panel::IsOwner(int playerId)
{
    for (int p : Player::takenColorSets_.Keys()) {
        if (playerId == p && Player::takenColorSets_[p] == colorSet_)
        {
            return true;
        }
    }
    return false;
}
bool Panel::HasOwner()
{
    if (Player::takenColorSets_.Values().Contains(colorSet_))
        return true;
    else
        return false;
}
