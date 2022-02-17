#ifndef BYZ_BIT_FLAGS_H
#define BYZ_BIT_FLAGS_H

#define BIT_FLAG(x) (1u << x)

#define bit_flags_set_flag(field, flag) (field |= flag)
#define bit_flags_has_flags(field, flag) ((field & flag) != 0)

#endif
