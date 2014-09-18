require 'mkmf'

$CFLAGS << " -Wall "

extension_name = 'socket_helper'
dir_config(extension_name)
create_makefile(extension_name)
