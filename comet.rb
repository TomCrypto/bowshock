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
end

software 'main', depends: ['hal'] do
  source language: :cpp, headers: [
    'src',
    '/usr/arm-none-eabi/include/c++/7.1.0',
    '/usr/arm-none-eabi/include/c++/7.1.0/arm-none-eabi/armv6-m'
  ] do
    import 'src/main.cpp'

    inject &cppflags
  end
end

hardware 'hal', targets: :lpc1114 do
  source language: :cpp, headers: [
    'src',
    '/usr/arm-none-eabi/include/c++/7.1.0',
    '/usr/arm-none-eabi/include/c++/7.1.0/arm-none-eabi/armv6-m'
  ] do
    import 'src/hal/lpc1114/**/*.cpp'
    import 'src/hal/lpc1100/**/*.cpp'
    import 'src/rtl/cortex-m0/**/*.cpp'
    import 'src/rtl/*.cpp'

    inject &cppflags
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
  target :lpc1114 do
    elf 'bin/bowshock.elf'
    bin 'bin/bowshock.bin'
    map 'bin/bowshock.map'
  end
end
