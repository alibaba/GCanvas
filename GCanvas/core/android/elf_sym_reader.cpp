#include <jni.h>
#include <android/log.h>
#include "elf_sym_reader.h"
#include "common.h"
#include "external.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <dlfcn.h>



#define TAG "fakeJni"

#define LOGE(fmt, args...) \
{ \
__android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args); \
}


#define LOGD(fmt, args...) \
{ \
__android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args); \
}


#define ABORT(...) if(0)


static int is_32bit_elf;
dwarf_vma (*byte_get)(unsigned char *, int);
static Elf_Internal_Shdr * section_headers;
static Elf_Internal_Ehdr elf_header;
static char *findSymbol;
static char * string_table;
static Elf_Internal_Shdr * symtab_shndx_hdr;

static unsigned long process_symbol_table(FILE * file);

dwarf_vma byte_get_little_endian(unsigned char *field, int size) {
	switch (size) {
	case 1:
		return *field;

	case 2:
		return ((unsigned int) (field[0])) | (((unsigned int) (field[1])) << 8);

	case 4:
		return ((unsigned long) (field[0])) | (((unsigned long) (field[1])) << 8) | (((unsigned long) (field[2])) << 16) | (((unsigned long) (field[3])) << 24);

	case 8:
		if (sizeof(dwarf_vma) == 8)
			return ((dwarf_vma) (field[0])) | (((dwarf_vma) (field[1])) << 8) | (((dwarf_vma) (field[2])) << 16) | (((dwarf_vma) (field[3])) << 24)
					| (((dwarf_vma) (field[4])) << 32) | (((dwarf_vma) (field[5])) << 40) | (((dwarf_vma) (field[6])) << 48) | (((dwarf_vma) (field[7])) << 56);
		else if (sizeof(dwarf_vma) == 4)
			/* We want to extract data from an 8 byte wide field and
			 place it into a 4 byte wide field.  Since this is a little
			 endian source we can just use the 4 byte extraction code.  */
			return ((unsigned long) (field[0])) | (((unsigned long) (field[1])) << 8) | (((unsigned long) (field[2])) << 16)
					| (((unsigned long) (field[3])) << 24);
		else {
			ABORT("error size (%d)",size);
		}

	default:
		ABORT("error size (%d)",size);
	}
	return *field;
}

dwarf_vma byte_get_big_endian(unsigned char *field, int size) {
	switch (size) {
	case 1:
		return *field;

	case 2:
		return ((unsigned int) (field[1])) | (((int) (field[0])) << 8);

	case 4:
		return ((unsigned long) (field[3])) | (((unsigned long) (field[2])) << 8) | (((unsigned long) (field[1])) << 16) | (((unsigned long) (field[0])) << 24);

	case 8:
		if (sizeof(dwarf_vma) == 8)
			return ((dwarf_vma) (field[7])) | (((dwarf_vma) (field[6])) << 8) | (((dwarf_vma) (field[5])) << 16) | (((dwarf_vma) (field[4])) << 24)
					| (((dwarf_vma) (field[3])) << 32) | (((dwarf_vma) (field[2])) << 40) | (((dwarf_vma) (field[1])) << 48) | (((dwarf_vma) (field[0])) << 56);
		else if (sizeof(dwarf_vma) == 4) {
			/* Although we are extracing data from an 8 byte wide field,
			 we are returning only 4 bytes of data.  */
			field += 4;
			return ((unsigned long) (field[3])) | (((unsigned long) (field[2])) << 8) | (((unsigned long) (field[1])) << 16)
					| (((unsigned long) (field[0])) << 24);
		} else {
			ABORT("error size (%d)",size);
		}

	default:
		//error (_("Unhandled data length: %d\n"), size);
		ABORT("error size (%d)",size);
	}
	return *field;
}

const char *get_elf_class(unsigned int elf_class) {
	static char buff[32];

	switch (elf_class) {
	case ELFCLASSNONE:
		return "none";
	case ELFCLASS32:
		return "ELF32";
	case ELFCLASS64:
		return "ELF64";
	default:
		sprintf(buff, "<unknown: %x>", elf_class);
		return buff;
	}
}

static void *get_data(void * var, FILE * file, long offset, size_t size, size_t nmemb, const char * reason) {
	void * mvar;

	if (size == 0 || nmemb == 0)
		return NULL;

	if (fseek(file, offset, SEEK_SET)) {
		LOGE("Unable to seek to 0x%lx for %s\n", (unsigned long ) offset, reason);
		return NULL;
	}

	mvar = var;
	if (mvar == NULL) {
		/* Check for overflow.  */
		if (nmemb < (~(size_t) 0 - 1) / size)
			/* + 1 so that we can '\0' terminate invalid string table sections.  */
			mvar = malloc(size * nmemb + 1);

		if (mvar == NULL) {
			LOGE("Out of memory allocating 0x%lx bytes for %s\n", (unsigned long )(size * nmemb), reason);
			return NULL;
		}

		((char *) mvar)[size * nmemb] = '\0';
	}

	if (fread(mvar, size, nmemb, file) != nmemb) {
		LOGE("Unable to read in 0x%lx bytes of %s\n", (unsigned long )(size * nmemb), reason);
		if (mvar != var)
			free(mvar);
		return NULL;
	}

	return mvar;
}

static int get_32bit_section_headers(FILE * file, unsigned int num) {
	Elf32_External_Shdr * shdrs;
	Elf_Internal_Shdr * internal;
	unsigned int i;

	shdrs = (Elf32_External_Shdr *) get_data(NULL, file, elf_header.e_shoff, elf_header.e_shentsize, num, "section headers");
	if (!shdrs)
		return 0;

	section_headers = (Elf_Internal_Shdr *) malloc(num * sizeof(Elf_Internal_Shdr));

	if (section_headers == NULL) {
		LOGE("Out of memory\n");
		return 0;
	}

	for (i = 0, internal = section_headers; i < num; i++, internal++) {
		internal->sh_name = BYTE_GET(shdrs[i].sh_name);
		internal->sh_type = BYTE_GET(shdrs[i].sh_type);
		internal->sh_flags = BYTE_GET(shdrs[i].sh_flags);
		internal->sh_addr = BYTE_GET(shdrs[i].sh_addr);
		internal->sh_offset = BYTE_GET(shdrs[i].sh_offset);
		internal->sh_size = BYTE_GET(shdrs[i].sh_size);
		internal->sh_link = BYTE_GET(shdrs[i].sh_link);
		internal->sh_info = BYTE_GET(shdrs[i].sh_info);
		internal->sh_addralign = BYTE_GET(shdrs[i].sh_addralign);
		internal->sh_entsize = BYTE_GET(shdrs[i].sh_entsize);
	}

	free(shdrs);

	return 1;
}

static int get_64bit_section_headers(FILE * file, unsigned int num) {
	Elf64_External_Shdr * shdrs;
	Elf_Internal_Shdr * internal;
	unsigned int i;

	shdrs = (Elf64_External_Shdr *) get_data(NULL, file, elf_header.e_shoff, elf_header.e_shentsize, num, "section headers");
	if (!shdrs)
		return 0;

	section_headers = (Elf_Internal_Shdr *) malloc(num * sizeof(Elf_Internal_Shdr));

	if (section_headers == NULL) {
		LOGE("Out of memory\n");
		return 0;
	}

	for (i = 0, internal = section_headers; i < num; i++, internal++) {
		internal->sh_name = BYTE_GET(shdrs[i].sh_name);
		internal->sh_type = BYTE_GET(shdrs[i].sh_type);
		internal->sh_flags = BYTE_GET(shdrs[i].sh_flags);
		internal->sh_addr = BYTE_GET(shdrs[i].sh_addr);
		internal->sh_size = BYTE_GET(shdrs[i].sh_size);
		internal->sh_entsize = BYTE_GET(shdrs[i].sh_entsize);
		internal->sh_link = BYTE_GET(shdrs[i].sh_link);
		internal->sh_info = BYTE_GET(shdrs[i].sh_info);
		internal->sh_offset = BYTE_GET(shdrs[i].sh_offset);
		internal->sh_addralign = BYTE_GET(shdrs[i].sh_addralign);
	}

	free(shdrs);

	return 1;
}

unsigned long min_vaddr = 0xffffffff;
static Elf_Internal_Phdr * program_headers;

static int get_32bit_program_headers(FILE * file, Elf_Internal_Phdr * program_headers) {
	Elf32_External_Phdr * phdrs;
	Elf32_External_Phdr * external;
	Elf_Internal_Phdr * internal;
	unsigned int i;

	phdrs = (Elf32_External_Phdr *) get_data(NULL, file, elf_header.e_phoff, elf_header.e_phentsize, elf_header.e_phnum, "program headers");

	if (!phdrs)
		return 0;

	for (i = 0, internal = program_headers, external = phdrs; i < elf_header.e_phnum; i++, internal++, external++) {
		internal->p_type = BYTE_GET(external->p_type);
		internal->p_offset = BYTE_GET(external->p_offset);
		internal->p_vaddr = BYTE_GET(external->p_vaddr);
		internal->p_paddr = BYTE_GET(external->p_paddr);
		internal->p_filesz = BYTE_GET(external->p_filesz);
		internal->p_memsz = BYTE_GET(external->p_memsz);
		internal->p_flags = BYTE_GET(external->p_flags);
		internal->p_align = BYTE_GET(external->p_align);
	}

	free(phdrs);

	return 1;
}

static int get_64bit_program_headers(FILE * file, Elf_Internal_Phdr * program_headers) {
	Elf64_External_Phdr * phdrs;
	Elf64_External_Phdr * external;
	Elf_Internal_Phdr * internal;
	unsigned int i;

	phdrs = (Elf64_External_Phdr *) get_data(NULL, file, elf_header.e_phoff, elf_header.e_phentsize, elf_header.e_phnum, "program headers");
	if (!phdrs)
		return 0;

	for (i = 0, internal = program_headers, external = phdrs; i < elf_header.e_phnum; i++, internal++, external++) {
		internal->p_type = BYTE_GET(external->p_type);
		internal->p_flags = BYTE_GET(external->p_flags);
		internal->p_offset = BYTE_GET(external->p_offset);
		internal->p_vaddr = BYTE_GET(external->p_vaddr);
		internal->p_paddr = BYTE_GET(external->p_paddr);
		internal->p_filesz = BYTE_GET(external->p_filesz);
		internal->p_memsz = BYTE_GET(external->p_memsz);
		internal->p_align = BYTE_GET(external->p_align);
	}

	free(phdrs);

	return 1;
}

static int get_program_headers(FILE * file) {
	Elf_Internal_Phdr * phdrs;

	/* Check cache of prior read.  */
	if (program_headers != NULL)
		return 1;

	phdrs = (Elf_Internal_Phdr *) malloc(elf_header.e_phnum * sizeof(Elf_Internal_Phdr));

	if (phdrs == NULL) {
		return 0;
	}

	if (is_32bit_elf ? get_32bit_program_headers(file, phdrs) : get_64bit_program_headers(file, phdrs)) {
		program_headers = phdrs;
		return 1;
	}

	free(phdrs);
	return 0;
}

static int process_program_headers(FILE * file) {

	Elf_Internal_Phdr * segment;
	unsigned int i;

	if (elf_header.e_phnum == 0) {
		return 0;
	}

	if (!get_program_headers(file))
		return 0;

	for (i = 0, segment = program_headers; i < elf_header.e_phnum; i++, segment++) {

		if (segment->p_type == PT_LOAD) {

			if ((unsigned long) segment->p_vaddr < min_vaddr) {

				min_vaddr = (unsigned long) segment->p_vaddr;
			}
		}

	}

	return 1;

}

unsigned long readElfSymbols(FILE * file) {

	unsigned char e_ident[EI_NIDENT];
	if (fread(e_ident, EI_NIDENT, 1, file) != 1)
		return 0;

	switch (e_ident[EI_DATA]) {
	default: /* fall through */
	case ELFDATANONE: /* fall through */
	case ELFDATA2LSB:
		byte_get = byte_get_little_endian;
		break;
	case ELFDATA2MSB:
		byte_get = byte_get_big_endian;
		break;
	}

	is_32bit_elf = (e_ident[EI_CLASS] != ELFCLASS64);

	if (is_32bit_elf) {
		Elf32_External_Ehdr ehdr32;
		if (fread(ehdr32.e_type, sizeof(ehdr32) - EI_NIDENT, 1, file) != 1)
			return 0;

		elf_header.e_type = BYTE_GET(ehdr32.e_type);
		elf_header.e_machine = BYTE_GET(ehdr32.e_machine);
		elf_header.e_version = BYTE_GET(ehdr32.e_version);
		elf_header.e_phoff = BYTE_GET(ehdr32.e_phoff);
		elf_header.e_shoff = BYTE_GET(ehdr32.e_shoff);
		elf_header.e_flags = BYTE_GET(ehdr32.e_flags);
		elf_header.e_ehsize = BYTE_GET(ehdr32.e_ehsize);
		elf_header.e_phentsize = BYTE_GET(ehdr32.e_phentsize);
		elf_header.e_phnum = BYTE_GET(ehdr32.e_phnum);
		elf_header.e_shentsize = BYTE_GET(ehdr32.e_shentsize);
		elf_header.e_shnum = BYTE_GET(ehdr32.e_shnum);
		elf_header.e_shstrndx = BYTE_GET(ehdr32.e_shstrndx);

	} else {
		Elf64_External_Ehdr ehdr64;
		if (fread(ehdr64.e_type, sizeof(ehdr64) - EI_NIDENT, 1, file) != 1)
			return 0;

		elf_header.e_type = BYTE_GET(ehdr64.e_type);
		elf_header.e_machine = BYTE_GET(ehdr64.e_machine);
		elf_header.e_version = BYTE_GET(ehdr64.e_version);
		elf_header.e_phoff = BYTE_GET(ehdr64.e_phoff);
		elf_header.e_shoff = BYTE_GET(ehdr64.e_shoff);
		elf_header.e_flags = BYTE_GET(ehdr64.e_flags);
		elf_header.e_ehsize = BYTE_GET(ehdr64.e_ehsize);
		elf_header.e_phentsize = BYTE_GET(ehdr64.e_phentsize);
		elf_header.e_phnum = BYTE_GET(ehdr64.e_phnum);
		elf_header.e_shentsize = BYTE_GET(ehdr64.e_shentsize);
		elf_header.e_shnum = BYTE_GET(ehdr64.e_shnum);
		elf_header.e_shstrndx = BYTE_GET(ehdr64.e_shstrndx);

	}

	unsigned long result = 0;

	process_program_headers(file);

	if (elf_header.e_shoff) {
		/* There may be some extensions in the first section header.  Don't
		 bomb if we can't read it.  */
		if (is_32bit_elf)
			get_32bit_section_headers(file, elf_header.e_shnum);
		else
			get_64bit_section_headers(file, elf_header.e_shnum);

		result = process_symbol_table(file);

		result = result - min_vaddr;

	}

	return result;

}

static Elf_Internal_Sym *get_32bit_elf_symbols(FILE * file, Elf_Internal_Shdr * section) {
	unsigned long number;
	Elf32_External_Sym * esyms;
	Elf_External_Sym_Shndx * shndx;
	Elf_Internal_Sym * isyms;
	Elf_Internal_Sym * psym;
	unsigned int j;

	esyms = (Elf32_External_Sym*) get_data(NULL, file, section->sh_offset, 1, section->sh_size, "symbols");
	if (!esyms)
		return NULL;

	shndx = NULL;
	if (symtab_shndx_hdr != NULL && (symtab_shndx_hdr->sh_link == (unsigned long) (section - section_headers))) {
		shndx = (Elf_External_Sym_Shndx *) get_data(NULL, file, symtab_shndx_hdr->sh_offset, 1, symtab_shndx_hdr->sh_size, "symtab shndx");
		if (!shndx) {
			free(esyms);
			return NULL;
		}
	}

	number = section->sh_size / section->sh_entsize;
	isyms = (Elf_Internal_Sym *) malloc(number * sizeof(Elf_Internal_Sym));

	if (isyms == NULL) {
		LOGE("Out of memory\n");
		if (shndx)
			free(shndx);
		free(esyms);
		return NULL;
	}

	for (j = 0, psym = isyms; j < number; j++, psym++) {
		psym->st_name = BYTE_GET(esyms[j].st_name);
		psym->st_value = BYTE_GET(esyms[j].st_value);
		psym->st_size = BYTE_GET(esyms[j].st_size);
		psym->st_shndx = BYTE_GET(esyms[j].st_shndx);
		if (psym->st_shndx == (SHN_XINDEX & 0xffff) && shndx != NULL)
			psym->st_shndx = byte_get((unsigned char *) &shndx[j], sizeof(shndx[j]));
		else if (psym->st_shndx >= (SHN_LORESERVE & 0xffff))
			psym->st_shndx += SHN_LORESERVE - (SHN_LORESERVE & 0xffff);
		psym->st_info = BYTE_GET(esyms[j].st_info);
		psym->st_other = BYTE_GET(esyms[j].st_other);
	}

	if (shndx)
		free(shndx);
	free(esyms);

	return isyms;
}

static Elf_Internal_Sym *get_64bit_elf_symbols(FILE * file, Elf_Internal_Shdr * section) {
	unsigned long number;
	Elf64_External_Sym * esyms;
	Elf_External_Sym_Shndx * shndx;
	Elf_Internal_Sym * isyms;
	Elf_Internal_Sym * psym;
	unsigned int j;

	esyms = (Elf64_External_Sym *) get_data(NULL, file, section->sh_offset, 1, section->sh_size, "symbols");
	if (!esyms)
		return NULL;

	shndx = NULL;
	if (symtab_shndx_hdr != NULL && (symtab_shndx_hdr->sh_link == (unsigned long) (section - section_headers))) {
		shndx = (Elf_External_Sym_Shndx *) get_data(NULL, file, symtab_shndx_hdr->sh_offset, 1, symtab_shndx_hdr->sh_size, "symtab shndx");
		if (!shndx) {
			free(esyms);
			return NULL;
		}
	}

	number = section->sh_size / section->sh_entsize;
	isyms = (Elf_Internal_Sym *) malloc(number * sizeof(Elf_Internal_Sym));

	if (isyms == NULL) {
		LOGE("Out of memory\n");
		if (shndx)
			free(shndx);
		free(esyms);
		return NULL;
	}

	for (j = 0, psym = isyms; j < number; j++, psym++) {
		psym->st_name = BYTE_GET(esyms[j].st_name);
		psym->st_info = BYTE_GET(esyms[j].st_info);
		psym->st_other = BYTE_GET(esyms[j].st_other);
		psym->st_shndx = BYTE_GET(esyms[j].st_shndx);
		if (psym->st_shndx == (SHN_XINDEX & 0xffff) && shndx != NULL)
			psym->st_shndx = byte_get((unsigned char *) &shndx[j], sizeof(shndx[j]));
		else if (psym->st_shndx >= (SHN_LORESERVE & 0xffff))
			psym->st_shndx += SHN_LORESERVE - (SHN_LORESERVE & 0xffff);
		psym->st_value = BYTE_GET(esyms[j].st_value);
		psym->st_size = BYTE_GET(esyms[j].st_size);
	}

	if (shndx)
		free(shndx);
	free(esyms);

	return isyms;
}

static unsigned int print_symbol(int width, const char * symbol, bfd_vma value) {
	const char * c;

	while (width) {
		int len;

		c = symbol;

		/* Look for non-printing symbols inside the symbol's name.
		 This test is triggered in particular by the names generated
		 by the assembler for local labels.  */
		while (isprint(*c))
			c++;

		len = c - symbol;

		if (len) {
			if (len > width)
				len = width;

			if (strstr(symbol, findSymbol)) {
				LOGD("yes i find (%s) addr is (0x%lx)", findSymbol, value);
				return 1;
			}

			width -= len;
		}

		if (*c == 0 || width == 0)
			break;
		symbol = c + 1;
	}

	return 0;
}

static unsigned long process_symbol_table(FILE * file) {

	unsigned int i;
	Elf_Internal_Shdr * section;
	bool isFindSymbol = false;
	unsigned long result = 0;

	for (i = 0, section = section_headers; i < elf_header.e_shnum && !isFindSymbol; i++, section++) {
		unsigned int si;
		char * strtab = NULL;
		unsigned long int strtab_size = 0;
		Elf_Internal_Sym * symtab;
		Elf_Internal_Sym * psym;

		if (section->sh_type != SHT_SYMTAB && section->sh_type != SHT_DYNSYM)
			continue;

		symtab = GET_ELF_SYMBOLS(file, section);
		if (symtab == NULL)
			continue;

		if (section->sh_link == elf_header.e_shstrndx) {
			continue;
		} else if (section->sh_link < elf_header.e_shnum) {
			Elf_Internal_Shdr * string_sec;

			string_sec = section_headers + section->sh_link;

			strtab = (char *) get_data(NULL, file, string_sec->sh_offset, 1, string_sec->sh_size, "string table");
			strtab_size = strtab != NULL ? string_sec->sh_size : 0;
		}

		int number = section->sh_size / section->sh_entsize;

		for (si = 0, psym = symtab; si < number; si++, psym++) {

			if (print_symbol(1024, psym->st_name < strtab_size ? strtab + psym->st_name : "<corrupt>", psym->st_value) == 1) {
				result = psym->st_value;
				isFindSymbol = true;

				uint8_t *value = (uint8_t*) get_data(NULL, file, result, 1, 64, "check result");

				break;

			}

		}

		free(symtab);
		if (strtab != string_table)
			free(strtab);

		if (isFindSymbol)
			break;

	}

	return result;

}

char* readIdent(void *addr) {

	unsigned char e_ident[EI_NIDENT];
	memcpy(e_ident, addr, EI_NIDENT);

	is_32bit_elf = (e_ident[EI_CLASS] != ELFCLASS64);

	if (is_32bit_elf) {
		return (char *) "/system/lib/libart.so";
	} else {
		return (char *) "/system/lib64/libart.so";
	}

}

void *getArtSoAddr(char *sopath) {

	char line[1024] = { 0 };
	FILE *fp;
	unsigned long addr = 0;
	char *pch;

	if ((fp = fopen("/proc/self/maps", "r")) == NULL) {
		LOGE("/proc/self/maps open failed (%s)", strerror(errno));
		return 0;
	}
	while (fgets(line, sizeof(line), fp)) {

		if (strstr(line, sopath)) {
			pch = strtok(line, "-");
			addr = strtoul(pch, NULL, 16);
			break;
		}
	}

	fclose(fp);

	return (void *) addr;
}

unsigned long getSymbolAddrNormal(char *symbol) {

	void *addr = dlsym(RTLD_DEFAULT, symbol);

	return (unsigned long) addr;

}

namespace ali {

uint32_t get_symbol_offset_file(const char* symbol) {

	FILE* fp;
	fp = fopen("/system/lib/libart.so", "r");
	if (fp == NULL) {
		fp = fopen("/system/lib/libaoc.so", "r");
	}
	if (fp == NULL) {
		ABORT("open /system/lib/libart.so or libaoc.so error!");
	}

	findSymbol = strdup(symbol);

	uint32_t offset = (uint32_t) readElfSymbols(fp);

	fclose(fp);

	return offset;

}

unsigned long getSymbolAddr(char *symbol, char *sopath,char *soname) {

	unsigned long result = 0;
	result = getSymbolAddrNormal(symbol);
	if (result != 0)
		return result;

	void *soAddr = getArtSoAddr(soname);
	if (soAddr == NULL) {
		LOGE("getSymbolAddr soAddr is %p", soAddr);
		return 0;
	}

	findSymbol = strdup(symbol);

	unsigned long offset = 0;
	FILE * file;
	file = fopen(sopath, "rb");

	if (file != NULL) {
		offset = readElfSymbols(file);
	} else {
		LOGE("file open failed (%s)", strerror(errno));
		return 0;
	}

	if (offset != 0) {
		result = (unsigned long) soAddr + offset;
	}

	fclose(file);

	return result;

}

} //end namespace ali

