#pragma once

#define BIT(n)              (1UL << (n))
#define SET_BIT(reg, bit)   ((reg) |= BIT(bit))
#define CLR_BIT(reg, bit)   ((reg) &= ~BIT(bit))
#define TOG_BIT(reg, bit)   ((reg) ^= BIT(bit))
#define READ_BIT(reg, bit)  (((reg) & BIT(bit)) != 0)
