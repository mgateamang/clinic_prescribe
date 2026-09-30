project('clinic_prescribe', 'cpp',
  version : '1.0.0',
  default_options : ['warning_level=2', 'cpp_std=c++17']
)

qt6 = import('qt6')
qt6_dep = dependency('qt6', modules : ['Core', 'Gui', 'Qml', 'Quick', 'Network'], required : true)

inc = include_directories('include')

# Preprocess MOC and QRC resources for main executable
qt_files = qt6.preprocess(
  moc_headers : ['include/prescribe_controller.hpp'],
  qresources : ['qrc/qml.qrc']
)

executable('clinic_prescribe',
  files('src/main.cpp', 'src/prescribe_controller.cpp') + qt_files,
  include_directories : inc,
  dependencies : qt6_dep,
  install : true
)

# Test runner (only needs MOC, no GUI window display required)
test_moc = qt6.preprocess(
  moc_headers : ['include/prescribe_controller.hpp']
)

test_exec = executable('clinic_prescribe_test',
  files('tests/test_prescribe.cpp', 'src/prescribe_controller.cpp') + test_moc,
  include_directories : inc,
  dependencies : qt6_dep
)

test('prescribe_tests', test_exec)
