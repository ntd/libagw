project('xpray', 'c', version: '0.1.0', license: 'LGPLv2.1+')

prefix     = get_option('prefix')
datadir    = join_paths(prefix, get_option('datadir'))
pkgdatadir = join_paths(datadir, meson.project_name())
assetsdir  = join_paths(pkgdatadir, 'assets')

gtk_dep  = dependency('gtk+-3.0')
rsvg_dep = dependency('librsvg-2.0')

subdir('src')
subdir('test')
