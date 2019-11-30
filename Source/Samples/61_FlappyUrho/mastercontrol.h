#include <Urho3D/Urho3D.h>
#include "Urho3DAll.h"
#include "global.h"

#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H

#define MC MasterControl::GetInstance()

class MasterControl : public Application
{
    URHO3D_OBJECT(MasterControl, Application);
public:
    MasterControl(Context* context);
    static MasterControl* GetInstance();
    void Setup();
    void Start();
    void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
    void UpdateUIVisibility();
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    String GetResourceFolder() const { return resourceFolder_; }
    Scene* GetScene() { return scene_; }

    Material* GetMaterial(String name) const { return CACHE->GetResource<Material>("Materials/" + name + ".xml"); }
    Model* GetModel(String name) const { return CACHE->GetResource<Model>("Models/" + name + ".mdl"); }
private:
    static MasterControl* instance_;
    Scene* scene_;
    String resourceFolder_;
    bool drawDebug_;

    void CreateScene();
    void CreateUrho();
    void CreateNets();
    void CreateWeeds();
    void CreateCrown();
    void CreateUI();
};

#endif // MASTERCONTROL_H

