#ifndef PANEL_H
#define PANEL_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"
#include "mastercontrol.h"

class Panel : public LogicComponent
{
    URHO3D_OBJECT(Panel, LogicComponent);
public:
    Panel(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Initialize(int colorSet);
    void Update(float timeStep) override;

    void ActivatePanel(StringHash eventType, VariantMap& eventData);
    void DeactivatePanel(StringHash eventType, VariantMap& eventData);

    void EnterPlay(StringHash eventType, VariantMap& eventData);
    void EnterLobby(StringHash eventType, VariantMap& eventData);

    bool HasOwner();
    bool IsOwner(int playerId);

private:
    Vector<Node*> infoNodes_;
    Node* currentInfoNode_;
    unsigned currentInfoNodeIndex_;
    float sinceInfoChange_;

    bool active_;

    int colorSet_;

    Scene* panelScene_;
    SharedPtr<Texture2D> panelTexture_;
    Node* panelTriggerNode_;
    Node* smallPanelNode_;
    Node* bigPanelNode_;

    void FadeInPanel();
    void FadeOutPanel(bool immediate = false);
    void CreatePanels();
    void SetCurrentInfoNode(Node* infoNode);
    void CreateInfos();
};

#endif // PANEL_H
