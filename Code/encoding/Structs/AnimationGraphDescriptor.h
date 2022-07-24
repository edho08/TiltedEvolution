#pragma once

struct AnimationGraphDescriptor
{
    AnimationGraphDescriptor() = default;

    template <std::size_t N, std::size_t O, std::size_t P>
    AnimationGraphDescriptor(const uint32_t (&acBooleanList)[N], const uint32_t (&acFloatList)[O],
                             const uint32_t (&acIntegerList)[P])
    {
        static_assert(N <= 64, "Too many boolean variables!");
        static_assert((1 + O + P) <= 64, "Too many float and integer!");

        BooleanLookUpTable.assign(acBooleanList, acBooleanList + N);
        FloatLookupTable.assign(acFloatList, acFloatList + O);
        IntegerLookupTable.assign(acIntegerList, acIntegerList + P);
    }

    //this is implemented so user can use behavior altering mods
    /* AnimationGraphDescriptor(TiltedPhoques::Vector<uint32_t> acBooleanList,
                                TiltedPhoques::Vector<uint32_t> acFloatList,
                             TiltedPhoques::Vector<uint32_t> acIntegerList)
    {
        // N,O,P
        const uint32_t N = acBooleanList.size(); 
        const uint32_t O = acFloatList.size();
        const uint32_t P = acIntegerList.size();

        //we cannot use static assert in this case
        assert(N <= 128, "Too many boolean variables!");
        assert((1 + O + P) <= 128, "Too many float and integer!");

        BooleanLookUpTable = TiltedPhoques::Vector<uint32_t>(acBooleanList);
        FloatLookupTable = TiltedPhoques::Vector<uint32_t>(acFloatList);
        IntegerLookupTable = TiltedPhoques::Vector<uint32_t>(acIntegerList);
    }*/

    bool IsSynced(uint32_t aIdx) const
    {
        auto itor = std::find(std::begin(BooleanLookUpTable), std::end(BooleanLookUpTable), aIdx);
        if (itor != std::end(BooleanLookUpTable))
            return true;

        itor = std::find(std::begin(FloatLookupTable), std::end(FloatLookupTable), aIdx);
        if (itor != std::end(FloatLookupTable))
            return true;

        itor = std::find(std::begin(IntegerLookupTable), std::end(IntegerLookupTable), aIdx);
        if (itor != std::end(IntegerLookupTable))
            return true;

        return false;
    }

    TiltedPhoques::Vector<uint32_t> BooleanLookUpTable;
    TiltedPhoques::Vector<uint32_t> FloatLookupTable;
    TiltedPhoques::Vector<uint32_t> IntegerLookupTable;
};
