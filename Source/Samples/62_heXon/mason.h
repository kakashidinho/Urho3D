/* heXon
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
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

#ifndef MASON_H
#define MASON_H

#include <Urho3D/Urho3D.h>

#include "enemy.h"

class Mason : public Enemy
{
    URHO3D_OBJECT(Mason, Enemy);
public:
    Mason(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;
    void Set(const Vector3 position) override;
private:
    Node* topNode_;
    Node* bottomNode_;

    SharedPtr<Material> blackMaterial_;
    SharedPtr<Material> glowMaterial_;

    int shots_;
    float sinceShot_;
    float shotInterval_;
    float spinInterval_;
    float toSpin_;
    float spun_;

    void Shoot();
    int MaxShots();
};

#endif // MASON_H
