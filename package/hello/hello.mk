################################################################################
#
# hello
#
################################################################################

HELLO_VERSION = 1.0
HELLO_SITE = ./package/hello/src
HELLO_SITE_METHOD = local
HELLO_DEPENDENCIES = opencv3

$(eval $(cmake-package))
