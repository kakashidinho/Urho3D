#ifndef GLOBAL_H
#define GLOBAL_H

#include "Urho3DAll.h"

#define NUM_BARRIERS 5
#define NUM_WEEDS 34

#define BAR_GAP 10.0f

#define BAR_INTERVAL 20.0f

#define BAR_SPEED (5.0f + Clamp(GLOBAL->sinceLastReset_ * 0.05f, 0.0f, 2.3f))

#define BAR_RANDOM_Y Random(-6.0f, 6.0f)
#define WEED_RANDOM_Y Random(-23.0f, -21.0f)

#define BAR_OUTSIDE_X 50.0f

#define GRAV_ACC 9.8f

#define UP_SPEED 10.0f

#define CAMERA_DEFAULT_POS Vector3(0.0f, 0.0f, -30.0f)

#define URHO_DEFAULT_ROTATION Quaternion(0.0f, -90.0f, 0.0f)

#define CACHE GetSubsystem<ResourceCache>()
#define GLOBAL GetSubsystem<Global>()
#define INPUT GetSubsystem<Input>()
#define UI_ROOT GetSubsystem<UI>()->GetRoot()
#define RENDERER GetSubsystem<Renderer>()
#define GRAPHICS GetSubsystem<Graphics>()

class Score3D;

enum GameState
{
    GS_INTRO = 0,

    GS_PLAY,

    GS_DEAD
};

class Global : public Object
{
    URHO3D_OBJECT(Global, Object);

public:
    Global(Context* context);
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
    
    unsigned GetScore() const { return score_; }
    void SetScore(unsigned score);
    unsigned GetHighscore() const { return highscore_; }
    void SetHighscore(unsigned highscore);

    GameState gameState_;
    GameState neededGameState_;
    float sinceLastReset_;

    void SetScores3D(Score3D* score3d, Score3D* highscore3d);
private:
    unsigned score_;
    unsigned highscore_;
    bool scoreTextDirty_;
    bool highscoreTextDirty_;

    Score3D* score3d_;
    Score3D* highscore3d_;
};

#endif // GLOBAL_H
