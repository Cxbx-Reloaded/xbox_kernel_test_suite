#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(HalGetInterruptVector)
{
    TEST_BEGIN();

    typedef struct {
        const char* device_name;
        ULONG irq;
    } irq_test_entry;

    // Xbox hardware IRQ assignments
    irq_test_entry irq_tests[] = {
        { "USB0 (OHCI)",       1 },
        { "NV2A GPU",          3 },
        { "NIC",               4 },
        { "APU",               6 },
        { "ACI (AC97)",        8 },
        { "USB1 (OHCI)",       9 },
        { "IDE",              14 },
    };
    size_t num_tests = ARRAY_SIZE(irq_tests);

    KIRQL irqls[ARRAY_SIZE(irq_tests)];
    ULONG vectors[ARRAY_SIZE(irq_tests)];

    // --- Test each known IRQ returns valid vector and IRQL ---
    for (unsigned i = 0; i < num_tests; i++) {
        vectors[i] = HalGetInterruptVector(irq_tests[i].irq, &irqls[i]);

        GEN_CHECK(vectors[i] != 0, TRUE, irq_tests[i].device_name);
        GEN_CHECK(irqls[i] > PASSIVE_LEVEL, TRUE, irq_tests[i].device_name);
        GEN_CHECK(irqls[i] <= HIGH_LEVEL, TRUE, irq_tests[i].device_name);
    }

    // --- All vectors must be unique (different IRQs => different vectors) ---
    for (unsigned i = 0; i < num_tests; i++) {
        for (unsigned j = i + 1; j < num_tests; j++) {
            GEN_CHECK(vectors[i] != vectors[j], TRUE, "unique vectors");
        }
    }

    // --- Same IRQ returns consistent results ---
    for (unsigned i = 0; i < num_tests; i++) {
        KIRQL irql2;
        ULONG vector2 = HalGetInterruptVector(irq_tests[i].irq, &irql2);
        GEN_CHECK(vector2, vectors[i], irq_tests[i].device_name);
        GEN_CHECK(irql2, irqls[i], irq_tests[i].device_name);
    }

    TEST_END();
}
