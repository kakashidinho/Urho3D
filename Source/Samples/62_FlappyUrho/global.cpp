#include <fstream>

#include "global.h"
#include "score3d.h"
#include "mastercontrol.h"

Global::Global(Context* context) :
    Object(context),
    gameState_{GS_INTRO},
    neededGameState_{GS_INTRO},
    sinceLastReset_{0.0f},
    score_{0},
    highscore_{0},
    scoreTextDirty_{true},
    highscoreTextDirty_{true}
{
    //Load highscore
    FileSystem* fileSystem{ GetSubsystem<FileSystem>() };
    if (fileSystem->FileExists(MC->GetResourceFolder() + "/.hi")) {
        File file{ MC->GetContext(), MC->GetResourceFolder() + "/.hi", FILE_READ };
        SetHighscore(file.ReadUInt());
        file.Close();
    }

    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Global, HandlePostUpdate));
}

void Global::SetScore(unsigned score)
{
    if (score_ == score)
        return;

    score_ = score;
    scoreTextDirty_ = true;
}

void Global::SetHighscore(unsigned highscore)
{
    if (highscore_ >= highscore)
        return;

    highscore_ = highscore;
    highscoreTextDirty_ = true;

    //Save highscore
    File file{ MC->GetContext() };
    file.Open( MC->GetResourceFolder() + "/.hi", FILE_WRITE);
    file.WriteUInt(highscore_);
    file.Close();
}

void Global::SetScores3D(Score3D* score3d, Score3D* highscore3d)
{
    score3d_ = score3d;
    highscore3d_ = highscore3d;
    highscore3d_->SetAlignRight(true);
}

void Global::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    if (scoreTextDirty_) {

        score3d_->SetScore(score_);
        scoreTextDirty_ = false;
    }
    if (highscoreTextDirty_)
    {
        highscore3d_->SetScore(highscore_);
        highscoreTextDirty_ = false;
    }
}
