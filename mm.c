#include "mm.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h> /*for getpagesize*/
#include <sys/mman.h>
#include <errno.h>
#include <Kernel/string.h>


static size_t SYSTEM_PAGE_SIZE = 0;
static vm_page_for_families_t *first_vm_page_for_families = NULL; //head

void *gb_hsba = NULL;

void mm_init(){
    SYSTEM_PAGE_SIZE =  getpagesize() * 2;
}



// Return the pointer to the start of the virtual memory region allocated
static void *mm_get_new_vm_page_from_kernel(int units){

    char * vm_page = mmap( 
            0, 
            units * SYSTEM_PAGE_SIZE, 
            PROT_READ|PROT_WRITE, 
            MAP_ANON|MAP_PRIVATE, 
            -1,0);
     if (vm_page == MAP_FAILED) {
        printf("Error : VM Page allocation Failed\n");
        return NULL;
    }
    memset(vm_page, 0, units * SYSTEM_PAGE_SIZE); 
    return (void *) vm_page;
}

//releases the virtual memory pages back to kernel - get the address of the first VM page and units which specifies how many contiguous VM pages to release
static void mm_return_vm_page_to_kernel(void *ptr, int units){
    if(munmap(ptr, units * SYSTEM_PAGE_SIZE)){ //system call to return VM page back to kernel
        printf("Error : Could not munmap VM page to kernel");
    }
}


//user space application calls this function (API) to register page family information
void
mm_instantiate_new_page_family(
    char *struct_name,
    uint32_t struct_size){

    vm_page_family_t *vm_page_family_curr = NULL;
    vm_page_for_families_t *new_vm_page_for_families = NULL;
    vm_page_for_families_t *vm_page_for_families_global;

    if(struct_size>SYSTEM_PAGE_SIZE){
        printf("Error: %s() structure %s size exceeds system page size\n", __FUNCTION__, struct_name);
        return;
    }

    if(!first_vm_page_for_families){
        first_vm_page_for_families = (vm_page_for_families_t *)mm_get_new_vm_page_from_kernel(1);
        first_vm_page_for_families->next = NULL;
        strncpy(vm_page_for_families_global->vm_page_family[0].struct_name, struct_name,
            MM_MAX_STRUCT_NAME);
        first_vm_page_for_families->vm_page_family[0].struct_size = struct_size;
        return;
    }
    uint32_t count = 0;
    
    ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_families, vm_page_family_curr){
        if(strncmp(vm_page_family_curr->struct_name,struct_name,MM_MAX_STRUCT_NAME)!=0){
            count++;
            continue;
        }
        assert(0);

    } ITERATE_PAGE_FAMILIES_END(first_vm_page_for_families, vm_page_family_curr);

    if(count == MAX_FAMILIES_PER_VM_PAGE){
        /*Request a new vm page from kernel to add a new family*/
        new_vm_page_for_families = (vm_page_for_families_t *)mm_get_new_vm_page_from_kernel(1);
        new_vm_page_for_families->next = first_vm_page_for_families;
        first_vm_page_for_families = new_vm_page_for_families;
        vm_page_family_curr = &first_vm_page_for_families->vm_page_family[0];
    }

    strncpy(vm_page_family_curr->struct_name, struct_name,
            MM_MAX_STRUCT_NAME);
    vm_page_family_curr->struct_size = struct_size;
}

