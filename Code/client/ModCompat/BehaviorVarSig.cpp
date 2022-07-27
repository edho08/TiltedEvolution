#include <ModCompat/BehaviorVarSig.h>
#include <Games/ActorExtension.h>
#include <BSAnimationGraphManager.h>
#include <Structs/AnimationGraphDescriptorManager.h>

#include <iostream>
#include <fstream>
#include <algorithm>

#include <mutex>
//debug
#define BEHAVIOR_DEBUG 1

#ifdef BEHAVIOR_DEBUG
#define D(x) (spdlog::info(x))
#endif

std::mutex mutex_lock;

BehaviorVarSig* BehaviorVarSig::single = nullptr;

BehaviorVarSig* BehaviorVarSig::Get()
{
    if (!BehaviorVarSig::single)
        BehaviorVarSig::single = new BehaviorVarSig();
    return BehaviorVarSig::single;
}

void BehaviorVarSig::initialize()
{
    const std::string PATH = TiltedPhoques::GetPath().string() + "/behaviors";
    std::vector<std::string> dirs = loadDirs(PATH);
    for (auto item : dirs)
    {
        Sig* sig = loadSigFromDir(item);
        if (sig)
        {
            sigPool.push_back(*sig);
        }
    }
}

void removeWhiteSpace(std::string &aString)
{
    //TODO: FIX THIS GODDAMN THING
    //aString.erase(std::remove_if(aString.begin(), aString.end(), std::isspace), aString.end());
}

void BehaviorVarSig::_patch(BSAnimationGraphManager* apManager, Actor* apActor)
{   /////////////////////////////////////////////////////////////////////////
    // check with animation graph holder
    ////////////////////////////////////////////////////////////////////////
    auto pExtendedActor = apActor->GetExtension();
    const AnimationGraphDescriptor* pGraph =
        AnimationGraphDescriptorManager::Get().GetDescriptor(pExtendedActor->GraphDescriptorHash);

    ////////////////////////////////////////////////////////////////////////
    // Already created
    ////////////////////////////////////////////////////////////////////////
    if (pGraph || failedSig.find(pExtendedActor->GraphDescriptorHash) != failedSig.end())
    {
        return;
    }
        

#if BEHAVIOR_DEBUG
    spdlog::info("actor with formID {} with hash of {} has modded behavior", apActor->formID,
                 pExtendedActor->GraphDescriptorHash);
#endif

    ////////////////////////////////////////////////////////////////////////
    // getVar
    ////////////////////////////////////////////////////////////////////////
    auto dumpVar = apManager->DumpAnimationVariables(false);
    std::unordered_map<std::string, uint32_t> reverseMap;

    ////////////////////////////////////////////////////////////////////////
    // reverse the map
    ////////////////////////////////////////////////////////////////////////
    for (auto item : dumpVar)
    {
        reverseMap.insert({(std::string)item.second, item.first});
    }

/* #if BEHAVIOR_DEBUG
    spdlog::info("known behavior variables");
    for (auto pair : dumpVar)
    {
        spdlog::info("{}:{}", pair.first, pair.second);
    }    
#endif*/
    ////////////////////////////////////////////////////////////////////////
    // do the sig
    ////////////////////////////////////////////////////////////////////////
    for (auto sig : sigPool)
    {
#if BEHAVIOR_DEBUG
        spdlog::info("sig {}", sig.sigName);
#endif
        bool isSig = true;
        for (std::string sigVar : sig.sigStrings)
        {
            if (reverseMap.find(sigVar) != reverseMap.end())
                continue;
            else
            {
                isSig = false;
                break;
            }
        }
        ////////////////////////////////////////////////////////////////////////
        // sig not found found
        ////////////////////////////////////////////////////////////////////////
        if (!isSig)
            continue;

#if BEHAVIOR_DEBUG
        spdlog::info("sig found as {}", sig.sigName);
#endif
        ////////////////////////////////////////////////////////////////////////
        // calculate hash
        ////////////////////////////////////////////////////////////////////////
        uint64_t mHash = apManager->GetDescriptorKey();

#if BEHAVIOR_DEBUG
        spdlog::info("sig {} has a animGraph hash of {}", sig.sigName, mHash);
#endif
        ////////////////////////////////////////////////////////////////////////
        // prepare the synced var
        ////////////////////////////////////////////////////////////////////////
        TiltedPhoques::Vector<uint32_t> boolVar;
        TiltedPhoques::Vector<uint32_t> floatVar;
        TiltedPhoques::Vector<uint32_t> intVar;

        ////////////////////////////////////////////////////////////////////////
        // fill the vector
        ////////////////////////////////////////////////////////////////////////
#if BEHAVIOR_DEBUG
        spdlog::info("prepraring var to sync");
#endif
#if BEHAVIOR_DEBUG
        spdlog::info("boolean variable");
#endif
        for (std::string var : sig.syncBooleanVar)
        {
            if (reverseMap.find(var) != reverseMap.end())
            {
#if BEHAVIOR_DEBUG
                spdlog::info("{}:{}", reverseMap[var], var);
#endif
                boolVar.push_back(reverseMap[var]);
            }
        }
#if BEHAVIOR_DEBUG
        spdlog::info("float variable");
#endif
        for (std::string var : sig.syncFloatVar)
        {
            if (reverseMap.find(var) != reverseMap.end())
            {
#if BEHAVIOR_DEBUG
                spdlog::info("{}:{}", reverseMap[var], var);
#endif
                floatVar.push_back(reverseMap[var]);
            }
        }
#if BEHAVIOR_DEBUG
        spdlog::info("integer variable");
#endif
        for (std::string var : sig.syncIntegerVar)
        {
            if (reverseMap.find(var) != reverseMap.end())
            {
#if BEHAVIOR_DEBUG
                spdlog::info("{}:{}", reverseMap[var], var);
#endif
                intVar.push_back(reverseMap[var]);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //Very hacky and shouldnt be allowed
        //This is a breach in the dev code and will not be merged
        ////////////////////////////////////////////////////////////////////////
#if BEHAVIOR_DEBUG
        spdlog::info("building animgraph var for {}", apActor->formID);
#endif
        auto animGrapDescriptor = new AnimationGraphDescriptor({0}, {0}, {0});
        animGrapDescriptor->BooleanLookUpTable = boolVar;
        animGrapDescriptor->FloatLookupTable = floatVar;
        animGrapDescriptor->IntegerLookupTable = intVar;

        ////////////////////////////////////////////////////////////////////////
        //add the new graph to the var graph
        ////////////////////////////////////////////////////////////////////////
        new AnimationGraphDescriptorManager::Builder(AnimationGraphDescriptorManager::Get(), mHash, *animGrapDescriptor);

        ////////////////////////////////////////////////////////////////////////
        //change the actor hash? is this even necessary?
        ////////////////////////////////////////////////////////////////////////
        pExtendedActor->GraphDescriptorHash = mHash;

        ////////////////////////////////////////////////////////////////////////
        //handle hard coded case
        ////////////////////////////////////////////////////////////////////////
        if (sig.sigName == "master")
        {
            humanoidSig = {mHash, sig};
        }
        else if (sig.sigName == "werewolf")
            werewolfSig = {mHash, sig};
        else if (sig.sigName == "vampire_lord")
            vampireSig = {mHash, sig};

        ////////////////////////////////////////////////////////////////////////
        //take a break buddy
        ////////////////////////////////////////////////////////////////////////
        return;
    }

    //sig failed
#if BEHAVIOR_DEBUG
    spdlog::info("sig for actor {} failed", apActor->formID);
#endif
    failedSig[pExtendedActor->GraphDescriptorHash] = true;
}


std::vector<std::string> BehaviorVarSig::loadDirs(const std::string &acPATH)
{
    std::vector<std::string> result;
    for (auto& p : std::filesystem::directory_iterator(acPATH))
        if (p.is_directory())
            result.push_back(p.path().string());
    return result;
}

BehaviorVarSig::Sig* BehaviorVarSig::loadSigFromDir(std::string aDir)
{
#if BEHAVIOR_DEBUG
    spdlog::info("creating sig");
#endif
    std::string nameVarFileDir;
    std::string sigFileDir;
    std::vector<std::string> floatVarFileDir;
    std::vector<std::string> intVarFileDir;
    std::vector<std::string> boolVarFileDir;

    ////////////////////////////////////////////////////////////////////////
    // Enumerate all files in this directory
    ////////////////////////////////////////////////////////////////////////

    for (auto& p : std::filesystem::directory_iterator(aDir))
    {
        std::string path = p.path().string();
        std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
#if BEHAVIOR_DEBUG
        spdlog::info("base_path: {}", base_filename);
#endif
        if (base_filename.find("__name.txt") != std::string::npos)
        {
            nameVarFileDir = path;
#if BEHAVIOR_DEBUG
            spdlog::info("name file: {}", nameVarFileDir);
#endif
        }
        else if (base_filename.find("__sig.txt") != std::string::npos)
        {
            sigFileDir = path;
#if BEHAVIOR_DEBUG
            spdlog::info("sig file: {}", path);
#endif
        }
        else if (base_filename.find("__float.txt") != std::string::npos)
        {
            floatVarFileDir.push_back(path);
#if BEHAVIOR_DEBUG
            spdlog::info("float file: {}", path);
#endif
        }
        else if (base_filename.find("__int.txt") != std::string::npos)
        {
            intVarFileDir.push_back(path);
#if BEHAVIOR_DEBUG
            spdlog::info("int file: {}", path);
#endif
        }
        else if (base_filename.find("__bool.txt") != std::string::npos)
        {
            boolVarFileDir.push_back(path);
#if BEHAVIOR_DEBUG
            spdlog::info("bool file: {}", path);
#endif
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // sanity check
    ////////////////////////////////////////////////////////////////////////
    if (nameVarFileDir == "" || sigFileDir == "")
    {
        return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////
    // read the files
    ////////////////////////////////////////////////////////////////////////
    std::string name = "";
    std::vector<std::string> sig;
    std::set<std::string> floatVar;
    std::set<std::string> intVar;
    std::set<std::string> boolVar;

    //read name var
    std::string tempString;
    std::ifstream file(nameVarFileDir);
    getline(file, tempString);
    name = tempString;
    removeWhiteSpace(name);
    file.close();
    if (name == "")
        return nullptr;

    //read sig var
#if BEHAVIOR_DEBUG
    spdlog::info("creating sig for {}", name);
#endif
    std::ifstream file1(sigFileDir);
    while (std::getline(file1, tempString))
    {
        removeWhiteSpace(tempString);
        sig.push_back(tempString);
#if BEHAVIOR_DEBUG
        spdlog::info("{}:{}", name, tempString);
#endif
    }
    file1.close();
    if (sig.size() < 1)
    {
        return nullptr;
    }

#if BEHAVIOR_DEBUG
    spdlog::info("reading float var", name, tempString);
#endif
    //read float var
    for (auto item : floatVarFileDir)
    {
        std::ifstream file2(item);
        while (std::getline(file2, tempString))
        {
            removeWhiteSpace(tempString);
            floatVar.insert(tempString);
#if BEHAVIOR_DEBUG
            spdlog::info(tempString);
#endif
        }
        file2.close();
    }

#if BEHAVIOR_DEBUG
    spdlog::info("reading int var", name, tempString);
#endif
    // read int var
    for (auto item : intVarFileDir)
    {
        std::ifstream file3(item);
        while (std::getline(file3, tempString))
        {
            removeWhiteSpace(tempString);
            intVar.insert(tempString);
#if BEHAVIOR_DEBUG
            spdlog::info(tempString);
#endif
        }
        file3.close();
    }

#if BEHAVIOR_DEBUG
    spdlog::info("reading bool var", name, tempString);
#endif
    //read bool var
    for (auto item : boolVarFileDir)
    {
        std::ifstream file4(item);
        while (std::getline(file4, tempString))
        {
            removeWhiteSpace(tempString);
            boolVar.insert(tempString);
#if BEHAVIOR_DEBUG
            spdlog::info(tempString);
#endif
        }
        file4.close();
    }

    //convert set to vector
    std::vector<std::string> floatVector;
    std::vector<std::string> intVector;
    std::vector<std::string> boolVector;

    for (auto item : floatVar)
    {
        floatVector.push_back(item);
    }

    for (auto item : intVar)
    {
        intVector.push_back(item);
    }

    for (auto item : boolVar) 
    {
        boolVector.push_back(item);
    }

    //create the sig
    Sig* result = new Sig();

    result->sigName = name;
    result->sigStrings = sig;
    result->syncBooleanVar = boolVector;
    result->syncFloatVar = floatVector;
    result->syncIntegerVar = intVector;

    return result;
}

void BehaviorVarSig::patch(BSAnimationGraphManager* apManager, Actor* apActor)
{
    std::lock_guard<std::mutex> guard(mutex_lock);
    _patch(apManager, apActor);
}
