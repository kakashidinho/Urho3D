/* Edddy
// Copyright (C) 2019 LucKey Productions (luckeyproductions.nl)
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


#ifndef COINPUMP_H
#define COINPUMP_H

#include <Urho3D/Urho3D.h>
#include "enemy.h"

class CoinPump : public Enemy
{
    URHO3D_OBJECT(CoinPump, Enemy);
public:
    static void RegisterObject(Context* context);
    CoinPump(Context* context);

    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;
    void Hit(float damage, const int colorSet) override;

private:
    float sinceHit_;
};

#endif // COINPUMP_H
