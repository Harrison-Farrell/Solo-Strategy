# Data Types
All integer fields are big endian (network byte order) binary encoded numbers. 
Unless otherwise noted, they are unsigned. All alpha fields are ASCII fields 
which are left justified and padded on the right with spaces.

Prices are integer fields, supplied with an associated precision. When converted
 to a decimal format, prices are in fixed point format, where the precision 
 defines the number of decimal places. For example, a field flagged as Price (4)
 has an implied 4 decimal places. The maximum value of price (4) in TotalView 
 ITCH is 200,000.0000 (decimal,77359400 hex). Timestamps are represented as 
nanoseconds since midnight


The ITCH Messages are wrapped within the moldupd64 packet

Message Block
{
    Message Length  (2 Bytes)
    Message Data    (Length Bytes)
}

1.3 Add Order Message
1.4 Modify Order Messages


