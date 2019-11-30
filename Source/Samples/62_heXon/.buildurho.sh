#!/bin/sh

cd `dirname $0`;
if [ ! -d Urho3D ]
then
    git clone https://github.com/Urho3D/Urho3D
    cd Urho3D 
else
    cd Urho3D
    git checkout master
    git pull
fi

./cmake_clean.sh
./cmake_generic.sh . \
    -DURHO3D_ANGELSCRIPT=0 -DURHO3D_LUA=0 -DURHO3D_URHO2D=0 \
    -DURHO3D_SAMPLES=0 -DURHO3D_TOOLS=0
make
cd ..
