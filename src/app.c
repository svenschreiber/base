#include "base.h"
#include "platform.h"
#include "memory_arena.h"

void app_init() {

}

void app_update() {
    
}


// =============================
// >> PLATFORM SPECIFIC

#if defined(BUILD_WIN32)
#include "win32/win32_app.c"
#else
#error The specified platform is not yet supported. Make sure the correct define is set in the build file.
#endif
