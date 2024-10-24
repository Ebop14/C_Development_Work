#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/webpage.h"

bool pagedir_init(const char* pageDirectory);

void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

webpage_t* pagedir_load(char* filePath);

#endif //__PAGEDIR_H