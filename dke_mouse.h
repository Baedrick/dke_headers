// dke_mouse - v0.1 - public domain mouse interface
//
// Distributed under dual-license, see notice at the end of this file.
//
// #define DKE_MOUSE_IMPLEMENTATION
// before you include this file in one C or C++ file to create the implementation.
//
// i.e. it should look like this
// #include ...
// #include ...
// #define DKE_MOUSE_IMPLEMENTATION
// #include "dke_mouse.h"
//
// You can #define dke_mouse_assert(x) before the #include to avoid using assert.h

#ifndef DKE_MOUSE_H__
#define DKE_MOUSE_H__

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////
//~ Dedrick: Recommended Default Memory Sizes
//
// These values are determined from back-of-the-envelope calculations and
// real world values. These defaults are derived from common wireless/wired
// gaming mice, which means it should cover any office mouse. Power users
// may calculate their own defaults instead.
//
// These are the reference specs:
// - Polling Rate: 1000Hz (1ms)
// - Sensor Resolution: 3200DPI
// - Physical Move Ceiling: 200IPS (~5.08m/s)
// - USB Report Rate: 1000Hz (1ms)
// - BLE Rate: 133Hz (7.5ms) / 66.7 Hz (15ms)
//
// Peak Movement Load Derivation:
// The max displacement per frame (1ms @ 1000Hz):
//
//   200IPS * 3200DPI = 640,000 counts/sec
//   640,000 counts/sec / 1,000 frames/sec = 640 counts/frame
//
// HID relative axes use signed 16-bit values clamped to [-32767, 32767].
// A single HID report can only convey up to 32767 counts per axis.
// Therefore, the minimum reports required:
//
//   ceil(640 counts / 32,767) = 1 report/frame
//
// Additional snapshots per frame are needed only when button state
// changes mid-frame (button barriers) to preserve click order relative
// to motion. 4 snapshots allow up to 3 button transitions per frame.

#define DKE_MOUSE_DEFAULT_FRAME_SCRATCH_SIZE     (4 * sizeof(DKE_Mouse_FrameSnapshot))
#define DKE_MOUSE_DEFAULT_REPORTS_PER_FRAME_SIZE (4 * sizeof(DKE_Mouse_Report))
#define DKE_MOUSE_DEFAULT_RING_SIZE_USB 512
#define DKE_MOUSE_DEFAULT_RING_SIZE_BLE 1024

////////////////////////////////////////////////////////////
//~ Dedrick: Overrideable Basic Operations

#if !defined(dke_mouse_assert)
#	include <assert.h>
#	define dke_mouse_assert assert
#endif

////////////////////////////////////////////////////////////
//~ Dedrick: Overrideable Memory Operations
//
// To override the slow/default memory implementation used by the library,
// do the following:
//
// #define DKE_MOUSE_MEMSET_OVERRIDE
// #define dke_mouse_memset <your_memset>
//
// #define DKE_MOUSE_MEMCPY_OVERRIDE
// #define dke_mouse_memcpy <your_memcpy>

#if !defined(dke_mouse_memset)
#	define dke_mouse_memset dke_mouse_memset_fallback
#endif

#if !defined(dke_mouse_memcpy)
#	define dke_mouse_memcpy dke_mouse_memcpy_fallback
#endif

////////////////////////////////////////////////////////////
//~ Dedrick: Basic Integer Types

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

////////////////////////////////////////////////////////////
//~ Dedrick: Memory Primitives

#define DKE_MOUSE_ARENA_HEADER_SIZE 32

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

////////////////////////////////////////////////////////////
//~ Dedrick: Mouse HID Building

typedef DKE_U8 DKE_Mouse_UsageFlags;
enum {
	DKE_Mouse_UsageFlag_Wheel  = 1 << 0,
	DKE_Mouse_UsageFlag_Pan    = 1 << 1,
};

typedef struct DKE_Mouse_Spec {
	DKE_Mouse_UsageFlags usage;
	DKE_U8 num_buttons; // Max 32
} DKE_Mouse_Spec;

////////////////////////////////////////////////////////////
//~ Dedrick: Mouse Types

typedef DKE_U32 DKE_Mouse_ButtonFlags;
enum {
	DKE_Mouse_ButtonFlag_Left     = 1 << 0,
	DKE_Mouse_ButtonFlag_Right    = 1 << 1,
	DKE_Mouse_ButtonFlag_Middle   = 1 << 2,
	DKE_Mouse_ButtonFlag_Back     = 1 << 3,
	DKE_Mouse_ButtonFlag_Forward  = 1 << 4,

	//~ Dedrick: Generic button names.
	DKE_Mouse_ButtonFlag_Button6   = 1 << 5,
	DKE_Mouse_ButtonFlag_Button7   = 1 << 6,
	DKE_Mouse_ButtonFlag_Button8   = 1 << 7,
	DKE_Mouse_ButtonFlag_Button9   = 1 << 8,
	DKE_Mouse_ButtonFlag_Button10  = 1 << 9,
	DKE_Mouse_ButtonFlag_Button11  = 1 << 10,
	DKE_Mouse_ButtonFlag_Button12  = 1 << 11,
	DKE_Mouse_ButtonFlag_Button13  = 1 << 12,
	DKE_Mouse_ButtonFlag_Button14  = 1 << 13,
	DKE_Mouse_ButtonFlag_Button15  = 1 << 14,
	DKE_Mouse_ButtonFlag_Button16  = 1 << 15,
	DKE_Mouse_ButtonFlag_Button17  = 1 << 16,
	DKE_Mouse_ButtonFlag_Button18  = 1 << 17,
	DKE_Mouse_ButtonFlag_Button19  = 1 << 18,
	DKE_Mouse_ButtonFlag_Button20  = 1 << 19,
	DKE_Mouse_ButtonFlag_Button21  = 1 << 20,
	DKE_Mouse_ButtonFlag_Button22  = 1 << 21,
	DKE_Mouse_ButtonFlag_Button23  = 1 << 22,
	DKE_Mouse_ButtonFlag_Button24  = 1 << 23,
	DKE_Mouse_ButtonFlag_Button25  = 1 << 24,
	DKE_Mouse_ButtonFlag_Button26  = 1 << 25,
	DKE_Mouse_ButtonFlag_Button27  = 1 << 26,
	DKE_Mouse_ButtonFlag_Button28  = 1 << 27,
	DKE_Mouse_ButtonFlag_Button29  = 1 << 28,
	DKE_Mouse_ButtonFlag_Button30  = 1 << 29,
	DKE_Mouse_ButtonFlag_Button31  = 1 << 30,
	DKE_Mouse_ButtonFlag_Button32  = 1 << 31,
};

typedef DKE_U8 DKE_Mouse_EventKind;
enum {
	DKE_Mouse_EventKind_Null = 0,
	DKE_Mouse_EventKind_Move,
	DKE_Mouse_EventKind_Wheel,
	DKE_Mouse_EventKind_Pan,
	DKE_Mouse_EventKind_ButtonDown,
	DKE_Mouse_EventKind_ButtonUp,
	DKE_Mouse_EventKind_COUNT,
};

typedef struct DKE_Mouse_EventMove {
	DKE_Mouse_EventKind kind;
	DKE_S32 x_offset;
	DKE_S32 y_offset;
} DKE_Mouse_EventMove;

typedef struct DKE_Mouse_EventWheel {
	DKE_Mouse_EventKind kind;
	DKE_S32 offset;
} DKE_Mouse_EventWheel;

typedef struct DKE_Mouse_EventPan {
	DKE_Mouse_EventKind kind;
	DKE_S32 offset;
} DKE_Mouse_EventPan;

typedef struct DKE_Mouse_EventButton {
	DKE_Mouse_EventKind kind;
	DKE_Mouse_ButtonFlags buttons;
} DKE_Mouse_EventButton;

typedef union DKE_Mouse_Event {
	DKE_Mouse_EventKind kind;
	DKE_Mouse_EventMove move;
	DKE_Mouse_EventWheel wheel;
	DKE_Mouse_EventPan pan;
	DKE_Mouse_EventButton button;
} DKE_Mouse_Event;

typedef struct DKE_Mouse_EventNode {
	DKE_Mouse_EventNode *next;
	DKE_Mouse_Event v;
} DKE_Mouse_EventNode;

typedef struct DKE_Mouse_EventList {
	DKE_Mouse_EventNode *first;
	DKE_Mouse_EventNode *last;
	DKE_U32 node_count;
	DKE_U32 barrier_count;
} DKE_Mouse_EventList;

typedef struct DKE_Mouse_Report {
	DKE_U8 size; // Max: 10: 4 (buttons) + 4 (axis) + 1 (wheel) + 1 (pan)
	DKE_U8 data[15];
} DKE_Mouse_Report;

typedef struct DKE_Mouse_ReportChunkNode {
	DKE_Mouse_ReportChunkNode *next;
	DKE_Mouse_Report *v;
	DKE_U32 count;
	DKE_U32 capacity;
} DKE_Mouse_ReportChunkNode;

typedef struct DKE_Mouse_ReportChunkList {
	DKE_Mouse_ReportChunkNode *first;
	DKE_Mouse_ReportChunkNode *last;
	DKE_U32 chunk_count;
	DKE_U32 total_count;
} DKE_Mouse_ReportChunkList;

typedef struct DKE_Mouse_ReportArray {
	DKE_Mouse_Report *v;
	DKE_U32 count;
} DKE_Mouse_ReportArray;

////////////////////////////////////////////////////////////
//~ Dedrick: Basic Helpers

//~ Dedrick: Memory Operations
#if !defined(DKE_MOUSE_MEMSET_OVERRIDE)
void *dke_mouse_memset_fallback(void *dst, DKE_U8 c, DKE_U32 size);
#endif

#if !defined(DKE_MOUSE_MEMCPY_OVERRIDE)
void *dke_mouse_memcpy_fallback(void *dst, void const *src, DKE_U32 size);
#endif

//~ Dedrick: Memory Primitives
DKE_Mouse_Arena *dke_mouse_arena_make(void *memory, DKE_U32 size);
void *dke_mouse_arena_push(DKE_Mouse_Arena *arena, DKE_U32 size, DKE_U32 align);
#define dke_mouse_push_array(arena, T, count, align) (T *)dke_mouse_arena_push((arena), (count) * sizeof(T), align)

void dke_mouse_arena_clear(DKE_Mouse_Arena *arena);
DKE_U32 dke_mouse_arena_pos(DKE_Mouse_Arena *arena);
void dke_mouse_arena_pop(DKE_Mouse_Arena *arena, DKE_U32 amount);
void dke_mouse_arena_pop_to(DKE_Mouse_Arena *arena, DKE_U32 pos);

DKE_Mouse_Ring dke_mouse_ring_make(void *memory, DKE_U32 size);
DKE_B32 dke_mouse_ring_has_items(DKE_Mouse_Ring const *ring);
DKE_B32 dke_mouse_ring_write(DKE_Mouse_Ring *ring, void const *src, DKE_U32 size);
DKE_B32 dke_mouse_ring_read(DKE_Mouse_Ring *ring, void *dst, DKE_U32 size);
#define dke_mouse_ring_write_struct(ring, ptr) dke_mouse_ring_write((ring), (ptr), sizeof(*(ptr)))
#define dke_mouse_ring_read_struct(ring, ptr) dke_mouse_ring_read((ring), (ptr), sizeof(*(ptr)))

////////////////////////////////////////////////////////////
//~ Dedrick: Mouse API

DKE_U32 dke_mouse_hid_descriptor_size_from_spec(DKE_Mouse_Spec spec);
void dke_mouse_hid_descriptor_fill_from_spec(DKE_U8 *dst, DKE_U32 size, DKE_Mouse_Spec spec);

void dke_mouse_event_list_push_move(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_S32 x_offset, DKE_S32 y_offset);
void dke_mouse_event_list_push_wheel(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_S32 offset);
void dke_mouse_event_list_push_pan(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_S32 offset);
void dke_mouse_event_list_push_button_down(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_Mouse_ButtonFlags buttons);
void dke_mouse_event_list_push_button_up(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_Mouse_ButtonFlags buttons);

DKE_Mouse_Report dke_mouse_report_make(DKE_Mouse_Spec spec, DKE_S32 x_offset, DKE_S32 y_offset, DKE_S32 wheel, DKE_S32 pan, DKE_Mouse_ButtonFlags buttons);
void dke_mouse_report_chunk_list_push(DKE_Mouse_Arena *arena, DKE_Mouse_ReportChunkList *list, DKE_U32 capacity, DKE_Mouse_Report report);
DKE_Mouse_ReportArray dke_mouse_report_array_from_chunk_list(DKE_Mouse_Arena *arena, DKE_Mouse_ReportChunkList const *list);

DKE_Mouse_ReportArray dke_mouse_report_array_from_event_list(DKE_Mouse_Arena *arena, DKE_Mouse_Spec spec, DKE_Mouse_EventList const *list);

void dke_mouse_ring_serial_push_reports(DKE_Mouse_Ring *ring, DKE_Mouse_ReportArray const *reports);
DKE_Mouse_Report dke_mouse_ring_serial_pop_report(DKE_Mouse_Ring *ring);

#ifdef __cplusplus
}
#endif

#endif // DKE_MOUSE_H__

#ifdef DKE_MOUSE_IMPLEMENTATION

static const DKE_U8 dke__mouse_hid_prefix_descriptor[] = {
	0x05, 0x01,        // Usage Page (Generic Desktop)
	0x09, 0x02,        // Usage (Mouse)
	0xA1, 0x01,        // Collection (Application)
	0x09, 0x01,        //   Usage (Pointer)
	0xA1, 0x00,        //   Collection (Physical)
};

static const DKE_U8 dke__mouse_hid_axis_descriptor[] = {
	0x05, 0x01,        //   Usage Page (Generic Desktop)
	0x09, 0x30,        //   Usage (X)
	0x09, 0x31,        //   Usage (Y)
	0x16, 0x01, 0x80,  //   Logical Minimum (-32767)
	0x26, 0xFF, 0x7F,  //   Logical Maximum (32767)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x02,        //   Report Count (2)
	0x81, 0x06,        //   Input (Data, Var, Rel)
};

static const DKE_U8 dke__mouse_hid_button_descriptor[] = {
	0x05, 0x09,        //   Usage Page (Button)
	0x19, 0x01,        //   Usage Minimum (1)
	0x29, 0x20,        //   Usage Maximum (N) - NOTE(Dedrick): Defined by impl.
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x95, 0x20,        //   Report Count (N) - NOTE(Dedrick): Defined by impl.
	0x75, 0x01,        //   Report Size (1)
	0x81, 0x02,        //   Input (Data, Var, Abs)
	0x95, 0x00,        //   Report Count (Padding bits) - NOTE(Dedrick): Defined by impl.
	0x75, 0x01,        //   Report Size (1)
	0x81, 0x03,        //   Input (Const, Var, Abs)
};

static const DKE_U8 dke__mouse_hid_wheel_descriptor[] = {
	0x05, 0x01,        //   Usage Page (Generic Desktop)
	0x09, 0x38,        //   Usage (Wheel)
	0x15, 0x81,        //   Logical Minimum (-127)
	0x25, 0x7F,        //   Logical Maximum (127)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x06,        //   Input (Data, Var, Rel)
};

static const DKE_U8 dke__mouse_hid_pan_descriptor[] = {
	0x05, 0x0C,        //   Usage Page (Consumer)
	0x0A, 0x38, 0x02,  //   Usage (AC Pan)
	0x15, 0x81,        //   Logical Minimum (-127)
	0x25, 0x7F,        //   Logical Maximum (127)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x06,        //   Input (Data, Var, Rel)
};

static const DKE_U8 dke__mouse_hid_suffix_descriptor[] = {
	0xC0,              //   End Collection (Physical)
	0xC0,              // End Collection (Application)
};

#define dke__mouse_abs(x) ((x) < 0 ? -(x) : (x))
#define dke__mouse_min(a, b) ((a) < (b) ? (a) : (b))
#define dke__mouse_max(a, b) ((a) < (b) ? (b) : (a))
#define dke__mouse_align_pow2(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define dke__mouse_clamp(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

#define dke__mouse_sll_queue_push(f, l, n) \
do { \
	(n)->next = 0; \
	if (*(f) == 0) { \
		*(f) = *(l) = (n); \
	} else { \
		(*(l))->next = (n); \
		*(l) = (n); \
	} \
} while (0)

#if !defined(DKE_MOUSE_MEMSET_OVERRIDE)
void *dke_mouse_memset_fallback(void *dst, DKE_U8 c, DKE_U32 size) {
	for (DKE_U32 idx = 0; idx < size; ++idx) {
		((DKE_U8 *)dst)[idx] = c;
	}
	return dst;
}
#endif

#if !defined(DKE_MOUSE_MEMCPY_OVERRIDE)
void *dke_mouse_memcpy_fallback(void *dst, void const *src, DKE_U32 size) {
	for (DKE_U32 idx = 0; idx < size; ++idx) {
		((DKE_U8 *)dst)[idx] = ((DKE_U8 const *)src)[idx];
	}
	return dst;
}
#endif // DKE_MOUSE_MEMCPY_OVERRIDE

DKE_Mouse_Arena *dke_mouse_arena_make(void *memory, DKE_U32 size) {
	dke_mouse_assert(size >= DKE_MOUSE_ARENA_HEADER_SIZE);
	DKE_Mouse_Arena *arena = (DKE_Mouse_Arena *)memory;
	arena->memory = (DKE_U8 *)memory;
	arena->size = size;
	arena->pos = DKE_MOUSE_ARENA_HEADER_SIZE;
	return arena;
}

void *dke_mouse_arena_push(DKE_Mouse_Arena *arena, DKE_U32 size, DKE_U32 align) {
	void *result = 0;
	DKE_U32 const align_pos = dke__mouse_align_pow2(arena->pos, align);
	if (align_pos + size <= arena->size) {
		result = arena->memory + align_pos;
		arena->pos = align_pos + size;
		dke_mouse_memset(result, 0, size);
	}
	return result;
}

void dke_mouse_arena_clear(DKE_Mouse_Arena *arena) {
	dke_mouse_arena_pop_to(arena, 0);
}

DKE_U32 dke_mouse_arena_pos(DKE_Mouse_Arena *arena) {
	return arena->pos;
}

void dke_mouse_arena_pop(DKE_Mouse_Arena *arena, DKE_U32 amount) {
	DKE_U32 const curr_pos = dke_mouse_arena_pos(arena);
	DKE_U32 new_pos = curr_pos;
	if (amount < curr_pos) {
		new_pos = curr_pos - amount;
	}
	dke_mouse_arena_pop_to(arena, new_pos);
}

void dke_mouse_arena_pop_to(DKE_Mouse_Arena *arena, DKE_U32 pos) {
	arena->pos = dke__mouse_max(DKE_MOUSE_ARENA_HEADER_SIZE, pos);
}

DKE_Mouse_Ring dke_mouse_ring_make(void *memory, DKE_U32 size) {
	dke_mouse_assert((size & (size - 1)) == 0); // Ring size must be power of two.
	DKE_Mouse_Ring ring = { 0 };
	ring.memory = (DKE_U8 *)memory;
	ring.size = size;
	return ring;
}

DKE_B32 dke_mouse_ring_has_items(DKE_Mouse_Ring const *ring) {
	return ring->write_pos - ring->read_pos > 0;
}

DKE_B32 dke_mouse_ring_write(DKE_Mouse_Ring *ring, void const *src, DKE_U32 size) {
	DKE_B32 result = 0;
	DKE_U32 const bytes_unconsumed = ring->write_pos - ring->read_pos;
	DKE_U32 const bytes_available = ring->size - bytes_unconsumed;
	if (bytes_available >= size) {
		result = 1;
		dke_mouse_assert(size <= ring->size);
		DKE_U32 const ring_offset = ring->write_pos % ring->size;
		DKE_U32 const bytes_before_split = ring->size - ring_offset;
		DKE_U32 const pre_split_bytes = dke__mouse_min(bytes_before_split, size);
		DKE_U32 const pst_split_bytes = size - pre_split_bytes;
		void const *pre_split_data = src;
		void const *pst_split_data = (DKE_U8 const *)src + pre_split_bytes;
		dke_mouse_memcpy(ring->memory + ring_offset, pre_split_data, pre_split_bytes);
		dke_mouse_memcpy(ring->memory, pst_split_data, pst_split_bytes);
		ring->write_pos += size;
	}
	return result;
}

DKE_B32 dke_mouse_ring_read(DKE_Mouse_Ring *ring, void *dst, DKE_U32 size) {
	DKE_B32 result = 0;
	DKE_U32 const bytes_unconsumed = ring->write_pos - ring->read_pos;
	if (bytes_unconsumed >= size) {
		result = 1;
		dke_mouse_assert(size <= ring->size);
		DKE_U32 const ring_offset = ring->read_pos % ring->size;
		DKE_U32 const bytes_before_split = ring->size - ring_offset;
		DKE_U32 const pre_split_bytes = dke__mouse_min(bytes_before_split, size);
		DKE_U32 const pst_split_bytes = size - pre_split_bytes;
		dke_mouse_memcpy(dst, ring->memory + ring_offset, pre_split_bytes);
		dke_mouse_memcpy((DKE_U8 *)dst + pre_split_bytes, ring->memory, pst_split_bytes);
		ring->read_pos += size;
	}
	return result;
}

DKE_U32 dke_mouse_hid_descriptor_size_from_spec(DKE_Mouse_Spec spec) {
	DKE_U8 const num_padding_bits = (8 - (spec.num_buttons % 8)) % 8;
	DKE_U32 const hid_button_descriptor_size = 16 + (num_padding_bits > 0 ? 6 : 0);
	DKE_U32 size = sizeof(dke__mouse_hid_prefix_descriptor);
	size += hid_button_descriptor_size;
	size += sizeof(dke__mouse_hid_axis_descriptor);
	if ((spec.usage & DKE_Mouse_UsageFlag_Wheel) != 0) {
		size += sizeof(dke__mouse_hid_wheel_descriptor);
	}
	if ((spec.usage & DKE_Mouse_UsageFlag_Pan) != 0) {
		size += sizeof(dke__mouse_hid_pan_descriptor);
	}
	size += sizeof(dke__mouse_hid_suffix_descriptor);
	return size;
}

void dke_mouse_hid_descriptor_fill_from_spec(DKE_U8 *dst, DKE_U32 size, DKE_Mouse_Spec spec) {
	dke_mouse_assert(dke_mouse_hid_descriptor_size_from_spec(spec) <= size);
	DKE_U32 cursor = 0;

	//~ Dedrick: Fill prefix.
	{
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_prefix_descriptor, sizeof(dke__mouse_hid_prefix_descriptor));
		cursor += sizeof(dke__mouse_hid_prefix_descriptor);
	}

	//~ Dedrick: Fill buttons.
	{
		DKE_U8 const num_padding_bits = (8 - (spec.num_buttons % 8)) % 8;
		DKE_U32 const hid_button_descriptor_size = 16 + (num_padding_bits > 0 ? 6 : 0);
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_button_descriptor, hid_button_descriptor_size);
		dst[cursor + 5] = spec.num_buttons;
		dst[cursor + 11] = spec.num_buttons;
		if (num_padding_bits > 0) {
			dst[cursor + 17] = num_padding_bits;
		}
		cursor += hid_button_descriptor_size;
	}

	//~ Dedrick: Fill axes.
	{
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_axis_descriptor, sizeof(dke__mouse_hid_axis_descriptor));
		cursor += sizeof(dke__mouse_hid_axis_descriptor);
	}

	//~ Dedrick: Fill wheel if needed.
	if ((spec.usage & DKE_Mouse_UsageFlag_Wheel) != 0) {
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_wheel_descriptor, sizeof(dke__mouse_hid_wheel_descriptor));
		cursor += sizeof(dke__mouse_hid_wheel_descriptor);
	}

	//~ Dedrick: Fill pan if needed.
	if ((spec.usage & DKE_Mouse_UsageFlag_Pan) != 0) {
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_pan_descriptor, sizeof(dke__mouse_hid_pan_descriptor));
		cursor += sizeof(dke__mouse_hid_pan_descriptor);
	}

	//~ Dedrick: Fill suffix.
	{
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_suffix_descriptor, sizeof(dke__mouse_hid_suffix_descriptor));
		cursor += sizeof(dke__mouse_hid_suffix_descriptor);
	}
}

static void dke__mouse_event_list_push(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_Mouse_Event event) {
	DKE_Mouse_EventNode *node = dke_mouse_push_array(arena, DKE_Mouse_EventNode, 1, 4);
	node->v = event;
	dke__mouse_sll_queue_push(&list->first, &list->last, node);
	list->node_count += 1;
}

void dke_mouse_event_list_push_move(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_S32 x_offset, DKE_S32 y_offset) {
	DKE_Mouse_Event event = { DKE_Mouse_EventKind_Move };
	event.move.x_offset = x_offset;
	event.move.y_offset = y_offset;
	dke__mouse_event_list_push(arena, list, event);
}

void dke_mouse_event_list_push_wheel(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_S32 offset) {
	DKE_Mouse_Event event = { DKE_Mouse_EventKind_Wheel };
	event.wheel.offset = offset;
	dke__mouse_event_list_push(arena, list, event);
}

void dke_mouse_event_list_push_pan(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_S32 offset) {
	DKE_Mouse_Event event = { DKE_Mouse_EventKind_Pan };
	event.pan.offset = offset;
	dke__mouse_event_list_push(arena, list, event);
}

void dke_mouse_event_list_push_button_down(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_Mouse_ButtonFlags buttons) {
	DKE_Mouse_Event event = { DKE_Mouse_EventKind_ButtonDown };
	event.button.buttons = buttons;
	dke__mouse_event_list_push(arena, list, event);
}

void dke_mouse_event_list_push_button_up(DKE_Mouse_Arena *arena, DKE_Mouse_EventList *list, DKE_Mouse_ButtonFlags buttons) {
	DKE_Mouse_Event event = { DKE_Mouse_EventKind_ButtonUp };
	event.button.buttons = buttons;
	dke__mouse_event_list_push(arena, list, event);
}

DKE_Mouse_Report dke_mouse_report_make(DKE_Mouse_Spec spec, DKE_S32 x_offset, DKE_S32 y_offset, DKE_S32 wheel, DKE_S32 pan, DKE_Mouse_ButtonFlags buttons) {
	// NOTE(Dedrick): Data coming in needs to be clamped.
	dke_mouse_assert(dke__mouse_abs(x_offset) <= 32767);
	dke_mouse_assert(dke__mouse_abs(y_offset) <= 32767);
	dke_mouse_assert(dke__mouse_abs(wheel) <= 127);
	dke_mouse_assert(dke__mouse_abs(pan) <= 127);

	DKE_Mouse_Report result = { 0 };
	DKE_U32 cursor = 0;

	//~ Dedrick: Fill buttons.
	{
		DKE_U32 const num_button_bytes = (spec.num_buttons + 7) / 8;
		for (DKE_U32 idx = 0; idx < num_button_bytes; ++idx) {
			result.data[cursor++] = (DKE_U8)((buttons >> (idx * 8)) & 0xFF);
		}
	}

	//~ Dedrick: Fill axes.
	{
		result.data[cursor++] = (DKE_U8)(x_offset & 0xFF);
		result.data[cursor++] = (DKE_U8)((x_offset >> 8) & 0xFF);
		result.data[cursor++] = (DKE_U8)(y_offset & 0xFF);
		result.data[cursor++] = (DKE_U8)((y_offset >> 8) & 0xFF);
	}

	//~ Dedrick: Fill wheel if needed.
	if ((spec.usage & DKE_Mouse_UsageFlag_Wheel) != 0) {
		result.data[cursor++] = (DKE_U8)wheel;
	}

	//~ Dedrick: Fill pan if needed.
	if ((spec.usage & DKE_Mouse_UsageFlag_Pan) != 0) {
		result.data[cursor++] = (DKE_U8)pan;
	}

	//~ Dedrick: Write size.
	result.size = (DKE_U8)cursor;

	return result;
}

void dke_mouse_report_chunk_list_push(DKE_Mouse_Arena *arena, DKE_Mouse_ReportChunkList *list, DKE_U32 capacity, DKE_Mouse_Report report) {
	DKE_Mouse_ReportChunkNode *node = list->last;
	if (node == 0 || node->count >= node->capacity) {
		node = dke_mouse_push_array(arena, DKE_Mouse_ReportChunkNode, 1, 4);
		dke_mouse_assert(node != 0);
		dke__mouse_sll_queue_push(&list->first, &list->last, node);
		node->capacity = capacity;
		node->v = dke_mouse_push_array(arena, DKE_Mouse_Report, node->capacity, 1);
		dke_mouse_assert(node->v != 0);
		list->chunk_count += 1;
	}
	dke_mouse_memcpy(&node->v[node->count], &report, sizeof(DKE_Mouse_Report));
	node->count += 1;
	list->total_count += 1;
}

DKE_Mouse_ReportArray dke_mouse_report_array_from_chunk_list(DKE_Mouse_Arena *arena, DKE_Mouse_ReportChunkList const *list) {
  DKE_Mouse_ReportArray array = { 0 };
  array.count = list->total_count;
  array.v = dke_mouse_push_array(arena, DKE_Mouse_Report, array.count, 1);
  dke_mouse_assert(array.v != 0);
  DKE_U32 idx = 0;
  for (DKE_Mouse_ReportChunkNode const *node = list->first; node != 0; node = node->next) {
  	dke_mouse_memcpy(array.v + idx, node->v, node->count * sizeof(DKE_Mouse_Report));
  	idx += node->count;
  }
  return array;
}

DKE_Mouse_ReportArray dke_mouse_report_array_from_event_list(DKE_Mouse_Arena *arena, DKE_Mouse_Spec spec, DKE_Mouse_EventList const *list)  {
	DKE_Mouse_ReportChunkList reports = { 0 };

	//~ Dedrick: Event stream -> reports.
	DKE_S32 x_offset = 0;
	DKE_S32 y_offset = 0;
	DKE_S32 wheel = 0;
	DKE_S32 pan = 0;
	DKE_Mouse_ButtonFlags buttons_state = 0;
	for (DKE_Mouse_EventNode const *node = list->first; node != 0; node = node->next) {
		switch (node->v.kind) {
			case DKE_Mouse_EventKind_Move: {
				x_offset += node->v.move.x_offset;
				y_offset += node->v.move.y_offset;
				break;
			}
			case DKE_Mouse_EventKind_Wheel: {
				dke_mouse_assert((spec.usage & DKE_Mouse_UsageFlag_Wheel) != 0);
				wheel += node->v.wheel.offset;
				break;
			}
			case DKE_Mouse_EventKind_Pan: {
				dke_mouse_assert((spec.usage & DKE_Mouse_UsageFlag_Pan) != 0);
				pan += node->v.pan.offset;
				break;
			}
			case DKE_Mouse_EventKind_ButtonDown: {
			case DKE_Mouse_EventKind_ButtonUp:
				DKE_Mouse_ButtonFlags next_buttons_state = buttons_state;
				if (node->v.kind == DKE_Mouse_EventKind_ButtonDown) {
					next_buttons_state |= node->v.button.buttons;
				}
				else {
					next_buttons_state &= ~node->v.button.buttons;
				}
				if (buttons_state != next_buttons_state) {
					while ((x_offset | y_offset | wheel | pan) != 0) {
						//~ Dedrick: Load clamped deltas.
						DKE_S32 const x_offset_clamped = dke__mouse_clamp(x_offset, -32767, 32767);
						DKE_S32 const y_offset_clamped = dke__mouse_clamp(y_offset, -32767, 32767);
						DKE_S32 const wheel_clamped = dke__mouse_clamp(wheel, -127, 127);
						DKE_S32 const pan_clamped = dke__mouse_clamp(pan, -127, 127);

						//~ Dedrick: Write report.
						DKE_Mouse_Report const report = dke_mouse_report_make(spec, x_offset_clamped, y_offset_clamped, wheel_clamped, pan_clamped, buttons_state);
						dke_mouse_report_chunk_list_push(arena, &reports, 4, report);

						//~ Dedrick: Update accumulators.
						x_offset -= x_offset_clamped;
						y_offset -= y_offset_clamped;
						wheel -= wheel_clamped;
						pan -= pan_clamped;
					}

					//~ Dedrick: Set new button state.
					buttons_state = next_buttons_state;

					//~ Dedrick: Push new state.
					DKE_Mouse_Report const report = dke_mouse_report_make(spec, 0, 0, 0, 0, buttons_state);
					dke_mouse_report_chunk_list_push(arena, &reports, 4, report);
				}
				break;
			}
		}
	}

	//~ Dedrick: Write tail reports.
	while ((x_offset | y_offset | wheel | pan) != 0) {
		//~ Dedrick: Load clamped deltas.
		DKE_S32 const x_offset_clamped = dke__mouse_clamp(x_offset, -32767, 32767);
		DKE_S32 const y_offset_clamped = dke__mouse_clamp(y_offset, -32767, 32767);
		DKE_S32 const wheel_clamped = dke__mouse_clamp(wheel, -127, 127);
		DKE_S32 const pan_clamped = dke__mouse_clamp(pan, -127, 127);

		//~ Dedrick: Write report.
		DKE_Mouse_Report const report = dke_mouse_report_make(spec, x_offset_clamped, y_offset_clamped, wheel_clamped, pan_clamped, buttons_state);
		dke_mouse_report_chunk_list_push(arena, &reports, 4, report);

		//~ Dedrick: Update accumulators.
		x_offset -= x_offset_clamped;
		y_offset -= y_offset_clamped;
		wheel -= wheel_clamped;
		pan -= pan_clamped;
	}

	DKE_Mouse_ReportArray const array = dke_mouse_report_array_from_chunk_list(arena, &reports);
	return array;
}

void dke_mouse_ring_serial_push_reports(DKE_Mouse_Ring *ring, DKE_Mouse_ReportArray const *reports) {
	for (DKE_U32 idx = 0; idx < reports->count; ++idx) {
		DKE_Mouse_Report const *report = &reports->v[idx];
		dke_mouse_ring_write(ring, &report->size, sizeof(report->size));
		dke_mouse_ring_write(ring, report->data, report->size);
	}
}

DKE_Mouse_Report dke_mouse_ring_serial_pop_report(DKE_Mouse_Ring *ring) {
	DKE_Mouse_Report report = { 0 };
	dke_mouse_ring_read(ring, &report.size, sizeof(report.size));
	DKE_B32 const good = dke_mouse_ring_read(ring, &report.data, report.size);
	dke_mouse_assert(good); // Must succeed.
	return report;
}

#endif // DKE_MOUSE_IMPLEMENTATION

// This software is available under two licenses (A) or (B). Choose whichever you prefer.
//
// (A) MIT License
//
// Copyright (c) 2026 Koh Swee Teck Dedrick
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// (B) Public Domain (www.unlicense.org)
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute
// this software, either in source code form or as a compiled binary, for any
// purpose, commercial or non-commercial, and by any means.
//
// In jurisdictions that recognize copyright laws, the author or authors of
// this software dedicate any and all copyright interest in the software to the
// public domain. We make this dedication for the benefit of the public at
// large and to the detriment of our heirs and successors. We intend this
// dedication to be an overt act of relinquishment in perpetuity of all present
// and future rights to this software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
