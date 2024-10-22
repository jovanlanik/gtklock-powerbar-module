# gtklock-powerbar-module
# Copyright (c) 2024 Jovan Lanik

project(
  'gtklock-powerbar-module', 'c',
  version : '4.0.0',
  license : 'GPLv3',
  default_options : ['c_std=c11'],
)

gtk = dependency('gtk+-3.0')
gmodule_export = dependency('gmodule-export-2.0')

dependencies = [
  gtk,
  gmodule_export,
]

gtklock_powerbar_module_sources = files('source.c')

gnome = import('gnome')
icons_resources = gnome.compile_resources(
  'icons-resources', 'icons.gresource.xml',
  c_name : 'icons',
)

gtklock_powerbar_module_set = [
  gtklock_powerbar_module_sources,
  icons_resources,
]

if import('fs').is_absolute(get_option('libdir'))
  gtklock_module_dir = get_option('libdir') / 'gtklock'
else
  gtklock_module_dir = get_option('prefix') / get_option('libdir') / 'gtklock'
endif

shared_library(
  'powerbar-module',
  gtklock_powerbar_module_set,
  name_prefix : '',
  dependencies : dependencies,
  install : true,
  install_dir : gtklock_module_dir,
)
