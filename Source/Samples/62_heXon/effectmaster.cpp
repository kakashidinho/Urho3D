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

#include "effectmaster.h"

using namespace Urho3D;

EffectMaster::EffectMaster(Context* context) : Object(context)
{
}

void EffectMaster::FadeTo(Material* material, Color color, float duration, float delay, String parameter)
{
    if (!duration && !delay) {
        material->SetShaderParameter(parameter, color);
        return;
    }

    Color startColor{ material->GetShaderParameter(parameter).GetColor() };
    ValueAnimation* fade{ new ValueAnimation(context_) };

    fade->SetKeyFrame(0.0f, startColor);
    if (delay)
        fade->SetKeyFrame(delay, startColor);
    fade->SetKeyFrame(delay + duration, color);


    material->SetShaderParameterAnimation(parameter, fade, WM_ONCE);
}

void EffectMaster::FadeTo(Light* light, float brightness, float duration)
{
    ValueAnimation* fade{new ValueAnimation(context_)};
    fade->SetKeyFrame(0.0f, light->GetBrightness());
    fade->SetKeyFrame(duration, brightness);
    light->SetAttributeAnimation("Brightness Multiplier", fade, WM_ONCE);
}

void EffectMaster::FadeTo(SoundSource* soundSource, float gain, float duration)
{
    ValueAnimation* fade{new ValueAnimation(context_)};
    fade->SetKeyFrame(0.0f, soundSource->GetGain());
    fade->SetKeyFrame(0.42f * duration, Lerp(soundSource->GetGain(), gain, 0.5f));
    fade->SetKeyFrame(duration, gain);
    soundSource->SetAttributeAnimation("Gain", fade, WM_ONCE);
}
void EffectMaster::FadeOut(SoundSource* soundSource, float duration)
{
    float lastGain{soundSource->GetGain()};

    ValueAnimation* fade{new ValueAnimation(context_)};
    fade->SetKeyFrame(0.0f, lastGain);
    fade->SetKeyFrame(0.2f * duration, 0.5f * lastGain);
    fade->SetKeyFrame(0.46f * duration, 0.1f * lastGain);
    fade->SetKeyFrame(duration, 0.0f);
    soundSource->SetAttributeAnimation("Gain", fade, WM_ONCE);
}

void EffectMaster::TransformTo(Node* node, Vector3 pos, Quaternion rot, float duration)
{
    TranslateTo(node, pos, duration);
    RotateTo(node, rot, duration);
}
void EffectMaster::TranslateTo(Node* node, Vector3 pos, float duration)
{
    ValueAnimation* posAnim{new ValueAnimation(context_)};
    posAnim->SetKeyFrame(0.0f, node->GetPosition());
    posAnim->SetKeyFrame(duration, pos);
    posAnim->SetInterpolationMethod(IM_SPLINE);
    node->SetAttributeAnimation("Position", posAnim, WM_ONCE);
}
void EffectMaster::RotateTo(Node* node, Quaternion rot, float duration)
{
    ValueAnimation* rotAnim{new ValueAnimation(context_)};
    rotAnim->SetKeyFrame(0.0f, node->GetRotation());
    rotAnim->SetKeyFrame(duration, rot);
    node->SetAttributeAnimation("Rotation", rotAnim, WM_ONCE);
}
void EffectMaster::ScaleTo(Node* node, Vector3 scale, float duration)
{
    ValueAnimation* scaleAnim{new ValueAnimation(context_)};
    scaleAnim->SetKeyFrame(0.0f, node->GetScale());
    scaleAnim->SetKeyFrame(duration, scale);
    node->SetAttributeAnimation("Scale", scaleAnim, WM_ONCE);
}

void EffectMaster::ArchTo(Node* node, Vector3 pos, Quaternion rot, float archHeight, float duration, float delay)
{
    ValueAnimation* posAnim{new ValueAnimation(context_)};
    posAnim->SetKeyFrame(0.0f, node->GetPosition());

    for (int i{0}; i < WAYPOINTS - 1; ++i){
        float t{static_cast<float>(i) / WAYPOINTS};
        float t2 = 0.5f * (t + (0.5f + 0.5f * pow(2.0f * (t - 0.5f), 3.0f)));
        posAnim->SetKeyFrame(delay + (duration * t2),
                             node->GetPosition().Lerp(pos, t) + archHeight * Vector3::UP * Arch(t));
    }
    posAnim->SetKeyFrame(delay + duration, pos);
    node->SetAttributeAnimation("Position", posAnim, WM_ONCE);

    ValueAnimation* rotAnim{new ValueAnimation(context_)};
    rotAnim->SetKeyFrame(0.0f, node->GetRotation());
    if (delay != 0.0f)
        rotAnim->SetKeyFrame(delay, node->GetRotation());
    rotAnim->SetKeyFrame(duration, rot);
    node->SetAttributeAnimation("Rotation", rotAnim, WM_ONCE);
}
