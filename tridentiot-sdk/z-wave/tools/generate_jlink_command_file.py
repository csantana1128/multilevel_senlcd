# SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
# SPDX-License-Identifier: LicenseRef-TridentMSLA

import sys, getopt
import re
from jinja2 import Template
import os
from pathlib import Path
import argparse

def print_help():
    print ('generate_runner.py -i <inputfile> -t <templatefile>')


def main(argv):
    parser = argparse.ArgumentParser(
                    prog='generate_jlink_command_file',
                    description='Generates a J-Link command file based on a HEX file and a template.',
                    epilog='')
    
    parser.add_argument('--hex-file', required=True, help="HEX file to use in the J-Link command file.")
    parser.add_argument('--template', required=True, help="J-Link command file template.")
    parser.add_argument('--output-dir', required=True, help="Directory where the generated J-Link command file will be located.")
    parser.add_argument('--device', required=True, help="Device to use in the J-Link command file.")

    args = parser.parse_args()

    inputfile = ''
    templatefile = ''
    output_dir = ''
    device = ''
    if args.hex_file != '':
        inputfile = args.hex_file
    if args.template != '':
        templatefile = args.template
    if args.output_dir != '':
        output_dir = args.output_dir
    if args.device != '':
        device = args.device
    
    with open(templatefile) as file_:
        template = Template(file_.read())
        output = template.render(hex_file=inputfile, device=device)

        input_file_name = Path(inputfile).stem
        outputfile = output_dir + "/" + input_file_name + ".jlink"
        print("Writing output to " + outputfile)
        with open(outputfile, "w") as fh:
            fh.write(output)


if __name__ == "__main__":
   main(sys.argv[1:])
