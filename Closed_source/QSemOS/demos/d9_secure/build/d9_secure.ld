OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(_start)
SECTIONS
{
    . = 0x140000;

    _start = .;

    /* text/read-only data */
    .text : {
        __text_start = .;
        KEEP(*(.text.boot.vectab1))
        KEEP(*(.text.boot.vectab2))
        KEEP(*(.text.boot))
        *(.text* .sram.text.glue_7* .gnu.linkonce.t.*)
    }

    .interp : { *(.interp) }
    .hash : { *(.hash) }
    .dynsym : { *(.dynsym) }
    .dynstr : { *(.dynstr) }
    .rel.text : { *(.rel.text) *(.rel.gnu.linkonce.t*) }
    .rela.text : { *(.rela.text) *(.rela.gnu.linkonce.t*) }
    .rel.data : { *(.rel.data) *(.rel.gnu.linkonce.d*) }
    .rela.data : { *(.rela.data) *(.rela.gnu.linkonce.d*) }
    .rel.rodata : { *(.rel.rodata) *(.rel.gnu.linkonce.r*) }
    .rela.rodata : { *(.rela.rodata) *(.rela.gnu.linkonce.r*) }
    .rel.got : { *(.rel.got) }
    .rela.got : { *(.rela.got) }
    .rel.ctors : { *(.rel.ctors) }
    .rela.ctors : { *(.rela.ctors) }
    .rel.dtors : { *(.rel.dtors) }
    .rela.dtors : { *(.rela.dtors) }
    .rel.init : { *(.rel.init) }
    .rela.init : { *(.rela.init) }
    .rel.fini : { *(.rel.fini) }
    .rela.fini : { *(.rela.fini) }
    .rel.bss : { *(.rel.bss) }
    .rela.bss : { *(.rela.bss) }
    .rel.plt : { *(.rel.plt) }
    .rela.plt : { *(.rela.plt) }
    .init : { *(.init) } =0x9090
    .plt : { *(.plt) }

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    __exidx_start = .;
    .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
    __exidx_end = .;

    .dummy_post_text : {
        __text_end = .;
    }

    .rodata : ALIGN(4) {
        __rodata_start = .;
        __fault_handler_table_start = .;
        KEEP(*(.rodata.fault_handler_table))
        __fault_handler_table_end = .;
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }

    /*
     * extra linker scripts tend to insert sections just after .rodata,
     * so we want to make sure this symbol comes after anything inserted above,
     * but not aligned to the next section necessarily.
     */
    .dummy_post_rodata : {
        __rodata_end = .;
    }

    .data : ALIGN(4) {
        /* writable data  */
        __data_start_rom = .;
        /* in one segment binaries, the rom data address is on top of the ram data address */
        __data_start = .;
        *(.data .data.* .gnu.linkonce.d.*)
    }

    .ctors : ALIGN(4) {
        __ctor_list = .;
        KEEP(*(.ctors .init_array))
        __ctor_end = .;
    }
    .dtors : ALIGN(4) {
        __dtor_list = .;
        KEEP(*(.dtors .fini_array))
        __dtor_end = .;
    }
    .got : { *(.got.plt) *(.got) }
    .dynamic : { *(.dynamic) }

    /*
     * extra linker scripts tend to insert sections just after .data,
     * so we want to make sure this symbol comes after anything inserted above,
     * but not aligned to the next section necessarily.
     */
    .dummy_post_data : {
        __data_end = .;
    }

    .nocache : ALIGN(1024) {
        _nocacheable_start = .;
        *(.nocache)
        . = ALIGN(1024);
        _nocacheable_end = .;
    }

    /* uninitialized data (in same segment as writable data) */
    .bss : ALIGN(4) {
        KEEP(*(.bss.prebss.*))
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    }

    /* os data stack */
    .os.data : ALIGN(4) {
        *(.os.data)
    }

    _end = .;

    .heap : ALIGN(4) {
        _heap_start = .;
        *(.heap)
    }

    . = 0x140000 + 0x80000;

    _heap_end = .;

    _end_of_ram = .;

    /* Add a labelize section of ext memory start addr, which could
     * be locate to extra linker script */
    .ext.mem (NOLOAD) : {
        . = ALIGN(4);
        __ext_mem_start = .;
        *(.ext.mem .ext.mem.*)
        . = ALIGN(4);
        __ext_mem_end = .;

    }

    /* Strip unnecessary stuff */
    /DISCARD/ : { *(.comment .note .eh_frame) }
}

