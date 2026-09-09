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
//   ceil(640 counts / 32767) = 1 report/frame
//
// Additional snapshots per frame are needed only when button state
// changes mid-frame (button barriers) to preserve click order relative
// to motion. 4 snapshots allow up to 3 button transitions per frame.
//

#define DKE_MOUSE_DEFAULT_FRAME_SCRATCH_SIZE     (4 * sizeof(DKE_Mouse_FrameSnapshot))
#define DKE_MOUSE_DEFAULT_REPORTS_PER_FRAME_SIZE (4 * sizeof(DKE_Mouse_Report))
#define DKE_MOUSE_DEFAULT_RING_SIZE_USB 512
#define DKE_MOUSE_DEFAULT_RING_SIZE_BLE 1024

////////////////////////////////////////////////////////////
//~ Dedrick: Overrideable Memory Operations
//
// To override the slow/default memory implementation used by the library,
// do the following:
//
// #define DKE_MOUSE_MEMCPY_OVERRIDE
// #define dke_mouse_memcpy <your_memcpy>

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
	DKE_Mouse_FeatureFlag_Wheel  = 1 << 0,
	DKE_Mouse_FeatureFlag_Pan    = 1 << 1,
};

typedef struct DKE_Mouse_Config {
	DKE_Mouse_FeatureFlags features;
	DKE_U8 num_buttons; // Max 32
} DKE_Mouse_Config;

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

typedef struct DKE_Mouse_Report {
	DKE_U8 size; // Max: 4 (buttons) + 4 (axis) + 1 (x_scroll) + 1 (y_scroll)
	DKE_U8 data[15];
} DKE_Mouse_Report;

typedef struct DKE_Mouse_FrameSnapshot {
	DKE_Mouse_ButtonFlags buttons_state;
	DKE_S16 x_offset;
	DKE_S16 y_offset;
	DKE_S8 wheel;
	DKE_S8 pan;
	DKE_U8 pad[2];
} DKE_Mouse_FrameSnapshot;

typedef struct DKE_Mouse_Frame {
	DKE_Mouse_Config cfg;
	DKE_U8 *scratch_memory;
	DKE_U32 scratch_size;
	DKE_U32 scratch_pos;
	DKE_Mouse_ButtonFlags buttons_state;
	DKE_S32 x_offset;
	DKE_S32 y_offset;
	DKE_S32 wheel;
	DKE_S32 pan;
} DKE_Mouse_Frame;

typedef struct DKE_Mouse_SerializedReports {
	DKE_Mouse_Report const *v;
	DKE_U32 count;
} DKE_Mouse_SerializedReports;

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

DKE_Mouse_Frame dke_mouse_frame_make(DKE_Mouse_Config cfg, void *scratch_memory, DKE_U32 scratch_size);
void dke_mouse_frame_reset(DKE_Mouse_Frame *frame);
void dke_mouse_frame_push_move(DKE_Mouse_Frame *frame, DKE_S32 x_offset, DKE_S32 y_offset);
void dke_mouse_frame_push_wheel(DKE_Mouse_Frame *frame, DKE_S32 offset);
void dke_mouse_frame_push_pan(DKE_Mouse_Frame *frame, DKE_S32 offset);
void dke_mouse_frame_push_button_down(DKE_Mouse_Frame *frame, DKE_Mouse_ButtonFlags buttons);
void dke_mouse_frame_push_button_up(DKE_Mouse_Frame *frame, DKE_Mouse_ButtonFlags buttons);

DKE_U32 dke_mouse_serialized_reports_memory_size_from_frame(DKE_Mouse_Frame const *frame);
DKE_Mouse_SerializedReports dke_mouse_serialized_reports_from_frame(void *memory, DKE_U32 size, DKE_Mouse_Frame const *frame);

void dke_mouse_ring_push_serialized_reports(DKE_Mouse_Ring *ring, DKE_Mouse_SerializedReports const *frame);
DKE_Mouse_Report dke_mouse_ring_pop_report(DKE_Mouse_Ring *ring);

#ifdef __cplusplus
}
#endif

#endif // DKE_MOUSE_H__

#ifdef DKE_MOUSE_IMPLEMENTATION

#define dke__mouse_assert(x) (void)(x)

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

static DKE_S32 dke__mouse_clamp_s8(DKE_S32 x) {
	if (x > 127) { x = 127; }
	if (x < -127) { x = -127; }
	return x;
}

static DKE_S32 dke__mouse_clamp_s16(DKE_S32 x) {
	if (x > 32767) { x = 32767; }
	if (x < -32767) { x = -32767; }
	return x;
}

#if !defined(DKE_MOUSE_MEMCPY_OVERRIDE)
void *dke_mouse_memcpy_fallback(void *dst, void const *src, DKE_U32 size) {
	for (DKE_U32 idx = 0; idx < size; ++idx) {
		((DKE_U8 *)dst)[idx] = ((DKE_U8 const *)src)[idx];
	}
	return dst;
}
#endif // DKE_MOUSE_MEMCPY_OVERRIDE

DKE_Mouse_Ring dke_mouse_ring_make(void *memory, DKE_U32 size) {
	DKE_Mouse_Ring ring = { 0 };
	ring.memory = memory;
	ring.size = size;
	return ring;
}

DKE_B32 dke_mouse_ring_write(DKE_Mouse_Ring *ring, void const *src, DKE_U32 size) {
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

DKE_B32 dke_mouse_ring_read(DKE_Mouse_Ring *ring, void *dst, DKE_U32 size) {
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

DKE_Mouse_Frame dke_mouse_frame_make(DKE_Mouse_Config cfg, void *scratch_memory, DKE_U32 scratch_size) {
	DKE_Mouse_frame frame = { 0 };
	frame.cfg = cfg;
	frame.scratch_memory = scratch_memory;
	frame.scratch_size = scratch_size;
	return frame;
}

void dke_mouse_frame_reset(DKE_Mouse_Frame *frame) {
	//~ Dedrick: Reset scratch memory.
	frame->scratch_pos = 0;

	//~ Dedrick: Reset frame accumulators.
	frame->buttons_state = 0;
	frame->x_offset = 0;
	frame->y_offset = 0;
	if ((frame->cfg.features & DKE_Mouse_FeatureFlag_Wheel) != 0) {
		frame->wheel = 0;
	}
	if ((frame->cfg.features & DKE_Mouse_FeatureFlag_Pan) != 0) {
		frame->pan = 0;
	}
}

void dke_mouse_frame_push_move(DKE_Mouse_Frame *frame, DKE_S32 x_offset, DKE_S32 y_offset) {
	frame->x_offset += x_offset;
	frame->y_offset += y_offset;
}

void dke_mouse_frame_push_wheel(DKE_Mouse_Frame *frame, DKE_S32 offset) {
	if ((frame->cfg.features & DKE_Mouse_FeatureFlag_Wheel) != 0) {
		frame->wheel += offset;
	}
}

void dke_mouse_frame_push_pan(DKE_Mouse_Frame *frame, DKE_S32 offset) {
	if ((frame->cfg.features & DKE_Mouse_FeatureFlag_Pan) != 0) {
		frame->pan += offset;
	}
}

static DKE_B32 dke__mouse_frame_has_pending_motion(DKE_Mouse_Frame const *frame) {
	DKE_B32 const result = (frame->x_offset | frame->y_offset | frame->wheel | frame->pan) != 0;
	return result;
}

static void *dke__mouse_scratch_push(DKE_Mouse_Frame *frame, DKE_U32 size) {
	// TODO(Dedrick): Compile switch to assert on out of mem.
	void *result = 0;
	if (frame->scratch_pos + size <= frame->scratch_size) {
		result = frame->scratch_memory + frame->scratch_pos;
		frame->scratch_pos += size;
	}
	return result;
}

static void dke__mouse_frame_place_button_barrier(DKE_Mouse_Frame *frame, DKE_Mouse_ButtonFlags next_buttons_state) {
	if (frame->buttons_state != next_buttons_state) {
		//~ Dedrick: Write snapshots up to this point.
		if (dke__mouse_frame_has_pending_motion(frame)) {

		}

		//~ Dedrick:
		frame->buttons_state = next_buttons_state;
	}
}

void dke_mouse_frame_push_button_down(DKE_Mouse_Frame *frame, DKE_Mouse_ButtonFlags buttons) {
	dke__mouse_frame_button_barrier(frame, frame->buttons_state | buttons);
}

void dke_mouse_frame_push_button_up(DKE_Mouse_Frame *frame, DKE_Mouse_ButtonFlags buttons) {
	dke__mouse_frame_button_barrier(frame, frame->buttons_state & ~buttons);
}

DKE_Mouse_SerializedReports dke_mouse_serialized_reports_from_frame(DKE_Mouse_Frame const *frame) {
	DKE_U32 const num_snapshots = frame->scratch_pos / sizeof(DKE_Mouse_FrameSnapshot);
	return (num_snapshots + 1) * sizeof(DKE_Mouse_Report);
}

void dke_mouse_ring_push_serialized_reports(DKE_Mouse_Ring *ring, DKE_Mouse_SerializedReports const *reports) {
	for (DK_U32 idx = 0; idx < reports->count; ++idx) {
		DKE_Mouse_Report const *report = &reports->v[idx];
		dke_mouse_ring_write_struct(ring, report);
	}
}

DKE_Mouse_Report dke_mouse_ring_pop_report(DKE_Mouse_Ring *ring) {
	DKE_Mouse_Report result = { 0 };
	dke_mouse_ring_read(ring, &result.size, sizeof(DKE_U8));
	dke_mouse_ring_read(ring, &result.data, result.size);
	return result;
}

#endif // DKE_MOUSE_IMPLEMENTATION
