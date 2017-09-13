cppflags = proc do
  option :ffreestanding
  option :nostdlib
  option :std => 'c++1z'
  option 'fshort-enums'
  option 'fno-exceptions'
  option :Wall
  option :Wextra
  option :Wpedantic
  option :pedantic
  option :g
  option :O2
  option 'fno-use-cxa-atexit'
end

headers = [
  '/usr/arm-none-eabi/include/c++/7.1.0',
  '/usr/arm-none-eabi/include/c++/7.1.0/arm-none-eabi/armv6-m'
]

software 'led-matrix', depends: ['hal'] do
  source language: :cpp, headers: ['src', *headers] do
    import 'src/app/led-matrix/**/*.cpp'

    inject &cppflags
  end
end

software 'control-firmware', depends: ['control', 'hal'] do
  source language: :cpp, headers: ['src', *headers] do
    import 'nothing'
  end
end

hardware 'control', targets: :lpc1100 do
  source language: :cpp, headers: ['src', *headers] do
    import 'src/app/control/lpc1100.cpp'

    inject &cppflags
  end
end

software 'main', depends: ['hal'] do
  source language: :cpp, headers: ['src', *headers] do
    import 'src/main.cpp'

    inject &cppflags
  end
end

hardware 'hal', targets: :lpc1100 do
  source language: :cpp, headers: ['src', *headers] do
    import 'src/hal/lpc1100/**/*.cpp'
    import 'src/rtl/cortex-m0/**/*.cpp'
    import 'src/rtl/*.cpp'

    inject &cppflags
    define :RTL_CORTEX_M0
  end

  source language: :native do
    import 'src/rtl/cortex-m0/**/*.S'
  end

  linker 'arm-none-eabi', isa: 'armv6-m', cpu: 'cortex-m0', opt: 2 do
    script 'src/hal/lpc1100/layout.ld'
    option :static
    option :nostdlib
  end

  import 'gcc'
end

firmware 'bowshock', imports: ['main'] do
  target :lpc1100 do
    elf 'bin/bowshock.elf'
    bin 'bin/bowshock.bin'
    map 'bin/bowshock.map'
  end
end

firmware 'led-matrix', imports: ['led-matrix'] do
  target :lpc1100 do
    elf 'bin/led-matrix.elf'
    bin 'bin/led-matrix.bin'
    map 'bin/led-matrix.map'
  end
end

firmware 'control-firmware', imports: ['control-firmware'] do
  target :lpc1100 do
    elf 'bin/control-firmware.elf'
    bin 'bin/control-firmware.bin'
    map 'bin/control-firmware.map'
  end
end
