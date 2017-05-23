/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * See md5.c for more information.
 */

#include <autograph/Core/Exports.h>
#include <array>
#include <cstdint>
#include <string>

/* Any 32-bit or wider unsigned integer data type will do */

namespace ag {
struct AG_CORE_API MD5Hasher {
  using Hash = std::array<uint8_t, 16>;

  struct CTX {
    uint32_t lo, hi;
    uint32_t a, b, c, d;
    unsigned char buffer[64];
    uint32_t block[16];
  };

  MD5Hasher() { init(); }

  void init();
  void update(const void *data, size_t size);
  void finalize(Hash &outHash);

  static std::string hashToString(const Hash &hash);

  CTX ctx;
};
}