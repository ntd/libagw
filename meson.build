project('libagw', 'c', version: '0.1.1', license: 'LGPLv2.1+')

# How to handle LT versions (current:revision:age):
# - If the library source code has changed at all since the last
#   update, then increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
# - If any interfaces have been added, removed, or changed since the
#   last update, increment current, and set revision to 0.
# - If any interfaces have been added since the last public release,
#   then increment age.
# - If any interfaces have been removed or changed since the last
#   public release, then set age to 0.
agw_current  = 0
agw_revision = 1
agw_age      = 0

prefix     = get_option('prefix')
datadir    = join_paths(prefix, get_option('datadir'))
pkgdatadir = join_paths(datadir, meson.project_name())
assetsdir  = join_paths(pkgdatadir, 'assets')

gtk_dep    = dependency('gtk+-3.0')
serial_dep = dependency('libserialport')
rsvg_dep   = dependency('librsvg-2.0')
gladeui_dep= dependency('gladeui-2.0', required: false)

subdir('src')
subdir('test')
