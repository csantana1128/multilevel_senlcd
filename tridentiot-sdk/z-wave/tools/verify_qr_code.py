# SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
# SPDX-License-Identifier: LicenseRef-TridentMSLA

# pylint: disable=line-too-long
""" This file ... """
import argparse
import sys
from modules.qr_code_parser import QRCode

def main() -> int:
    """ Main function """

    parser = argparse.ArgumentParser(
                    prog='qr_code',
                    description='Parses a given QR code string')

    parser.add_argument('--stdin', help="Process the QR code string from stdin.", action="store_true", required=True)

    args = parser.parse_args()

    qr_code_str = ""

    if args.stdin:
        qr_code_str = sys.stdin.read()
    else:
        # This should never happen.
        pass

    qr_code = QRCode()
    print("Parsing " + qr_code_str)
    result = qr_code.parse_from_string(qr_code_str)
    if result > 0:
        print("Parsing failed")
        return result

    print(qr_code)

    # Check Switch On/Off security keys.
    # Yes, this is hardcoded. We can improve it later or replace by pytest.
    if qr_code.get_requested_keys() != 131:
        print("Requested keys doesn't match.")
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
