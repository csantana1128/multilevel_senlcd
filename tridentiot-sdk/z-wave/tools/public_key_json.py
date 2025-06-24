# SPDX-License-Identifier: LicenseRef-TridentMSLA
# SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
""" Generates json file contains the pubæic key x, y tokens """
import sys
import getopt
import json

def extract_key(key_data, is_x):
    """ Extracts the key componenet. """
    if is_x:
        key_part = key_data[27:59]
    else:
        key_part = key_data[59:]
    return key_part[::-1]

def bytes_to_hex(arr):
    """ Convert byte array to hex string. """
    return " ".join(f"0x{arr[0]:02X}" if i == 0 else f"{byte:02X}" for i, byte in enumerate(arr))

def write_to_json(file_path, key_x, key_y):
    """ Writes the x and y components of the public key to the JSON file. """
    data = {
        "TR_MFG_TOKEN_SIGNED_BOOTLOADER_KEY_X": bytes_to_hex(key_x),
        "TR_MFG_TOKEN_SIGNED_BOOTLOADER_KEY_Y": bytes_to_hex(key_y)
    }

    with open(file_path, "w", encoding="utf-8") as json_file:
        json.dump(data, json_file, indent=4)

def main(argv):
    """ Main function that parses options and performs the conversion. """
    input_file = None
    output_file = None
    try:
        opts, _ = getopt.getopt(argv,"hb:i:o:",["help","input=","output="])
    except getopt.GetoptError:
        print("public_key_json.py -i <public_key_file> -o <public_key_json_file>")
        print("Extrace the public key from the der file and save as json file")
        print("<public_key_file> the input public_key der file")
        print("<public_key_json_file> the output json file")
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print("public_key_json.py -i <public_key_file> -o <public_key_json_file>")
            sys.exit()
        elif opt in ("-i", "--input"):
            input_file = arg
        elif opt in ("-o", "--output"):
            output_file = arg

    # Open the binary file for reading in binary mode
    with open(input_file,"rb") as readf:
        binary_data = readf.read()

    public_key_x = extract_key(binary_data, True)
    public_key_y = extract_key(binary_data, False)
    print("public_key x :" + bytes_to_hex(public_key_x))
    print("public_key y :" + bytes_to_hex(public_key_y))

    # Write to JSON file
    write_to_json(output_file, public_key_x, public_key_y)


if __name__== "__main__":
    main(sys.argv[1:])
