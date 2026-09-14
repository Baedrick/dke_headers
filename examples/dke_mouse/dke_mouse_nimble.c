#include <string.h>

#define DKE_MOUSE_MEMSET_OVERRIDE
#define dke_mouse_memset memset
#define DKE_MOUSE_MEMCPY_OVERRIDE
#define dke_mouse_memcpy memcpy
#define DKE_MOUSE_IMPLEMENTATION
#include "../dke_mouse.h"

void app_main(void) {
	// TODO(Dedrick)
}
