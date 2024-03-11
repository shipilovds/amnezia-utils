# Amnezia-Utils

This utility helps convert [Amnezia](https://github.com/amnezia-vpn/amneziawg-go) client configurations between JSON and a "connection string".
You can also use the [qrencode](https://fukuchi.org/works/qrencode/) utility to generate a QR code for the client.

## Build

```bash
docker build -t amnezia-builder .
id=$(docker create amnezia-builder)
docker cp ${id}:/code/artifacts/amnezia-utils .
docker rm ${id}
```

## Usage

#### General Options

```
Usage: amnezia-utils [-e|-d] [input_file|-]
Options:
  -e              Encode JSON data
  -d              Decode Amnezia configuration
  -h              Display this help message
```

> The utility can handle data either from a file (you provide the file path) or directly from the system pipe.

#### QR Code Generation

```bash
amnezia-utils -e awg_config.json | sed "s/vpn:\/\///" | qrencode -o qr.png
```

## Runtime Dependencies

The only non-standard system library is `libQt6Core.so.6`.

To install it on Ubuntu or Debian systems, simply run `apt install libqt6core6`.
