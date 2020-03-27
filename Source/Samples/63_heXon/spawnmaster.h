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

#ifndef SPAWNMASTER_H
#define SPAWNMASTER_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

#define CHAOINTERVAL Random(23.0f, 55.0f)


class SpawnMaster : public Object
{
    friend class MasterControl;
    URHO3D_OBJECT(SpawnMaster, Object);
public:
    SpawnMaster(Context* context);

    void Clear();
    Vector3 SpawnPoint(int fromEdge = 0);
    static Vector3 NearestGridPoint(Vector3 position);
    static Vector3 RandomGridPoint(int fromEdge = 0);

    void ChaoPickup() { sinceLastChaoPickup_ = 0.0f; chaoInterval_ = CHAOINTERVAL; }

    template <class T> T* Create(bool recycle = true)
    {
        T* created{ nullptr };

        if (recycle) {
            PODVector<Node*> correctType{};
            MC->scene_->GetChildrenWithComponent<T>(correctType);
            for (Node* n : correctType) {

                if (!n->IsEnabled()) {
                    created = n->GetComponent<T>();
                    break;
                }
            }
        }
        if(!created) {

            Node* spawnedNode{ MC->scene_->CreateChild(T::GetTypeStatic().ToString()) };
            created = spawnedNode->CreateComponent<T>();
            spawnedNode->SetEnabledRecursive(false);
        }

        return created;
    }

    template <class T> int CountActive()
    {
        int count{0};
        PODVector<Node*> result{};
        MC->scene_->GetChildrenWithComponent<T>(result);

        for (Node* r : result) {

            if (r->IsEnabled()) ++count;
        }
        return count;
    }

    void Prespawn();
    
    void SpawnPattern();
    void SpawnDeathFlower(Vector3 position);

private:
    void HandleUpdate(StringHash eventType, VariantMap &eventData);

    bool spawning_;

    float razorInterval_;
    float sinceRazorSpawn_;

    float spireInterval_;
    float sinceSpireSpawn_;

    float masonInterval_;
    float sinceMasonSpawn_;

    float bubbleInterval_;
    float sinceBubbleSpawn_;

    float sinceLastChaoPickup_;
    float chaoInterval_;

    Vector3 BubbleSpawnPoint();
    int MaxRazors();
    int MaxSpires();
    int MaxMasons();

    void Activate();
    void Deactivate();
    void Restart();
};

#endif // SPAWNMASTER_H
