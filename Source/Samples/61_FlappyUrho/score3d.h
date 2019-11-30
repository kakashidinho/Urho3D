
#ifndef SCORE3D_H
#define SCORE3D_H

#include <Urho3D/Urho3D.h>
#include "Urho3DAll.h"

#define DIGIT_SPACING 1.5f

class Score3D : public LogicComponent
{

    URHO3D_OBJECT(Score3D, LogicComponent);
public:
    Score3D(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

    void SetScore(unsigned score);
    void SetAlignRight(bool alignRight);
private:
    int score_;
    bool alignRight_;
    Vector<Node*> digitNodes_;

    Vector3 GetDigitTargetPosition(int index);
    Vector3 GetRootPosition();

    void AddDigit();
    void RemoveDigit();
};

#endif // SCORE3D_H
