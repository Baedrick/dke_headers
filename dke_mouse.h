#ifndef DKE_MOUSE_H__
#define DKE_MOUSE_H__

#ifndef DKE_MOUSE_QUEUE_CAPACITY
#	define DKE_MOUSE_QUEUE_CAPACITY 32
#endif

//~ Dedrick: Basic types
#ifndef DKE_BASIC_TYPES
#define DKE_BASIC_TYPES
#	include <stdint.h>
typedef uint8_t  DKE_U8;
typedef uint16_t DKE_U16;
typedef uint32_t DKE_U32;
typedef uint64_t DKE_U64;
typedef int8_t  DKE_S8;
typedef int16_t DKE_S16;
typedef int32_t DKE_S32;
typedef int64_t DKE_S64;
typedef DKE_S32 DKE_B32;
#endif // DKE_BASIC_TYPES

#ifdef __cplusplus
extern "C" {
#endif

typedef DKE_U8 DKE_Mouse_ButtonFlags;
enum {
	DKE_Mouse_ButtonFlag_Left     = 1 << 0,
	DKE_Mouse_ButtonFlag_Right    = 1 << 1,
	DKE_Mouse_ButtonFlag_Middle   = 1 << 2,
	DKE_Mouse_ButtonFlag_Back     = 1 << 3,
	DKE_Mouse_ButtonFlag_Forward  = 1 << 4,
};

// TODO(Dedrick): 5 button mouse with 2 scroll axis.
extern const DKE_U8 dke_mouse_hid_descriptor[52];
extern const DKE_U32 dke_mouse_hid_descriptor_size;

typedef struct DKE_Mouse_Report {
	DKE_Mouse_ButtonFlags button_flags;
	DKE_S8 x_offset;
	DKE_S8 y_offset;
	DKE_S8 x_scroll;
	DKE_S8 y_scroll;
} DKE_Mouse;

typedef struct DKE_Mouse_Arena {
	DKE_U8 *memory;
	DKE_U32 size;
	DKE_U32 pos;
} DKE_Mouse_Arena;

typedef struct DKE_Mouse_Ring {
	DKE_U8 *memory;
	DKE_U32 size;
	DKE_U32 write_pos;
	DKE_U32 read_pos;
} DKE_Mouse_Ring;

typedef struct DKE_Mouse_Context {
	DKE_Mouse_Arena *arena;
	DKE_Mouse_Ring report_ring;
} DKE_Mouse_Context;

DKE_U32 dke_mouse_min_memory_size(void);

DKE_Mouse_Arena *dke_mouse_arena_make(void *memory, DKE_U32 size);
void *dke_mouse_arena_push(DKE_Mouse_Arena *arena, DKE_U32 size, DKE_U32 align);

DKE_Mouse_Ring dke_mouse_ring_make(void *memory, DKE_U32 size);
DKE_U32 dke_mouse_ring_write(DKE_Mouse_Ring *ring, void const *src, DKE_U32 size);
DKE_U32 dke_mouse_ring_read(DKE_Mouse_Ring *ring, void *dst, DKE_U32 size);
#define dke_mouse_ring_write_struct(ring, ptr) dke_mouse_ring_write((ring), (ptr), sizeof(*(ptr)))
#define dke_mouse_ring_read_struct(ring, ptr) dke_mouse_ring_read((ring), (ptr), sizeof(*(ptr)))

DKE_Mouse_Context *dke_mouse_initialize(DKE_Mouse_Arena *arena);

void dke_mouse_push_move(DKE_Mouse_Context *m, DKE_S16 x_offset, DKE_S16 y_offset);
void dke_mouse_push_scroll(DKE_Mouse_Context *m, DKE_S16 x_offset, DKE_S16 y_offset);
void dke_mouse_push_button_down(DKE_Mouse_Context *m, DKE_Mouse_ButtonFlags flags);
void dke_mouse_push_button_up(DKE_Mouse_Context *m, DKE_Mouse_ButtonFlags flags);

#ifdef __cplusplus
}
#endif

#endif // DKE_MOUSE_H__

#ifdef DKE_MOUSE_IMPLEMENTATION

#define dke__mouse_align_pow2(x, a) (((x) + (a) - 1) & ~((a) - 1))

const DKE_U8 dke_mouse_hid_descriptor[52] = {
	// TODO(Dedrick): https://www.usb.org/sites/default/files/hut1_6.pdf
};
const DKE_U32 dke_mouse_hid_descriptor_size = sizeof(dke_mouse_hid_descriptor);

DKE_U32 dke_mouse_min_memory_size(void) {
	// TODO(Dedrick): Alloc
	return 0;
}

DKE_Mouse_Arena *dke_mouse_arena_make(void *memory, DKE_U32 size) {
	return NULL;
}

void *dke_mouse_arena_push(DKE_Mouse_Arena *arena, DKE_U32 size, DKE_U32 align) {
	void *result = NULL;

	return result;
}

DKE_Mouse_Ring dke_mouse_ring_make(void *memory, DKE_U32 size) {
	DKE_Mouse_Ring ring = { 0 };
	ring.memory = memory;
	ring.size = size;
	return ring;
}

DKE_U32 dke_mouse_ring_write(DKE_Mouse_Ring *ring, void const *src, DKE_U32 size) {

}

DKE_U32 dke_mouse_ring_read(DKE_Mouse_Ring *ring, void *dst, DKE_U32 size) {

}

#endif // DKE_MOUSE_IMPLEMENTATION
