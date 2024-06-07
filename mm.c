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

void *gb_hsba = NULL;

void mm_init(){
    SYSTEM_PAGE_SIZE =  getpagesize() * 2;
}


// Argument to the API is the number of contiguous virtual memory pages which our user process is requesting from the kernel - it allocates contiguous VM pages
// Return the pointer to the start of the virtual memory region allocated
static void *mm_get_new_vm_page_from_kernel(int units){

    char * vm_page = mmap( //system call to allocate VM page
            0, 
            units * SYSTEM_PAGE_SIZE, // amount of memory we need - our system page size * units needed
            PROT_READ|PROT_WRITE, //permissions
            MAP_ANON|MAP_PRIVATE, //flags - inorder to request a virtual memory page from the kernel space from the virtual address space of the process - use these flags
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

int main(){
    mm_init();
    printf("VM page size is = %lu\n", SYSTEM_PAGE_SIZE);

    void *addr1 = mm_get_new_vm_page_from_kernel(1);
    void *addr2 = mm_get_new_vm_page_from_kernel(1);

    printf("page1 = %p, page2 = %p\n", addr1, addr2);

    if (addr1) {
        mm_return_vm_page_to_kernel(addr1, 1);
    }
    if (addr2) {
        mm_return_vm_page_to_kernel(addr2, 1);
    }

    return 0;
}
