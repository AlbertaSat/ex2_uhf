--retain="*(.intvecs)"


MEMORY
{
    VECTORS    (X)   : origin=0x00000000 length=0x00000200
    BOOT_LOAD  (RX)  : origin=0x00000200 length=0x00001000
    FLASH_API  (RX)  : origin=0x00001200 length=0x00001000
    FLASH0     (RX)  : origin=0x00002200 length=0x00018000
    FLASH1     (RX)  : origin=0x00020000 length=0x00060000
    FLASH2     (RX)  : origin=0x00080000 length=0x00080000
    FLASH3     (RX)  : origin=0x00100000 length=0x00080000
    SRAM       (RW)  : origin=0x08002000 length=0x00020000
    STACK      (RW)  : origin=0x08000000 length=0x00001FF0
}
SECTIONS
{
   .intvecs : {} > VECTORS

   boot_code :    {..\Release\sys_core.obj    (.text)}  > BOOT_LOAD
   eabi_start :   {..\Release\sys_startup.obj (.text)}  > BOOT_LOAD

   flashAPI :
   {
     ..\Release\Fapi_UserDefinedFunctions.obj (.text)
     ..\Release\bl_flash.obj (.text)

     --library= F021_API_CortexR4_BE.lib <FlashStateMachine.obj
                                          Program.obj
                                          Init.obj
                                          Utilities.obj
                                          Async.obj> (.text)
   } load = FLASH_API, run = SRAM, LOAD_START(api_load), RUN_START(api_run), SIZE(api_size)

   .text  > FLASH0
   .const > FLASH0
   .cinit > FLASH0
   .pinit > FLASH0
   .data  > SRAM
   .bss   > SRAM
}
