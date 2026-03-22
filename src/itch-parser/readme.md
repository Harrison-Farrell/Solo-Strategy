# ITCH Parser

Parses NASDAQ ITCH protocol messages to extract market data.

## Message Format
- **Integers**: Big-endian (network byte order), binary encoded, unsigned by default.
- **Alphanumeric**: ASCII, left-justified, right-padded with spaces.
- **Prices**: Fixed-point integer format with implied precision (e.g., Price(4) implies 4 decimal places). Maximum value is 200,000.0000.
- **Timestamps**: Nanoseconds since midnight.

ITCH Messages are wrapped within a `moldupd64` packet structure:
```text
Message Length (2 Bytes)
Message Data   (Length Bytes)
```

## Usage Example

```cpp
#include "itch-parser/itch_parser.h"

int main() {
    ITCH_Parser parser("path/to/itch/data.bin");
    parser.Execute();
    // Messages are parsed and placed in the internal message queue
    return 0;
}
```
