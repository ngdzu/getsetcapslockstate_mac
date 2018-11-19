/* file: main.c function: get and set capslock state complie: gcc -std=c99 -framework IOKit -framework CoreFoundation -o
 * capslock main.c usage: ./capslock [0|1|-1] 0 : capslock off 1 : capslock on -1 : toggle capslock state : print
 * current capslock state (0|1) * written and published as free software by Hiroto, 2015-06 */
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDParameter.h>
#include <libgen.h> // basename
#define CAPSLOCK_OFF 0
#define CAPSLOCK_ON 1
#define CAPSLOCK_TOGGLE -1
#define CAPSLOCK_QUERY 9
int main(int argc, char **argv) {
	kern_return_t kr;
	io_service_t ios;
	io_connect_t ioc;
	CFMutableDictionaryRef mdict;
	int op;
	bool state;
	if (argc < 2)
		op = CAPSLOCK_QUERY;
	else
		op = atoi(argv[1]);
	if (op != CAPSLOCK_ON && op != CAPSLOCK_OFF && op != CAPSLOCK_TOGGLE && op != CAPSLOCK_QUERY) {
		fprintf(stderr, "Usage: %s %s\n\t\%s\n\t%s\n\t%s\n\t%s\n", basename(argv[0]), "[0|1|-1]", " 0 : capslock off",
				" 1 : capslock on", "-1 : toggle capslock state", " : print current capslock state (0|1)");
		return 1;
	}
	mdict = IOServiceMatching(kIOHIDSystemClass);
	ios = IOServiceGetMatchingService(kIOMasterPortDefault, (CFDictionaryRef)mdict);
	if (!ios) {
		if (mdict)
			CFRelease(mdict);
		fprintf(stderr, "IOServiceGetMatchingService() failed: %x\n", kr);
		return (int)kr;
	}
	kr = IOServiceOpen(ios, mach_task_self(), kIOHIDParamConnectType, &ioc);
	IOObjectRelease(ios);
	if (kr != KERN_SUCCESS) {
		fprintf(stderr, "IOServiceOpen() failed: %x\n", kr);
		return (int)kr;
	}
	switch (op) {
		case CAPSLOCK_ON:
		case CAPSLOCK_OFF:
			state = (op == CAPSLOCK_ON);
			kr = IOHIDSetModifierLockState(ioc, kIOHIDCapsLockState, state);
			if (kr != KERN_SUCCESS) {
				IOServiceClose(ioc);
				fprintf(stderr, "IOHIDSetModifierLockState() failed: %x\n", kr);
				return (int)kr;
			}
			break;
		case CAPSLOCK_TOGGLE:
			kr = IOHIDGetModifierLockState(ioc, kIOHIDCapsLockState, &state);
			if (kr != KERN_SUCCESS) {
				IOServiceClose(ioc);
				fprintf(stderr, "IOHIDGetModifierLockState() failed: %x\n", kr);
				return (int)kr;
			}
			state = !state;
			kr = IOHIDSetModifierLockState(ioc, kIOHIDCapsLockState, state);
			if (kr != KERN_SUCCESS) {
				IOServiceClose(ioc);
				fprintf(stderr, "IOHIDSetModifierLockState() failed: %x\n", kr);
				return (int)kr;
			}
			break;
		case CAPSLOCK_QUERY:
			kr = IOHIDGetModifierLockState(ioc, kIOHIDCapsLockState, &state);
			if (kr != KERN_SUCCESS) {
				IOServiceClose(ioc);
				fprintf(stderr, "IOHIDGetModifierLockState() failed: %x\n", kr);
				return (int)kr;
			}
			break;
	}
	IOServiceClose(ioc);
	fprintf(stdout, "%d", (int)state);
	return 0;
}
