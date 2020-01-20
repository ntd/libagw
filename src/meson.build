agw_sources = files([
    'agw-gauge.h',
    'agw-gauge.c',
])

agw_assets = files([
    'assets/clock-drop-shadow.svg',
    'assets/clock-face-shadow.svg',
    'assets/clock-face.svg',
    'assets/clock-frame.svg',
    'assets/clock-glass.svg',
    'assets/clock-hour-hand-shadow.svg',
    'assets/clock-hour-hand.svg',
    'assets/clock-marks.svg',
    'assets/clock-minute-hand-shadow.svg',
    'assets/clock-minute-hand.svg',
    'assets/clock-second-hand-shadow.svg',
    'assets/clock-second-hand.svg',
])

agw_soversion = '@0@.@1@.@2@'.format(agw_current - agw_age,
                                     agw_age,
                                     agw_revision)

agw_deps = [
    gtk_dep,
    rsvg_dep,
]

agw_cflags = [
    '-DSRCDIR="' + meson.current_source_dir() + '"',
    '-DPKGDATADIR="' + pkgdatadir + '"',
]

install_data(agw_assets,  install_dir: assetsdir)

agw = library('agw',
              sources: agw_sources,
              dependencies: agw_deps,
              version: agw_soversion,
              c_args: agw_cflags,
              install: true)


# Install the glade catalog file, if possible
if gladeui_dep.found()
    catalog_dir = gladeui_dep.get_pkgconfig_variable('catalogdir')
    catalog_files = files([
        'libagw.xml',
    ])
    install_data(catalog_files,  install_dir: catalog_dir)
endif
