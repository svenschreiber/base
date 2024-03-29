#ifndef KEY_INPUT_H
#define KEY_INPUT_H

typedef s32 Key;
enum Key {
#define Key(name, str) KEY_##name,
#include "key_list.inc"
#undef Key
    KEY_MAX
};

typedef s32 Key_Modifiers;
enum Key_Modifiers {
    KEY_MODIFIER_CTRL  = (1<<0),
    KEY_MODIFIER_SHIFT = (1<<1),
    KEY_MODIFIER_ALT   = (1<<2)
};

String get_key_name(s32 index) {
    static char *strings[KEY_MAX] = {
#define Key(name, str) str,
#include "key_list.inc"
#undef Key
    };

    char *string = "INVALID";
    if (index >= 0 && index < KEY_MAX) {
        string = strings[index];
    }

    String result;
    result.str = (u8 *)string;
    result.size = strlen(string);

    return result;
}

#endif
