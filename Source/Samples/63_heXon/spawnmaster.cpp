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

#include "spawnmaster.h"

#include "arena.h"
#include "tile.h"
#include "player.h"
#include "ship.h"
#include "chaoball.h"
#include "chaomine.h"
#include "chaozap.h"
#include "razor.h"
#include "spire.h"
#include "mason.h"
#include "baphomech.h"
#include "seeker.h"
#include "brick.h"
#include "flash.h"
#include "bubble.h"
#include "line.h"
#include "coin.h"
#include "coinpump.h"
#include "phaser.h"

SpawnMaster::SpawnMaster(Context* context):
    Object(context),
    spawning_{false},
    razorInterval_{2.0f},
    sinceRazorSpawn_{0.0f},
    spireInterval_{23.0f},
    sinceSpireSpawn_{0.0f},
    masonInterval_{123.0f},
    sinceMasonSpawn_{0.0f},
    bubbleInterval_{0.23f},
    sinceBubbleSpawn_{bubbleInterval_},
    sinceLastChaoPickup_{0.0f},
    chaoInterval_{CHAOINTERVAL}
{
}

void SpawnMaster::Prespawn()
{
    AUDIO->SetMasterGain(SOUND_EFFECT, 0.0f);
    for (int r{0}; r < 23; ++r) { Create<Razor>(false); }
    for (int s{0}; s < 7; ++s) { Create<Spire>(false); }
    for (int m{0}; m < 2; ++m) { Create<Mason>(false); }
    for (int m{0}; m < 8; ++m) { Create<ChaoMine>(false); }
    for (int s{0}; s < 13; ++s) { Create<Seeker>(false); }
    for (int s{0}; s < 13; ++s) { Create<Brick>(false); }
    for (int h{0}; h < 16; ++h) { Create<HitFX>(false); }
    for (int e{0}; e < 9; ++e) { Create<Explosion>(false); }
    for (int f{0}; f < 13; ++f) { Create<Flash>(false); };
    for (int b{0}; b < 42; ++b) { Create<Bubble>(false); }
    for (int l{0}; l < 2048; ++l) { Create<Line>(false); }
    for (int z{0}; z < 8; ++z) { Create<ChaoZap>(false); }
    AUDIO->SetMasterGain(SOUND_EFFECT, 1.0f);
}

void SpawnMaster::Activate()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SpawnMaster, HandleUpdate));
}
void SpawnMaster::Deactivate()
{
    UnsubscribeFromAllEvents();
}
void SpawnMaster::Clear()
{
    for (SharedPtr<Node> n : MC->scene_->GetChildren()) {
        for (SharedPtr<Component> c : n->GetComponents()) {

            if (c->IsInstanceOf<Enemy>()
             || c->IsInstanceOf<Effect>()
             || c->IsInstanceOf<Seeker>()
             || c->IsInstanceOf<Brick>()
             || c->IsInstanceOf<Coin>()
             || c->IsInstanceOf<Phaser>())
            {
                SceneObject* s{ static_cast<SceneObject*>(c.Get()) };
                s->Disable();
                break;
            }
        }
    }

    if (MC->chaoBall_)
        MC->chaoBall_->Disable();
}

void SpawnMaster::Restart()
{
    Clear();
    razorInterval_      = 2.0f;
    sinceRazorSpawn_    = 0.0f;
    spireInterval_      = 23.0f;
    sinceSpireSpawn_    = 0.0f;
    masonInterval_      = 123.0f;
    sinceMasonSpawn_    = 0.0f;

    sinceLastChaoPickup_ = 0.0f;
    chaoInterval_ = CHAOINTERVAL;

    Activate();
//    SpawnPattern();
    //    Create<Baphomech>()->Set(Vector3::ZERO);
//    Create<CoinPump>()->Set(Vector3::ZERO);
}

void SpawnMaster::SpawnDeathFlower(Vector3 position)
{
    unsigned int  radius { (unsigned int )(2 + 2 * Random(3)) };
    unsigned int petals{ (unsigned int )Random(7) };
    float farOut{ ARENA_RADIUS - radius * 2.0f };

    float typeFactor{ MC->SinceLastReset() };
    for (Ship* s : MC->GetComponentsInScene<Ship>()) {
        if (s->IsEnabled())
            typeFactor -= Max(0.0f, 10.0f - s->GetHealth());
    }
    int type{ Random(1 + (typeFactor > 100.0f)) };

    if (position.Length() > farOut)
        position = position.Normalized() * farOut;

    Vector3 spawnPosition{ NearestGridPoint(position) + Vector3::DOWN * 13.0f };

    if (type == 0)
        Create<Spire>()->Set(spawnPosition);
    else
        Create<Mason>()->Set(spawnPosition);

    if      (petals < 2) petals = 2;
    else if (petals < 5) petals = 3;
    else                 petals = 6;


    const Vector3 initialOffset{ Quaternion(60.0f * Random(3), Vector3::UP) * Vector3::RIGHT };
    for (unsigned p{0}; p < petals; ++p) {

        Vector3 petalPos{ spawnPosition + Vector3::DOWN * 10.0f * (p + 1)
                    + Quaternion(60.0f * p * (6 / petals), Vector3::UP) * initialOffset * radius };

        if (type == 0)
            Create<Razor>()->Set(petalPos);
        else
            Create<Spire>()->Set(petalPos);
    }
}
Vector3 SpawnMaster::NearestGridPoint(Vector3 position)
{

    float tileWidth{ 2.0f };
    float rowSpacing{ 1.8f };
    Vector2 scalar{ 0.5f * tileWidth, rowSpacing };

    Vector2 flatPos{ position.x_, position.z_ };
    IntVector2 unified{ VectorFloorToInt(flatPos / scalar) };
    bool flip{ (Abs(unified.x_) % 2) == (Abs(unified.y_) % 2)};

    Vector2 normal{ scalar.Normalized() };
    Vector2 local{ flatPos - scalar * Vector2(unified) };

    if (flip)
        local.x_ = scalar.x_ - local.x_;

    if (local.DotProduct(normal) > (0.5f * scalar.Length())) {

        ++unified.y_;
        if (!flip)
            ++unified.x_;

    } else if (flip) {

        ++unified.x_;
    }


    flatPos = scalar * Vector2(unified);

    return Vector3{flatPos.x_, 0.0f, flatPos.y_ + 0.5f * rowSpacing};
}
void SpawnMaster::SpawnPattern()
{
    SpawnDeathFlower(Vector3::ZERO);
}

Vector3 SpawnMaster::SpawnPoint(int fromEdge)
{
    fromEdge *= 2;
    Vector3 originOffset{ Vector3::UP * 5.0f };
    auto origin { [fromEdge, originOffset](){ return RandomGridPoint(fromEdge) + originOffset; } };

    PhysicsRaycastResult hitResult{};
    Ray tileRay{ origin(), Vector3::DOWN };

    int attempts{ 5 };
    bool staticObject{ false };

    while (MC->PhysicsRayCastSingle(hitResult, tileRay, 34.0f) && (attempts > 0 || staticObject)) {

        staticObject = hitResult.body_->GetLinearFactor().Length() < 1.0e-9;
        --attempts;

        tileRay.origin_ = origin();
    }

    return Vector3::DOWN * 23.0f + tileRay.origin_;
}
Vector3 SpawnMaster::RandomGridPoint(int fromEdge)
{
    return NearestGridPoint(LucKey::PolarPoint(Random(ARENA_RADIUS - fromEdge), Random(360.0f)));
}
void SpawnMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    if (!MC->scene_->IsUpdateEnabled())
        return;

    const float timeStep{ eventData[Update::P_TIMESTEP].GetFloat() };

    sinceRazorSpawn_ += timeStep;
    sinceSpireSpawn_ += timeStep;
    sinceMasonSpawn_ += timeStep;

    if ((sinceRazorSpawn_ > razorInterval_ || CountActive<Razor>() == 0) && CountActive<Razor>() < MaxRazors()) {

        Razor* razor{ Create<Razor>() };
        razor->Set(SpawnPoint());

        sinceRazorSpawn_ = 0.0f;
        razorInterval_ = (7.0f - CountActive<Ship>())
                * pow(0.95f, ((MC->SinceLastReset()) + 10.0f) / 10.0f);

    }
    if (sinceSpireSpawn_ > spireInterval_ && CountActive<Spire>() < MaxSpires()) {

        Spire* spire{ Create<Spire>() };
        spire->Set(SpawnPoint(2));

        sinceSpireSpawn_ = 0.0f;
        spireInterval_ = (23.0f - CountActive<Ship>() * 2)
                * pow(0.95f, ((MC->scene_->GetElapsedTime() - MC->world.lastReset) + 42.0f) / 42.0f);

    }
    if (sinceMasonSpawn_ > masonInterval_ && CountActive<Mason>() < MaxMasons()) {

        Mason* mason{ Create<Mason>() };
        mason->Set(SpawnPoint(3));

        sinceMasonSpawn_ = 0.0f;
        masonInterval_ = (123.0f - CountActive<Ship>() * 3)
                * pow(0.95f, ((MC->scene_->GetElapsedTime() - MC->world.lastReset) + 123.0f) / 123.0f);

    }

    if (!MC->chaoBall_->IsEnabled() && MC->GetGameState() == GS_PLAY) {
        if (sinceLastChaoPickup_ > chaoInterval_)
            MC->chaoBall_->Respawn();
        else sinceLastChaoPickup_ += timeStep;
    }


    sinceBubbleSpawn_ += timeStep;

    if (sinceBubbleSpawn_ > bubbleInterval_) {
        Create<Bubble>()->Set(BubbleSpawnPoint());
        sinceBubbleSpawn_ = 0.0f;
    }
}
Vector3 SpawnMaster::BubbleSpawnPoint()
{
    return Quaternion(( Random(5) - 2 ) * 60.0f, Vector3::UP) *
            (Vector3::FORWARD * 20.0f + Vector3::RIGHT * Random(-10.0f, 10.0f))
            + Vector3::DOWN * 23.0f;
}

int SpawnMaster::MaxRazors()
{
    return Clamp(static_cast<int>(23 * MC->SinceLastReset() * 0.0042f), CountActive<Ship>() * 2, 23);
}
int SpawnMaster::MaxSpires()
{
    return Clamp(static_cast<int>(7 * MC->SinceLastReset() * 0.0023f), CountActive<Ship>(), 7);
}
int SpawnMaster::MaxMasons()
{
    return Clamp(static_cast<int>(3 * MC->SinceLastReset() * 0.0013f), 1 + CountActive<Ship>() / 2, 3);
}
