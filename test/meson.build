ardecoder_sources = files([
    'ardecoder.c',
])

ardecoder_deps = [
    gtk_dep,
    serial_dep,
]

executable('ardecoder',
           sources: ardecoder_sources,
           link_with: libagw,
           dependencies: ardecoder_deps,
           c_args: libagw_cflags,
           install: false)
