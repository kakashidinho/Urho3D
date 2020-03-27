/* Quatter
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

#ifndef EFFECTMASTER_H
#define EFFECTMASTER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

#define WAYPOINTS 42

class EffectMaster : public Object
{
    URHO3D_OBJECT(EffectMaster, Object);
public:
    EffectMaster(Context* context);

    void FadeTo(Material* material, Color color, float duration = 0.23f, float delay = 0.0f, String parameter = "MatDiffColor");
    void FadeTo(Light* light, float brightness, float duration = 0.23f);
    void FadeTo(SoundSource* soundSource, float gain, float duration = 2.3f);

    void FadeOut(Material* material, float duration = 0.23f) {FadeTo(material, material->GetShaderParameter("MatDiffColor").GetColor() * 0.0f, duration); }
    void FadeOut(Light* light) { FadeTo(light, 0.0f); }
    void FadeOut(SoundSource* soundSource, float duration = 5.0f);

    void TranslateTo(Node* node, Vector3 pos, float duration);
    void RotateTo(Node* node, Quaternion rot, float duration);
    void ScaleTo(Node* node, Vector3 scale, float duration);
    void ScaleTo(Node* node, float scale, float duration) { ScaleTo(node, Vector3::ONE * scale, duration); }
    void TransformTo(Node* node, Vector3 pos, Quaternion rot, float duration);

    void ArchTo(Node* node, Vector3 pos, Quaternion rot, float archHeight, float duration, float delay = 0.0f);
    float Arch(float t) const noexcept { return 1.0f - pow(2.0f * (t-0.5f), 4.0f); }
};

#endif // EFFECTMASTER_H
