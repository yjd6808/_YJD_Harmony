#pragma once
#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/Contents/UIManager.h"

#define REGISTER_UI(name, className)                                    \
    static UIRootGroup* Create##name##UI(UIGroupInfo* _pInfo) {         \
        return dbg_new className(_pInfo);                                \
    }                                                                    \
    namespace {                                                          \
        struct Register##name {                                          \
            Register##name() {                                           \
                UIManager::Get()->RegisterUIFactory(#name,               \
                    &Create##name##UI);                                   \
            }                                                            \
        } g_register_##name;                                             \
    }