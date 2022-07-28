#pragma once

struct BehaviorVarSig
{
    struct Sig
    {
        std::string sigName;
        std::vector<std::string> sigStrings;
        std::vector<std::string> negSigStrings;
        std::vector<std::string> syncBooleanVar;
        std::vector<std::string> syncFloatVar;
        std::vector<std::string> syncIntegerVar;
    };

    //hardcoded
    std::pair<uint64_t, Sig> humanoidSig;
    std::pair<uint64_t, Sig> werewolfSig;
    std::pair<uint64_t, Sig> vampireSig;

    //Sig pool
    std::vector<Sig> sigPool;
    std::unordered_map<uint64_t, bool> failedSig;

    static BehaviorVarSig* Get();
    void initialize();
    void patch(BSAnimationGraphManager* apManager, Actor* apActor);

    private:
        static BehaviorVarSig *single;
        Sig* loadSigFromDir(std::string aDir);
        std::vector<std::string> loadDirs(const std::string& acPATH);
        void _patch(BSAnimationGraphManager* apManager, Actor* apActor);
};
