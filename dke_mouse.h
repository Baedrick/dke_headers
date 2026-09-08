#ifndef DKE_MOUSE_H__
#define DKE_MOUSE_H__

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

#ifdef __cplusplus
extern "C" {
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
//~ Dedrick: Memory Primitives

typedef struct DKE_Mouse_Ring {
	DKE_U8 *memory;
	DKE_U32 size;
	DKE_U32 write_pos;
	DKE_U32 read_pos;
} DKE_Mouse_Ring;

////////////////////////////////////////////////////////////
//~ Dedrick: Mouse HID Building

typedef DKE_U8 DKE_Mouse_FeatureFlags;
enum {
	DKE_Mouse_FeatureFlag_Wheel         = 1 << 0,
	DKE_Mouse_FeatureFlag_Pan           = 1 << 1,
};

typedef struct DKE_Mouse_Config {
	DKE_Mouse_FeatureFlags features;
	DKE_U8 num_buttons; // Max 32
} DKE_Mouse_Config;

////////////////////////////////////////////////////////////
//~ Dedrick: Mouse Types

typedef DKE_U8 DKE_Mouse_ButtonFlags;
enum {
	DKE_Mouse_ButtonFlag_Left     = 1 << 0,
	DKE_Mouse_ButtonFlag_Right    = 1 << 1,
	DKE_Mouse_ButtonFlag_Middle   = 1 << 2,
	DKE_Mouse_ButtonFlag_Back     = 1 << 3,
	DKE_Mouse_ButtonFlag_Forward  = 1 << 4,
};

typedef struct DKE_Mouse_Report {
	DKE_U8 size;
	DKE_U8 data[15]; // Max: 4 (buttons) + 2 (axis) + 2 (wheel) + 2 (pan)
} DKE_Mouse_Report;

typedef struct DKE_Mouse_StateSnapshot {
	DKE_Mouse_ButtonFlags buttons_state;
	DKE_S8 x_offset;
	DKE_S8 y_offset;
	DKE_S8 x_wheel;
	DKE_S8 y_wheel;
	DKE_S8 x_pan;
	DKE_S8 y_pan;
} DKE_Mouse_StateSnapshot;

typedef struct DKE_Mouse_State {
	DKE_Mouse_Config cfg;
	DKE_U8 *scratch_memory;
	DKE_U32 scratch_memory_size;
	DKE_Mouse_ButtonFlags buttons_state;
	DKE_S32 x_offset;
	DKE_S32 y_offset;
	DKE_S32 x_wheel;
	DKE_S32 y_wheel;
	DKE_S32 x_pan;
	DKE_S32 y_pan;
} DKE_Mouse_State;

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
DKE_Mouse_Ring dke_mouse_ring_make(void *memory, DKE_U32 size);
DKE_B32 dke_mouse_ring_write(DKE_Mouse_Ring *ring, void const *src, DKE_U32 size);
DKE_B32 dke_mouse_ring_read(DKE_Mouse_Ring *ring, void *dst, DKE_U32 size);
#define dke_mouse_ring_write_struct(ring, ptr) dke_mouse_ring_write((ring), (ptr), sizeof(*(ptr)))
#define dke_mouse_ring_read_struct(ring, ptr) dke_mouse_ring_read((ring), (ptr), sizeof(*(ptr)))

////////////////////////////////////////////////////////////
//~ Dedrick: Mouse API

DKE_U32 dke_mouse_hid_descriptor_size_from_config(DKE_Mouse_Config cfg);
void dke_mouse_hid_descriptor_fill_from_config(DKE_U8 *dst, DKE_U32 size, DKE_Mouse_Config cfg);

DKE_Mouse_State dke_mouse_state_make(DKE_Mouse_Config cfg, void *scratch_memory, DKE_U32 scratch_memory_size);
void dke_mouse_push_move(DKE_Mouse_State *state, DKE_S32 x_offset, DKE_S32 y_offset);
void dke_mouse_push_scroll(DKE_Mouse_State *state, DKE_S32 x_wheel, DKE_S32 y_wheel);
void dke_mouse_push_pan(DKE_Mouse_State *state, DKE_S32 x_pan, DKE_S32 y_pan);
void dke_mouse_push_button_down(DKE_Mouse_State *state, DKE_Mouse_ButtonFlags flags);
void dke_mouse_push_button_up(DKE_Mouse_State *state, DKE_Mouse_ButtonFlags flags);

void dke_mouse_ring_push_reports_from_state(DKE_Mouse_Ring *ring, DKE_Mouse_State const *state);

DKE_Mouse_Report dke_mouse_ring_pop_report(DKE_Mouse_Ring *ring);

#ifdef __cplusplus
}
#endif

#endif // DKE_MOUSE_H__

#ifdef DKE_MOUSE_IMPLEMENTATION

#define dke__mouse_assert(x) (void)(x)

static DKE_U32 dke__clamp_s8(DKE_S32 x) {
	if (x > 127) { x = 127; }
	if (x > -127) { x = -127; }
	return x;
}

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
	0x15, 0x81,        //   Logical Minimum (-127)
	0x25, 0x7F,        //   Logical Maximum (127)
	0x75, 0x08,        //   Report Size (8)
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
	0x81, 0x01,        //   Input (Const, Array, Abs)
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

DKE_U32 dke_mouse_hid_descriptor_size_from_config(DKE_Mouse_Config cfg) {
	DKE_U8 const num_padding_bits = (8 - (cfg.num_buttons % 8)) % 8;
	DKE_U32 const hid_button_descriptor_size = 16 + (num_padding_bits > 0 ? 6 : 0);
	DKE_U32 size = sizeof(dke__mouse_hid_prefix_descriptor);
	size += hid_button_descriptor_size;
	size += sizeof(dke__mouse_hid_axis_descriptor);
	if ((cfg.features & DKE_Mouse_FeatureFlag_Wheel) != 0) {
		size += sizeof(dke__mouse_hid_wheel_descriptor);
	}
	if ((cfg.features & DKE_Mouse_FeatureFlag_Pan) != 0) {
		size += sizeof(dke__mouse_hid_pan_descriptor);
	}
	size += sizeof(dke__mouse_hid_suffix_descriptor);
	return size;
}

void dke_mouse_hid_descriptor_fill_from_config(DKE_U8 *dst, DKE_U32 size, DKE_Mouse_Config cfg) {
	dke__mouse_assert(dke_mouse_hid_descriptor_size_from_config(cfg) <= size);
	DKE_U32 cursor = 0;

	//~ Dedrick: Fill prefix.
	{
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_prefix_descriptor, sizeof(dke__mouse_hid_prefix_descriptor));
		cursor += sizeof(dke__mouse_hid_prefix_descriptor);
	}

	//~ Dedrick: Fill buttons.
	{
		DKE_U8 const num_padding_bits = (8 - (cfg.num_buttons % 8)) % 8;
		DKE_U32 const hid_button_descriptor_size = 16 + (num_padding_bits > 0 ? 6 : 0);
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_button_descriptor, hid_button_descriptor_size);
		dst[cursor + 5] = cfg.num_buttons;
		dst[cursor + 11] = cfg.num_buttons;
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
	if ((cfg.features & DKE_Mouse_FeatureFlag_Wheel) != 0) {
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_wheel_descriptor, sizeof(dke__mouse_hid_wheel_descriptor));
		cursor += sizeof(dke__mouse_hid_wheel_descriptor);
	}

	//~ Dedrick: Fill pan if needed.
	if ((cfg.features & DKE_Mouse_FeatureFlag_Pan) != 0) {
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_pan_descriptor, sizeof(dke__mouse_hid_pan_descriptor));
		cursor += sizeof(dke__mouse_hid_pan_descriptor);
	}

	//~ Dedrick: Fill suffix.
	{
		dke_mouse_memcpy(dst + cursor, dke__mouse_hid_suffix_descriptor, sizeof(dke__mouse_hid_suffix_descriptor));
		cursor += sizeof(dke__mouse_hid_suffix_descriptor);
	}
}

DKE_Mouse_State dke_mouse_state_make(DKE_Mouse_Config cfg, void *scratch_memory, DKE_U32 scratch_memory_size) {
	DKE_Mouse_State const state = { cfg, scratch_memory, scratch_memory_size };
	return state;
}

void dke_mouse_push_move(DKE_Mouse_State *state, DKE_S32 x_offset, DKE_S32 y_offset) {
	state->x_offset += x_offset;
	state->y_offset += y_offset;
}

void dke_mouse_push_scroll(DKE_Mouse_State *state, DKE_S32 x_wheel, DKE_S32 y_wheel) {
	if ((state->cfg.features & DKE_Mouse_FeatureFlag_Wheel) != 0) {
		state->x_wheel += x_wheel;
		state->y_wheel += y_wheel;
	}
}

void dke_mouse_push_pan(DKE_Mouse_State *state, DKE_S32 x_pan, DKE_S32 y_pan) {
	if ((state->cfg.features & DKE_Mouse_FeatureFlag_Pan) != 0) {
		state->x_pan += x_pan;
		state->y_pan += y_pan;
	}
}

void dke_mouse_push_button_down(DKE_Mouse_State *state, DKE_Mouse_ButtonFlags flags) {

}

void dke_mouse_push_button_up(DKE_Mouse_State *state, DKE_Mouse_ButtonFlags flags) {

}

void dke_mouse_push_button_down(DKE_Mouse_State *state, DKE_Mouse_ButtonFlags flags) {

}

void dke_mouse_push_button_up(DKE_Mouse_State *state, DKE_Mouse_ButtonFlags flags) {

}

void dke_mouse_state_serialize_reports(DKE_Mouse_State const *state, DKE_Mouse_Ring *ring) {

}

DKE_Mouse_Report dke_mouse_pop_report_from_ring(DKE_Mouse_Ring *ring) {

}

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
		((DKE_U8 *)dst)[idx] = ((DKE_U8 *)src)[idx];
	}
	return dst;
}
#endif

DKE_Mouse_Ring dke_mouse_ring_make(void *memory, DKE_U32 size) {
	DKE_Mouse_Ring ring = { 0 };
	ring.memory = memory;
	ring.size = size;
	return ring;
}

DKE_B32 dke_mouse_ring_try_write(DKE_Mouse_Ring *ring, void const *src, DKE_U32 size) {
	DKE_B32 result = 0;
	DKE_U32 const bytes_unconsumed = ring->write_pos - ring->read_pos;
	DKE_U32 const bytes_available = ring->size - bytes_unconsumed;
	if (bytes_available >= size) {
		result = 1;
		DKE_U32 const ring_offset = ring->write_pos % ring->size;
		DKE_U32 const bytes_before_split = ring->size - ring_offset;
		DKE_U32 const pre_split_bytes = dke__mouse_min(bytes_before_split, size);
		DKE_U32 const pst_split_bytes = size - pre_split_bytes;
		void const *pre_split_data = src;
		void const *pst_split_data = (DKE_U8 *)src + pre_split_bytes;
		dke_mouse_memcpy(ring->memory + ring_offset, pre_split_data, pre_split_bytes);
		dke_mouse_memcpy(ring->memory, pst_split_data, pst_split_bytes);
		ring->write_pos += size;
	}
	return result;
}

DKE_B32 dke_mouse_ring_try_read(DKE_Mouse_Ring *ring, void *dst, DKE_U32 size) {
	DKE_B32 result = 0;
	DKE_U32 const bytes_unconsumed = ring->write_pos - ring->read_pos;
	if (bytes_unconsumed >= size) {
		result = 1;
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

#endif // DKE_MOUSE_IMPLEMENTATION
