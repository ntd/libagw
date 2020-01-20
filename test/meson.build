ardecoder_sources = files([
    'ardecoder.c',
])

ardecoder_deps = [
    gtk_dep,
    serial_dep,
]

executable('ardecoder',
           sources: ardecoder_sources,
           link_with: agw,
           dependencies: ardecoder_deps,
           c_args: agw_cflags,
           install: false)
