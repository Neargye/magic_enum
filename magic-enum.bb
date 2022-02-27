SUMMARY = "Static reflection for enum"
HOMEPAGE = "https://github.com/Neargye/magic_enum"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://LICENSE;md5=edd5a32f4a54b363083dbb3cd2aa761b"

SRC_URI = "git://github.com/Neargye/magic_enum.git"
SRCREV = "c9388f1c99f1867bb9b1d6c813156d599ee00438"

S="${WORKDIR}/git"

inherit cmake

