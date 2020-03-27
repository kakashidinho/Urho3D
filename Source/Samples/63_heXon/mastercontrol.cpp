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

#include "mastercontrol.h"

#include <fstream>
#include "TailGenerator.h"
#include "mirage.h"

#include "hexocam.h"
#include "effectmaster.h"
#include "inputmaster.h"
#include "spawnmaster.h"
#include "settings.h"

#include "player.h"
#include "pilot.h"
#include "ship.h"

#include "apple.h"
#include "heart.h"
#include "chaoball.h"
#include "chaoflash.h"
#include "chaomine.h"
#include "chaozap.h"

#include "bullet.h"
#include "seeker.h"
#include "brick.h"

#include "razor.h"
#include "spire.h"
#include "mason.h"
#include "baphomech.h"

#include "effectinstance.h"
#include "soundeffect.h"
#include "flash.h"
#include "hitfx.h"
#include "explosion.h"
#include "bubble.h"
#include "line.h"
#include "coin.h"
#include "coinpump.h"
#include "muzzle.h"

#include "lobby.h"
#include "splatterpillar.h"
#include "door.h"
#include "highest.h"
#include "phaser.h"
#include "gui3d.h"
#include "panel.h"
#include "arena.h"
#include "tile.h"

#include "GameController.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl* MasterControl::instance_ = NULL;

MasterControl* MasterControl::GetInstance()
{
    return instance_;
}

MasterControl::MasterControl(Context *context):
    Application(context),
    aspectRatio_{},
    paused_{false},
    currentState_{GS_INTRO},
    sinceStateChange_{0.0f},
    sinceFrameRateReport_{0.0f},
    players_{}
{
    instance_ = this;
}

void MasterControl::Setup()
{
    SetRandomSeed(TIME->GetSystemTime());

    engineParameters_[EP_RESOURCE_PATHS] = "Data/heXon;Data;CoreData;";
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetProgramDir() + "heXon.log";
    engineParameters_[EP_FULL_SCREEN]   = false;
    engineParameters_[EP_WINDOW_WIDTH]  = 1280;
    engineParameters_[EP_WINDOW_HEIGHT] = 720;
    engineParameters_[EP_WINDOW_TITLE] = "heXon";
    engineParameters_[EP_WINDOW_ICON] = "icon.png";

   
    Settings* settings{ new Settings(context_) };
    context_->RegisterSubsystem(settings);
/*
    if (settings->Load()) {

        engineParameters_[EP_WINDOW_WIDTH] = settings->GetResolution().x_;
        engineParameters_[EP_WINDOW_HEIGHT] = settings->GetResolution().y_;
        engineParameters_[EP_FULL_SCREEN] = settings->GetFullscreen();
    }
 */
}

void MasterControl::Start()
{
    GetSubsystem<Engine>()->SetMaxFps(GetSubsystem<Graphics>()->GetRefreshRate()/2);

    CreateColorSets();

    TailGenerator::RegisterObject(context_);
    AnimatedBillboardSet::RegisterObject(context_);
    Mirage::RegisterObject(context_);

    heXoCam::RegisterObject(context_);
    Lobby::RegisterObject(context_);
    Door::RegisterObject(context_);
    SplatterPillar::RegisterObject(context_);
    Arena::RegisterObject(context_);
    Apple::RegisterObject(context_);
    Heart::RegisterObject(context_);
    ChaoBall::RegisterObject(context_);
    Tile::RegisterObject(context_);
    Highest::RegisterObject(context_);
    Ship::RegisterObject(context_);
    Pilot::RegisterObject(context_);
    Bullet::RegisterObject(context_);
    Muzzle::RegisterObject(context_);
    Phaser::RegisterObject(context_);
    GUI3D::RegisterObject(context_);
    Panel::RegisterObject(context_);

    ChaoFlash::RegisterObject(context_);
    ChaoMine::RegisterObject(context_);
    ChaoZap::RegisterObject(context_);
    Coin::RegisterObject(context_);
    CoinPump::RegisterObject(context_);

    Razor::RegisterObject(context_);
    Spire::RegisterObject(context_);
    Seeker::RegisterObject(context_);
    Mason::RegisterObject(context_);
    Brick::RegisterObject(context_);
    Baphomech::RegisterObject(context_);

    EffectInstance::RegisterObject(context_);
    SoundEffect::RegisterObject(context_);
    HitFX::RegisterObject(context_);
    Bubble::RegisterObject(context_);
    Flash::RegisterObject(context_);
    Explosion::RegisterObject(context_);
    Line::RegisterObject(context_);

//    context_->RegisterSubsystem(this);
    context_->RegisterSubsystem(new EffectMaster(context_));
    context_->RegisterSubsystem(new InputMaster(context_));
    context_->RegisterSubsystem(new SpawnMaster(context_));
    DebugHud* debugHud{ engine_->CreateDebugHud() };
//    context_->RegisterSubsystem(debugHud);
    debugHud->SetDefaultStyle(CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml"));
//    debugHud->ToggleAll();
//    debugHud->SetMode(DEBUGHUD_SHOW_PROFILER);

    if (GRAPHICS) {

        aspectRatio_ = static_cast<float>(GRAPHICS->GetWidth()) / GRAPHICS->GetHeight();

        // Precache shaders if possible
        if (!ENGINE->IsHeadless() && CACHE->Exists("Shaders/Shaders.xml")) {

            GRAPHICS->PrecacheShaders(*CACHE->GetFile("Shaders/Shaders.xml"));
        }

        CreateUI();

//        GRAPHICS->BeginDumpShaders("Resources/Shaders/LatestShaders.xml");
    }


    CreateScene();
    PlaySample(GetSample("Welcome"), 0.9f);

    menuMusic_ = GetMusic("Modanung - BulletProof MagiRex");
    gameMusic_ = GetMusic("Alien Chaos - Disorder");
    Node* musicNode{ scene_->CreateChild("Music") };
    musicSource_ = musicNode->CreateComponent<SoundSource>();
    musicSource_->SetSoundType(SOUND_MUSIC);

//        GetSubsystem<Audio>()->Stop(); ///////////////////////////////////////////////////////////////////////
//    AUDIO->SetMasterGain(SOUND_MASTER, 1.0);
//    AUDIO->SetMasterGain(SOUND_EFFECT, 1.0f);

    SetGameState(GS_LOBBY);

    SubscribeToEvents();
    
    // TBD ELI
    context_->RegisterSubsystem(new GameController(context_));
}
void MasterControl::Stop()
{
    GetSubsystem<Settings>()->Save();

//    engine_->DumpResources(true);
//    GRAPHICS->EndDumpShaders();
}

void MasterControl::SubscribeToEvents()
{
    //Subscribe scene update event.
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MasterControl, HandleUpdate));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MasterControl, HandlePostRenderUpdate));
    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(MasterControl, HandleBeginFrame));
    SubscribeToEvent(E_JOYSTICKCONNECTED, URHO3D_HANDLER(MasterControl, HandleJoystickConnected));
}

void MasterControl::CreateUI()
{
    //Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will control the camera
    world.cursor.uiCursor = new Cursor(context_);
    world.cursor.uiCursor->SetVisible(false);
    GetSubsystem<UI>()->SetCursor(world.cursor.uiCursor);

    //Set starting position of the cursor at the rendering window center
    world.cursor.uiCursor->SetPosition(GRAPHICS->GetWidth() / 2, GRAPHICS->GetHeight() / 2);
}

Sound* MasterControl::GetMusic(String name) const
{
    Sound* song{ CACHE->GetResource<Sound>("Music/" + name + ".ogg") };
    song->SetLooped(true);
    return song;
}
Sound* MasterControl::GetSample(String name)
{
    unsigned nameHash{ name.ToHash() };

    if (samples_.Contains(nameHash))
        return samples_[nameHash].Get();

    Sound* sample{ CACHE->GetResource<Sound>("Samples/" + name + ".wav") };
    samples_[nameHash] = sample;
    playedSamples_.Insert(nameHash);

    return sample;
}
bool MasterControl::SamplePlayed(unsigned nameHash) const
{
    return playedSamples_.Contains(nameHash);
}
void MasterControl::PlaySample(Sound* sample, const float gain)
{
    if (SamplePlayed(sample->GetNameHash().Value()))
        return;

    for (SoundSource* s : innerEar_)

        if (!s->IsPlaying()) {

            s->SetFrequency(44080);
            s->SetGain(gain);
            s->Play(sample);
            return;
        }
}
void MasterControl::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    playedSamples_.Clear();
}
void MasterControl::CreateColorSets()
{
    for (int c : { 0, 1, 2, 3, 4 }) {

        ColorSet set{};

        switch (c){
        case 0: set.colors_.first_ = Color(0.23f, 0.5f, 1.0f);
                set.colors_.second_ = Color(0.05f, 0.05f, 0.05f);
            break;
        case 1: set.colors_.first_ = Color(0.38f, 0.42f, 0.01f);
                set.colors_.second_ = Color(0.1f, 0.3f, 0.05f);
            break;
        case 2: set.colors_.first_ = Color(0.5f, 0.32f, 0.01f);
                set.colors_.second_ = Color(0.16f, 0.0f, 0.38f);
            break;
        case 3: set.colors_.first_ = Color(0.45f, 0.1f, 0.42f);
                set.colors_.second_ = Color(0.0f, 0.27f, 0.42f);
            break;
        case 4: set.colors_.first_ = Color(0.42f, 0.023f, 0.01f);
                set.colors_.second_ = Color(0.34f, 0.34f, 0.34f);
            break;
        }

        set.glowMaterial_ = GetMaterial("Glow")->Clone();
        set.hullMaterial_ = GetMaterial("Hull")->Clone();
        set.bulletMaterial_ = GetMaterial("Bullet")->Clone();
        set.addMaterial_ = GetMaterial("Add")->Clone();
        set.panelMaterial_ = GetMaterial("Panel")->Clone();

        set.glowMaterial_->SetShaderParameter("MatEmissiveColor", set.colors_.first_ * 0.8f);
        set.glowMaterial_->SetShaderParameter("MatDiffColor", set.colors_.first_ * 0.23f);
        set.glowMaterial_->SetShaderParameter("MatSpecColor", (set.colors_.first_ + Color::WHITE) * 0.05f);

        set.hullMaterial_->SetShaderParameter("MatDiffColor", set.colors_.second_);
        set.hullMaterial_->SetShaderParameter("MatSpecColor", set.colors_.second_.Lerp(Color::WHITE, 0.13f));

        set.bulletMaterial_->SetShaderParameter("MatDiffColor", set.colors_.first_);

        set.addMaterial_->SetShaderParameter("MatDiffColor", set.colors_.first_ * 1.13f);

        set.panelMaterial_->SetShaderParameter("MatEmissiveColor", set.colors_.first_ * 0.666f);
        set.panelMaterial_->SetShaderParameter("MatDiffColor", set.colors_.first_ * 0.23f);
        set.panelMaterial_->SetShaderParameter("MatSpecColor", (set.colors_.first_ + Color::WHITE) * 0.23f);

        SharedPtr<Material> flash{ GetMaterial("Flash")->Clone() };
        flash->SetShaderParameter("MatDiffColor", set.colors_.first_ * 1.23f);
        set.hitFx_ = CACHE->GetResource<ParticleEffect>("Particles/HitFX.xml")->Clone();
        set.hitFx_->SetMaterial(flash);

        colorSets_[c] = set;
    }
}

void MasterControl::HandleJoystickConnected(StringHash eventType, VariantMap& eventData)
{
    if (INPUT->GetNumJoysticks() > NumPlayers())
        AddPlayer();
}
void MasterControl::AddPlayer()
{
    if (NumPlayers() >= 4)
        return;

    int playerId{ 1 };

    Vector<int> takenIds{};

    for (SharedPtr<Player> player : players_)
        takenIds.Push(player.Get()->GetPlayerId());

    while (takenIds.Contains(playerId))
        ++playerId;

    Player* newPlayer{ new Player(playerId, context_) };
    players_.Push(SharedPtr<Player>(newPlayer));

    Pilot* pilot{ GetSubsystem<SpawnMaster>()->Create<Pilot>(GetGameState() == GS_LOBBY) };
    GetSubsystem<InputMaster>()->SetPlayerControl(playerId, pilot);
    pilot->Initialize();

    if (GetGameState() == GS_LOBBY)
        newPlayer->EnterLobby();
}

void MasterControl::RemoveAutoPilot()
{
    if (players_.Size() > 1 || NoHumans()) {
        for (Pilot* pilot : GetComponentsInScene<Pilot>()) {

            Player* player{ pilot->GetPlayer() };

            if (player && !player->IsHuman()) {

                pilot->LeaveLobby();
                return;
            }
        }
    }
}

void MasterControl::RemovePlayer(Player* player)
{
    if (player->gui3d_)
        player->gui3d_->SetScore(0);

    Player::takenColorSets_.Erase(player->GetPlayerId());
    InputMaster* inputMaster{ GetSubsystem<InputMaster>() };
    Controllable* controllable{ inputMaster->GetControllableByPlayer(player->GetPlayerId()) };
    controllable->ClearControl();
    inputMaster->SetPlayerControl(player->GetPlayerId(), nullptr);

    for (SharedPtr<Player> playerPtr : players_) {

        if (playerPtr.Get() == player)
            players_.Remove(playerPtr);
    }
}

void MasterControl::FillInnerEar()
{
    for (int i{0}; i < 42; ++i) {

        SoundSource* sampleSource{ scene_->CreateComponent<SoundSource>() };
        sampleSource->SetSoundType(SOUND_EFFECT);
        innerEar_.Push(sampleSource);
    }
}

void MasterControl::CreateScene()
{
    scene_ = new Scene(context_);
    scene_->SetTimeScale(1.23f);

    world.octree = scene_->CreateComponent<Octree>();
    world.octree->SetSize(BoundingBox(-42.0f * Vector3::ONE, 42.0f * Vector3::ONE), 8);
    physicsWorld_ = scene_->CreateComponent<PhysicsWorld>();
//    physicsWorld_->SetGravity(Vector3::ZERO);
//    physicsWorld_->SetMaxSubSteps(5);
//    physicsWorld_->SetFps(23);
    physicsWorld_->SetSplitImpulse(true);
    scene_->CreateComponent<DebugRenderer>();
    FillInnerEar();

    //Create a Zone component for fog control
    Node* zoneNode{ scene_->CreateChild("Zone") };
    Zone* zone{ zoneNode->CreateComponent<Zone>() };
    zone->SetBoundingBox(BoundingBox( Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 5.0f, 100.0f) ));
    zone->SetFogColor(Color(0.0f, 0.0f, 0.0f));
    zone->SetFogStart(60.0f);
    zone->SetFogEnd(62.3f);
    zone->SetHeightFog(true);
    zone->SetFogHeight(-10.0f);
    zone->SetFogHeightScale(0.23f);

    //Create cursor
    world.cursor.sceneCursor = scene_->CreateChild("Cursor");
    StaticModel* cursorObject{ world.cursor.sceneCursor->CreateComponent<StaticModel>() };
    cursorObject->SetModel(GetModel("Hexagon"));
    cursorObject->SetMaterial(GetMaterial("Glow"));
    world.cursor.sceneCursor->SetEnabled(false);

    //Create an solid black plane to hide everything beyond full fog
    world.voidNode = scene_->CreateChild("Void");
    world.voidNode->SetPosition(Vector3::DOWN * 10.0f);
    world.voidNode->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));
    StaticModel* planeObject{ world.voidNode->CreateComponent<StaticModel>() };
    planeObject->SetModel(GetModel("Plane"));
    planeObject->SetMaterial(GetMaterial("PitchBlack"));

    //Create camera
    if (GRAPHICS) {

        Node* cameraNode{ scene_->CreateChild("Camera", LOCAL) };
        world.camera = cameraNode->CreateComponent<heXoCam>();
    }

    //Create arena
    Node* arenaNode{ scene_->CreateChild("Arena", LOCAL) };
    arena_ = arenaNode->CreateComponent<Arena>();

    SpawnMaster* sm{ GetSubsystem<SpawnMaster>() };
    sm->Prespawn();

    //Construct lobby
    Node* lobbyNode{ scene_->CreateChild("Lobby", LOCAL) };
    lobby_ = lobbyNode->CreateComponent<Lobby>();
    //Create ships
    for (int s : { 3, 4, 2, 1 }) {

        GetSubsystem<SpawnMaster>()->Create<Ship>()
                ->Set(Quaternion(60.0f + ((s % 2) * 60.0f - (s / 2) * 180.0f), Vector3::UP) * Vector3(0.0f, 0.6f, 2.3f),
                      Quaternion(60.0f + ((s % 2) * 60.0f - (s / 2) * 180.0f), Vector3::UP));
    }

    apple_ = sm->Create<Apple>();
    heart_ = sm->Create<Heart>();
    chaoBall_ = sm->Create<ChaoBall>();
    chaoBall_->Disable();

    NavigationMesh* navMesh{ scene_->CreateComponent<NavigationMesh>() };
    navMesh->SetAgentRadius(0.34f);
    navMesh->SetPadding(Vector3::UP);
    navMesh->SetAgentMaxClimb(0.23f);
    navMesh->SetCellSize(0.05f);
    navMesh->SetTileSize(256);
    navMesh->Build();

    if (GetPlatform() != "Android" && GetPlatform() != "iOS")
    {
        for (unsigned p{1}; p <= Max(INPUT->GetNumJoysticks(), 1 * !engineParameters_[EP_HEADLESS].GetBool()); ++p) {

           AddPlayer();
        }
    }
}

void MasterControl::SetGameState(const GameState newState)
{
    if (newState != currentState_) {

        LeaveGameState();
        previousState_ = currentState_;
        currentState_ = newState;
        sinceStateChange_ = 0.0f;
        EnterGameState();
    }
}

void MasterControl::SetPaused(bool paused)
{
    paused_ = paused;
    scene_->SetUpdateEnabled(!paused);

    if (paused) {

        AUDIO->PauseSoundType(SOUND_MUSIC);
        GetSubsystem<Engine>()->SetMaxFps(0);

    } else {

        AUDIO->ResumeSoundType(SOUND_MUSIC);
        GetSubsystem<Engine>()->SetMaxFps(GetSubsystem<Graphics>()->GetRefreshRate());

    }
}
void MasterControl::LeaveGameState()
{
    switch (currentState_) {
    case GS_INTRO : {
    } break;
    case GS_LOBBY : {
    } break;
    case GS_PLAY : {
        GetSubsystem<SpawnMaster>()->Deactivate();
    }   break;
    case GS_DEAD : {
        world.camera->SetGreyScale(false);
        musicSource_->SetGain(musicSource_->GetGain() / 0.666f);

    } break;
    case GS_EDIT : break; //Disable EditMaster
    default: break;
    }
}
void MasterControl::EnterGameState()
{
    switch (currentState_) {
    case GS_INTRO : break;
    case GS_LOBBY : {
        SendEvent(E_ENTERLOBBY);

        GetSubsystem<SpawnMaster>()->Clear();

        musicSource_->Play(menuMusic_);

        apple_->Disable();
        heart_->Disable();
        chaoBall_->Disable();
    } break;
    case GS_PLAY : {
        SendEvent(E_ENTERPLAY);

        musicSource_->Play(gameMusic_);

        apple_->Respawn(true);
        heart_->Respawn(true);
        chaoBall_->Deactivate();

        world.lastReset = scene_->GetElapsedTime();
        GetSubsystem<SpawnMaster>()->Restart();
    } break;
    case GS_DEAD : {
        GetSubsystem<SpawnMaster>()->Deactivate();
        world.camera->SetGreyScale(true);
        musicSource_->SetGain(musicSource_->GetGain() * 0.666f);
    } break;
    default: break;
    }
}

void MasterControl::Eject()
{
    SetGameState(GS_LOBBY);
}

bool MasterControl::AllReady(bool onlyHuman)
{
    if (!players_.Size())
        return false;

    for (Controllable* c : GetSubsystem<InputMaster>()->GetControlled()) {

        if (c && c->IsInstanceOf<Pilot>()) {

                if ( onlyHuman && c->GetPlayer()->IsHuman() )
                    return false;
                else if (!onlyHuman)
                    return false;
        }
    }
    return true;
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    float timeStep{ eventData[Update::P_TIMESTEP].GetFloat() };

    //Output FPS
    /*
    secondsPerFrame_ *= 1.0f - timeStep;
    secondsPerFrame_ += timeStep * timeStep;
    sinceFrameRateReport_ += timeStep;
    if (sinceFrameRateReport_ >= 1.0f) {
        Log::Write(3, String(1.0f / secondsPerFrame_));
        sinceFrameRateReport_ = 0.0f;
    }
    */
    sinceStateChange_ += timeStep;
    UpdateCursor(timeStep);

    switch (currentState_) {
    case GS_LOBBY: {

        if (GetComponentsInScene<Door>().Front()->HidesAllPilots(false))
            Exit();

        if (AllReady(false))
            SetGameState(GS_PLAY);

    } break;
    case GS_PLAY: {
    } break;
    case GS_DEAD: {
        if (sinceStateChange_ > 5.0f && NoHumans())
            SetGameState(GS_LOBBY);
    }
    default: break;
    }
}

void MasterControl::UpdateCursor(const float timeStep)
{ (void)timeStep;/*
    world.cursor.sceneCursor->Rotate(Quaternion(0.0f,100.0f*timeStep,0.0f));
    //world.cursor.sceneCursor->SetScale((world.cursor.sceneCursor->GetWorldPosition() - world.camera->GetWorldPosition()).Length()*0.05f);
    if (CursorRayCast(250.0f, world.cursor.hitResults))
        for (RayQueryResult r : world.cursor.hitResults)
            if (r.node_->GetNameHash() == N_TILE)
                world.cursor.sceneCursor->SetWorldPosition(r.node_->GetPosition()+Vector3::UP);
                Vector3 camHitDifference = world.camera->rootNode_->GetWorldPosition() - world.cursor.hitResults[i].position_;
                camHitDifference /= world.camera->rootNode_->GetWorldPosition().y_ - world.voidNode->GetPosition().y_;
                camHitDifference *= world.camera->rootNode_->GetWorldPosition().y_;
                world.cursor.sceneCursor->SetWorldPosition(world.camera->rootNode_->GetWorldPosition()-camHitDifference);*/
}

bool MasterControl::CursorRayCast(const float maxDistance, PODVector<RayQueryResult> &hitResults)
{
    IntVector2 mousePos{ world.cursor.uiCursor->GetPosition() };
    Ray cameraRay{ world.camera->camera_->GetScreenRay((float)mousePos.x_ / GRAPHICS->GetWidth(),
                                                       (float)mousePos.y_ / GRAPHICS->GetHeight())
                 };
    RayOctreeQuery query{ hitResults, cameraRay, RAY_TRIANGLE, maxDistance, (unsigned char)DRAWABLE_GEOMETRY };
    scene_->GetComponent<Octree>()->Raycast(query);

    if (hitResults.Size())
        return true;
    else
        return false;
}

bool MasterControl::PhysicsRayCast(PODVector<PhysicsRaycastResult>& hitResults, const Ray ray,
                                   const float distance, const unsigned collisionMask)
{
    if (distance > 1.0e-9)
        physicsWorld_->Raycast(hitResults, ray, distance, collisionMask);

    return (hitResults.Size() > 0);
}
bool MasterControl::PhysicsRayCastSingle(PhysicsRaycastResult& hitResult, const Ray ray,
                                   const float distance, const unsigned collisionMask)
{
    if (distance > 1.0e-9)
        physicsWorld_->RaycastSingle(hitResult, ray, distance, collisionMask);

    return hitResult.body_;
}
bool MasterControl::PhysicsSphereCast(PODVector<RigidBody*> &hitResults, const Vector3 center,
                                      const float radius, const unsigned collisionMask)
{
    physicsWorld_->GetRigidBodies(hitResults, Sphere(center, radius), collisionMask);

    if (hitResults.Size())
        return true;
    else
        return false;
}

void MasterControl::Exit()
{
    engine_->Exit();
}

float MasterControl::Sine(const float freq, const float min, const float max, const float shift)
{
    float phase{ SinePhase(freq, shift) };
    float add{ 0.5f * (min + max) };

    return LucKey::Sine(phase) * 0.5f * (max - min) + add;
}
float MasterControl::Cosine(const float freq, const float min, const float max, const float shift)
{
    return Sine(freq, min, max, shift + 0.25f);
}
float MasterControl::SinePhase(float freq, float shift)
{
    return M_PI * 2.0f * (freq * scene_->GetElapsedTime() + shift);
}

bool MasterControl::InsideHexagon(Vector3 position, float radius) const
{
    Vector3 flatPosition{ position * Vector3(1.0f, 0.0f, 1.0f) };
    Vector3 hexant{ GetHexant(flatPosition) };

    float boundsCheck{ flatPosition.Length() * LucKey::Cosine(M_DEGTORAD * flatPosition.Angle(hexant)) };
    if (boundsCheck > radius)

        return false;
    else
        return true;
}
Vector3 MasterControl::GetHexant(Vector3 position) const
{
    position.y_ = 0.0f;
    int sides{ 6 };

    Vector3 hexant{ Vector3::FORWARD };

    for (int h{0}; h < sides; ++h) {

        Vector3 otherHexantNormal{ Quaternion(h * (360.0f / sides), Vector3::UP) * Vector3::FORWARD };
        hexant = position.Angle(otherHexantNormal) < position.Angle(hexant) ? otherHexantNormal : hexant;
    }

    return hexant;
}


Vector<SharedPtr<Player> > MasterControl::GetPlayers()
{
    return players_;
}
Player* MasterControl::GetPlayer(int playerId) const
{
    for (Player* p : players_) {

        if (p->GetPlayerId() == playerId){
            return p;
        }
    }
    return nullptr;
}
Player* MasterControl::GetPlayerByColorSet(int colorSet)
{
    for (Ship* s : GetComponentsInScene<Ship>()) {

        if (s->GetColorSet() == colorSet)
            return s->GetPlayer();
    }
    return nullptr;
}
Player* MasterControl::GetNearestPlayer(Vector3 pos)
{
    Player* nearest{};

    for (Player* p : players_) {
        if (p->IsAlive()) {

            if (!nearest) {
                nearest = p;
                continue;
            }

            Controllable* nearestControlled{ GetSubsystem<InputMaster>()->GetControllableByPlayer(nearest->GetPlayerId()) };
            Controllable* controlled{ GetSubsystem<InputMaster>()->GetControllableByPlayer(p->GetPlayerId()) };

            if (nearestControlled->GetPosition().DistanceToPoint(pos)
                     > controlled->GetPosition().DistanceToPoint(pos)) {

                nearest = p;
            }
        }
    }

    return nearest;
}
bool MasterControl::AllPlayersScoreZero(bool onlyHuman)
{
    for (Player* p : players_) {

        if ((p->IsHuman() || !onlyHuman) && p->GetScore() != 0)
            return false;
    }

    return true;
}

Ship* MasterControl::GetShipByColorSet(int colorSet_)
{
    for (Ship* s : GetComponentsInScene<Ship>()) {

        if (s->GetColorSet() == colorSet_)
            return s;
    }

    return nullptr;
}

Door *MasterControl::GetDoor()
{
    for (Door* d : GetComponentsInScene<Door>())
        return d;

    return nullptr;
}

bool MasterControl::NoHumans()
{
    for (Player* p : players_) {

        if (p->IsHuman())
            return false;
    }

    return true;
}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    return;

    physicsWorld_->DrawDebugGeometry(true);
    if (GetGameState() == GS_LOBBY)
        scene_->GetComponent<NavigationMesh>()->DrawDebugGeometry(false);
}

