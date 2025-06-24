# SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
# SPDX-License-Identifier: LicenseRef-TridentMSLA

# pylint: disable=line-too-long, too-many-return-statements, too-few-public-methods
""" This file ... """
import hashlib
import argparse
import sys
import re
import json


class TLV():
    """ This class ... """
    def __init__(self, type_, critical, length, value):
        self.type = type_
        self.critical = critical
        self.length = length
        self.value = value

    def __str__(self):
        str_  = "Type: " + str(self.type) + "\n"
        str_ += "Critical: " + str(self.critical) + "\n"
        str_ += "Length: " + str(self.length) + "\n"
        str_ += "Value: " + str(self.value) + "\n"
        return str_


class QRCode():
    """ This class ... """
    def __init__(self):
        self.input = ""

        # Fields
        self.lead_in = 0
        self.version = 0
        self.checksum = 0
        self.requested_keys = 0
        self.dsk = []
        self.tlv = []

    def parse_from_string(self, input_: str) -> int:
        """ Parses a QR code string """
        self.input = re.sub(r'\D', '', input_.strip())

        if len(self.input) < 52:
            # Minimum length is 52.
            return 1

        total_length = len(self.input)
        remaining_length = total_length

        # Parse lead-in
        self.lead_in = int(self.input[:2])
        if self.lead_in != 90:
            return 2
        remaining_length -= 2

        # Parse version
        self.version = int(self.input[2:4])
        if self.version not in [0,1]:
            return 3
        remaining_length -= 2

        # Parse checksum
        self.checksum = int(self.input[4:9])
        if self.checksum > 0xFFFF:
            return 4
        remaining_length -= 5

        # Calculate and compare checksom of fields following the "Checksum" field.
        checksum_calculated = hashlib.sha1(bytearray(self.input[9:], 'utf-8'))
        checksum_part = checksum_calculated.hexdigest()[:4]
        if int(checksum_part, 16) != self.checksum:
            return 5

        self.requested_keys = int(self.input[9:12])
        if self.requested_keys > 255:
            return 6
        remaining_length -= 3

        chunk_size = 5
        dsk_field = self.input[12:52]
        self.dsk = [dsk_field[i:i + chunk_size] for i in range(0, len(dsk_field), chunk_size)]
        remaining_length -= 40

        while remaining_length >= 4:
            # TLV fields are available

            # Type/Critical (2) and length (2) are mandatory.
            offset_type = total_length - remaining_length
            type_ = int(self.input[offset_type:offset_type+2])
            critical = type_ & 1
            type_ = type_ >> 1
            remaining_length -= 2
            offset_length = total_length - remaining_length
            length = int(self.input[offset_length:offset_length+2])
            remaining_length -= 2
            if length > 0:
                offset_value = total_length - remaining_length
                value = self.input[offset_value:offset_value+length]
                self.tlv.append(TLV(type_, critical, length, value))
                remaining_length -= length

        return 0

    def get_lead_in(self) -> int:
        """ Returns the lead-in """
        return self.lead_in

    def get_version(self) -> int:
        """ Returns the version """
        return self.version

    def get_checksum(self) -> int:
        """ Returns the checksum """
        return self.checksum

    def get_requested_keys(self) -> int:
        """ Returns the requested security keys """
        return self.requested_keys

    def get_dsk(self) -> list:
        """ Returns the DSK """
        return self.dsk

    def __str__(self):
        str_  = "Lead-in: " + str(self.lead_in) + "\n"
        str_ += "Version: " + str(self.version) + "\n"
        str_ += "Checksum: " + hex(self.checksum) + "\n"
        str_ += f"Requested keys: 0b{self.requested_keys:b} ({self.requested_keys:d})\n".format(self.requested_keys)
        str_ += "DSK: " + "-".join(self.dsk) + "\n"
        str_ += '\n'.join(str(tlv) for tlv in self.tlv)
        return str_

    def to_dict(self):
        """ returns the class as a custom dictionary """
        _dictionary = {}

        _dictionary['input'] = self.input
        _dictionary['lead_in'] = self.lead_in
        _dictionary['version'] = self.version
        _dictionary['checksum'] = self.checksum
        _dictionary['requested_keys'] = self.requested_keys
        _dictionary['dsk'] = '-'.join(self.dsk)
        _dictionary['tlv'] = [ tlv.__dict__ for tlv in self.tlv ]

        return _dictionary


def test():
    """ Run tests """
    qr_code_str = ("90" # Lead-in
                    "01" # Version
                    "56310" # Checksum
                    "131" # Requested keys
                    "3992527154232235522738430239966379741306" # DSK
                    "00100409601792" # Product type
                    "022000000000040000202579" # Product ID
                    "0803003") # Supported protocols

    qr_code = QRCode()
    result = qr_code.parse_from_string(qr_code_str)
    assert 0 == result, "Parsing failed"

    lead_in = qr_code.get_lead_in()
    assert isinstance(lead_in, int)
    assert lead_in == 90, "Lead-in doesn't match"

    version = qr_code.get_version()
    assert isinstance(version, int)
    assert version == 1, "Version doesn't match"

    checksum = qr_code.get_checksum()
    assert isinstance(checksum, int)
    assert checksum == 0xdbf6, "Checksum doesn't match"

    requested_keys = qr_code.get_requested_keys()
    assert isinstance(requested_keys, int)
    assert requested_keys == 131, "Requested keys doesn't match"

    dsk = qr_code.get_dsk()
    assert isinstance(dsk, list)
    assert dsk == ["39925", "27154", "23223", "55227", "38430", "23996", "63797", "41306"], "DSK doesn't match"


def main() -> int:
    """ Main function """

    parser = argparse.ArgumentParser(
                    prog='qr_code',
                    description='Parses a given QR code string')

    parser.add_argument("--json", action="store_true", help="Outputs the QR Code paramters as a json string")

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--test', help="Run test of this module.", action="store_true")
    group.add_argument('--stdin', help="Process the QR code string from stdin.", action="store_true")
    group.add_argument('--qr', type=str, help="Process the QR code string as an argument.")

    args = parser.parse_args()

    qr_code_str = ""

    if args.test:
        test() # Will exit on assertion.
        return 0

    if args.stdin:
        qr_code_str = sys.stdin.read()
    elif args.qr:
        qr_code_str = args.qr
    else:
        # This should never happen.
        pass

    qr_code = QRCode()
    result = qr_code.parse_from_string(qr_code_str)
    if args.json:
        qr_code_json = json.dumps(qr_code.to_dict())
        print(str(qr_code_json))

    else:

        print( "\n" + "Parsing " + qr_code_str)
        if result > 0:
            print("Parsing failed")
            return result

        print(qr_code)

    return 0


if __name__ == "__main__":
    sys.exit(main())
