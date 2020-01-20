libagw_sources = files([
    'agw-gauge.h',
    'agw-gauge.c',
])

libagw_assets = files([
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

libagw_deps = [
    gtk_dep,
    rsvg_dep,
]

libagw_cflags = [
    '-DSRCDIR="' + meson.current_source_dir() + '"',
    '-DPKGDATADIR="' + pkgdatadir + '"',
]

install_data(libagw_assets,  install_dir: assetsdir)

libagw = library('libagw',
                 sources: libagw_sources,
                 dependencies: libagw_deps,
                 c_args: libagw_cflags,
                 install: true)
