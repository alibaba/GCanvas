/*
 * elfSymReader.h
 *
 *  Created on: 2016年12月7日
 *      Author: lyz
 */

#ifndef BASE_ELF_READER_ELF_SYM_READER_H_
#define BASE_ELF_READER_ELF_SYM_READER_H_

#include <stdint.h>

namespace ali {

uint32_t get_symbol_offset_file(const char* symbol);
unsigned long getSymbolAddr(char *symbol,char *sopath,char *soname);



}  // namespace ali



#endif /* BASE_ELF_READER_ELF_SYM_READER_H_ */
