#include <ModCompat/MasterBehaviorVars.h>
#include <BSAnimationGraphManager.h>
#include <Havok/hkbBehaviorGraph.h>
#include <Havok/hkbStateMachine.h>
#include <Structs/AnimationGraphDescriptor.h>
#include <Structs/AnimationGraphDescriptorManager.h>

bool MasterBehaviorVars::bIsMasterBehaviorVariableSet = false;
bool MasterBehaviorVars::bIsPatchFailed = false;
MasterBehaviorVars* MasterBehaviorVars::single = nullptr;

// Dev hand picked var for master behavior
const std::string MasterBehaviorVars::boolVar[] = {
    "kbEquipOk",
    "kbMotionDriven",
    "kIsBeastRace",
    "kIsSneaking",
    "kIsBleedingOut",
    "kIsCastingDual",
    "kIs1HM",
    "kIsCastingRight",
    "kIsCastingLeft",
    "kIsBlockHit",
    "kIsPlayer",
    "kIsNPC",
    "kbIsSynced",
    "kbVoiceReady",
    "kbWantCastLeft",
    "kbWantCastRight",
    "kbWantCastVoice",
    "kb1HM_MLh_attack",
    "kb1HMCombat",
    "kbAnimationDriven",
    "kbCastReady",
    "kIsAttacking",
    "kbAllowRotation",
    "kbMagicDraw",
    "kbMLh_Ready",
    "kbMRh_Ready",
    "kbInMoveState",
    "kbSprintOK",
    "kbIdlePlaying",
    "kbIsDialogueExpressive",
    "kbAnimObjectLoaded",
    "kbEquipUnequip",
    "kbAttached",
    "kbIsH2HSolo",
    "kbHeadTracking",
    "kbIsRiding",
    "kbTalkable",
    "kbRitualSpellActive",
    "kbInJumpState",
    "kbHeadTrackSpine",
    "kbLeftHandAttack",
    "kbIsInMT",
    "kbHumanoidFootIKEnable",
    "kbHumanoidFootIKDisable",
    "kbStaggerPlayerOverride",
    "kbNoStagger",
    "kbIsStaffLeftCasting",
    "kbPerkShieldCharge",
    "kbPerkQuickShot",
    "kIsBlocking",
    "kIsBashing",
    "kIsStaggering",
    "kIsRecoiling",
    "kIsEquipping",
    "kIsUnequipping",
    "kisInFurniture",
    "kbNeutralState",
    "kbBowDrawn",
    "kPitchOverride",
    "kNotCasting"
};
const std::string MasterBehaviorVars::intVar[] = {
    "kiRightHandEquipped",
    "kiLeftHandEquipped",
    "ki1HMState",
    "kiState",
    "kiLeftHandType",
    "kiRightHandType",
    "kiSyncIdleLocomotion",
    "kiSyncForwardState",
    "kiSyncTurnState",
    "kiIsInSneak",
    "kiWantBlock",
    "kiRegularAttack",
    "ktestint",
    "kcurrentDefaultState"

};
const std::string MasterBehaviorVars::floatVar[] = {
    "kTurnDelta",
    "kDirection",
    "kSpeedSampled",
    "kweapAdj",
    "kSpeed",
    "kCastBlend",
    "kPitchOffset",
    "kSpeedDamped",
    "kPitch",
    "kVelocityZ",
    "k1stPRot",
    "k1stPRotDamped",
    "kCastBlendDamped"
};

MasterBehaviorVars* MasterBehaviorVars::Get()
{
    //return the patch if it has been created
    if (MasterBehaviorVars::single)
        return MasterBehaviorVars::single;

    //else create the object
    MasterBehaviorVars::single = new MasterBehaviorVars;
    return MasterBehaviorVars::single;
}

void MasterBehaviorVars::patch(Actor* apActor, BSAnimationGraphManager* pManager)
{
    const BShkbAnimationGraph* pGraph = nullptr;
    if (pManager && apActor)
    {
        if (pManager->animationGraphIndex < pManager->animationGraphs.size)
        {
            const BShkbAnimationGraph* pGraph = nullptr;
            pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);
            if (pGraph && pGraph->behaviorGraph && pGraph->behaviorGraph->stateMachine &&
                pGraph->behaviorGraph->stateMachine->name)
            {
                //get dumped variable 
                SortedMap<uint32_t, String> varDump = pManager->DumpAnimationVariables(false);

                //add k
                for (auto item : varDump)
                {
                    varDump[item.first] = "k" + item.second;
                }

                //reverse map
                for (auto item : varDump)
                {
                    aBehaviorVariableMap.insert({(std::string) item.second, item.first});
                }

                //create an array for handpicked var
                TiltedPhoques::Vector<uint32_t> boolBehaviorVar;
                TiltedPhoques::Vector<uint32_t> intBehaviorVar;
                TiltedPhoques::Vector<uint32_t> floatBehaviorVar;

                //fill the array
                for (int i = 0; i < boolVar->length(); i++)
                {
                    boolBehaviorVar.push_back(aBehaviorVariableMap[boolVar[i]]);
                }
                for (int i = 0; i < intVar->length(); i++)
                {
                    intBehaviorVar.push_back(aBehaviorVariableMap[intVar[i]]);
                }
                for (int i = 0; i < floatVar->length(); i++)
                {
                    floatBehaviorVar.push_back(aBehaviorVariableMap[floatVar[i]]);
                }

                //create a patch animation descriptor
                //TODO: this is a very hacky way to do this.
                //Im sure the dev wont approve of such monstrosity
                AnimationGraphDescriptor* animationDescriptor = new AnimationGraphDescriptor({0}, {0}, {0});
                animationDescriptor->BooleanLookUpTable = boolBehaviorVar;
                animationDescriptor->FloatLookupTable = floatBehaviorVar;
                animationDescriptor->IntegerLookupTable = intBehaviorVar;

                //create a patch hash key
                uint64_t aKey = pManager->GetDescriptorKey();

                //register the patch to AnimationGraphDescriptorManager
                new AnimationGraphDescriptorManager::Builder(AnimationGraphDescriptorManager::Get(), aKey, *animationDescriptor);

                //Debug
                spdlog::info("boolvar");
                for (int i = 0; i < boolBehaviorVar.size(); i++)
                {
                    spdlog::info(animationDescriptor->BooleanLookUpTable[i]);
                }
                spdlog::info("intvar");
                for (int i = 0; i < intBehaviorVar.size(); i++)
                {
                    spdlog::info(animationDescriptor->IntegerLookupTable[i]);
                }
                spdlog::info("floatvar");
                for (int i = 0; i < floatBehaviorVar.size(); i++)
                {
                    spdlog::info(animationDescriptor->FloatLookupTable[i]);
                }


                //check
                auto desc = AnimationGraphDescriptorManager::Get().GetDescriptor(aKey);
                if (!desc)
                {
                    spdlog::info("Failed to generate behavior of actor with formid {} with possible hash of {}",
                                 apActor->formID, aKey);

                    bIsPatchFailed = true;
                    bIsMasterBehaviorVariableSet = false;
                }

                //housekeeping
                bIsMasterBehaviorVariableSet = true;
                bIsPatchFailed = false;

                spdlog::info("Actor with formid {} now has animation graph hash of {}", apActor->formID, aKey);
                return;
               
                //TODO: Add a way to allow sync of modded behavior var.
                //TODO: Add a way to sync other behavior files.
            }
        }
    }

    spdlog::info("Failed to generate behavior");

    bIsPatchFailed = true;
    bIsMasterBehaviorVariableSet = false;
    return;
}
