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

#ifndef MINE_H
#define MINE_H

#include <Urho3D/Urho3D.h>

#include "enemy.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class ChaoMine : public Enemy
{
    URHO3D_OBJECT(ChaoMine, Enemy);
public:
    ChaoMine(Context* context);
    static void RegisterObject(Context* context);
    void Set(const Vector3 position, int colorSet);
    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;
protected:
    Node* innerNode_;
    Node* outerNode_;
    StaticModel* innerModel_;
    StaticModel* outerModel_;
    float countDown_;

    void CheckHealth();
    void HandleNodeCollision(StringHash, VariantMap &eventData);
private:
    int colorSet_;
};

#endif // MINE_H
