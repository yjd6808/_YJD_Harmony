#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UITextureEntry.h"

class UITextureCache
{
public:
    UITextureCache();
    ~UITextureCache();

    void SetMaxMemoryBytes(size_t _bytes) { maxMemoryBytes_ = _bytes; }

    UITextureEntry* Find(const UITextureCacheKey& _key);
    void Insert(const UITextureCacheKey& _key, UITextureEntry* _entry);
    void Remove(const UITextureCacheKey& _key);
    void Clear();

    size_t GetCurrentMemoryBytes() const { return currentMemoryBytes_; }

private:
    struct CacheEntry
    {
        UITextureCacheKey key;
        UITextureEntry* entry;
        uint64_t lastAccessFrame = 0;
    };

    void EvictIfNeeded();

    jc::Vector<CacheEntry> entries_;
    size_t maxMemoryBytes_ = 64 * 1024 * 1024;
    size_t currentMemoryBytes_ = 0;
    uint64_t frameCounter_ = 0;
};

class UITextureDeferredRelease
{
public:
    void ReleaseAfterFrames(cc::Ref* _obj, int _frames);
    void Update();
    void Flush();

private:
    struct DeferredEntry
    {
        cc::Ref* obj;
        int remainingFrames;
    };

    jc::Vector<DeferredEntry> queue_;
};
