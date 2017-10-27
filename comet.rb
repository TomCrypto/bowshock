cppflags = proc do
  option :ffreestanding
  option :nostdlib
  option :std => 'c++17'
  option 'fshort-enums'
  option 'fno-exceptions'
  option :Wall
  option :Wextra
  option :Wpedantic
  option :pedantic
  option :g
  option :O3
  option 'fno-use-cxa-atexit'
end

headers = [
  '/usr/arm-none-eabi/include/c++/7.2.0',
  '/usr/arm-none-eabi/include/c++/7.2.0/arm-none-eabi/armv6-m'
]

software 'test', depends: ['hal'] do
  source language: :cpp, headers: ['src', 'spec/support', *headers] do
    import 'spec/breadboard/board.cpp'

    inject &cppflags
  end
end

software 'mmio-test', depends: ['hal'] do
  source language: :cpp, headers: ['src', 'spec/support', *headers] do
    import 'spec/lpc1100/mmio/board.cpp'

    inject &cppflags
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
    option :'fuse-ld' => 'bfd'
    option 'L/usr/lib/gcc/arm-none-eabi/7.2.0/armv6-m'
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

firmware 'test', imports: ['test'] do
  target :lpc1100 do
    elf 'bin/test-firmware.elf'
    bin 'bin/test-firmware.bin'
    map 'bin/test-firmware.map'
  end
end

firmware 'mmio-test', imports: ['mmio-test'] do
  target :lpc1100 do
    elf 'bin/lpc1100-mmio-firmware.elf'
    bin 'bin/lpc1100-mmio-firmware.bin'
    map 'bin/lpc1100-mmio-firmware.map'
  end
end
