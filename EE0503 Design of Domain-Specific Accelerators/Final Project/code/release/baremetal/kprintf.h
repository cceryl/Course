#ifndef _BOOT_KPRINTF_H
#define _BOOT_KPRINTF_H

extern void kputc(char ch);
extern void kputs(const char *);
extern void printf(const char *, ...);

#endif /* _BOOT_KPRINTF_H */
