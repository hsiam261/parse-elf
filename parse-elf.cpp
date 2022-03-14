#include <cstdint>
#include <cstdio>
#include <utility>
#include <vector>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <elf.h>

using namespace std;

void load_from_file_to_buffer(fstream& file,streamoff offset,streamsize size,char* buff) {
	if(!file.seekg(offset)) {
		cerr << "Failed to seek offset " << offset << endl;
		exit(1);
	}
	if (!file.read(buff,size)) {
		cerr << "Could not read " << size << "bytes. Only " << file.gcount() << "bytes were read." << endl;
		exit(1);
	}		
}

void load_elf64_header(fstream& elffile,Elf64_Ehdr* header) {
	load_from_file_to_buffer(elffile,0,sizeof(Elf64_Ehdr),(char*)header);
}

Elf64_Ehdr* load_elf64_header(fstream& elffile) {
	Elf64_Ehdr* header = new Elf64_Ehdr();
	load_from_file_to_buffer(elffile,0,sizeof(Elf64_Ehdr),(char*)header);
	return header;
}

void load_elf64_section_header_table(fstream& elffile,Elf64_Ehdr* header,Elf64_Shdr* section_header_table) {
	load_from_file_to_buffer(elffile,header->e_shoff,header->e_shnum*header->e_shentsize,(char*)section_header_table);
}


Elf64_Shdr* load_elf64_section_header_table(fstream& elffile,Elf64_Ehdr* header) {
	Elf64_Shdr* section_header_table = new Elf64_Shdr[header->e_shnum];
	load_elf64_section_header_table(elffile,header,section_header_table);
	return section_header_table;
}

void load_elf64_section(fstream& elffile, Elf64_Shdr* section_header,char* section) {	
	auto section_size = section_header->sh_size;
	if(section_header->sh_type==SHT_NOBITS) section_size=0;
	load_from_file_to_buffer(elffile,section_header->sh_offset,section_size,section);
}

char* load_elf64_section(fstream& elffile, Elf64_Shdr* section_header) {	
	auto section_size = section_header->sh_size;
	if(section_header->sh_type==SHT_NOBITS) section_size=0;
	char* section = new char[section_size];
	load_from_file_to_buffer(elffile,section_header->sh_offset,section_size,section);
	return section;
}

void load_elf64_section_and_linked_section(fstream& elffile,Elf64_Shdr* section_header_table,Elf64_Shdr* section_header,
		char* section, char* linked_section) {
	auto linked_section_header = section_header_table+section_header->sh_link;
	load_elf64_section(elffile,section_header,section);
	load_elf64_section(elffile,linked_section_header,linked_section);
}

std::pair<char*,char*> load_elf64_section_and_linked_section(fstream& elffile,Elf64_Shdr* section_header_table,Elf64_Shdr* section_header) {
	auto linked_section_header = section_header_table+section_header->sh_link;
	auto section = load_elf64_section(elffile,section_header);
	auto linked_section = load_elf64_section(elffile,linked_section_header);
	return {section,linked_section};

}

Elf64_Half get_section_header_string_table_section_index(Elf64_Ehdr* header,Elf64_Shdr* first_entry) {
	if(header->e_shstrndx==SHN_UNDEF) return SHN_UNDEF;
	if(header->e_shstrndx>=SHN_LORESERVE) return first_entry->sh_link; //header->e_shstrndx contains SHN_XINDEX
	else return header->e_shstrndx;
} 

void print_elf64_header(Elf64_Ehdr* header) {
	ios_base::fmtflags f(cout.flags());
	cout << "ELF Header:\n";

	// An array of size EI_NIDENT (16) bytes
	cout << left << setw(52)  << "E_IDENT:";
	for(int i=0;i<EI_NIDENT;i++) {
		cout << hex << setfill('0') << setw(2) << (uint32_t)header->e_ident[i] << " ";
	}
	cout << endl;
	// First 4 bytes of array is the magic number (0x7fELF)
	cout << left << setw(45) << setfill(' ') << "\tMAGIC[0-3]:" << setfill('0') << setw(2) << "0x" << (uint32_t)header->e_ident[EI_MAG0];
	cout.flags(f);
	cout << header->e_ident[EI_MAG1] << header->e_ident[EI_MAG2] << header->e_ident[EI_MAG3] << endl;
	// Next byte defines the class of the file 
	// can have 3 values ELFCLASSNONE(0), ELFCLASS32(1), ELFCLASS64(2)
	cout << left << setw(45) << setfill(' ')  << "\tCLASS:";
	switch (header->e_ident[EI_CLASS]) {
		case ELFCLASSNONE:
			cout << "Invalid File Type";
			break;
		case ELFCLASS32:
			cout << "32 bit ELF file";
			break;
		case ELFCLASS64:
			cout << "64 bit ELF file";
			break;
		default:
			cout << "Unknown: " << (int)header->e_ident[EI_CLASS];
	}
	cout << endl;
	//Next byte specifies data encoding or the endianness
	//can have 3 values: ELFDATANONE(0), ELFDATA2LSB(1), ELFDATA2MSB(2)
	cout << left << setw(45) << setfill(' ') << "\tDATA:";
	switch (header->e_ident[EI_DATA]) {
		case ELFDATANONE:
			cout << "Unknown data format";
			break;
		case ELFDATA2LSB:
			cout << "2's compliment Little Endian";
			break;
		case ELFDATA2MSB:
			cout << "2's compliment Big Endian";
			break;
		default:
			cout << "Error: " << (int)header->e_ident[EI_DATA];
	}
	cout << endl;
	//next byte is specifies the version of elf format
	//there is only one version (version 1)
	//So this byte can have two values: EV_NONE (0) and EV_CURRENT(1)
	cout << left << setw(45) << setfill(' ') << "\tELF_VERSION:";
	switch (header->e_ident[EI_VERSION]) {
		case EV_NONE:
			cout << "Invalid Version";
			break;
		case EV_CURRENT:
			cout << "Current Version (1)";
			break;
		default:
			cout << "Unknown: " << header->e_ident[EI_VERSION];

	}
	cout << endl;
	//next byte is the OS ABI 
	//can have many values
	cout << left << setw(45) << setfill(' ') << "\tOS ABI:";
	switch (header->e_ident[EI_OSABI]) {
		case ELFOSABI_SYSV:
			cout << "Unix SYSV ABI";
			break;
		case ELFOSABI_HPUX:
			cout << "HP-UX ABI";
			break;
		case ELFOSABI_NETBSD:
			cout << "NET BSD ABI";
			break;
		case ELFOSABI_LINUX:
			cout << "Linux ABI";
			break;
		default:
			cout << header->e_ident[EI_OSABI];
	}
	cout << endl;
	//next byte is the ABI version
	//not used in wild
	//EI_ABIVERSION
	//
	
	//next 7 bytes are paddings (of 0s)


	cout << endl;

	
	//e_type specifies the type of the binary file
	//can have 5 values: ET_NONE(0), ET_REL(1), ET_EXEC(2), ET_DYN(3), ET_CORE(4)
	cout << left << setw(52) << setfill(' ') << "E_TYPE:";
	switch (header->e_type) {
		case ET_NONE:
			cout << "Unknown File Type";
			break;
		case ET_REL:
			cout << "A relocatable object file, --unlinked binary";
			break;
		case ET_EXEC:
			cout << "Executable binary";
			break;
		case ET_DYN:
			cout << "Shared Object File, --dynamic libraries or pie executable";
			break;
		case ET_CORE:
			cout << "A core file, --memory image dumped from a process, can be used to debug a crash";
			break;
		default:
			cout << "Error: " << header->e_type << endl;
	}
	cout << endl;

	//e_machine specifies the hardware architecture
	cout << left << setw(52) << setfill(' ')<< "E_MACHINE:";
	switch (header->e_machine) {
		case EM_386:
			cout << "Intel 80386";
			break;
		case EM_860:
			cout << "Intel 80860";
			break;
		case EM_X86_64:
			cout << "AMD 64";
			break;
		default:
			cout << header->e_machine;
	}
	cout << endl;


	//e_version specifies version number
	//currently can have two values EV_NONE and EV_CURRENT
	cout << left << setw(52) << setfill(' ')<<"E_VERSION:";
	switch (header->e_version) {
		case EV_NONE:
			cout << "Invalid Version";
			break;
		case EV_CURRENT:
			cout << "Current Version";
			break;
		default:
			cout << "Error: " << header->e_version;
	}
	cout << endl;

	//entry point of process
	//virtual adress to entry point 
	cout << left << setw(52) << setfill(' ')<< "Entry Point:" << hex << "0x"<< header->e_entry << endl;
	cout.flags(f);

	//file offset to the program header or segment header table 
	cout << left << setw(52) << setfill(' ')<< "File offset to Program Header Table:" << header->e_phoff << endl;


	//file offset to the section header table 
	cout << left << setw(52) << setfill(' ')<<"File offset to Section Header Table:" << header->e_shoff << endl;

	//processor specific flags
	cout << left << setw(52) << setfill(' ')<<"Flags(Not defined yet):" << header->e_flags << endl;

	//program header size in bytes 
	cout << left << setw(52) << setfill(' ')<<"Program Header Size (in bytes):" <<header->e_phentsize << endl;

	//number of program headers in the program header table 
	cout << left << setw(52) << setfill(' ')<<"Number of Program Header Entries:" << header->e_phnum << endl;


	//section header size in bytes 
	cout << left << setw(52) << setfill(' ')<<"Section Header Size (in bytes):" <<header->e_shentsize << endl;

	//number of section headers in the program header table 
	cout << left << setw(52) << setfill(' ')<<"Number of Section Header Entries:" << header->e_shnum << endl;

	//section name string table index
	cout << left << setw(52) << setfill(' ')<<"Section Table index for section name string table:";
	switch (header->e_shstrndx) {
		case SHN_UNDEF:
			cout << "No section name string table exists";
			break;
		default:
			if(header->e_shstrndx<SHN_LORESERVE) cout << header->e_shstrndx;
			else cout << "Section String Table Index is in sh_link of the 0th section (" << header->e_shstrndx << ")"; 
	}
	cout << endl;
	
	cout.flags(f);

}


void print_elf64_section_header(Elf64_Shdr* section,char* section_header_string_table) {
	ios_base::fmtflags f(cout.flags());

	//section header name index in string table
	cout << left <<  setw(52) <<  "Name:" << section_header_string_table+section->sh_name << endl;
	
	//section type
	cout << left <<  setw(52) << "Type:";
	switch (section->sh_type) {
		case SHT_NULL:
			cout << "SH_NULL (Unused Section Header)";
			break;
		case SHT_PROGBITS: 
			cout << "SHT_PROGBITS (Program Data)";
			break;
		case SHT_SYMTAB:
			cout << "Symbol Table";
			break;
		case SHT_STRTAB:
			cout << "String Table";
			break;
		case SHT_RELA:
			cout << "'Rela' Relocation Table";
			break;
		case SHT_HASH:
			cout << "Symbol Hash Table";
			break;
		case SHT_DYNAMIC:
			cout << "Dynamic Linking Table";
			break;
		case SHT_NOTE:
			cout << "SHT_NOTE (Note)";
			break;
		case SHT_NOBITS:
			cout << "SHT_NOBITS (Uninitialized Space)";
			break;
		case SHT_REL:
			cout << "Relocation Table";
			break;
		case SHT_SHLIB:
			cout << "SHT_SHLIB (Reserved)";
			break;
		case SHT_DYNSYM:
			cout << "Dynamic Loader Symbol Table";
			break;
		default:
			if(section->sh_type>=SHT_LOOS && section->sh_type<=SHT_HIOS)
				cout << "Environment Specific Type " << section->sh_type;
			else if(section->sh_type>=SHT_LOPROC && section->sh_type<=SHT_HIPROC)
				cout << "Processor Specific Type " << section->sh_type;
			else cout << "Error: Unknown Type " << section->sh_type;
	}
	cout << endl;

	//flags
	//16 flags
	//Writable         	-  W
	//Allocated        	-  A  - Retains memory during process execution 
	//Executable       	-  X
	//SHF_MERGE        	-  M  - The data section may be merged (?)
	//SHF_STRINGS      	-  S  - Section contains null terminated strings
	//SHF_INFO_LINK         -  I  - sht_info contains SHT index 
	//SHF_LINK_ORDER        -  L  - preserve link order after linking    
	//SHF_OS_NONCONFORMING  -  O  - Non-standard os specific handling required 
	//SHF_GROUP	        -  G  -	Section is member of a group
	//SHF_TLS		-  T  - Section hold thread-local data
	//SHF_COMPRESSED	-  C  - Section with compressed data
	//SHF_MASKOS	     	-  o  - OS-specific
	//SHF_MASKPROC	     	-  p  - Processor-specific
	//SHF_GNU_RETAIN	-  D  - Not to be GCed by linker
	//SHF_ORDERED		-  R  - Special ordering requirement (Solaris)  
	//SHF_EXCLUDE	     	-  E  - Section is excluded unless referenced or allocated (Solaris)
	cout << left <<  setw(52) << "Flags:";
	uint64_t flag_masks[] = {SHF_WRITE,SHF_ALLOC,SHF_EXECINSTR,SHF_STRINGS,SHF_STRINGS,SHF_INFO_LINK,SHF_LINK_ORDER,
	SHF_OS_NONCONFORMING,SHF_GROUP,SHF_TLS,SHF_COMPRESSED,SHF_MASKOS,SHF_MASKPROC,SHF_GNU_RETAIN,SHF_ORDERED,SHF_EXCLUDE};
	char flag[] = "WAXMSILOGTCopDRE";
	
	for(int i=0;i<16;i++) {
		if(section->sh_flags&flag_masks[i]) cout << flag[i];
		else cout << "_";
	}
	cout << endl;

	//virtual address of section (has A flag turned on)
	cout << left <<  setw(52) << "Virtual Address:" << hex << section->sh_addr << endl;
	cout.flags(f);
	//File offset 
	cout << left <<  setw(52) << "File Offset:" << section->sh_offset << endl;

	//size 
	cout << left <<  setw(52) << "Size (occupied in file):" << (section->sh_type==SHT_NOBITS?0:section->sh_size) << endl;

	//sh_link depends on section type 
	switch (section->sh_type) {
		case SHT_DYNAMIC:
		case SHT_SYMTAB:
		case SHT_DYNSYM:
			cout << left <<  setw(52) << "Section index of Associated String Table:" << section->sh_link;
			break;
		case SHT_HASH:
		case SHT_REL:
		case SHT_RELA:
			cout << left <<  setw(52) << "Section index of Associated Symbol Table:" << section->sh_link;
			break;
		default:
			if(section->sh_link==SHN_UNDEF) cout << left <<  setw(52) << "SH_LINK:" << "SHN_UNDEF";
			else cout << left <<  setw(52) << "SH_LINK:" << section->sh_link << "(Error)";
	}
	cout << endl;

	//sh_info
	switch (section->sh_type) {
		case SHT_SYMTAB:
		case SHT_DYNSYM:
			cout << left <<  setw(52) << "Index of first non local symbol:" << section->sh_info;
			break;
		case SHT_REL:
		case SHT_RELA:
			cout << left <<  setw(52) << "Section index of section to apply relocations:" << section->sh_info;
			break;
		default:
			if(section->sh_info==SHN_UNDEF) cout << left <<  setw(52) << "SH_LINK:" << "SHN_UNDEF";
			else cout << left <<  setw(52) << "SH_INFO:" << section->sh_info << "(Error)";
	}
	cout << endl;

	//alignment_requirements
	//must be a power of 2
	//the virtual address it will be mapped to will be a multiple of this value 
	cout << left <<  setw(52) << "Alignment:"  << section->sh_addralign << endl;

	//size of entries of each entries in the section for sections with fixed size entries
	//0 otherwise 
	cout << left <<  setw(52) << "Entry Size:" << section->sh_entsize << endl;
	cout.flags(f);
}

void print_elf64_section_header_table(Elf64_Shdr* section_header_table,int section_number,char* section_header_string_table) {
	for(int i=0;i<section_number;i++) {
		cout << "[" << i <<"]" << endl;
		print_elf64_section_header(section_header_table+i,section_header_string_table);
		cout << endl;
	}
}

void print_elf64_symbol(Elf64_Sym* symbol,char* string_table,Elf64_Half file_type) {
	ios_base::fmtflags f(cout.flags());
	
	//symbol_name
	cout << left <<setw(52) <<"Name:" << string_table+symbol->st_name << endl;
	
	//symbol binding 
	//Higher order 4 bits of symbol info
	//There are 3 types
	//local, global, weak
	//others are environment or processor specific
	int binding = ELF64_ST_BIND(symbol->st_info);
	cout << left <<setw(52) << "Binding:";
	switch (binding) {
		case STB_GLOBAL:
			cout << "Global (Non static global variabe or function)";
			break;
		case STB_LOCAL:
			cout << "Local (static Global Variables and Functions (only visible from the file))";
			break;
		case STB_WEAK:
			cout << "Weak (Global Variables or functions that are specified as weak. Can be overridden if linker finds a Global Symbol with same name )";
			break;
		default:
			if(binding>=STB_LOOS && binding<=STB_HIOS)
				cout << "Environment Speific Binding " << binding;
			else if(binding>=STB_LOPROC && binding <= STB_HIPROC)
				cout << "Processor Specific Binding " << binding;
			else 
				cout << "Unknown Binding " << binding;
	}
	cout << endl;

	//symbol type
	//lower order 4 bits of symbol info
	//6 types of symbols
	// STT_NOTYPE, STT_OBJECT, STT_FUNC, STT_SECTION, STT_FILE, STT_COMMON, STT_TLS	
	// others are environment or processor specific	
	int type = ELF64_ST_TYPE(symbol->st_info);
	cout << left << setw(52) << "Type:";
	switch (type) {
		case STT_NOTYPE:
			cout << "Type not specified (absolute type)";
			break;
		case STT_OBJECT:
			cout << "Data Object";
			break;
		case STT_FUNC:
			cout << "Code Object (function)";
			break;
		case STT_SECTION:
			cout << "Section Object (section name)";
			break;
		case STT_FILE:
			cout << "File Object (file name)";
			break;
		case STT_COMMON:
			cout << "Common Data Object (Common blocks with same name overlap each other in memory)";
			break;
		case STT_TLS:
			cout << "Thread Local Data Object";
			break;
		default:
			if(type>=STT_LOOS && type<=STT_HIOS) cout << "OS specific type " << type;
			else if(type>=STT_LOPROC && type<=STT_HIPROC) cout << "Processor specific type " << type;
	}
	cout << endl;
	
	//st_other
	//First 2 bytes control visibility
	//the other are 0 
	//visibility of symbol
	//can have 4 values
	//STV_DEFAULT, STV_INTERNAL, STV_HIDDEN, STV_PROTECTED
	cout << left << setw(52) << "Visibility:";
	int visibility =  ELF64_ST_VISIBILITY(symbol->st_other);
	switch (visibility) {
		case STV_DEFAULT:
			cout << "Default (global and weak symbol are visible and can be preempted)";
			break;
		case STV_PROTECTED:
			cout << "Protected (global and weak symbols are visible but not preemptable)";
			break;
		case STV_HIDDEN:
			cout << "Hidden (not visible)";
			break;
		case STV_INTERNAL:
			cout << "Internal (Processor Specific Visibility Control)";
			break;
		default:
			cout << "Error";

	}	
	cout << endl;

	//section index
	cout << left << setw(52) << "Section Index:";
	switch (symbol->st_shndx) {
		case SHN_UNDEF:
			cout << "SHN_UNDEF (undefined)";
			break;
		case SHN_ABS:
			cout << "SHN_ABS (absolute symbol - the corresponding address is an absolute address)";
			break;
		case SHN_COMMON:
			cout << "SHN_COMMON (symbol has been declared as a common block)";
			break;
		default:
			cout << symbol->st_shndx;
	}
	cout << endl;

	//section value
	//depending on file type and section index can be one of the three things
	//- alignment contraints (for relocatable file and section index is the SHN_COMMON)
	//- offset from begining of section (for relocatable files otherwise)
	//- virtual address (for relocatable files and executables)
	cout << left << setw(52) << "section value:";
	switch (file_type) {
		case ET_REL:
			if(symbol->st_shndx==SHN_COMMON) cout << symbol->st_value << " (Alignment)";
			else cout << symbol->st_value << " (byte offset from begining of the section)";
			break;
		case ET_DYN:
		case ET_EXEC:
			cout << "0x" << hex  << symbol->st_value;
			cout.flags(f);
			break;
		case ET_NONE:
		case ET_CORE:
			cout << symbol->st_value << " (Unknown) ";
	}
	cout << endl;
}




void print_elf64_symbol_table(Elf64_Sym* symbol_table,char* string_table,Elf64_Xword symbol_table_size,Elf64_Half file_type) {
	for(int j=0;j<symbol_table_size;j++) {
		cout << "[" << j << "]" << endl;
		print_elf64_symbol(symbol_table+j,string_table,file_type);
		cout << endl;
	}

}

void print_elf64_rela(Elf64_Rela* relocation_entry,Elf64_Sym* symbol_table,char* string_table, 
		Elf64_Half file_type) {
	ios_base::fmtflags f(cout.flags());
	const int WIDTH = 52;
	
	//byte_offset
	switch (file_type) {
		case ET_REL:
			cout << left << setw(WIDTH) <<"Offset into file:" << relocation_entry->r_offset;
			break;
		case ET_DYN:
		case ET_EXEC:
			cout << left << setw(WIDTH) << "Virtual Address:" << hex << relocation_entry->r_offset;
			break;
	}
	cout.flags(f);
	cout << endl;
	
	//symbol_index
	//can be read from r_info>>32 (or using ELF64_R_SYM macro)
	cout << left << setw(WIDTH) << "Symbol Index:" << ELF64_R_SYM(relocation_entry->r_info) << endl;
	print_elf64_symbol(symbol_table+ELF64_R_SYM(relocation_entry->r_info),string_table,file_type);

	//relocation_type
	//can be read from the lower order 4 bytes of r_info (or using the ELF64_R_TYPE macro)
	//There 38 different relocation_type in amd64 hardware (0-38)
	//they paired with the addend describe what relocation to do
	//amd64 only uses the rela structures (doesn't use Elf64_Rel)
	cout << left << setw(WIDTH) << "Relocation Type:" << ELF64_R_TYPE(relocation_entry->r_info) << endl;

	//addend
	cout << left << setw(WIDTH) << "Addend:" << relocation_entry->r_addend << endl;
}

void print_elf64_rela_table(Elf64_Rela* relocation_table,Elf64_Xword size,Elf64_Sym* symbol_table, char* string_table,
		Elf64_Half file_type) {
	for(int i=0;i<size;i++) {
		cout << "[" << i << "]" << endl;
		print_elf64_rela(relocation_table+i,symbol_table,string_table,file_type);
		cout << endl;
	}
}

void elf64_print_all_symbol_tables(fstream& elffile,Elf64_Ehdr* header,Elf64_Shdr* section_header_table,
		char* section_header_string_table) {
	for(int i=0;i<header->e_shnum;i++) {
		if(section_header_table[i].sh_type==SHT_SYMTAB || section_header_table[i].sh_type==SHT_DYNSYM) {
			auto l = load_elf64_section_and_linked_section(elffile,section_header_table,section_header_table+i);
			auto n = section_header_table[i].sh_size/section_header_table[i].sh_entsize;
			Elf64_Sym* symbol_table = (Elf64_Sym*)l.first;
			
			cout << "Symbol Table: " << i << endl;
			print_elf64_section_header(section_header_table+i,section_header_string_table);
			cout << "--------------------------------------------------" << endl;
			print_elf64_symbol_table(symbol_table,l.second,n,header->e_type);

			delete [] l.first;
			delete [] l.second;
			cout << endl;
		}
	}
}

void elf64_print_all_relocation_tables(fstream& elffile,Elf64_Ehdr* header,Elf64_Shdr* section_header_table,
		char* section_header_string_table) {
	for(int i=0;i<header->e_shnum;i++) {
		if(section_header_table[i].sh_type==SHT_RELA) {
			cout << "Relocation Table: " << i << endl;	
			print_elf64_section_header(section_header_table+i,section_header_string_table);
			cout << "--------------------------------------------------" << endl;

			auto symbol_table_header = section_header_table+section_header_table[i].sh_link;
			auto l = load_elf64_section_and_linked_section(elffile,section_header_table,symbol_table_header);
			Elf64_Rela* relocation_table = (Elf64_Rela*)load_elf64_section(elffile,section_header_table+i);
			print_elf64_rela_table(relocation_table,section_header_table[i].sh_size
					/section_header_table[i].sh_entsize,(Elf64_Sym*) l.first,l.second,header->e_type);
			delete [] relocation_table;
			delete [] l.first;
			delete [] l.second;
		}
	}

}

int main(int argc,char** argv) {
	if(argc!=2) {
		cerr << "Usage: parse-elf elffile" << endl;
		exit(1);
	}

	fstream elffile;
	elffile.open(argv[1],ios::in|ios::binary);

	if(!elffile.is_open()) {
		cerr << "Failed to open file " << argv[1] << endl;
		exit(1);
	}

	Elf64_Ehdr header;
	load_elf64_header(elffile,&header);

	print_elf64_header(&header);
	cout << endl;	
	
	//load section header table 
	Elf64_Shdr* section_header_table = load_elf64_section_header_table(elffile,&header);	
	char* section_header_string_table = load_elf64_section(elffile,section_header_table+get_section_header_string_table_section_index(&header,section_header_table));

	//print section header table
	print_elf64_section_header_table(section_header_table, header.e_shnum,section_header_string_table);
	
	//print all symbol_tables	
	elf64_print_all_symbol_tables(elffile,&header,section_header_table,section_header_string_table);

	//print all relocation_tables
	elf64_print_all_relocation_tables(elffile,&header,section_header_table,section_header_string_table);

	elffile.close();
}
