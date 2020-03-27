/*
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

#ifndef LUCKEY_H
#define LUCKEY_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Container/HashBase.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/DecalSet.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Math/Plane.h>
#include <Urho3D/Math/Sphere.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Urho2D/TileMap2D.h>
#include <Urho3D/Urho2D/TmxFile2D.h>
#include <Urho3D/Scene/ValueAnimation.h>

#include <Urho3D/DebugNew.h>

#include <initializer_list>
#include "mastercontrol.h"

#define FILES GetSubsystem<FileSystem>()
#define ENGINE GetSubsystem<Engine>()
#define TIME GetSubsystem<Time>()
#define CACHE GetSubsystem<ResourceCache>()
#define INPUT GetSubsystem<Input>()
#define GRAPHICS GetSubsystem<Graphics>()
#define RENDERER GetSubsystem<Renderer>()
#define AUDIO GetSubsystem<Audio>()

#define MC GetSubsystem<MasterControl>()

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
class Viewport;
class RenderPath;
class Camera;
}

using namespace Urho3D;
namespace LucKey {

enum SixaxisButton {  SB_SELECT, SB_LEFTSTICK, SB_RIGHTSTICK, SB_START,
                      SB_DPAD_UP, SB_DPAD_RIGHT, SB_DPAD_DOWN, SB_DPAD_LEFT,
                      SB_L2, SB_R2, SB_L1, SB_R1,
                      SB_TRIANGLE, SB_CIRCLE, SB_CROSS, SB_SQUARE,
                      SB_PS };

unsigned IntVector2ToHash(IntVector2 vec);

float Delta(float lhs, float rhs, bool angle = false);
float Distance(const Vector3 from, const Vector3 to);
Vector3 Scale(const Vector3 lhs, const Vector3 rhs);
IntVector2 Scale(const IntVector2 lhs, const IntVector2 rhs);
Vector2 Rotate(const Vector2 vec2, const float angle);
float RandomSign();
Color RandomColor();
Color RandomSkinColor();
Color RandomHairColor(bool onlyNatural = false);

float Sine(float x);
float Cosine(float x);

int Cycle(int x, int min, int max);
float Cycle(float x, float min, float max);
}

#endif // LUCKEY_H
