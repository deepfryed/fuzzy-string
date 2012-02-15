#!/usr/bin/env ruby

require 'mkmf'

$CFLAGS = '-std=c99'

have_library('stemmer') or raise "please install snowball stemmer (debian package: libstemmer-dev)"
create_makefile 'fuzzy-string/fuzzy_string'
