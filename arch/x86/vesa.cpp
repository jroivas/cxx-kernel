#include "vesa.h"
#include "types.h"
#include "port.h"
//#include "3rdparty/libx86/x86emu/include/libx86.h"

#if 0
#ifdef __cplusplus
extern "C" {
#endif
#include "x86emu/x86emu.h"
#ifdef __cplusplus
}
#endif
#else
#include "x86emu.h"
#endif

#include "string.h"
#include "../platform.h"
#include "vbe_priv.h"
#include "bios.h"

#if 0
struct vbe_info_block {
        char vbe_signature[4];
        short vbe_version;
        unsigned short oem_string_off;
        unsigned short oem_string_seg;
        int capabilities;
        unsigned short video_mode_list_off;
        unsigned short video_mode_list_seg;
        short total_memory;
        short oem_software_rev;
        unsigned short oem_vendor_name_off;
        unsigned short oem_vendor_name_seg;
        unsigned short oem_product_name_off;
        unsigned short oem_product_name_seg;
        unsigned short oem_product_rev_off;
        unsigned short oem_product_rev_seg;
        char reserved[222];
        char oem_data[256];
} __attribute__ ((packed));

struct vbe_mode_info_block {
        unsigned short mode_attributes;
        unsigned char win_a_attributes;
        unsigned char win_b_attributes;
        unsigned short win_granularity;
        unsigned short win_size;
        unsigned short win_a_segment;
        unsigned short win_b_segment;
        unsigned short win_func_ptr_off;
        unsigned short win_func_ptr_seg;
        unsigned short bytes_per_scanline;
        unsigned short x_resolution;
        unsigned short y_resolution;
        unsigned char x_char_size;
        unsigned char y_char_size;
        unsigned char number_of_planes;
        unsigned char bits_per_pixel;
        unsigned char number_of_banks;
        unsigned char memory_model;
        unsigned char bank_size;
        unsigned char number_of_image_pages;
        unsigned char res1;
        unsigned char red_mask_size;
        unsigned char red_field_position;
        unsigned char green_mask_size;
        unsigned char green_field_position;
        unsigned char blue_mask_size;
        unsigned char blue_field_position;
        unsigned char rsvd_mask_size;
        unsigned char rsvd_field_position;
        unsigned char direct_color_mode_info;
        unsigned int phys_base_ptr;
        unsigned int offscreen_mem_offset;
        unsigned short offscreen_mem_size;
        unsigned char res2[206];
} __attribute__ ((packed));
#endif

Vesa::Vesa() : FB()
{
#if 0
	if (LRMI_init()) {
		lrmi_ok = true;
#if 0
		vbe.info = (struct vbe_info_block *)LRMI_alloc_real(sizeof(struct vbe_info_block) + sizeof(struct vbe_mode_info_block));
		if (vbe.info == NULL) lrmi_ok = false;
		else {
			vbe.mode = (struct vbe_mode_info_block *)(vbe.info + 1);
		}
#endif
	}
	else
#else
	//(void)vbe;
#endif
	vbe_info_t *info = NULL;
	(void)info;
	Platform::video()->printf("PRE bIOS\n");
	//BIOS tmp;
	BIOS *bios = BIOS::get();
	(void)bios;
	Platform::video()->printf("bIOS\n");
	//mmap(sizeof(vbe_info_t), MM_SLEEP);
}

FB::ModeConfig *Vesa::query(FB::ModeConfig *prefer)
{
	(void)prefer;

#if 0
	struct LRMI_regs r;
	r.eax = 0x4f00;
	r.es = (unsigned int)vbe.info >> 4;
	r.edi = 0;
	Mem::copy(vbe.info->vbe_signature, (void*)"VBE2", 4);
	if (!LRMI_int(0x10, &r)) {
		Platform::video()->printf("=== VBE2 Init failure\n");
		return NULL;
	}
	Platform::video()->printf("VBE2 got: %d %d\n",(int)(vbe.info->vbe_version >> 8) & 0xff,(int)vbe.info->vbe_version & 0xff);
	Platform::video()->printf("%s\n", (char *)(vbe.info->oem_string_seg * 16 + vbe.info->oem_string_off));
#endif
	

	return NULL;
}

bool Vesa::configure(ModeConfig *mode)
{
	(void)mode;
	return FB::configure(mode);
}

void Vesa::blit()
{
}
