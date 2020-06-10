# Generates a private and public 2048-bit key

openssl prime -generate -bits 2048 > public_key
openssl prime -generate -bits 2048 > private_key
