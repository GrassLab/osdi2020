#include "mmu/slab.h"
#include "mmu/buddy.h"
#include "mmu/page.h"
#include "MiniUart.h"

typedef struct __Slab {
    Page *obj_page;
    size_t obj_size;
    uint64_t *free_list_head;
    
    struct __Slab *next;
} Slab;

typedef struct __SlabManager {
    Page *slab_obj_page;
    Slab *free_list_head;
    Slab **indirect_free_list_head;
    Slab *inuse_list_head;
    Slab **indirect_inuse_list_head;
} SlabManager;

static SlabManager manager;

static const size_t kDefaultObjectNum = 256;
static const size_t kDefaultSlabObjectNum = 40;

static void initSlab(void) {
    manager.slab_obj_page = gBuddy.allocate(kDefaultSlabObjectNum * sizeof(Slab));
    manager.free_list_head = (Slab *)gPage.getBlock(manager.slab_obj_page)->lower;
    manager.indirect_free_list_head = &manager.free_list_head;

    // initialize slab object list
    for (size_t i = 0; i < kDefaultSlabObjectNum - 1; ++i) {
        manager.free_list_head[i].next = manager.free_list_head + i + 1;
    }
    manager.free_list_head[kDefaultSlabObjectNum - 1].next = NULL;

    manager.inuse_list_head = NULL;
    manager.indirect_inuse_list_head = &manager.inuse_list_head;
}

static Slab *getFreeSlab(void) {
    Slab *slab = manager.free_list_head;
    manager.free_list_head = slab->next;

    slab->obj_page = NULL;
    slab->obj_size = 0;
    slab->free_list_head = NULL;
    slab->next = *(manager.indirect_inuse_list_head);
    *(manager.indirect_inuse_list_head) = slab;
    manager.indirect_inuse_list_head = &(slab->next);

    return slab;
}

size_t alignSize(size_t size) {
    const uint64_t alignment = 8;
    return (size & ~(alignment - 1)) + ((size & (alignment - 1)) || 0) * 8;
}

static uint64_t registSlab(size_t object_size) {
    Slab *slab = getFreeSlab();
    slab->obj_size = alignSize(object_size);
    slab->next = NULL;

    sendStringUART("[Slab] regist object size ");
    sendHexUART(object_size);
    sendStringUART(", give it slab of object size ");
    sendHexUART(slab->obj_size);
    sendUART('\n');
    sendStringUART("[Slab] token ");
    sendHexUART((uint64_t)slab);
    sendUART('\n');

    return (uint64_t)slab;
}

static void deleteSlab(uint64_t token) {
    Slab *inuse_slab = manager.inuse_list_head;
    Slab **indirect_prev_slab_next = manager.indirect_inuse_list_head;

    while (inuse_slab) {
        if (inuse_slab == (Slab *)token) {
            gBuddy.deallocate(inuse_slab->obj_page);

            // remove inuse_slab node
            *indirect_prev_slab_next = inuse_slab->next;

            // place inuse_slab back to free list
            inuse_slab->next = *manager.indirect_free_list_head;
            *manager.indirect_free_list_head = inuse_slab;

            sendStringUART("[Slab] delete token ");
            sendHexUART(token);
            sendUART('\n');

            break;
        }
        indirect_prev_slab_next = &inuse_slab->next;
        inuse_slab = inuse_slab->next;
    }
}

static void finiSlab(void) {
    Slab *inuse_slab = manager.inuse_list_head;

    while (inuse_slab) {
        gBuddy.deallocate(inuse_slab->obj_page);
        inuse_slab = inuse_slab->next;
    }

    gBuddy.deallocate(manager.slab_obj_page);
}

static void bookkeepObjPage(Page *page, size_t obj_size) {
    Block *block = gPage.getBlock(page);
    uint64_t *cur_obj = (uint64_t *)block->lower;

    for (size_t i = 0; i < kDefaultObjectNum; ++i) {
        uint64_t *next_obj = (uint64_t *)((uint64_t)cur_obj + obj_size);
        *cur_obj = (uint64_t)next_obj;
        cur_obj = next_obj;
    }
}

static void *allocateObjectSpace(uint64_t token) {
    Block *slab_objs_block = gPage.getBlock(manager.slab_obj_page);
    if (!(slab_objs_block->lower <= token && token < slab_objs_block->upper)) {
        // TODO: check position correctness through alignment
        sendStringUART("[Slab] Wrong token!");
        return NULL;
    }

    Slab *slab = (Slab *)token;

    if (!slab->obj_page) {
        sendStringUART("[Slab] block_size: ");
        sendHexUART(kDefaultObjectNum * slab->obj_size);
        sendUART('\n');
        slab->obj_page = gBuddy.allocate(kDefaultObjectNum * slab->obj_size);
        bookkeepObjPage(slab->obj_page, slab->obj_size); 
        slab->free_list_head = (uint64_t *)gPage.getBlock(slab->obj_page)->lower;
    }

    if (!slab->free_list_head) {
        // TODO: get another page
        sendStringUART("[Slab] Object page is not enough!\n");
        return NULL;
    }

    uint64_t *retval = slab->free_list_head;
    slab->free_list_head = (uint64_t *)*(slab->free_list_head);
    *retval = 0;

    sendStringUART("[Slab] Allocate object with size ");
    sendHexUART(slab->obj_size);
    sendUART('\n');

    return retval;
}

static void deallocateObjectSpace(void *ptr) {
    Slab *inuse_slab = manager.inuse_list_head;

    while (inuse_slab) {
        if (gPage.inPage(inuse_slab->obj_page, (uint64_t)ptr)) {
            uint64_t *cur_obj = ptr;
            *cur_obj = (uint64_t)inuse_slab->free_list_head;
            inuse_slab->free_list_head = cur_obj;

            sendStringUART("[Slab] Deallocate object with size ");
            sendHexUART(inuse_slab->obj_size);
            sendUART('\n');

            return;
        }
        inuse_slab = inuse_slab->next;
    }

    sendStringUART("[Slab] Wrong address space! Not in any memory pool\n");
}

SlabType gSlab = {
    .init = initSlab,
    .fini = finiSlab,
    .regist = registSlab,
    .delete = deleteSlab,
    .allocate = allocateObjectSpace,
    .deallocate = deallocateObjectSpace
};
