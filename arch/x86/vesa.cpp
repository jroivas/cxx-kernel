#include "vesa.h"
#include "types.h"
#include "port.h"
#include "mm.h"


#include "string.h"
#include "../platform.h"
#include "vbe_priv.h"
#include "bios.h"

#define VBE_realSeg(x) (((ptr_val_t)x) >> 4)
#define VBE_realOff(x) (((ptr_val_t)x)&0xF)
#define VBE_Ptr(x) (((x & 0xffff0000) >> 12) + (x & 0xffff))

Vesa::Vesa() : FB()
{
	//
}

int Vesa::modeDiff(FB::ModeConfig *conf, FB::ModeConfig *cmp)
{
	if (conf==NULL || cmp==NULL) return -1;
	int diff=0;

	if (conf->width!=cmp->width) diff++;
	if (conf->width>cmp->width) diff++;
	if (conf->height!=cmp->height) diff++;
	if (conf->height>cmp->height) diff++;
	if (conf->depth!=cmp->depth) diff++;
	if (conf->depth>cmp->depth) diff++;
	if (conf->bytes_per_line!=cmp->bytes_per_line) diff++;

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

bool Vesa::getVESA(void *ptr)
{
	vbe_info_t *info = (vbe_info_t*)ptr;
	BIOS *bios = BIOS::get();
/*
	info->vbe_signature[0]='V';
	info->vbe_signature[1]='E';
	info->vbe_signature[2]='S';
	info->vbe_signature[3]='A';
*/
	info->vbe_signature[0]='V';
	info->vbe_signature[1]='B';
	info->vbe_signature[2]='E';
	info->vbe_signature[3]='2';

	Regs r;
	//Mem::set(&r, 0, sizeof(Regs));
	r.eax = 0x4f00;
	r.es = VBE_realSeg(info);
	r.edi = VBE_realOff(info);
	//r.edi = (ptr_val_t)info;
	//Platform::video()->printf("=== bios ptr %x\n",(ptr_val_t)bios);
	Platform::video()->printf("=== VBE2 ptr %x\n",(ptr_val_t)info);
	bios->runInt(0x10, &r);

	if ((r.eax&0xFF)==0x4f && (r.eax&0xFF00)==0) {
		Platform::video()->printf("=== VBE2 Init ok\n");
		Platform::video()->printf("    sig: %c%c%c%c\n",info->vbe_signature[0],info->vbe_signature[1],info->vbe_signature[2],info->vbe_signature[3]); 
		Platform::video()->printf("    ver: %x\n",info->vbe_version);
		Platform::video()->printf("    cap: %x\n",info->capabilities);
		Platform::video()->printf("    mem: %d\n",info->total_memory*64);
		Platform::video()->printf("    mod: %x\n",info->video_mode_ptr);
		if (info->vbe_version==0) {
			Platform::video()->printf("=== VBE2 invalid version: %x\n",info->vbe_version);
		}
	} else {
		Platform::video()->printf("=== VBE2 init FAILED\n");
		return false;
	}
	return true;
}

FB::ModeConfig *Vesa::query(FB::ModeConfig *prefer)
{
	(void)prefer;

	BIOS *bios = BIOS::get();
	vbe_info_t *info = (vbe_info_t *)bios->alloc(sizeof(vbe_info_t));
	if (!getVESA(info)) return NULL;

	uint16_t *loc = (uint16_t*)VBE_Ptr((uint32_t)info->video_mode_ptr);
	Platform::video()->printf("=== VBE2 %d %x %x\n",(loc<(uint16_t*)(info+sizeof(info))),loc,info+sizeof(info));
	if (loc==NULL) return NULL;

	/* Reserve structures */
	vbe_mode_info_t *modeinfo = (vbe_mode_info_t *)bios->alloc(sizeof(vbe_mode_info_t));

	FB::ModeConfig *res = (FB::ModeConfig *)MM::instance()->alloc(sizeof(FB::ModeConfig));
	FB::ModeConfig *conf = (FB::ModeConfig *)MM::instance()->alloc(sizeof(FB::ModeConfig));
	int bestdiff = -1;


	for (uint16_t i=0; loc[i]!=0xFFFF; i++) {
		Regs r;
		r.eax = 0x4f01;
		r.ecx = loc[i];
		Mem::set(modeinfo, 0, sizeof(vbe_mode_info_t));
		r.es = VBE_realSeg(modeinfo);
		r.edi = VBE_realOff(modeinfo);
		//r.edi = (ptr_val_t)modeinfo;

		bios->runInt(0x10, &r);
		if ((r.eax&0xFF)!=0x4f) {
			Platform::video()->printf("=== VBE2 mode info not supported\n");
			return NULL;
		}
		if ((r.eax&0xFF00)!=0) {
			Platform::video()->printf("=== VBE2 mode info failed %d %d\n",i,loc[i]);
			//Platform::video()->printf("=== VBE2 mode info failed %d\n",*loc);
			return NULL;
		}
		if (modeinfo->memory_model!=4 && modeinfo->memory_model!=6) {
			//Platform::video()->printf("=== a: %x\n",modeinfo->memory_model);
			continue;
		}
		else if (modeinfo->phys_base_ptr==0) {
			continue;
		}
		else if ((modeinfo->mode_attributes&1)==0) {
			continue;
		}
		else if ((modeinfo->mode_attributes&(1<<1))==0) {
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

		Platform::video()->printf("%d: %dx%d BPP: %d\n",loc[i],modeinfo->x_resolution, modeinfo->y_resolution, modeinfo->bits_per_pixel);
		conf->width = modeinfo->x_resolution;
		conf->height = modeinfo->y_resolution;
		conf->depth = modeinfo->bits_per_pixel;
		conf->base = (unsigned char*)modeinfo->phys_base_ptr;
		conf->bytes_per_line = modeinfo->bytes_per_scan_line; //modeinfo->x_resolution * modeinfo->bits_per_pixel/8;
		conf->id = loc[i];

		if (prefer==NULL) {
			if (bestdiff==-1) {
				bestdiff=0;
				copyMode(res, conf);
			} else {
				if (res->width<=conf->width && res->height<=conf->height && res->depth<=conf->depth) {
				//if (res->width<conf->width || res->height<conf->height || res->depth<conf->depth) {
					copyMode(res, conf);
				}
			}
		} else {
			int rdiff = modeDiff(prefer, conf);
			if (bestdiff==-1 && rdiff!=-1) {
				bestdiff=rdiff;
				copyMode(res, conf);
			} else if (bestdiff!=-1 && rdiff!=-1 && rdiff<bestdiff) {
				bestdiff=rdiff;
				copyMode(res, conf);
			}
		}
	}

	MM::instance()->free(conf);
	if (bestdiff==-1) {
		MM::instance()->free(res);
		return NULL;
	}

	// TODO make this cleaner
	Paging p;
	ptr_val_t newbase;
	uint32_t s = res->bytes_per_line*(res->height+1);
	p.map(res->base, &newbase, 0x3);
	for (uint32_t i=PAGE_SIZE; i<=s; i+=PAGE_SIZE) {
		ptr_val_t tmpbase;
		p.map(res->base+i, &tmpbase, 0x3);
	}
	res->base = (unsigned char*)newbase;

	Platform::video()->printf("%d: %dx%d BPP: %d  BPL: %d  Base: %x %x  %d\n",res->id,res->width, res->height, res->depth, res->bytes_per_line, res->base, newbase, bestdiff);
	for (int i=0; i<0xfffff; i++) {
		for (int j=0; j<0x2ff; j++) ;
	}

	return res;
}

void Vesa::setMode(ModeConfig *mode)
{
	if (mode==NULL) return;

#if 1
	BIOS *bios = BIOS::get();
	Regs r;
	r.eax = 0x4f02;
	r.ebx = mode->id|(1<<14);
	bios->runInt(0x10, &r);
	if ((r.eax&0xFF00)!=0) {
		Platform::video()->printf("=== VBE2 mode set failed\n");
	}
#endif
}

bool Vesa::configure(ModeConfig *mode)
{
	setMode(mode);
	return FB::configure(mode);
}

#if 0
void Vesa::putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	(void)a;
#if 0
	*(current->base+(y*current->bytes_per_line + x*3))=r;
	*(current->base+(y*current->bytes_per_line + x*3+1))=g;
	*(current->base+(y*current->bytes_per_line + x*3+2))=b;
#else
	unsigned char *pos = (unsigned char*)(current->base+(y*current->bytes_per_line + x*3));
	*pos++ = r&0xff;
	*pos++ = g&0xff;
	*pos = b&0xff;
#endif
}
#endif

void Vesa::blit()
{
	if (direct) return;
	Mem::copy(current->base, buffer, size);
}
