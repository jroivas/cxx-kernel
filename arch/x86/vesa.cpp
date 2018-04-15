#include "vesa.h"
#include "types.h"
#include "port.h"
#include "mm.h"
#include "memcpy.h"


#include "string.hh"
#include "../platform.h"
#include "vbe_priv.h"
#include "bios.h"

#define VBE_realSeg(x) (((ptr_val_t)x) >> 4)
#define VBE_realOff(x) (((ptr_val_t)x) & 0xF)
#define VBE_Ptr(x) (((x & 0xffff0000) >> 12) + (x & 0xffff))

Vesa::Vesa() : FB()
{
}

Vesa::~Vesa()
{
}

int Vesa::modeDiff(FB::ModeConfig *conf, FB::ModeConfig *cmp)
{
    if (conf == nullptr || cmp == nullptr) return -1;
    int diff = 0;

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
    r.eax = 0x4f00;
    r.es = VBE_realSeg(info);
    r.edi = VBE_realOff(info);
    //Platform::video()->printf("=== bios ptr %x\n",(ptr_val_t)bios);
    //Platform::video()->printf("=== VBE2 ptr %x\n",(ptr_val_t)info);
    //Platform::video()->printf("=== VBE2 ptr %x %x %x\n",r.es,r.edi,(ptr_val_t)info);
    bios->runInt(0x10, &r);

    if ((r.eax & 0xFF) == 0x4f && (r.eax & 0xFF00)==0) {
        Platform::video()->printf("=== VBE2 Init ok\n");
#if 0
        Platform::video()->printf("    sig: %c%c%c%c\n",info->vbe_signature[0],info->vbe_signature[1],info->vbe_signature[2],info->vbe_signature[3]); 
        Platform::video()->printf("    ver: %x\n",info->vbe_version);
        Platform::video()->printf("    cap: %x\n",info->capabilities);
        Platform::video()->printf("    mem: %d\n",info->total_memory*64);
        Platform::video()->printf("    mod: %x\n",info->video_mode_ptr);
#endif
        if (info->vbe_version==0) {
            Platform::video()->printf("=== VBE2 invalid version: %x\n",info->vbe_version);
        }
    } else {
        Platform::video()->printf("=== VBE2 init FAILED\n");
        Platform::video()->printf("=== VBE2 invalid version: %x, %x\n",(int)(r.eax&0xFF), (int)(r.eax&0xFF00));
        Platform::video()->printf("=== VBE2 invalid version: %x\n",r.eax);
        return false;
    }
    return true;
}

FB::ModeConfig *Vesa::query(FB::ModeConfig *prefer)
{
    m.lock();

    BIOS *bios = BIOS::get();
    vbe_info_t *info = (vbe_info_t *)bios->alloc(sizeof(vbe_info_t));
    if (!getVESA(info)) {
        m.unlock();
        return nullptr;
    }

    uint16_t *loc = (uint16_t*)VBE_Ptr((uint32_t)info->video_mode_ptr);
    //Platform::video()->printf("=== VBE2 %d %x %x\n",(loc<(uint16_t*)(info+sizeof(info))),loc,info+sizeof(info));
    if (loc == nullptr) {
        m.unlock();
        return nullptr;
    }

    //Platform::video()->printf("=== Has MMX? %s\n",mmx_has()?"YES":"NO");

    /* Reserve structures */
    vbe_mode_info_t *modeinfo = (vbe_mode_info_t *)bios->alloc(sizeof(vbe_mode_info_t));

    FB::ModeConfig *res = (FB::ModeConfig *)MM::instance()->alloc(sizeof(FB::ModeConfig));
    FB::ModeConfig *conf = (FB::ModeConfig *)MM::instance()->alloc(sizeof(FB::ModeConfig));
    int bestdiff = -1;

    for (uint16_t i = 0; loc[i] != 0xFFFF; i++) {
        Regs r;
        r.eax = 0x4f01;
        r.ecx = loc[i];
        Mem::set(modeinfo, 0, sizeof(vbe_mode_info_t));
        r.es = VBE_realSeg(modeinfo);
        r.edi = VBE_realOff(modeinfo);

        bios->runInt(0x10, &r);
        if ((r.eax&0xFF) != 0x4f) {
            Platform::video()->printf("=== VBE2 mode info not supported\n");
            m.unlock();
            return nullptr;
        }
        if ((r.eax&0xFF00) != 0) {
            Platform::video()->printf("=== VBE2 mode info failed %d %d\n", i, loc[i]);
            m.unlock();
            return nullptr;
        }
        if (modeinfo->memory_model != 4 && modeinfo->memory_model != 6) {
            //Platform::video()->printf("=== a: %x\n",modeinfo->memory_model);
            continue;
        }
        else if (modeinfo->phys_base_ptr == 0) {
            continue;
        }
        else if ((modeinfo->mode_attributes & 1) == 0) {
            continue;
        }
        else if ((modeinfo->mode_attributes & (1 << 1)) == 0) {
            continue;
        }
        else if ((modeinfo->mode_attributes & (1 <<3)) == 0) {
            continue;
        }
        else if ((modeinfo->mode_attributes & (1 << 4)) == 0) {
            continue;
        }
        else if ((modeinfo->mode_attributes & (1 << 7)) == 0) {
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
        conf->bytes_per_line = modeinfo->bytes_per_scan_line; //modeinfo->x_resolution * modeinfo->bits_per_pixel/8;
        conf->id = loc[i];

        if (prefer==nullptr) {
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
            if (bestdiff == -1 && rdiff != -1) {
                bestdiff = rdiff;
                copyMode(res, conf);
            } else if (bestdiff != -1 && rdiff != -1 && rdiff < bestdiff) {
                bestdiff = rdiff;
                copyMode(res, conf);
            }
        }
    }

    MM::instance()->free(conf);
    if (bestdiff == -1) {
        MM::instance()->free(res);
        m.unlock();
        return nullptr;
    }

    // TODO make this cleaner
    Paging p;
    p.lock();

    uint32_t s = res->bytes_per_line * (res->height);
    ptr_val_t newbase;
    p.map(res->base, &newbase, 0x3);
    //ptr_val_t prev = newbase;
    ptr_val_t tmpbase = 0;
    for (uint32_t i=PAGE_SIZE; i<=s; i+=PAGE_SIZE) {
        p.map(res->base+i, &tmpbase, 0x3);
        //if (prev+PAGE_SIZE != tmpbase) {
        if (newbase+i != tmpbase) {
            Platform::video()->printf("Discontinuation: %x\n",tmpbase);
            for (int j=0; j<0xffffff; j++) ;
        }
        //prev = tmpbase;
    }
    p.unlock();

    Platform::video()->printf("%d: %dx%d BPP: %d  BPL: %d  Base: %x -> %x  %d\n",res->id,res->width, res->height, res->depth, res->bytes_per_line, res->base, newbase, bestdiff);
    res->base = (unsigned char*)newbase;
    //p.lock();
    //void *tt = p.alloc(1);
    //p.unlock();
#if 0
    void *tt = malloc(PAGE_SIZE-1);
    Platform::video()->printf("%x\n",(ptr_val_t)tt);
    for (int i=0; i<0xfffff; i++) {
            for (int j=0; j<0x1ff; j++) ;
    }
#endif
    m.unlock();

    return res;
}

void Vesa::setMode(ModeConfig *mode)
{
#if 1
    BIOS *bios = BIOS::get();
    Regs r;
    r.eax = 0x4f02;
    r.ebx = mode->id|(1<<14);
    bios->runInt(0x10, &r);
    if ((r.eax&0xFF00)!=0) {
        Platform::video()->printf("=== VBE2 mode set failed\n");
        //while(1);
    }
#endif
}

bool Vesa::configure(ModeConfig *mode)
{
    if (mode == nullptr) return false;

    setMode(mode);
    if (FB::configure(mode)) {
        //setDirect();
        //setSingleBuffer();
        clear();
        return true;
    }
    return false;
}

void Vesa::clear()
{
    Mem::set(m_current->base, 0, m_size);
}

void Vesa::blit()
{
    if (m_direct) return;

#if 0
    if (!m_double_buffer) Mem::copy(m_current->base, m_backbuffer, m_size);
    else memcpy_opt(m_current->base, m_buffer, m_size);
#endif
    m.lock();
    memcpy_opt(m_current->base, m_buffer, m_size);
    m.unlock();
    //Mem::copy(m_current->base,m_buffer,m_size);
}
