#import <Cocoa/Cocoa.h>

@interface MacApp : NSWindow<NSApplicationDelegate>

@property (nonatomic, retain) NSOpenGLView *glView;
- (void)drawLoop:(NSTimer *)timer;

@end


static MacApp *app;

// VirtualAlloc equivalent on mac os with mmap taken from:
// https://web.archive.org/web/20160104083454/http://blog.nervus.org/managing-virtual-address-spaces-with-mmap/
void *platform_reserve_memory(u64 size) {
    void *mem = mmap((void*)0, size, PROT_NONE, MAP_PRIVATE|MAP_ANON, -1, 0);
    msync(mem, size, MS_SYNC|MS_INVALIDATE);
    return mem;
}

void platform_commit_memory(void *mem, u64 size) {
    void *tmp = mmap(mem, size, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_SHARED|MAP_ANON, -1, 0);
    msync(mem, size, MS_SYNC|MS_INVALIDATE);
}

void platform_release_memory(void *mem, u64 size) {
    msync(mem, size, MS_SYNC);
    munmap(mem, size);
}

void platform_decommit_memory(void *mem, u64 size) {
    mmap(mem, size, PROT_NONE, MAP_FIXED|MAP_PRIVATE|MAP_ANON, -1, 0);
    msync(mem, size, MS_SYNC|MS_INVALIDATE);
}

void platform_log(char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void platform_swap_buffers() {
    [[app glView] update];
    [[[app glView] openGLContext] flushBuffer];
    
}

b32 platform_read_entire_file(char *file_name, Platform_File *result) {
    FILE *stream = fopen(file_name, "rb");

    fseek(stream, 0, SEEK_END);
    result->size = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    result->data = platform_reserve_memory(result->size);
    platform_commit_memory(result->data, result->size);

    fread(result->data, result->size, 1, stream);
    fclose(stream);

    return 1;
}

void init() {
}

void update() {

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, 0.0f);
    glEnd();
    glFlush();

    platform_swap_buffers();
}

@implementation MacApp

@synthesize glView;

BOOL shouldStop = NO;

- (id)initWithContentRect:(NSRect)contentRect styleMask:(enum NSWindowStyleMask)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag {
    if (self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:flag]) {
        [self setTitle:[[NSProcessInfo processInfo] processName]];

        NSOpenGLPixelFormatAttribute pixelFormatAttributes[] = {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
                NSOpenGLPFAColorSize, 24,
                NSOpenGLPFAAlphaSize, 8,
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFAAccelerated,
                NSOpenGLPFANoRecovery,
                0
        };

        NSOpenGLPixelFormat* format = [[NSOpenGLPixelFormat alloc]initWithAttributes:pixelFormatAttributes];
        glView = [[NSOpenGLView alloc]initWithFrame:contentRect pixelFormat:format];
        
        [[glView openGLContext]makeCurrentContext];
      
        [self setContentView:glView];
        [glView prepareOpenGL];
        [self makeKeyAndOrderFront:self];
        [self setAcceptsMouseMovedEvents:YES];
        [self makeKeyWindow];
        [self setOpaque:YES];

        init();
    }
    return self;
}

- (void)windowWillClose:(NSNotification *)notification {
    [NSApp terminate:self];
}

- (void)drawLoop:(NSTimer *)timer {
    if (shouldStop) {
        [self close];
        return;
    }
    if ([self isVisible]) {
        update();
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    [NSTimer scheduledTimerWithTimeInterval:0.000001 target:self selector:@selector(drawLoop:) userInfo:nil repeats:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication     *)theApplication{
    return YES;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification{
    shouldStop = YES;
}

@end

int main(int argc, char **argv) {
    @autoreleasepool {
        NSApplication *application = [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        app = [[MacApp alloc] initWithContentRect:NSMakeRect(200, 200, 600, 600) styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable backing:NSBackingStoreBuffered defer:YES];

        [application setDelegate:app];
        [application run];
    }
    
    return 0;
}
