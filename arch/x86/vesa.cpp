#include "vesa.h"
#include "types.h"
#include "port.h"
//#include "mm.h"


#include "string.h"
#include "../platform.h"
#include "vbe_priv.h"
#include "bios.h"

Vesa::Vesa() : FB()
{
	vbe_info_t *info = NULL;
	(void)info;
	//Platform::video()->printf("PRE bIOS\n");
	//BIOS tmp;
	BIOS *bios = BIOS::get();
	bios = BIOS::get();
	(void)bios;
	//Platform::video()->printf("bIOS\n");
	//mmap(sizeof(vbe_info_t), MM_SLEEP);
}

int Vesa::modeDiff(FB::ModeConfig *conf, FB::ModeConfig *cmp)
{
	if (conf==NULL || cmp==NULL) return -1;
	int diff=0;

	if (conf->width!=cmp->width) diff++;
	if (conf->height!=cmp->height) diff++;
	if (conf->depth!=cmp->depth) diff++;

	return diff;
}

void Vesa::copyMode(FB::ModeConfig *dest, FB::ModeConfig *src)
{
	dest->width = src->width;
	dest->height = src->height;
	dest->depth = src->depth;
	dest->base = src->base;
	dest->bytes_per_line = src->bytes_per_line;
	dest->id = src->id;
}

ptr_t Vesa::getVESA()
{
	BIOS *bios = BIOS::get();
	vbe_info_t *info = (vbe_info_t *)bios->alloc(sizeof(vbe_info_t));
	info->vbe_signature[0]='V';
	info->vbe_signature[1]='B';
	info->vbe_signature[2]='E';
	info->vbe_signature[3]='2';

	Regs r;
	r.eax = 0x4f00;
	r.edi = (ptr_val_t)info;
	Platform::video()->printf("=== bios ptr %x\n",(ptr_val_t)bios);
	Platform::video()->printf("=== VBE2 ptr %x\n",(ptr_val_t)info);
	bios->runInt(0x10, &r);

	if ((r.eax&0xFF)==0x4f) {
		Platform::video()->printf("=== VBE2 Init ok\n");
		Platform::video()->printf("    sig: %c%c%c%c\n",info->vbe_signature[0],info->vbe_signature[1],info->vbe_signature[2],info->vbe_signature[3]); 
		Platform::video()->printf("    ver: %x\n",info->vbe_version);
		Platform::video()->printf("    cap: %x\n",info->capabilities);
		Platform::video()->printf("    mem: %d\n",info->total_memory*64);
		Platform::video()->printf("    mod: %x\n",info->video_mode_ptr);
	} else {
		Platform::video()->printf("=== VBE2 init FAILED\n");
		return NULL;
	}
	return (ptr_t)info->video_mode_ptr;
}

FB::ModeConfig *Vesa::query(FB::ModeConfig *prefer)
{
	(void)prefer;

#if 1
	BIOS *bios = BIOS::get();
	vbe_info_t *info = (vbe_info_t *)bios->alloc(sizeof(vbe_info_t));
	info->vbe_signature[0]='V';
	info->vbe_signature[1]='B';
	info->vbe_signature[2]='E';
	info->vbe_signature[3]='2';

	Regs r;
	r.eax = 0x4f00;
	r.edi = (ptr_val_t)info;
	//r.es = 0;
	Platform::video()->printf("=== bios ptr %x\n",(ptr_val_t)bios);
	Platform::video()->printf("=== VBE2 ptr %x\n",(ptr_val_t)info);

	bios->runInt(0x10, &r);

	if ((r.eax&0xFF)==0x4f) {
		Platform::video()->printf("=== VBE2 Init ok\n");
		Platform::video()->printf("    sig: %c%c%c%c\n",info->vbe_signature[0],info->vbe_signature[1],info->vbe_signature[2],info->vbe_signature[3]); 
		Platform::video()->printf("    ver: %x\n",info->vbe_version);
		Platform::video()->printf("    cap: %x\n",info->capabilities);
		Platform::video()->printf("    mem: %d\n",info->total_memory*64);
		Platform::video()->printf("    mod: %x\n",info->video_mode_ptr);
	} else {
		Platform::video()->printf("=== VBE2 init FAILED\n");
		return NULL;
	}
#endif
#if 1
	uint16_t *loc = NULL;
	if ((loc=(uint16_t*)getVESA())==NULL) return NULL;
#endif

#if 0
	BIOS *bios = BIOS::get();
	vbe_mode_info_t *modeinfo = (vbe_mode_info_t *)bios->alloc(sizeof(vbe_mode_info_t));

	FB::ModeConfig *res = (FB::ModeConfig *)MM::instance()->alloc(sizeof(FB::ModeConfig));

	int bestdiff = -1;
	FB::ModeConfig *conf = (FB::ModeConfig *)MM::instance()->alloc(sizeof(FB::ModeConfig));

	//uint16_t *loc = (uint16_t*)info->video_mode_ptr;

	for (int i=0; loc[i]!=0xFFFF; i++) {
		Regs r;
		r.eax = 0x4f01;
		r.ecx = loc[i];
		r.edi = (ptr_val_t)modeinfo;
		bios->runInt(0x10, &r);
		if ((r.eax&0xFF00)!=0) {
			Platform::video()->printf("=== VBE2 mode info failed %d %d\n",i,loc[i]);
			return NULL;
		}
		if (modeinfo->memory_model!=4 && modeinfo->memory_model!=6) {
			continue;
		}
		else if (modeinfo->phys_base_ptr==0) {
			continue;
		}
		else if ((modeinfo->mode_attributes&1)==0) {
			continue;
		}
		else if ((modeinfo->mode_attributes&(1<<3))==0) {
			continue;
		}
		else if ((modeinfo->mode_attributes&(1<<4))==0) {
			continue;
		}
		else if ((modeinfo->mode_attributes&(1<<7))==0) {
			continue;
		}
		switch (modeinfo->bits_per_pixel) {
			/* Support */
			case 32:
			case 24:
			case 16:
				break;

			/* Skip */
			case 8:
			default:
				continue;
		}

		//Platform::video()->printf("%d: %dx%d BPP: %d\n",loc[i],modeinfo->x_resolution, modeinfo->y_resolution, modeinfo->bits_per_pixel);
		conf->width = modeinfo->x_resolution;
		conf->height = modeinfo->y_resolution;
		conf->depth = modeinfo->bits_per_pixel;
		conf->base = (unsigned char*)modeinfo->phys_base_ptr;
		conf->bytes_per_line = modeinfo->x_resolution * modeinfo->x_resolution/8;
		conf->id = loc[i];

		int rdiff = modeDiff(prefer, conf);
		if (bestdiff==-1 && rdiff!=-1) {
			bestdiff=rdiff;
			copyMode(res, conf);
		} else if (bestdiff!=-1 && rdiff!=-1 && rdiff<bestdiff) {
			bestdiff=rdiff;
			copyMode(res, conf);
		}
	}
#endif
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

#if 0
	MM::instance()->free(conf);
	if (bestdiff==-1) {
		MM::instance()->free(res);
		return NULL;
	}

	return res;
#else
	return NULL;
#endif

}

void Vesa::setMode(ModeConfig *mode)
{
	if (mode==NULL) return;

/*
	BIOS *bios = BIOS::get();
	Regs r;
	r.eax = 0x4f02;
	r.ebx = mode->id|(1<<14);
	bios->runInt(0x10, &r);
	if ((r.eax&0xFF00)!=0) {
		Platform::video()->printf("=== VBE2 mode set failed\n");
	}
*/
}

bool Vesa::configure(ModeConfig *mode)
{
	//(void)mode;
	setMode(mode);
	return FB::configure(mode);
}

void Vesa::blit()
{
}
