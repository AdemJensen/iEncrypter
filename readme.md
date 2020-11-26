# iEncrypter
iEncrypter is a simple AES encrypt / decrypt sample program using OpenSSL's lib.

The program contains following features:
- Encrypt a file using AES algorithm (Password or key file).
- Decrypt a file using AES algorithm (Password or key file).
- Generate a key file from password.

Well, actually, the key file is just a 32-characters-long MD5 hash value, which is perfect for OpenSSL's AES key.

## User manual
To use this program, please use the following command:

### File Encryption
- Using password:

```bash
ien encrypt -i [Input] -o [Output] -p [Password]
```

For example:

```bash
ien encrypt -i "1.txt" -o "2.txt" -p "123456"
```

- Using key file:

```bash
ien encrypt -i [Input] -o [Output] -k [Key Path]
```

For example:

```bash
ien encrypt -i "1.txt" -o "2.txt" -k "key.txt"
```

### File Decryption
- Using password:

```bash
ien decrypt -i [Input] -o [Output] -p [Password]
```

For example:

```bash
ien decrypt -i "2.txt" -o "3.txt" -p "123456"
```

- Using key file:

```bash
ien decrypt -i [Input] -o [Output] -k [Key Path]
```

For example:

```bash
ien decrypt -i "2.txt" -o "3.txt" -k "key.txt"
```

### Key file generation
```bash
ien genkey -o [Output] -p [Password]
```

For example:

```bash
ien genkey -o "key.txt" -p "123456"
```

## Build from source
- On Windows: You need `nmake`.
- On Unix based systems (Including OSX): You need command `make` to work. 
    - On mac, you can use Homebrew to install `make`.
    - On Linux, you can use your favorite package manager to install `make` or `GNU make`.

## Inside the system
The encryption and decryption uses `AES_Encrypt` and `AES_Decrypt`. Key gen uses `MD5`.

Take `AES_Encrypt` for example:

The OpenSSL's `aes.h` defines blocks with length of `16`. Which means that **all the plaintext and ciphertext must have length no more than 16**. Any string longer than 16 will be ignored. 

And even though you may get ciphertexts longer or shorter than 16, they are indeed 16 characters. You can use `strlen()` to verify that.

Another thing to be noticed: **You must memset all the input and output character arrays**. Don't ask why, you can try.

Other things such as file operations, they have nothing to talk about. Anyone with some basic knowledge of programming can read them easily (probably).

## Postscript

You may wonder why I wrote this. Well, as you can see, encryption programs like OpenSSL's official CMD tools, they usually contains large amount of codes. It's hard to find a simple program that encrypts and decrypts files.

That's right, this program is written for studying purpose.

## License
This project has inherited **[the dual OpenSSL and SSLeay license](https://www.openssl.org/source/license-openssl-ssleay.txt)** from OpenSSL, I guess.

Or what else, I don't know.