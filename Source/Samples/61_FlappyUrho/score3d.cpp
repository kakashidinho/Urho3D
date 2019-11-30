#include "mastercontrol.h"

#include "score3d.h"

void Score3D::RegisterObject(Context* context)
{
    context->RegisterFactory<Score3D>();
}

Score3D::Score3D(Context* context) : LogicComponent(context)
{
}

void Score3D::OnNodeSet(Node* node)
{ if(!node) return;

    node_->SetRotation(Quaternion(180.0f, Vector3::UP));

    SetAlignRight(false);
    AddDigit();
    SetScore(0);//node_->GetName().Contains("High") ? GLOBAL->GetHighscore() : 0 );
}

void Score3D::Update(float timeStep)
{
}

void Score3D::SetScore(unsigned score)
{
    score_ = score;
    String scoreString{ score_ };

    while (scoreString.Length() != digitNodes_.Size())
    {
        if (digitNodes_.Size() < scoreString.Length())
            AddDigit();
        else if (digitNodes_.Size() > scoreString.Length())
            RemoveDigit();
    }

    //Update score graphics
    for (Node* n : digitNodes_) {

        StaticModel* digitModel{ n->GetComponent<StaticModel>() };
        digitModel->SetModel(MC->GetModel(String(
                             static_cast<int>(score_ / static_cast<unsigned>(pow(10, digitNodes_.IndexOf(n)))) % 10 )));
        digitModel->SetMaterial(MC->GetMaterial("Bubble"));
    }
}

void Score3D::SetAlignRight(bool alignRight)
{
    alignRight_ = alignRight;

    node_->SetPosition(GetRootPosition());
    for (Node* n : digitNodes_) {
        n->SetPosition(GetDigitTargetPosition(digitNodes_.IndexOf(n)));
    }
}

Vector3 Score3D::GetDigitTargetPosition(int index)
{
    return Vector3::RIGHT * DIGIT_SPACING * index;
}
Vector3 Score3D::GetRootPosition()
{
    float ratio{ static_cast<float>(GRAPHICS->GetWidth()) / GRAPHICS->GetHeight() };
    float scoreWidth{ DIGIT_SPACING * digitNodes_.Size() - 1 };
    return Vector3(alignRight_ ? 9.5f * ratio : -9.5f * ratio + scoreWidth, 8.0f, -5.0f);
}


void Score3D::AddDigit()
{
    Node* digitNode{ node_->CreateChild("Digit") };
    digitNode->SetPosition(GetDigitTargetPosition(digitNodes_.Size()));
    digitNode->CreateComponent<StaticModel>();
    digitNodes_.Push(digitNode);

//    if (!alignRight_)
    node_->SetPosition(GetRootPosition());
}

void Score3D::RemoveDigit()
{
    Node* lastDigit{ digitNodes_.Back() };
    lastDigit->RemoveAllComponents();
    lastDigit->Remove();
    digitNodes_.Pop();

//    if (!alignRight_)
    node_->SetPosition(GetRootPosition());
}

