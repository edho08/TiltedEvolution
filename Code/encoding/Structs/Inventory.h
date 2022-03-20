#pragma once

#include <TiltedCore/Stl.hpp>
#include <Structs/GameId.h>

using TiltedPhoques::Buffer;
using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct Inventory
{
    struct EffectItem
    {
        EffectItem() = default;
        ~EffectItem() = default;

        float Magnitude{};
        int32_t Area{};
        int32_t Duration{};
        float RawCost{};
        GameId EffectId{};

        void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
        void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
    };

    struct EnchantmentData
    {
        EnchantmentData() = default;
        ~EnchantmentData() = default;

        bool IsWeapon{};
        Vector<EffectItem> Effects{};
    };

    struct Entry
    {
        Entry() = default;
        ~Entry() = default;

        GameId BaseId{};
        int32_t Count{};

        float ExtraCharge{};

        GameId ExtraEnchantId{};
        uint16_t ExtraEnchantCharge{};
        EnchantmentData EnchantData{};

        float ExtraHealth{};

        GameId ExtraPoisonId{};
        uint32_t ExtraPoisonCount{};

        int32_t ExtraSoulLevel{};

        String ExtraTextDisplayName{};

        bool ExtraEnchantRemoveUnequip{};
        bool ExtraWorn{};
        bool ExtraWornLeft{};

        bool operator==(const Entry& acRhs) const noexcept;
        bool operator!=(const Entry& acRhs) const noexcept;

        void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
        void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

        bool ContainsExtraData() const noexcept
        {
            return !IsExtraDataEquals(Entry{});
        }

        bool CanBeMerged(const Entry& acRhs) const noexcept
        {
            return BaseId == acRhs.BaseId && IsExtraDataEquals(acRhs);
        }

        bool IsExtraDataEquals(const Entry& acRhs) const noexcept
        {
            return ExtraCharge == acRhs.ExtraCharge &&
                   ExtraEnchantId == acRhs.ExtraEnchantId &&
                   ExtraEnchantCharge == acRhs.ExtraEnchantCharge &&
                   ExtraEnchantRemoveUnequip == acRhs.ExtraEnchantRemoveUnequip &&
                   ExtraHealth == acRhs.ExtraHealth &&
                   ExtraPoisonId == acRhs.ExtraPoisonId &&
                   ExtraPoisonCount == acRhs.ExtraPoisonCount &&
                   ExtraSoulLevel == acRhs.ExtraSoulLevel &&
                   //ExtraTextDisplayName == acRhs.ExtraTextDisplayName &&
                   ExtraWorn == acRhs.ExtraWorn &&
                   ExtraWornLeft == acRhs.ExtraWornLeft;
        }
    };

    Inventory() = default;
    ~Inventory() = default;

    bool operator==(const Inventory& acRhs) const noexcept;
    bool operator!=(const Inventory& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    Vector<Entry> Entries{};

    GameId RightHandWeapon{};
#if TP_SKYRIM
    GameId LeftHandWeapon{};
    GameId LeftHandSpell{};
    GameId RightHandSpell{};
    GameId Shout{};
    GameId Ammo{};
#endif
};
