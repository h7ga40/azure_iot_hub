MEMORY
{
	RAM : ORIGIN = 0x0, LENGTH = 131072
	ROM : ORIGIN = 0xFFF00000, LENGTH = 1048576
}

SECTIONS
{
	.fvectors 0xFFFFFF80 : AT (0xFFFFFF80)
	{
		KEEP(*(.fvectors))
	} > ROM
	.text 0xFFF00000 : AT (0xFFF00000)
	{
		*(.text)
		*(.text.*)
		*(P)
		etext = .;
	} > ROM
	.rvectors : 
	{
		_rvectors_start = .;
		KEEP(*(.rvectors))
		_rvectors_end = .;
	} > ROM
	.init : 
	{
		*(.init)
		__preinit_array_start = .;
		*(.preinit_array)
		__preinit_array_end = .;
		__init_array_start = (. + 3) & ~ 3;
		*(.init_array)
		*(SORT(.init_array.*))
		__init_array_end = .;
		__fini_array_start = .;
		*(.fini_array)
		*(SORT(.fini_array.*))
		__fini_array_end = .;
	} > ROM
	.fini : 
	{
		*(.fini)
	} > ROM
	.got : 
	{
		*(.got)
		*(.got.plt)
	} > ROM
	.rodata : 
	{
		_brodata = .;
		*(.rodata)
		*(.rodata.*)
		*(C_1)
		*(C_2)
		*(C)
		_mrb_preset_const_start = .;
		*(.mrb_preset_const)
		_mrb_preset_const_end = .;
		_erodata = .;
	} > ROM
	.eh_frame_hdr : 
	{
		*(.eh_frame_hdr)
	} > ROM
	.eh_frame : 
	{
		*(.eh_frame)
	} > ROM
	.jcr : 
	{
		*(.jcr)
	} > ROM
	.tors : 
	{
		__CTOR_LIST__ = .;
		. = ALIGN(2);
		___ctors = .;
		*(.ctors)
		___ctors_end = .;
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		___dtors = .;
		*(.dtors)
		___dtors_end = .;
		__DTOR_END__ = .;
		. = ALIGN(2);
		_mdata = .;
	} > ROM
	.istack 0x00000 (NOLOAD) : AT (0x00000)
	{
		*(SI)
		_istack = .;
	} > RAM
	.ustack 0x00400 (NOLOAD) : AT (0x00400)
	{
		*(SU)
		_ustack = .;
	} > RAM
	.gcc_exc :
	{
		*(.gcc_exc)
	} > RAM
	.data : AT (_mdata)
	{
		_data = .;
		*(.data)
		*(.data.*)
		. = ALIGN(4);
		*(D)
		*(D_1)
		. = ALIGN(2);
		*(D_2)
		_mrb_preset_data_start = .;
		*(.mrb_preset_data)
		_mrb_preset_data_end = .;
		_edata = .;
	} > RAM
	.bss : 
	{
		_bss = .;
		*(.bss)
		*(.bss.**)
		*(COMMON)
		. = ALIGN(4);
		*(B)
		*(B_1)
		. = ALIGN(2);
		*(B_2)
		_ebss = .;
		. = ALIGN(4);
		_end = .;
	} > RAM
	.eth_memory (NOLOAD) : 
	{
		. = ALIGN(32);
		KEEP(*(ETH_MEMORY))
	} > RAM
	.heap (NOLOAD) :
	{
		_heap_stop = .;
		*(.heap*)
		__HeapBase = .;
	} > RAM
	__HeapLimit = ORIGIN(RAM) + LENGTH(RAM);
}
