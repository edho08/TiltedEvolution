#pragma once


struct MasterBehaviorVars
{
  public:
    static bool bIsMasterBehaviorVariableSet;
    static bool bIsPatchFailed;
    static MasterBehaviorVars* Get();
    void patch(Actor* apActor, BSAnimationGraphManager* pManager);

  private:
    uint32_t mHash;
    std::unordered_map<std::string, uint32_t> aBehaviorVariableMap;
    static MasterBehaviorVars* single;
    
    //Dev hand picked var for master behavior
    static const std::string boolVar[];
    static const std::string intVar[];
    static const std::string floatVar[];
};
