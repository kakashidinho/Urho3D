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

#ifndef SHIP_H
#define SHIP_H

#include <Urho3D/Urho3D.h>
#include "TailGenerator.h"

#include "controllable.h"

class Bullet;
class Muzzle;

class Ship : public Controllable
{
    URHO3D_OBJECT(Ship, LogicComponent);
public:
    Ship(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    virtual void Set(const Vector3 position, const Quaternion rotation);
    void Update(float timeStep) override;
    void FixedUpdate(float timeStep) override;
    void ApplyMovement(float timeStep);

    void HandleSetControlled() override;
    void EnterPlay(StringHash eventType, VariantMap &eventData);
    void EnterLobby(StringHash eventType, VariantMap &eventData);

    void Pickup(PickupType pickup);
    void PowerupWeapons();
    void PowerupShield();
    void SetHealth(float health);
    float GetHealth() const { return health_; }
    void Hit(float damage, bool melee);
    void Eject();

    void Think() override;
    int GetColorSet() const { return colorSet_; }
    GUI3D* gui3d_;
protected:
    void Blink(Vector3 newPosition) override;
private:
    bool initialized_;
    Vector3 initialPosition_;
    Quaternion initialRotation_;
    int colorSet_;

    const float initialHealth_;
    float health_;

    int weaponLevel_;
    int bulletAmount_;
    const float initialShotInterval_;
    float shotInterval_;
    float sinceLastShot_;

    int appleCount_;
    int heartCount_;

    Muzzle* muzzle_;
    ParticleEmitter* shineEmitter_;
    Vector<TailGenerator*> tailGens_;
    Node* shieldNode_;
    StaticModel* shieldModel_;
    SharedPtr<Material> shieldMaterial_;

    void RemoveTails();
    void CreateTails();
    void Shoot(Vector3 aim);
    void FireBullet(Vector3 direction);
    void MoveMuzzle();
    void PlayPickupSample(int pickupCount);
    void Explode();
    void SetTailsEnabled(bool enabled);
    void PickupChaoBall();
    void SetColors();
    Vector3 Sniff(float playerFactor, Vector3 &move, bool taste = false);
};

#endif // SHIP_H
