#ifndef _LOADER_H
#define _LOADER_H

#define MAX_FILE_PATH 256

struct loader_args {
    unsigned long loader_addr;
    unsigned long entry_point;
    unsigned long sp;
    char filename[MAX_FILE_PATH];
    unsigned long vcpu_num;
};

struct vm_struct2;
int elf_binary_loader(void *, unsigned long *, unsigned long *, struct vm_struct2 *);
int raw_binary_loader(void *, unsigned long *, unsigned long *, struct vm_struct2 *);

struct vm_struct2;
void copy_code_to_memory(struct vm_struct2 *vm, unsigned long va, unsigned long from, unsigned long size);
int load_file_to_memory(struct vm_struct2 *vm, const char *name, unsigned long va);

#endif
