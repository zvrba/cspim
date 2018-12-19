#ifndef MIPS_ENDIAN_H_
#define MIPS_ENDIAN_H_

#include "portable_endian.h"

#if defined(MIPS_BE)
#   define te16toh(x) be16toh(x)
#   define htote16(x) htobe16(x)
#   define te32toh(x) be32toh(x)
#   define htote32(x) htobe32(x)
#   define te64toh(x) be64toh(x)
#   define htote64(x) htobe64(x)
#else
#   define te16toh(x) le16toh(x)
#   define htote16(x) htole16(x)
#   define te32toh(x) le32toh(x)
#   define htote32(x) htole32(x)
#   define te64toh(x) le64toh(x)
#   define htote64(x) htole64(x)
#endif

// For self-documentation/consistency
#define te8toh(x) (x)
#define htote8(x) (x)

#endif /* MIPS_ENDIAN_H_ */