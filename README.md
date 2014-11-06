Web-Security-Course
===================


This repository contains assignments or course project of course web security

### Assignment 1 - MD5
MD5 algorithm

```
1. Show me how MD5 works.
2. Design an executable MD5 program in C/C++ with some short samples.
```

### Assignment 2 - short thesis
Write a short thesis on one of the three applications 

```
1. MD5 and password protection. [chosed]
2. AES and Wi‐Fi protected access.
3. RSA and electronic business.
```

### Assignment 3 - X.509 certificate
Give an example of X.509 certificate and state how it works

* Useful commands

 1.generate certificate
 ```shell
 openssl genrsa 1024 > key.pem
 openssl req -x509 -new -key key.pem > x509.crt
 ```
 2.print X.509 certificate in a pretty way
 ```shell
 openssl x509 -in x509.crt -text
 ```

### Assignment 4 - IPsec
State the packing and unpacking procedure of IPsec packets under transport mode
