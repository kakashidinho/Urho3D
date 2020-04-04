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

#include "sceneobject.h"

#include "arena.h"
#include "spawnmaster.h"
#include "settings.h"
#include "bullet.h"
#include "brick.h"
#include "seeker.h"
#include "flash.h"
#include "mirage.h"
#include "hitfx.h"

SceneObject::SceneObject(Context* context):
    LogicComponent(context),
    blink_{true},
    big_{true}
{
}

void SceneObject::OnNodeSet(Node *node)
{ if (!node) return;

    AddSampleSource();
}

void SceneObject::Set(const Vector3 position)
{
    StopAllSound();
    node_->SetEnabledRecursive(true);
    node_->SetPosition(position);

    if (blink_)
        SubscribeToEvent(E_ENDFRAME, URHO3D_HANDLER(SceneObject, BlinkCheck));

    if (node_->HasComponent<Light>()) {

        node_->GetComponent<Light>()->SetEnabled(GetSubsystem<Settings>()->GetManyLights());
    }
}
void SceneObject::Set(const Vector3 position, const Quaternion rotation)
{
    node_->SetRotation(rotation);
    Set(position);
}

void SceneObject::Disable()
{
//    if (node_->HasComponent<Mirage>())
//        node_->GetComponent<Mirage>()
    node_->SetEnabledRecursive(false);

    if (blink_)
        UnsubscribeFromEvent(E_ENDFRAME);

    UnsubscribeFromEvent(E_NODECOLLISIONSTART);
}

void SceneObject::AddSampleSource()
{
    SoundSource3D* sampleSource3D{ node_->CreateComponent<SoundSource3D>() };
    sampleSource3D->SetDistanceAttenuation(42.0f, 256.0f, 2.0f);
    sampleSource3D->SetSoundType(SOUND_EFFECT);
    sampleSources3D_.Push(sampleSource3D);
}
void SceneObject::PlaySample(Sound* sample, const float gain, bool localized)
{
    if (MC->SamplePlayed(sample->GetNameHash().Value()))
        return;

    if (localized) {

        for (SoundSource3D* s : sampleSources3D_)
            if (!s->IsPlaying()){
                s->SetGain(gain);
                s->Play(sample);
                return;
            }

        AddSampleSource();
        PlaySample(sample, gain, localized);

    } else {

        MC->PlaySample(sample, gain);
//        for (SoundSource* s : sampleSources_)
//            if (!s->IsPlaying()){
//                s->SetGain(gain);
//                s->Play(sample);
//                return;
//            }
    }
}
void SceneObject::StopAllSound()
{
    for (SoundSource3D* s : sampleSources3D_)
        s->Stop();

//    for (SoundSource* s : sampleSources_)
//        s->Stop();
}
bool SceneObject::IsPlayingSound()
{
    for (SoundSource3D* s : sampleSources3D_)
        if (s->IsPlaying()) return true;
//    for (SoundSource* s : sampleSources_)
//        if (s->IsPlaying()) return true;
    return false;
}

void SceneObject::Blink(Vector3 newPosition)
{
    Vector3 oldPosition{ GetPosition() };
    node_->SetPosition(newPosition);

    Player* nearestPlayerA{ MC->GetNearestPlayer(oldPosition) };
    Player* nearestPlayerB{ MC->GetNearestPlayer(newPosition) };

    float distanceToNearestPlayer{};

    if (nearestPlayerA && nearestPlayerB) {
        distanceToNearestPlayer = Min(nearestPlayerA->GetPosition().DistanceToPoint(oldPosition),
                                      nearestPlayerB->GetPosition().DistanceToPoint(newPosition));
    } else {
        distanceToNearestPlayer = 23.0f;
    }

    float gain{ 0.042f };//Max(0.07f, 0.13f - distanceToNearestPlayer * 0.0023f) };

    SPAWN->Create<Flash>()->Set(oldPosition, 0.0f, big_);
    SPAWN->Create<Flash>()->Set(newPosition, gain, big_);
}

void SceneObject::BlinkCheck(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    if (MC->IsPaused())
        return;

    Vector3 position{ node_->GetPosition() };
    float radius{ ARENA_RADIUS };

    if (!MC->InsideHexagon(position, radius)) {


        if (node_->HasComponent<Bullet>()
         || node_->HasComponent<Seeker>()
         || node_->HasComponent<Brick>()){

            HitFX* hitFx{ GetSubsystem<SpawnMaster>()->Create<HitFX>() };
            hitFx->Set(position, 0, false);
            Disable();

        } else if (blink_){
            Vector3 newPosition{ position - 1.999f * radius * MC->GetHexant(position) };
            Blink(newPosition);
        }
    }
}

void SceneObject::Emerge(const float timeStep)
{
    if (!IsEmerged())
        node_->Translate(2.3f * Vector3::UP * timeStep *
                             (0.023f - node_->GetPosition().y_),
                             TS_WORLD);
}
