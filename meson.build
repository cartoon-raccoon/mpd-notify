project('mpdnotify', 'cpp', version : '0.1.0', default_options : ['cpp_std=c++20'])

deps = [dependency('libnotify'), dependency('libmpdclient')]
includes = include_directories('include')

srcdir = 'src'
plainfiles = [
    'mpdnotify.cpp',
    'main.cpp',
]

# add the source dir to each
srcfiles = []
foreach f : plainfiles
    srcfiles += [srcdir / f]
endforeach

# the main mpdnofity executable
executable('mpdnotify', srcfiles, 
    dependencies: deps, 
    include_directories: includes)

# the test mpd async executable
executable('mpdasync', 'src/mpdasync.cpp',
    dependencies: deps,
    include_directories: includes)