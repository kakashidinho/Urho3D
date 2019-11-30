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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class Settings : public Object
{
    URHO3D_OBJECT(Settings, Object);
public:
    Settings(Context* context);
    
    bool Load();
    void Save();

    bool GetAntiAliasing() const noexcept { return antiAliasing_; }
    bool GetManyLights() const noexcept { return manyLights_; }

    IntVector2 GetResolution() const { return IntVector2(width_, height_); }
    bool GetFullscreen() const noexcept { return fullscreen_; }
private:
    Scene* settingsScene_;
    Camera* settingsCam_;
    
    Vector<Node*> panels_;

    int width_;
    int height_;
    int refreshRate_;
    bool fullscreen_;

    bool antiAliasing_;
    bool manyLights_;
};

#endif // SETTINGS_H
