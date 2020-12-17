# aes-enc-dec-tool
------------
## What it can do ?

The tool can encrypt and decrypt binary files using AES-256 encryption alghorithm.

While encrypting, it appends header [(magic number), (original file size), (CRC)] which is used when you want to decrypt your data back.

It prints header on encryption completion.

On decryption start, the program prints out the header. On the decryption finish it compares CRC in the header and CRC of decrypted data.

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

 long arguments present:
  --encrypt 
  --decrypt 
  --key 
  --input
  --output 
  --verbose 
  --help
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


