/*publicly exposed structures and APIs of LMM in this header file
  uapi_mm.h is an interface between LMM lib and application
*/
#ifndef __UAPI_MM__
#define __UAPI_MM__

#include <stdint.h>
typedef struct mm_instance_ mm_instance_t;

/*Initialization Functions*/
void mm_init();

/*Registration function*/
void mm_instantiate_new_page_family(char *struct_name, uint32_t struct_size);


#define MM_REG_STRUCT(struct_name)  \
    (mm_instantiate_new_page_family(#struct_name, sizeof(struct_name)))

#endif /* __UAPI_MM__ */
