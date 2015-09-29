tftp-bootloader-client-app is a bootloading application. It runs on a
Linux host, and interfaces directly with POSIX sockets.

tftp-bootloader-client-app bootloads an EBL file to a single target at
a time. It allows bootload resuming via the --resume argument (see
below). It has no knowledge of 15.4 networks, so to bootload a target
on a 15.4 network, you need to be connected to a 15.4 gateway.

tftp-bootloader-client-app and the tftp-bootloader server, running on
a bootload target, utilize the tftp-bootloader protocol. The
tftp-bootloader protocol is comprised of two components: TFTP and a
synchronization protocol. The synchronization protocol serves a few
purposes. It allows the bootload target to hold off the bootload
process while it erases its FLASH storage. It allows resuming if the
bootload process fails due to communication errors, and it also allows
security checking via the manufacturer ID, device type, and version
number arguments (see below).

To turn bootload logging on on your SoC, run the command:
log X "bootload"
where X is your APP_SERIAL value.

Example:

tftp-bootloader-client-app --target "target ipv6 address" --file your_ebl_file.ebl

Optional arguments:

  --resume          attempt a resume, no value needed
  --manufacturer_id manufacturer id, 16 bit integer
  --device_type     device type, 8 bit integer
  --version_number  version number, 32 bit integer

The manufacturer_id, device_type, and version_number arguments are verified on
the bootload target in the emberVerifyBootloadRequest() function.
