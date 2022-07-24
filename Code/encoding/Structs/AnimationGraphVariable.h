#pragma once

struct AnimationGraphVariable
{
    AnimationGraphVariable(const std::vector<std::string> aVanillaVariables,
                           const std::vector<std::string> aInternalVariables,
                           const std::vector<std::string> aModdedVariables)
    {
        const uint32_t aVanillaCount = aVanillaVariables.size();
        const uint32_t aInternalCount = aInternalVariables.size();
        const uint32_t aModdedCount = aModdedVariables.size();

        uint32_t pointer = 0;
        uint32_t start = pointer;
        for (pointer; pointer < aVanillaCount; pointer++)
        {
            aAnimationVariables[aVanillaVariables[pointer]] = pointer;
        }
        start = pointer;
        for (pointer; pointer < start + aModdedCount; pointer++)
        {
            aAnimationVariables[aModdedVariables[pointer - aVanillaCount]] = pointer;
        }
        start = pointer;
        for (pointer; pointer < start + aInternalCount; pointer++)
        {
            aAnimationVariables[aInternalVariables[pointer - aVanillaCount - aModdedCount]] = pointer;
        }
    }
    uint32_t operator[](std::string aVariableName) noexcept
    {
        return aAnimationVariables[aVariableName];
    }

  private:
    std::unordered_map<std::string, std::uint32_t> aAnimationVariables;
};
