#include <b64/b64.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char B64_TABLE[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const uint32_t B64_DECODE_TABLE[] = {
    62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1,
    -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

static uint32_t b64_encoded_size(uint32_t len) {
  uint32_t result = len;

  if (result % 3 != 0) {
    result += 3 - (len % 3);
  }

  result /= 3;
  result *= 4;

  return result;
}

static uint32_t b64_decoded_size(const char *s) {
  if (!s)
    return 0;

  uint32_t len = strlen(s);
  uint32_t result = len / 4 * 3;

  for (uint32_t i = len; i-- > 0;) {
    if (s[i] == '=') {
      result--;
    } else {
      break;
    }
  }

  return result;
}

static unsigned int b64_is_valid_char(char c) {
  return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
          (c >= 'a' && c <= 'z') || (c == '+') || (c == '/') || (c == '='));
}

char *b64_encode(const char *s) {
  char *out;
  uint32_t len = strlen(s);

  if (s == 0 || len == 0) {
    fprintf(stderr, "Error (b64_encode): Invalid input string.\n");
    return 0;
  }

  uint32_t elen = b64_encoded_size(len);
  out = malloc(elen + 1);

  if (!out) {
    fprintf(
        stderr,
        "Error: (b64_encode): Failed to allocate memory for output string.\n");
  }

  out[elen] = '\0';

  uint32_t v;
  uint32_t j;
  uint32_t i;
  for (i = 0, j = 0; i < len; i += 3, j += 4) {
    v = s[i];
    v = i + 1 < len ? v << 8 | s[i + 1] : v << 8;
    v = i + 2 < len ? v << 8 | s[i + 2] : v << 8;

    out[j] = B64_TABLE[(v >> 18) & 0x3F];
    out[j + 1] = B64_TABLE[(v >> 12) & 0x3F];
    if (i + 1 < len) {
      out[j + 2] = B64_TABLE[(v >> 6) & 0x3F];
    } else {
      out[j + 2] = '=';
    }
    if (i + 2 < len) {
      out[j + 3] = B64_TABLE[v & 0x3F];
    } else {
      out[j + 3] = '=';
    }
  }

  return out;
}

char *b64_decode(const char *in) {
  uint32_t len;
  uint32_t i;
  uint32_t j;
  int v;

  if (in == 0) {
    fprintf(stderr, "Error (b64_decode): Input string is NULL.\n");
    return 0;
  }

  len = strlen(in);
  if (len % 4 != 0) {
    fprintf(
        stderr,
        "Error (b64_decode): Length of input string is not divisible by 4.\n");
    return 0;
  }

  for (i = 0; i < len; i++) {
    if (!b64_is_valid_char(in[i])) {
      fprintf(stderr, "Error (b64_decode): Invalid input string.\n");
      return 0;
    }
  }

  char *out = (char *)calloc(b64_decoded_size(in) + 1, sizeof(char *));

  if (!out) {
    fprintf(
        stderr,
        "Error (b64_decode): Failed to allocate memory for output string.\n");
    return 0;
  }

  for (i = 0, j = 0; i < len; i += 4, j += 3) {
    v = B64_DECODE_TABLE[in[i] - 43];
    v = (v << 6) | B64_DECODE_TABLE[in[i + 1] - 43];
    v = in[i + 2] == '=' ? v << 6 : (v << 6) | B64_DECODE_TABLE[in[i + 2] - 43];
    v = in[i + 3] == '=' ? v << 6 : (v << 6) | B64_DECODE_TABLE[in[i + 3] - 43];

    out[j] = (v >> 16) & 0xFF;
    if (in[i + 2] != '=')
      out[j + 1] = (v >> 8) & 0xFF;
    if (in[i + 3] != '=')
      out[j + 2] = v & 0xFF;
  }

  return out;
}
