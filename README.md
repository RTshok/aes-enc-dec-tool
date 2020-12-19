# aes-enc-dec-tool
------------
## What it can do ?

The tool can encrypt and decrypt binary files using AES-256 encryption alghorithm.

While encrypting, it appends header [(magic number), (original file size), (CRC)] which is used when you want to decrypt your data back.

It prints header on encryption completion.

On decryption start, the program prints out the header. On the decryption finish it compares CRC in the header and CRC of decrypted data.

By default block read is 16 bytes. This parameter could be changed by passing desired block size as an argument with key -s. Block size could be up to 8192 bytes. Block size should be as a power of 2 (16, 32, 64...8192)

----------
## How to build ?
Makefile commands

 - `make all` - building the tool. Output directory: ./bin
 - `make clean` - performs clean up of build artifacts

### Usage

For encryption:
`./bin/ED-tool -e -k 01234567890123456789012345678901 -i ./input.bin -o ./output.bin`

For decryption:
`./bin/ED-tool -d -k 01234567890123456789012345678901 -i ./output.bin -o ./output_decrypted.bin`

### Argument explanation
 ```
 -e - for encrypt 
 -d - for decrypt
 -k - 256 bit key 
 -i - input path 
 -o - output path
 -h - prints help 
 -v - verbose output
 -s - block size

 long arguments present:
  --encrypt 
  --decrypt 
  --key 
  --input
  --output 
  --verbose 
  --help
  --sizeb
  ```

### Output
```
ENCRYPTION
./bin/ED-tool -v -e -k 01234567890123456789012345678901 -i ./main.c -o ./test
input data length 6392
ciphered package length : 6400
Encryption completed !
 HEADER
______________________________________________________

magic number : deadbeef
size in bytes : 6392
crc32 : d899526f
______________________________________________________
package + header length : 6412 

DECRYPTION

./bin/ED-tool -v -d -k 01234567890123456789012345678901 -i ./test -o ./main_decrypted.c
input data length : 6412

 HEADER
______________________________________________________

magic number : deadbeef
size in bytes : 6392
crc32 : d899526f
______________________________________________________
data length without header : 6400
crc after decryption : d899526f
Decryption completed ! CRC's are equal !
Bytes written to file: 6392

```
---------

## Dependencies


`openssl-dev`
`build-essential`

------------
## Formatter

As formatter was used : `clang-formatter`


Usage : `./format.sh`

--------
## Tested platforms


`Ubuntu 19.10`

--------

## Minor performance comparison

Input file size - 100MB

Encyphering when block size is default - 16 bytes

``` time ./bin/ED-tool -e -i ./input.bin -o ./output.bin -k 01234567890123456789012345678901 -s 16
Size of block is default ! 16b 
file size : 100000000 
block amount 6250000 

 HEADER
______________________________________________________

magic number : deadbeef
unciphered size in bytes : 100000000
crc32 : 2142554d
______________________________________________________

real    0m29,244s
user    0m10,194s
sys     0m19,028s ```

Encyphering when block size is 8192 bytes

```time ./bin/ED-tool -e -i ./input.bin -o ./output.bin -k 01234567890123456789012345678901 -s 8192
Size of block isn't default ! Current size : 8192 
file size : 100000000 
block amount 12208 

 HEADER
______________________________________________________

magic number : deadbeef
unciphered size in bytes : 100000000
crc32 : 2142554d
______________________________________________________

real    0m0,922s
user    0m0,595s
sys     0m0,176s ```

As we can observe, the difference is enormous !

------------

## Memory consumption

No dynamic memory allocation. For encryption/decryption operation memory allocation is block_size + block_size + AES_BLOCK_SIZE(16).
F.e. block_size = 256, memory consumption at least is 528 bytes.
Increasing block_size leads to increasing the performance

