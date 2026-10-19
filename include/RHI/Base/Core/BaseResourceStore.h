#pragma once

#include <cstdint>
#include <vector>
#include <variant>

namespace RHI::Resource {
    class IResourceStore {
    public:
        virtual ~IResourceStore() = default;
        virtual void RemoveById(uint32_t id) = 0;
        virtual bool ContainsId(uint32_t id) const = 0;
        virtual size_t Size() const = 0;
        virtual void Clear() = 0;
    };

    template<typename NativeHandle>
    class BaseResourceStore : public IResourceStore {
    private:
        struct Slot {
            NativeHandle native{};
            bool alive = false;
        };

        std::vector<Slot> slots;
        std::vector<uint32_t> freeList;
        size_t liveCount = 0;

        [[nodiscard]] uint32_t AllocateSlot(NativeHandle native) {
            uint32_t id;
            if (!freeList.empty()) {
                id = freeList.back();
                freeList.pop_back();
                slots[id] = Slot{ native, true };
            } else {
                id = static_cast<uint32_t>(slots.size());
                slots.push_back(Slot{ native, true });
            }

            liveCount++;
            return id;
        }

    public:
        static constexpr uint32_t invalidId = 0;

        BaseResourceStore() {
            slots.push_back(Slot{});
        }

        template<typename HandleT>
        [[nodiscard]] HandleT Create(NativeHandle native) {
            const uint32_t id = AllocateSlot(native);
            return HandleT{ id };
        }

        [[nodiscard]] NativeHandle* GetNative(uint32_t id) noexcept {
            if (id >= slots.size()) return nullptr;
            Slot& s = slots[id];
            if (!s.alive) return nullptr;
            return &s.native;
        }

        [[nodiscard]] const NativeHandle* GetNative(uint32_t id) const noexcept {
            if (id >= slots.size()) return nullptr;
            const Slot& s = slots[id];
            if (!s.alive) return nullptr;
            return &s.native;
        }

        template<typename HandleT>
        [[nodiscard]] NativeHandle* GetNative(HandleT h) noexcept {
            return GetNative(h.id);
        }

        template<typename NativeT, typename HandleT>
        [[nodiscard]] NativeT* GetNativeAs(HandleT h) noexcept {
            auto* v = GetNative(h);
            if (!v) return nullptr;
            return std::get_if<NativeT>(v);
        }

        [[nodiscard]] bool IsValid(uint32_t id) const noexcept {
            return id < slots.size() && slots[id].alive;
        }

        template<typename HandleT>
        [[nodiscard]] bool IsValid(HandleT h) const noexcept {
            return IsValid(h.id);
        }

        void RemoveById(uint32_t id) override {
            if (id == invalidId || id >= slots.size()) return;
            Slot& s = slots[id];
            if (!s.alive) return;

            s.native.Destroy();
            s.alive = false;
            s.native = NativeHandle{};
            freeList.push_back(id);
            liveCount--;
        }

        [[nodiscard]] bool ContainsId(uint32_t id) const override {
            return IsValid(id);
        }

        [[nodiscard]] size_t Size() const override {
            return liveCount;
        }

        void Clear() override {
            slots.clear();
            slots.push_back(Slot{});
            freeList.clear();
            liveCount = 0;
        }

        void Reserve(size_t n) {
            slots.reserve(n + 1);
            freeList.reserve(n);
        }
    };
}