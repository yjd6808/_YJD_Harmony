#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UITextureCache.h"

UITextureCache::UITextureCache()
{
}

UITextureCache::~UITextureCache()
{
    Clear();
}

UITextureEntry* UITextureCache::Find(const UITextureCacheKey& _key)
{
    for (int idx = 0; idx < entries_.Size(); ++idx)
    {
        CacheEntry& e = entries_[idx];

        bool match = e.key.bakerVersion == _key.bakerVersion
            && e.key.mapperVersion == _key.mapperVersion
            && e.key.recipeVersion == _key.recipeVersion
            && e.key.dpiScaleQ8 == _key.dpiScaleQ8
            && e.key.pixelFormat == _key.pixelFormat
            && e.key.resolvedStyleHash == _key.resolvedStyleHash
            && e.key.recipeHash == _key.recipeHash
            && e.key.stateHash == _key.stateHash;

        if (match)
        {
            e.lastAccessFrame = ++frameCounter_;
            return e.entry;
        }
    }

    return nullptr;
}

void UITextureCache::Insert(const UITextureCacheKey& _key, UITextureEntry* _entry)
{
    CC_SAFE_RETAIN(_entry);

    CacheEntry e;
    e.key = _key;
    e.entry = _entry;
    e.lastAccessFrame = ++frameCounter_;

    size_t entryBytes = _entry->texture
        ? _entry->texture->getContentSizeInPixels().width * _entry->texture->getContentSizeInPixels().height * 4
        : 0;

    currentMemoryBytes_ += entryBytes;
    entries_.PushBack(e);

    EvictIfNeeded();
}

void UITextureCache::Remove(const UITextureCacheKey& _key)
{
    for (int idx = entries_.Size() - 1; idx >= 0; --idx)
    {
        CacheEntry& e = entries_[idx];

        bool match = e.key.bakerVersion == _key.bakerVersion
            && e.key.mapperVersion == _key.mapperVersion
            && e.key.recipeVersion == _key.recipeVersion
            && e.key.dpiScaleQ8 == _key.dpiScaleQ8
            && e.key.pixelFormat == _key.pixelFormat
            && e.key.resolvedStyleHash == _key.resolvedStyleHash
            && e.key.recipeHash == _key.recipeHash
            && e.key.stateHash == _key.stateHash;

        if (match)
        {
            size_t entryBytes = e.entry && e.entry->texture
                ? e.entry->texture->getContentSizeInPixels().width * e.entry->texture->getContentSizeInPixels().height * 4
                : 0;

            currentMemoryBytes_ -= entryBytes;
            CC_SAFE_RELEASE(e.entry);
            entries_.RemoveAt(idx);
            return;
        }
    }
}

void UITextureCache::Clear()
{
    for (int idx = 0; idx < entries_.Size(); ++idx)
    {
        CC_SAFE_RELEASE(entries_[idx].entry);
    }
    entries_.Clear();
    currentMemoryBytes_ = 0;
}

void UITextureCache::EvictIfNeeded()
{
    while (currentMemoryBytes_ > maxMemoryBytes_ && entries_.Size() > 1)
    {
        int lruIndex = 0;
        uint64_t oldestFrame = entries_[0].lastAccessFrame;

        for (int idx = 1; idx < entries_.Size(); ++idx)
        {
            if (entries_[idx].lastAccessFrame < oldestFrame)
            {
                oldestFrame = entries_[idx].lastAccessFrame;
                lruIndex = idx;
            }
        }

        CacheEntry& victim = entries_[lruIndex];
        size_t victimBytes = victim.entry && victim.entry->texture
            ? victim.entry->texture->getContentSizeInPixels().width * victim.entry->texture->getContentSizeInPixels().height * 4
            : 0;

        currentMemoryBytes_ -= victimBytes;
        CC_SAFE_RELEASE(victim.entry);
        entries_.RemoveAt(lruIndex);
    }
}

void UITextureDeferredRelease::ReleaseAfterFrames(cc::Ref* _obj, int _frames)
{
    if (!_obj) return;

    DeferredEntry e;
    e.obj = _obj;
    e.remainingFrames = _frames;
    queue_.PushBack(e);
}

void UITextureDeferredRelease::Update()
{
    for (int idx = queue_.Size() - 1; idx >= 0; --idx)
    {
        --queue_[idx].remainingFrames;
        if (queue_[idx].remainingFrames <= 0)
        {
            CC_SAFE_RELEASE(queue_[idx].obj);
            queue_.RemoveAt(idx);
        }
    }
}
