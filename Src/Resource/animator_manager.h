#pragma once
#include <memory>
#include <string>
#include <unordered_map>
class Animation;

class AnimatorManager
{
public:
    AnimatorManager();
    ~AnimatorManager();
    void Shutdown();
    Animation* Load(const std::string& path);
    Animation* GetAnimation(const std::string& name);
    static AnimatorManager& Get();
private:
    std::unordered_map<std::string,std::unique_ptr<Animation>> cache_;
};
