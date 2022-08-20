#import <Cocoa/Cocoa.h>

@interface MacView : NSView {

}
- (void)drawRect:(NSRect)rect;
@end

@implementation MacView

#define X(t) (sin(t) + 1) * width * 0.5
#define Y(t) (cos(t) + 1) * height * 0.5

- (void)drawRect:(NSRect)rect {
    double f,g;
    const double pi = 2.0 * acos(0.0);

    int n = 12;

    float width = [self bounds].size.width;
    float height = [self bounds].size.height;

    [[NSColor whiteColor] set];
    NSRectFill([self bounds]);

    [[NSColor blackColor] set];
    
    for (f = 0; f < 2 * pi; f += 2.0 * pi / n) {
        for (g = 0; g < 2.0 * pi; g += pi / n) {
            NSPoint p1 = NSMakePoint(X(f), Y(f));
            NSPoint p2 = NSMakePoint(X(g), Y(g));
            [NSBezierPath strokeLineFromPoint:p1 toPoint:p2];
        }
    }
}

- (void)windowWillClose:(NSNotification *)notification {
    [NSApp terminate:self];
}
@end

void setup() {
    NSWindow *window;
    NSView *view;
    NSRect rect;

    rect = NSMakeRect(100.0, 350.0, 400.0, 400.0);
    window = [[NSWindow alloc] initWithContentRect: rect
                                         styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];

    [window setTitle:@"Mac Application"];
    view = [[[MacView alloc] initWithFrame:rect] autorelease];
    [window setContentView:view];
    [window setDelegate:view];
    [window makeKeyAndOrderFront:nil];
}

int main(int argc, char **argv) {
    @autoreleasepool {
       NSApp = [NSApplication sharedApplication];

       setup();

       [NSApp run];
    }
    
    return 0;
}
