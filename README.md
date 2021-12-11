# b64
> Base64 encode / decode library for C.


## Usage
``` C

#include <b64/b64.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *encoded = b64_encode("hello world");
  char *decoded = b64_decode(encoded);

  printf("%s\n", encoded); // aGVsbG8gd29ybGQ=
  printf("%s\n", decoded); // hello world

  // dont forget to free the bytes
  free(encoded);
  free(decoded);

  return 0;
}
```

## Notes
> Both `b64_encode` and `b64_decode` will return `0` on fail. 
